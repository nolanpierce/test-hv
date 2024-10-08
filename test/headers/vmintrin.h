#pragma once
#include "types.h"

// Functions to read various segment selectors and descriptors
extern u64 __load_ar(u16 segment_selector);   // Load Access Rights (AR) for a given segment selector
extern u16 __read_ldtr(void);                 // Read Local Descriptor Table Register (LDTR)
extern u16 __read_tr(void);                   // Read Task Register (TR)
extern u16 __read_cs(void);                   // Read Code Segment (CS)
extern u16 __read_ss(void);                   // Read Stack Segment (SS)
extern u16 __read_ds(void);                   // Read Data Segment (DS)
extern u16 __read_es(void);                   // Read Extra Segment (ES)
extern u16 __read_fs(void);                   // Read FS Segment
extern u16 __read_gs(void);                   // Read GS Segment
extern u64 __read_rip(void);                  // Read Instruction Pointer (RIP)
extern u64 __read_rsp(void);                  // Read Stack Pointer (RSP)
