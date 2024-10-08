#include "vmxon.h"

int is_vmx_supported(void)
{
    __cpuid_t cpuid = { 0 };
    int cpu_info[4];  // Use int for CPU info to be compatible with __cpuidex


    logger("Checking if VMX is supported...\n");
    __cpuidex(cpu_info, 1, 0);  // In Windows kernel-mode, use __cpuidex


    cpuid.basic_info.stepping_id = cpu_info[0] & 0xF;
    cpuid.basic_info.model = (cpu_info[0] >> 4) & 0xF;
    cpuid.basic_info.family = (cpu_info[0] >> 8) & 0xF;
    cpuid.basic_info.processor_type = (cpu_info[0] >> 12) & 0x3;
    cpuid.basic_info.extended_model = (cpu_info[0] >> 16) & 0xF;
    cpuid.basic_info.extended_family = (cpu_info[0] >> 20) & 0xFF;
    cpuid.basic_info.brand_index = cpu_info[1] & 0xFF;
    cpuid.basic_info.clflush_line_size = (cpu_info[1] >> 8) & 0xFF;
    cpuid.basic_info.max_logical_processors = (cpu_info[1] >> 16) & 0xFF;
    cpuid.basic_info.initial_apic_id = (cpu_info[1] >> 24) & 0xFF;
    cpuid.basic_info.sse3 = cpu_info[2] & 0x1;
    cpuid.basic_info.pclmulqdq = (cpu_info[2] >> 1) & 0x1;
    cpuid.basic_info.dtes64 = (cpu_info[2] >> 2) & 0x1;
    cpuid.basic_info.monitor = (cpu_info[2] >> 3) & 0x1;
    cpuid.basic_info.ds_cpl = (cpu_info[2] >> 4) & 0x1;
    cpuid.basic_info.vmx = (cpu_info[2] >> 5) & 0x1;  // Virtual Machine Extensions flag (VMX)
    cpuid.basic_info.smx = (cpu_info[2] >> 6) & 0x1;

    if (cpuid.basic_info.vmx)
        logger("VMX is supported.\n");
    else
        logger("VMX is NOT supported.\n");

    return cpuid.basic_info.vmx;
}

int enable_vmx(void)
{
    logger("Enabling VMX...\n");
    union __cr4_t cr4 = { 0 };
    union __ia32_feature_control_msr_t feature_msr = { 0 };

    cr4.control = __readcr4();
    cr4.bits.vmx_enable = 1;
    __writecr4(cr4.control);

    //IA32_FEATURE_CONTROL MSR (0x3A)
    feature_msr.control = __readmsr(0x3A);
    logger("Feature MSR lock bit: %d\n", feature_msr.bits.lock);

    if (feature_msr.bits.lock == 0) {
        feature_msr.bits.vmxon_outside_smx = 1;
        feature_msr.bits.lock = 1;
        __writemsr(0x3A, feature_msr.control);
        logger("VMX successfully enabled.\n");
        return 1;
    }
    else if (feature_msr.bits.vmxon_outside_smx == 1) { 
        logger("VMX is already enabled.\n");
        return 1;
    }

    logger("Failed to enable VMX.\n");

    return 0;
}


void disable_vmx(void)
{
    logger("Disabling VMX...\n");

    union __cr4_t cr4 = { 0 };

    cr4.control = __readcr4();
    cr4.bits.vmx_enable = 0; // Clear the VMX enable bit
    __writecr4(cr4.control);

    logger("VMX disabled.\n");
}


void adjust_control_registers(void)
{
    logger("Adjusting control registers...\n");
    union __cr4_t cr4 = { 0 };
    union __cr0_t cr0 = { 0 };
    union __cr_fixed_t cr_fixed;

    // Read and adjust CR0 register
    cr_fixed.all = __readmsr(IA32_VMX_CR0_FIXED0);
    cr0.control = __readcr0();
    cr0.control |= cr_fixed.split.low; // Set required bits
    cr_fixed.all = __readmsr(IA32_VMX_CR0_FIXED1);
    cr0.control &= cr_fixed.split.low; // Clear disallowed bits
    __writecr0(cr0.control);

    // Read and adjust CR4 register
    cr_fixed.all = __readmsr(IA32_VMX_CR4_FIXED0);
    cr4.control = __readcr4();
    cr4.control |= cr_fixed.split.low; // Set required bits
    cr_fixed.all = __readmsr(IA32_VMX_CR4_FIXED1);
    cr4.control &= cr_fixed.split.low; // Clear disallowed bits
    __writecr4(cr4.control);

    logger("Control registers adjusted.\n");
}


struct __vmm_context_t* allocate_vmm_context(void)
{
    struct __vmm_context_t* vmm_context = NULL;

