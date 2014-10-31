//===-- RegisterInfos_powerpc.h ---------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//

#include <stddef.h>

// Computes the offset of the given GPR in the user data area.
#define GPR_OFFSET(regname)                                                 \
    (offsetof(GPR, regname))
#define FPR_OFFSET(regname)                                                 \
    (offsetof(FPR, regname))

#ifdef DECLARE_REGISTER_INFOS_POWERPC_STRUCT

// Note that the size and offset will be updated by platform-specific classes.
#define DEFINE_GPR(reg, alt, lldb_kind)           \
    { #reg, alt, sizeof(((GPR*)NULL)->reg), GPR_OFFSET(reg), eEncodingUint, \
      eFormatHex, { gcc_dwarf_##reg##_powerpc, gcc_dwarf_##reg##_powerpc, lldb_kind, gdb_##reg##_powerpc, gpr_##reg##_powerpc }, NULL, NULL }
#define DEFINE_FPR(reg, lldb_kind)           \
    { #reg, NULL, 8, FPR_OFFSET(reg), eEncodingIEEE754, \
      eFormatFloat, { gcc_dwarf_##reg##_powerpc, gcc_dwarf_##reg##_powerpc, lldb_kind, gdb_##reg##_powerpc, fpr_##reg##_powerpc }, NULL, NULL }

    // General purpose registers.                 GCC,                  DWARF,              Generic,                GDB
#define POWERPC_REGS \
    DEFINE_GPR(r0,       NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r1,       "sp",  LLDB_REGNUM_GENERIC_SP), \
    DEFINE_GPR(r2,       NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r3,       "arg1",LLDB_REGNUM_GENERIC_ARG1), \
    DEFINE_GPR(r4,       "arg2",LLDB_REGNUM_GENERIC_ARG2), \
    DEFINE_GPR(r5,       "arg3",LLDB_REGNUM_GENERIC_ARG3), \
    DEFINE_GPR(r6,       "arg4",LLDB_REGNUM_GENERIC_ARG4), \
    DEFINE_GPR(r7,       "arg5",LLDB_REGNUM_GENERIC_ARG5), \
    DEFINE_GPR(r8,       "arg6",LLDB_REGNUM_GENERIC_ARG6), \
    DEFINE_GPR(r9,       "arg7",LLDB_REGNUM_GENERIC_ARG7), \
    DEFINE_GPR(r10,      "arg8",LLDB_REGNUM_GENERIC_ARG8), \
    DEFINE_GPR(r11,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r12,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r13,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r14,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r15,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r16,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r17,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r18,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r19,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r20,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r21,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r22,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r23,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r24,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r25,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r26,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r27,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r28,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r29,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r30,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(r31,      NULL,  LLDB_INVALID_REGNUM), \
    DEFINE_GPR(lr,       "lr",  LLDB_REGNUM_GENERIC_RA), \
    DEFINE_GPR(cr,       "cr",  LLDB_REGNUM_GENERIC_FLAGS), \
    DEFINE_GPR(xer,      "xer", LLDB_INVALID_REGNUM), \
    DEFINE_GPR(ctr,      "ctr", LLDB_INVALID_REGNUM), \
    DEFINE_GPR(pc,       "pc",  LLDB_REGNUM_GENERIC_PC), \
    DEFINE_FPR(f0,       LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f1,       LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f2,       LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f3,       LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f4,       LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f5,       LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f6,       LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f7,       LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f8,       LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f9,       LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f10,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f11,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f12,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f13,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f14,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f15,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f16,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f17,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f18,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f19,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f20,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f21,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f22,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f23,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f24,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f25,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f26,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f27,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f28,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f29,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f30,      LLDB_INVALID_REGNUM), \
    DEFINE_FPR(f31,      LLDB_INVALID_REGNUM), \
    { "fpscr", NULL, 8, FPR_OFFSET(fpscr), eEncodingUint, eFormatHex, { gcc_dwarf_fpscr_powerpc, gcc_dwarf_fpscr_powerpc, LLDB_INVALID_REGNUM, gdb_fpscr_powerpc, fpr_fpscr_powerpc }, NULL, NULL },
    //{ NULL, NULL, sizeof(((GPR*)NULL)->r0), 0, LLDB_INVALID_REGNUM, LLDB_INVALID_REGNUM, LLDB_INVALID_REGNUM, LLDB_INVALID_REGNUM, gpr_cfa_powerpc}, NULL, NULL}
static RegisterInfo
g_register_infos_powerpc64[] =
{
#define GPR GPR64
    POWERPC_REGS
#undef GPR
};

static RegisterInfo
g_register_infos_powerpc32[] =
{
#define GPR GPR32
    POWERPC_REGS
#undef GPR
};
static_assert((sizeof(g_register_infos_powerpc32) / sizeof(g_register_infos_powerpc32[0])) == k_num_registers_powerpc,
    "g_register_infos_powerpc32 has wrong number of register infos");
static_assert((sizeof(g_register_infos_powerpc64) / sizeof(g_register_infos_powerpc64[0])) == k_num_registers_powerpc,
    "g_register_infos_powerpc64 has wrong number of register infos");

#undef DEFINE_FPR
#undef DEFINE_GPR

#endif // DECLARE_REGISTER_INFOS_POWERPC_STRUCT

#undef GPR_OFFSET

