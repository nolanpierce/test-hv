#ifndef VMX_FUNC_GUARD_H
#define VMX_FUNC_GUARD_H
#include <ntddk.h>
#include <intrin.h>
#include "types.h"
#include "logging.h"
#include "arch/cpuid.h"
#include "arch/cr.h"
#include "arch/msr.h"
#include "arch/vmx.h"
#include "arch/segments.h"
#include "arch/vmcs_types.h"
#include "vmintrin.h"

#define vCPU_TAG 'vCpT'
#define VMM_CONTEXT_TAG 'vMtx'
#define VMM_CPU_TABLE_TAG 'cptb'
#define VMM_STACK_TAG 'MsTc'
#define VMM_STACK_SIZE 0x2000  // 8 KB stack size
#define IA32_VMX_CR0_FIXED0  0x486
#define IA32_VMX_CR0_FIXED1  0x487
#define IA32_VMX_CR4_FIXED0  0x488
#define IA32_VMX_CR4_FIXED1  0x489
#define IA32_VMX_BASIC       0x480
// Model-Specific Register (MSR) indices
#define IA32_DEBUGCTL       0x1D9
#define IA32_SYSENTER_CS    0x174
#define IA32_SYSENTER_ESP   0x175
#define IA32_SYSENTER_EIP   0x176
#define IA32_FS_BASE        0xC0000100
#define IA32_GS_BASE        0xC0000101
// Host State Area Encodings
//#define HOST_CR0        VMCS_ENCODE_COMPONENT_FULL(host, natural, 0)
//#define HOST_CR3        VMCS_ENCODE_COMPONENT_FULL(host, natural, 1)
//#define HOST_CR4        VMCS_ENCODE_COMPONENT_FULL(host, natural, 2)
//#define HOST_RSP        VMCS_ENCODE_COMPONENT_FULL(host, natural, 3)
//#define HOST_RIP        VMCS_ENCODE_COMPONENT_FULL(host, natural, 4)
//#define HOST_FS_BASE    VMCS_ENCODE_COMPONENT_FULL(host, natural, 6)
//#define HOST_GS_BASE    VMCS_ENCODE_COMPONENT_FULL(host, natural, 7)
//#define HOST_SYSENTER_ESP  VMCS_ENCODE_COMPONENT_FULL(host, natural, 8)
//#define HOST_SYSENTER_EIP  VMCS_ENCODE_COMPONENT_FULL(host, natural, 9)
//#define HOST_SYSENTER_CS   VMCS_ENCODE_COMPONENT_FULL(host, doubleword, 10)


// VMCS Link Pointer Encoding
//#define VMCS_LINK_POINTER           0x2800
// Guest-State Area Encodings
//#define GUEST_CR0                   0x6800
//#define GUEST_CR3                   0x6802
//#define GUEST_CR4                   0x6804
//#define GUEST_RSP                   0x681C
//#define GUEST_RIP                   0x681E
//#define GUEST_RFLAGS                0x6820
//
//// Host-State Area Encodings
//#define HOST_CR0                    0x6C00
//#define HOST_CR3                    0x6C02
//#define HOST_CR4                    0x6C04
//#define HOST_RSP                    0x6C14
//#define HOST_RIP                    0x6C16
//
//// VM Execution Control Encodings
//#define PIN_BASED_VM_EXEC_CONTROL   0x4000
//#define CPU_BASED_VM_EXEC_CONTROL   0x4002
//#define VM_EXIT_CONTROLS            0x400C
//#define VM_ENTRY_CONTROLS           0x4012

extern void vmm_entrypoint(void); // Declare the VM exit handler entry point
extern int						is_vmx_supported(void);
extern int						enable_vmx(void);
extern void						disable_vmx(void);

extern void						adjust_control_registers(void);
extern struct __vmm_context_t*	allocate_vmm_context(void);
extern void						free_vmm_context(struct __vmm_context_t* vmm_context);

extern struct __vcpu_t*			allocate_vcpu(void);
extern void						free_vcpu(struct __vcpu_t* vcpu);

extern int						allocate_vmxon_region(struct __vcpu_t* vcpu);
extern void						free_vmxon_region(struct __vcpu_t* vcpu);

extern int						allocate_vmcs_region(struct __vcpu_t* vcpu);
extern void						free_vmcs_region(struct __vcpu_t* vcpu);
extern int						initialize_vmcs(struct __vcpu_t* vcpu, void* guest_rip, void* guest_rsp);

extern void						init_logical_processor(struct __vmm_context_t* context, void* guest_rsp);
extern int						vmm_init(void);
#endif // !VMX_FUNC_GUARD_H

