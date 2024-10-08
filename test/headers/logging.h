#ifndef LOGGER_GUARD_H
#define LOGGER_GUARD_H
#include <ntddk.h>
#include <stdarg.h>

void logger(const char* fmt, ...);

#endif // !LOGGER_GUARD_H
