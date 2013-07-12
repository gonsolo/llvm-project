; RUN: llc < %s -march=r600 -mcpu=tahiti | FileCheck %s

; CHECK: @fmul_f64
; CHECK: V_MUL_F64 {{VGPR[0-9]+_VGPR[0-9]+, VGPR[0-9]+_VGPR[0-9]+, VGPR[0-9]+_VGPR[0-9]+}}

define void @fmul_f64(double addrspace(1)* %out, double addrspace(1)* %in1,
                      double addrspace(1)* %in2) {
   %r0 = load double addrspace(1)* %in1
   %r1 = load double addrspace(1)* %in2
   %r2 = fmul double %r0, %r1
   store double %r2, double addrspace(1)* %out
   ret void
}
