#ifndef VMX_DEFINITIONS_H
#define VMX_DEFINITIONS_H
#define selector_mask 0xFFF8

struct __vmcs_t
{
    union
    {
        unsigned int all;
        struct
        {
            unsigned int revision_identifier : 31;
            unsigned int shadow_vmcs_indicator : 1;
        } bits;
    } header;
    unsigned int abort_indicator;
    char data[0x1000 - 2 * sizeof(unsigned)];
};

struct __vcpu_t
{
    struct __vmm_context_t* vmm_context;  // Pointer to the VMM context
    struct __vmcs_t* vmcs;
    unsigned __int64 vmcs_physical;
    struct __vmcs_t* vmxon;
    unsigned __int64 vmxon_physical;
    void* msr_bitmap;
    unsigned __int64 msr_bitmap_physical;
    unsigned __int64 guest_rsp;   // Add guest_rsp to store guest stack pointer
    unsigned __int64 guest_rip;   // Add guest_rip to store guest instruction pointer
};

struct __vmm_context_t {
    void* vmm_stack;               // Pointer to the VMM stack (host stack used for VM exit handling)
    struct __vcpu_t** vcpu_table;         // Table of vCPU
    u32 processor_count;           // Number of logical processors (from KeQueryActiveProcessorCountEx)
    void* stack;                   // Stack for VM-exit handlers or other VMM operations
};

#endif // !VMX_DEFINITIONS_H
