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


BOOLEAN vmexit_handler(struct __vmexit_guest_registers_t* guest_regs)
{
    DbgBreakPointWithStatus(STATUS_BREAKPOINT);

    return TRUE; // Continue execution
}


