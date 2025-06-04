#include "svm.h"

int is_svm_supported(void)
{
    int cpu_info[4];
    __cpuid(cpu_info, 0x80000001);
    return (cpu_info[2] & (1 << 2)) != 0; // ECX bit 2 indicates SVM support
}

int enable_svm(void)
{
    union __ia32_efer_t efer;
    efer.control = __readmsr(MSR_EFER);
    if (efer.control & EFER_SVME)
    {
        logger("SVM already enabled.\n");
        return 1;
    }

    efer.control |= EFER_SVME;
    __writemsr(MSR_EFER, efer.control);
    logger("SVM enabled.\n");
    return 1;
}

void disable_svm(void)
{
    union __ia32_efer_t efer;
    efer.control = __readmsr(MSR_EFER);
    efer.control &= ~EFER_SVME;
    __writemsr(MSR_EFER, efer.control);
    logger("SVM disabled.\n");
}

