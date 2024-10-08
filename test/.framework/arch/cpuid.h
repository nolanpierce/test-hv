#ifndef CPU_ID_DEFS_H
#define CPU_ID_DEFS_H

#pragma once
#include "types.h"

typedef struct {
    u32 eax;
    u32 ebx;
    u32 ecx;
    u32 edx;
} cpuid_leaf_t;

// CPUID [0.0] Vendor Information
typedef struct {
    u32 max_input_value;
    char vendor_id[13]; // "GenuineIntel" or similar, null-terminated
} cpuid_vendor_t;

// CPUID [1.0] Processor Info and Feature Bits
typedef struct {
    u8 stepping_id;
    u8 model;
    u8 family;
    u8 processor_type;
    u8 extended_model;
    u8 extended_family;

    u8 brand_index;
    u8 clflush_line_size;
    u8 max_logical_processors;
    u8 initial_apic_id;

    // Feature flags in ECX
    u32 sse3 : 1;
    u32 pclmulqdq : 1;
    u32 dtes64 : 1;
    u32 monitor : 1;
    u32 ds_cpl : 1;
    u32 vmx : 1;
    u32 smx : 1;
    u32 eist : 1;
    u32 tm2 : 1;
    u32 ssse3 : 1;
    u32 cnxt_id : 1;
    u32 sdbg : 1;
    u32 fma : 1;
    u32 cmpxchg16b : 1;
    u32 xtpr_update_control : 1;
    u32 pdcm : 1;
    u32 pcid : 1;
    u32 dca : 1;
    u32 sse4_1 : 1;
    u32 sse4_2 : 1;
    u32 x2apic : 1;
    u32 movbe : 1;
    u32 popcnt : 1;
    u32 tsc_deadline : 1;
    u32 aesni : 1;
    u32 xsave : 1;
    u32 osxsave : 1;
    u32 avx : 1;
    u32 f16c : 1;
    u32 rdrand : 1;

    // Feature flags in EDX
    u32 fpu : 1;
    u32 vme : 1;
    u32 de : 1;
    u32 pse : 1;
    u32 tsc : 1;
    u32 msr : 1;
    u32 pae : 1;
    u32 mce : 1;
    u32 cx8 : 1;
    u32 apic : 1;
    u32 sep : 1;
    u32 mtrr : 1;
    u32 pge : 1;
    u32 mca : 1;
    u32 cmov : 1;
    u32 pat : 1;
    u32 pse_36 : 1;
    u32 psn : 1;
    u32 clflush : 1;
    u32 ds : 1;
    u32 acpi : 1;
    u32 mmx : 1;
    u32 fxsr : 1;
    u32 sse : 1;
    u32 sse2 : 1;
    u32 ss : 1;
    u32 htt : 1;
    u32 tm : 1;
    u32 ia64 : 1;
    u32 pbe : 1;
} cpuid_info_t;

// CPUID [7.0] Structured Extended Feature Flags
typedef struct {
    u32 fsgsbase : 1;
    u32 tsc_adjust : 1;
    u32 sgx : 1;
    u32 bmi1 : 1;
    u32 hle : 1;
    u32 avx2 : 1;
    u32 smep : 1;
    u32 bmi2 : 1;
    u32 invpcid : 1;
    u32 rtm : 1;
    u32 avx512f : 1;
    u32 sha : 1;
    u32 rdseed : 1;
    u32 adx : 1;
    u32 smap : 1;
    u32 clflushopt : 1;
    u32 clwb : 1;
    u32 pt : 1;
    u32 avx512cd : 1;
    u32 avx512bw : 1;
    u32 avx512dq : 1;
    u32 avx512vl : 1;
    u32 prefetchwt1 : 1;
    u32 umip : 1;
    u32 pku : 1;
    u32 ospke : 1;
    u32 rdpid : 1;
    u32 kl : 1;
    u32 la57 : 1;
    u32 rdt_a : 1;
    u32 ibpb : 1;
    u32 ibrs : 1;
} cpuid_ext_features_t;

// Combined structure
typedef struct {
    cpuid_vendor_t vendor_info;
    cpuid_info_t basic_info;
    cpuid_ext_features_t extended_features;
    cpuid_leaf_t other_leaves[10]; // Add more CPUID leaves as necessary
} __cpuid_t;


#endif // !CPU_ID_DEFS_H
