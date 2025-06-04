#ifndef SVM_FUNC_GUARD_H
#define SVM_FUNC_GUARD_H
#include <ntddk.h>
#include <intrin.h>
#include "types.h"
#include "logging.h"

#define AMD_VENDOR_ID "AuthenticAMD"
#define MSR_EFER 0xC0000080
#define EFER_SVME (1ULL << 12)

int is_svm_supported(void);
int enable_svm(void);
void disable_svm(void);

#endif // !SVM_FUNC_GUARD_H
