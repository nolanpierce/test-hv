#include "logging.h"

void logger(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vDbgPrintExWithPrefix("[nucleus] - ", DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, fmt, args);
	va_end(args);
}