    // Allocate memory for the VMM context
    vmm_context = (struct __vmm_context_t*)ExAllocatePoolWithTag(NonPagedPool, sizeof(struct __vmm_context_t), VMM_CONTEXT_TAG);
    if (vmm_context == NULL) {
        logger("VMM context allocation failed.\n");
        return NULL;
    }

    // Get the number of processors
    vmm_context->processor_count = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);

    // Allocate the vCPU table
    vmm_context->vcpu_table = ExAllocatePoolWithTag(NonPagedPool, sizeof(struct __vcpu_t*) * vmm_context->processor_count, VMM_CPU_TABLE_TAG);
    if (!vmm_context->vcpu_table) {
        logger("vCPU table allocation failed.\n");
        ExFreePoolWithTag(vmm_context, VMM_CONTEXT_TAG);
        return NULL;
    }

    // Allocate the stack for VM-exit handlers
    vmm_context->stack = ExAllocatePoolWithTag(NonPagedPool, VMM_STACK_SIZE, VMM_STACK_TAG);
    if (!vmm_context->stack) {
        logger("VMM stack allocation failed.\n");
        ExFreePoolWithTag(vmm_context->vcpu_table, VMM_CPU_TABLE_TAG);
        ExFreePoolWithTag(vmm_context, VMM_CONTEXT_TAG);
        return NULL;
    }

    // Fill stack with debugging pattern (0xCC)
    memset(vmm_context->stack, 0xCC, VMM_STACK_SIZE);

    return vmm_context;
}

void free_vmm_context(struct __vmm_context_t* vmm_context) {
    if (!vmm_context) {
        return;
    }

    // Free all vCPUs in the vCPU table
    if (vmm_context->vcpu_table) {
        for (unsigned int i = 0; i < vmm_context->processor_count; i++) {
            if (vmm_context->vcpu_table[i]) {
                free_vcpu(vmm_context->vcpu_table[i]);
            }
        }
        // Free the vCPU table itself
        ExFreePoolWithTag(vmm_context->vcpu_table, VMM_CPU_TABLE_TAG);
        logger("Freed vCPU table.\n");
    }

    // Free the stack allocated for VM-exit handlers
    if (vmm_context->stack) {
        ExFreePoolWithTag(vmm_context->stack, VMM_STACK_TAG);
        logger("Freed VMM stack.\n");
    }

    // Free the VMM context itself
    ExFreePoolWithTag(vmm_context, VMM_CONTEXT_TAG);
    logger("Freed VMM context.\n");
}


struct __vcpu_t* allocate_vcpu(void)
{
    struct __vcpu_t* vcpu = NULL;
    vcpu = ExAllocatePoolWithTag(NonPagedPool, sizeof(struct __vcpu_t), vCPU_TAG);
    if (!vcpu) {
        logger("Oops! vcpu could not be allocated.\n");
        return NULL;
    }
    RtlSecureZeroMemory(vcpu, sizeof(struct __vcpu_t));
    //
    // Zero out msr bitmap so that no traps occur on MSR accesses
    // when in guest operation.
    //
    vcpu->msr_bitmap = ExAllocatePoolWithTag(NonPagedPool, PAGE_SIZE, vCPU_TAG);
    RtlSecureZeroMemory(vcpu->msr_bitmap, PAGE_SIZE);
    vcpu->msr_bitmap_physical = MmGetPhysicalAddress(vcpu->msr_bitmap).QuadPart;
    logger("vcpu entry allocated successfully at %llX\n", vcpu);
    return vcpu;
}

void free_vcpu(struct __vcpu_t* vcpu) {
    if (vcpu) {
        // Free VMXON region
        free_vmxon_region(vcpu);

        // Free VMCS region
        free_vmcs_region(vcpu);

        // Free the vCPU structure itself
        ExFreePoolWithTag(vcpu, vCPU_TAG);
        logger("Freed vCPU structure.\n");
    }
}


int allocate_vmxon_region(struct __vcpu_t* vcpu) {
    union __vmx_basic_msr_t vmx_basic = { 0 };
    PHYSICAL_ADDRESS max_physical_address;

    // Get VMX Basic MSR
    vmx_basic.control = __readmsr(IA32_VMX_BASIC);
    max_physical_address.QuadPart = ~0ULL;

    // Allocate VMXON region
    vcpu->vmxon = MmAllocateContiguousMemory(PAGE_SIZE, max_physical_address);
    if (!vcpu->vmxon) {
        logger("Failed to allocate VMXON region.\n");
        return FALSE;
    }

    vcpu->vmxon_physical = MmGetPhysicalAddress(vcpu->vmxon).QuadPart;
    RtlSecureZeroMemory(vcpu->vmxon, PAGE_SIZE);

    // Set the VMXON region revision identifier
    ((struct __vmcs_t*)vcpu->vmxon)->header.all = vmx_basic.bits.vmcs_revision_identifier;

    logger("VMXON region allocated at VA: %llX, PA: %llX\n", vcpu->vmxon, vcpu->vmxon_physical);
    return TRUE;
}

void free_vmxon_region(struct __vcpu_t* vcpu) {
    if (vcpu && vcpu->vmxon) {
        MmFreeContiguousMemory(vcpu->vmxon);
        vcpu->vmxon = NULL;
        logger("Freed VMXON region for vCPU.\n");
    }
}

int allocate_vmcs_region(struct __vcpu_t* vcpu) {
    struct __vmcs_t* vmcs;
    union __vmx_basic_msr_t vmx_basic = { 0 };
    PHYSICAL_ADDRESS physical_max;

    vmx_basic.control = __readmsr(IA32_VMX_BASIC);
    physical_max.QuadPart = ~0ULL;

    // Allocate VMCS region
    vcpu->vmcs = MmAllocateContiguousMemory(PAGE_SIZE, physical_max);
    vcpu->vmcs_physical = MmGetPhysicalAddress(vcpu->vmcs).QuadPart;

    RtlSecureZeroMemory(vcpu->vmcs, PAGE_SIZE);

    // Set VMCS revision identifier
    vmcs = vcpu->vmcs;
    vmcs->header.all = vmx_basic.bits.vmcs_revision_identifier;
    vmcs->header.bits.shadow_vmcs_indicator = 0;

    logger("VMCS region allocated at VA: %llX, PA: %llX\n", vcpu->vmcs, vcpu->vmcs_physical);

    // Clear VMCS
    if (__vmx_vmclear(&vcpu->vmcs_physical) != 0) {
        logger("Failed to clear VMCS for vcpu.\n");
        return FALSE;
    }

    // Load VMCS
    if (__vmx_vmptrld(&vcpu->vmcs_physical) != 0) {
        logger("Failed to load VMCS pointer for vcpu.\n");
        return FALSE;
    }

    logger("VMCS region cleared and loaded successfully for vcpu.\n");    
    return TRUE;
}

void free_vmcs_region(struct __vcpu_t* vcpu) {
    if (vcpu && vcpu->vmcs) {
        MmFreeContiguousMemory(vcpu->vmcs);
        vcpu->vmcs = NULL;
        logger("Freed VMCS region for vCPU.\n");
    }
}


void init_logical_processor(struct __vmm_context_t* context, void* guest_rsp)
{
    struct __vmm_context_t* vmm_context;
    struct __vcpu_t* vcpu;
    unsigned long processor_number;

    processor_number = KeGetCurrentProcessorNumber();

    vmm_context = (struct __vmm_context_t*)context;
    vcpu = vmm_context->vcpu_table[processor_number];

    logger("Initializing logical processor %lu with guest_rsp: %llX\n", processor_number, guest_rsp);

    adjust_control_registers();

    if (!is_vmx_supported()) {
        logger("VMX operation is not supported on this processor.\n");
        free_vmm_context(vmm_context);
        return;
    }

    if (!allocate_vmxon_region(vcpu)) {
        logger("VMXON failed to initialize for vcpu %d.\n", processor_number);
        free_vcpu(vcpu);
        disable_vmx();
        return;
    }

    if (__vmx_on(&vcpu->vmxon_physical) != 0) {
        logger("Failed to put vcpu %d into VMX operation.\n", processor_number);
        free_vcpu(vcpu);
        disable_vmx();
        free_vmm_context(vmm_context);
        return;
    }

    logger("vcpu %d is now in VMX operation.\n", processor_number);
}

int vmm_init(void)
{
    struct __vmm_context_t* vmm_context;
    PROCESSOR_NUMBER processor_number;
    GROUP_AFFINITY affinity, old_affinity;
    KIRQL old_irql;
    vmm_context = allocate_vmm_context();

    for (unsigned iter = 0; iter < vmm_context->processor_count; iter++) {
        vmm_context->vcpu_table[iter] = allocate_vcpu();
        vmm_context->vcpu_table[iter]->vmm_context = vmm_context;
    }

    for (unsigned iter = 0; iter < vmm_context->processor_count; iter++) {
        KeGetProcessorNumberFromIndex(iter, &processor_number);
        RtlSecureZeroMemory(&affinity, sizeof(GROUP_AFFINITY));
        affinity.Group = processor_number.Group;
        affinity.Mask = (KAFFINITY)1 << processor_number.Number;
        KeSetSystemGroupAffinityThread(&affinity, &old_affinity);
        init_logical_processor(vmm_context, 0);
        KeRevertToUserGroupAffinityThread(&old_affinity);

    }
    return TRUE;
}







