; RUN: llc -asm-verbose=false < %s -mattr=+vfp3,+fp16 | FileCheck %s -check-prefix=CHECK-FP16 -check-prefix=CHECK-ALL
; RUN: llc -asm-verbose=false < %s | FileCheck %s -check-prefix=CHECK-LIBCALL -check-prefix=CHECK-ALL

target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-n32"
target triple = "armv7-eabihf"

; CHECK-FP16-LABEL: test_fadd:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: ldrh r2, [r0]
; CHECK-FP16-NEXT: ldrh r1, [r1]
; CHECK-FP16-NEXT: vmov s0, r1
; CHECK-FP16-NEXT: vmov s2, r2
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s2, s2
; CHECK-FP16-NEXT: vadd.f32 s0, s2, s0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r1, s0
; CHECK-FP16-NEXT: strh r1, [r0]
; CHECK-FP16-NEXT: bx lr
; CHECK-LIBCALL-LABEL: test_fadd:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: vadd.f32
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_fadd(half* %p, half* %q) #0 {
  %a = load half, half* %p, align 2
  %b = load half, half* %q, align 2
  %r = fadd half %a, %b
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_fsub:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: ldrh r2, [r0]
; CHECK-FP16-NEXT: ldrh r1, [r1]
; CHECK-FP16-NEXT: vmov s0, r1
; CHECK-FP16-NEXT: vmov s2, r2
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s2, s2
; CHECK-FP16-NEXT: vsub.f32 s0, s2, s0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r1, s0
; CHECK-FP16-NEXT: strh r1, [r0]
; CHECK-FP16-NEXT: bx lr
; CHECK-LIBCALL-LABEL: test_fsub:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: vsub.f32
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_fsub(half* %p, half* %q) #0 {
  %a = load half, half* %p, align 2
  %b = load half, half* %q, align 2
  %r = fsub half %a, %b
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_fmul:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: ldrh r2, [r0]
; CHECK-FP16-NEXT: ldrh r1, [r1]
; CHECK-FP16-NEXT: vmov s0, r1
; CHECK-FP16-NEXT: vmov s2, r2
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s2, s2
; CHECK-FP16-NEXT: vmul.f32 s0, s2, s0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r1, s0
; CHECK-FP16-NEXT: strh r1, [r0]
; CHECK-FP16-NEXT: bx lr
; CHECK-LIBCALL-LABEL: test_fmul
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: vmul.f32
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_fmul(half* %p, half* %q) #0 {
  %a = load half, half* %p, align 2
  %b = load half, half* %q, align 2
  %r = fmul half %a, %b
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_fdiv:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: ldrh r2, [r0]
; CHECK-FP16-NEXT: ldrh r1, [r1]
; CHECK-FP16-NEXT: vmov s0, r1
; CHECK-FP16-NEXT: vmov s2, r2
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s2, s2
; CHECK-FP16-NEXT: vdiv.f32 s0, s2, s0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r1, s0
; CHECK-FP16-NEXT: strh r1, [r0]
; CHECK-FP16-NEXT: bx lr
; CHECK-LIBCALL-LABEL: test_fdiv
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: vdiv.f32
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_fdiv(half* %p, half* %q) #0 {
  %a = load half, half* %p, align 2
  %b = load half, half* %q, align 2
  %r = fdiv half %a, %b
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_frem:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r1]
; CHECK-FP16-NEXT: ldrh r1, [r4]
; CHECK-FP16-NEXT: vmov s2, r0
; CHECK-FP16-NEXT: vmov s0, r1
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s2, s2
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: vmov r1, s2
; CHECK-FP16-NEXT: bl fmodf
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_frem
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl fmodf
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_frem(half* %p, half* %q) #0 {
  %a = load half, half* %p, align 2
  %b = load half, half* %q, align 2
  %r = frem half %a, %b
  store half %r, half* %p
  ret void
}

; CHECK-ALL-LABEL: test_load_store:
; CHECK-ALL-NEXT: .fnstart
; CHECK-ALL-NEXT: ldrh r0, [r0]
; CHECK-ALL-NEXT: strh r0, [r1]
; CHECK-ALL-NEXT: bx lr
define void @test_load_store(half* %p, half* %q) #0 {
  %a = load half, half* %p, align 2
  store half %a, half* %q
  ret void
}

; Testing only successfull compilation of function calls.  In ARM ABI, half
; args and returns are handled as f32.

declare half @test_callee(half %a, half %b) #0

; CHECK-ALL-LABEL: test_call:
; CHECK-ALL-NEXT: .fnstart
; CHECK-ALL-NEXT: push {r11, lr}
; CHECK-ALL-NEXT: bl test_callee
; CHECK-ALL-NEXT: pop {r11, pc}
define half @test_call(half %a, half %b) #0 {
  %r = call half @test_callee(half %a, half %b)
  ret half %r
}

; CHECK-ALL-LABEL: test_call_flipped:
; CHECK-ALL-NEXT: .fnstart
; CHECK-ALL-NEXT: push {r11, lr}
; CHECK-ALL-NEXT: mov r2, r0
; CHECK-ALL-NEXT: mov r0, r1
; CHECK-ALL-NEXT: mov r1, r2
; CHECK-ALL-NEXT: bl test_callee
; CHECK-ALL-NEXT: pop {r11, pc}
define half @test_call_flipped(half %a, half %b) #0 {
  %r = call half @test_callee(half %b, half %a)
  ret half %r
}

; CHECK-ALL-LABEL: test_tailcall_flipped:
; CHECK-ALL-NEXT: .fnstart
; CHECK-ALL-NEXT: mov r2, r0
; CHECK-ALL-NEXT: mov r0, r1
; CHECK-ALL-NEXT: mov r1, r2
; CHECK-ALL-NEXT: b test_callee
define half @test_tailcall_flipped(half %a, half %b) #0 {
  %r = tail call half @test_callee(half %b, half %a)
  ret half %r
}

; Optimizer picks %p or %q based on %c and only loads that value
; No conversion is needed
; CHECK-BOTH-LABEL: test_select:
; CHECK-BOTH-NEXT: .fnstart
; CHECK-BOTH-NEXT: cmp r2, #0
; CHECK-BOTH-NEXT: movne r1, r0
; CHECK-BOTH-NEXT: ldrh r1, [r1]
; CHECK-BOTH-NEXT: strh r1, [r0]
; CHECK-BOTH-NEXT: bx lr
define void @test_select(half* %p, half* %q, i1 zeroext %c) #0 {
  %a = load half, half* %p, align 2
  %b = load half, half* %q, align 2
  %r = select i1 %c, half %a, half %b
  store half %r, half* %p
  ret void
}

; Test only two variants of fcmp.  These get translated to f32 vcmpe
; instructions anyway.
; CHECK-FP16-LABEL: test_fcmp_une:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: ldrh r2, [r0]
; CHECK-FP16-NEXT: ldrh r0, [r1]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vmov s2, r2
; CHECK-FP16-NEXT: mov r0, #0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s2, s2
; CHECK-FP16-NEXT: vcmpe.f32 s2, s0
; CHECK-FP16-NEXT: vmrs APSR_nzcv, fpscr
; CHECK-FP16-NEXT: movwne r0, #1
; CHECK-FP16-NEXT: bx lr
; CHECK-LIBCALL-LABEL: test_fcmp_une:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: vcmpe.f32
; CHECK-LIBCALL: movwne
define i1 @test_fcmp_une(half* %p, half* %q) #0 {
  %a = load half, half* %p, align 2
  %b = load half, half* %q, align 2
  %r = fcmp une half %a, %b
  ret i1 %r
}

; CHECK-FP16-LABEL: test_fcmp_ueq:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: ldrh r2, [r0]
; CHECK-FP16-NEXT: ldrh r0, [r1]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vmov s2, r2
; CHECK-FP16-NEXT: mov r0, #0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s2, s2
; CHECK-FP16-NEXT: vcmpe.f32 s2, s0
; CHECK-FP16-NEXT: vmrs APSR_nzcv, fpscr
; CHECK-FP16-NEXT: movweq r0, #1
; CHECK-FP16-NEXT: movwvs r0, #1
; CHECK-FP16-NEXT: bx lr
; CHECK-LIBCALL-LABEL: test_fcmp_ueq:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: vcmpe.f32
; CHECK-LIBCALL: movweq
define i1 @test_fcmp_ueq(half* %p, half* %q) #0 {
  %a = load half, half* %p, align 2
  %b = load half, half* %q, align 2
  %r = fcmp ueq half %a, %b
  ret i1 %r
}

; CHECK-FP16-LABEL: test_br_cc:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: ldrh r0, [r0]
; CHECK-FP16-NEXT: ldrh r1, [r1]
; CHECK-FP16-NEXT: vmov s0, r1
; CHECK-FP16-NEXT: vmov s2, r0
; CHECK-FP16-NEXT: mov r0, #0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s2, s2
; CHECK-FP16-NEXT: vcmpe.f32 s2, s0
; CHECK-FP16-NEXT: vmrs APSR_nzcv, fpscr
; CHECK-FP16-NEXT: strmi r0, [r3]
; CHECK-FP16-NEXT: strpl r0, [r2]
; CHECK-FP16-NEXT: bx lr
; CHECK-LIBCALL-LABEL: test_br_cc:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: vcmpe.f32
; CHECK-LIBCALL: strmi
; CHECK-LIBCALL: strpl
define void @test_br_cc(half* %p, half* %q, i32* %p1, i32* %p2) #0 {
  %a = load half, half* %p, align 2
  %b = load half, half* %q, align 2
  %c = fcmp uge half %a, %b
  br i1 %c, label %then, label %else
then:
  store i32 0, i32* %p1
  ret void
else:
  store i32 0, i32* %p2
  ret void
}

declare i1 @test_dummy(half* %p) #0
; CHECK-FP16-LABEL: test_phi:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push    {r4, lr}
; CHECK-FP16-NEXT: vpush   {d8, d9}
; CHECK-FP16-NEXT: mov     r4, r0
; CHECK-FP16-NEXT: ldrh    r0, [r4]
; CHECK-FP16-NEXT: vmov    s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16   s18, s0
; CHECK-FP16-NEXT: [[LOOP:.LBB[1-9_]+]]:
; CHECK-FP16-NEXT: ldrh    r0, [r4]
; CHECK-FP16-NEXT: vmov.f32        s16, s18
; CHECK-FP16-NEXT: vmov    s0, r0
; CHECK-FP16-NEXT: mov     r0, r4
; CHECK-FP16-NEXT: vcvtb.f32.f16   s18, s0
; CHECK-FP16-NEXT: bl      test_dummy
; CHECK-FP16-NEXT: tst     r0, #1
; CHECK-FP16-NEXT: bne     [[LOOP]]
; CHECK-FP16-NEXT: vcvtb.f16.f32   s0, s16
; CHECK-FP16-NEXT: vmov    r0, s0
; CHECK-FP16-NEXT: strh    r0, [r4]
; CHECK-LIBCALL-LABEL: test_phi:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: [[LOOP:.LBB[1-9_]+]]:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl test_dummy
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_phi(half* %p) #0 {
entry:
  %a = load half, half* %p
  br label %loop
loop:
  %r = phi half [%a, %entry], [%b, %loop]
  %b = load half, half* %p
  %c = call i1 @test_dummy(half* %p)
  br i1 %c, label %loop, label %return
return:
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_fptosi_i32:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: ldrh r0, [r0]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vcvt.s32.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: bx
; CHECK-LIBCALL-LABEL: test_fptosi_i32:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: vcvt.s32.f32
define i32 @test_fptosi_i32(half* %p) #0 {
  %a = load half, half* %p, align 2
  %r = fptosi half %a to i32
  ret i32 %r
}

; CHECK-FP16-LABEL: test_fptosi_i64:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r11, lr}
; CHECK-FP16-NEXT: ldrh r0, [r0]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: __aeabi_f2lz
; CHECK-FP16-NEXT: pop {r11, pc}
; CHECK-LIBCALL-LABEL: test_fptosi_i64:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __aeabi_f2lz
define i64 @test_fptosi_i64(half* %p) #0 {
  %a = load half, half* %p, align 2
  %r = fptosi half %a to i64
  ret i64 %r
}

; CHECK-FP16-LABEL: test_fptoui_i32:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: ldrh r0, [r0]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vcvt.u32.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: bx
; CHECK-LIBCALL-LABEL: test_fptoui_i32:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: vcvt.u32.f32
define i32 @test_fptoui_i32(half* %p) #0 {
  %a = load half, half* %p, align 2
  %r = fptoui half %a to i32
  ret i32 %r
}

; CHECK-FP16-LABEL: test_fptoui_i64:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r11, lr}
; CHECK-FP16-NEXT: ldrh r0, [r0]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: __aeabi_f2ulz
; CHECK-FP16-NEXT: pop {r11, pc}
; CHECK-LIBCALL-LABEL: test_fptoui_i64:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __aeabi_f2ulz
define i64 @test_fptoui_i64(half* %p) #0 {
  %a = load half, half* %p, align 2
  %r = fptoui half %a to i64
  ret i64 %r
}

; CHECK-FP16-LABEL: test_sitofp_i32:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvt.f32.s32 s0, s0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r1]
; CHECK-FP16-NEXT: bx
; CHECK-LIBCALL-LABEL: test_sitofp_i32:
; CHECK-LIBCALL: vcvt.f32.s32
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_sitofp_i32(i32 %a, half* %p) #0 {
  %r = sitofp i32 %a to half
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_uitofp_i32:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvt.f32.u32 s0, s0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r1]
; CHECK-FP16-NEXT: bx
; CHECK-LIBCALL-LABEL: test_uitofp_i32:
; CHECK-LIBCALL: vcvt.f32.u32
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_uitofp_i32(i32 %a, half* %p) #0 {
  %r = uitofp i32 %a to half
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_sitofp_i64:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r2
; CHECK-FP16-NEXT: bl __aeabi_l2f
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_sitofp_i64:
; CHECK-LIBCALL: bl __aeabi_l2f
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_sitofp_i64(i64 %a, half* %p) #0 {
  %r = sitofp i64 %a to half
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_uitofp_i64:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r2
; CHECK-FP16-NEXT: bl __aeabi_ul2f
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_uitofp_i64:
; CHECK-LIBCALL: bl __aeabi_ul2f
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_uitofp_i64(i64 %a, half* %p) #0 {
  %r = uitofp i64 %a to half
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_fptrunc_float:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r1]
; CHECK-FP16-NEXT: bx
; CHECK-LIBCALL-LABEL: test_fptrunc_float:
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_fptrunc_float(float %f, half* %p) #0 {
  %a = fptrunc float %f to half
  store half %a, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_fptrunc_double:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r2
; CHECK-FP16-NEXT: bl __aeabi_d2h
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_fptrunc_double:
; CHECK-LIBCALL: bl __aeabi_d2h
define void @test_fptrunc_double(double %d, half* %p) #0 {
  %a = fptrunc double %d to half
  store half %a, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_fpextend_float:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: ldrh r0, [r0]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: bx lr
; CHECK-LIBCALL-LABEL: test_fpextend_float:
; CHECK-LIBCALL: b __gnu_h2f_ieee
define float @test_fpextend_float(half* %p) {
  %a = load half, half* %p, align 2
  %r = fpext half %a to float
  ret float %r
}

; CHECK-FP16-LABEL: test_fpextend_double:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: ldrh r0, [r0]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vcvt.f64.f32 d16, s0
; CHECK-FP16-NEXT: vmov r0, r1, d16
; CHECK-FP16-NEXT: bx lr
; CHECK-LIBCALL-LABEL: test_fpextend_double:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: vcvt.f64.f32
define double @test_fpextend_double(half* %p) {
  %a = load half, half* %p, align 2
  %r = fpext half %a to double
  ret double %r
}

; CHECK-BOTH-LABEL: test_bitcast_halftoi16:
; CHECK-BOTH-NEXT: .fnstart
; CHECK-BOTH-NEXT: ldrh r0, [r0]
; CHECK-BOTH-NEXT: bx lr
define i16 @test_bitcast_halftoi16(half* %p) #0 {
  %a = load half, half* %p, align 2
  %r = bitcast half %a to i16
  ret i16 %r
}

; CHECK-BOTH-LABEL: test_bitcast_i16tohalf:
; CHECK-BOTH-NEXT: .fnstart
; CHECK-BOTH-NEXT: strh r0, [r1]
; CHECK-BOTH-NEXT: bx lr
define void @test_bitcast_i16tohalf(i16 %a, half* %p) #0 {
  %r = bitcast i16 %a to half
  store half %r, half* %p
  ret void
}

declare half @llvm.sqrt.f16(half %a) #0
declare half @llvm.powi.f16(half %a, i32 %b) #0
declare half @llvm.sin.f16(half %a) #0
declare half @llvm.cos.f16(half %a) #0
declare half @llvm.pow.f16(half %a, half %b) #0
declare half @llvm.exp.f16(half %a) #0
declare half @llvm.exp2.f16(half %a) #0
declare half @llvm.log.f16(half %a) #0
declare half @llvm.log10.f16(half %a) #0
declare half @llvm.log2.f16(half %a) #0
declare half @llvm.fma.f16(half %a, half %b, half %c) #0
declare half @llvm.fabs.f16(half %a) #0
declare half @llvm.minnum.f16(half %a, half %b) #0
declare half @llvm.maxnum.f16(half %a, half %b) #0
declare half @llvm.copysign.f16(half %a, half %b) #0
declare half @llvm.floor.f16(half %a) #0
declare half @llvm.ceil.f16(half %a) #0
declare half @llvm.trunc.f16(half %a) #0
declare half @llvm.rint.f16(half %a) #0
declare half @llvm.nearbyint.f16(half %a) #0
declare half @llvm.round.f16(half %a) #0
declare half @llvm.fmuladd.f16(half %a, half %b, half %c) #0

; CHECK-FP16-LABEL: test_sqrt:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: ldrh r1, [r0]
; CHECK-FP16-NEXT: vmov s0, r1
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vsqrt.f32 s0, s0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r1, s0
; CHECK-FP16-NEXT: strh r1, [r0]
; CHECK-FP16-NEXT: bx lr
; CHECK-LIBCALL-LABEL: test_sqrt:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: vsqrt.f32
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_sqrt(half* %p) #0 {
  %a = load half, half* %p, align 2
  %r = call half @llvm.sqrt.f16(half %a)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_fpowi:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r4]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: bl __powisf2
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_fpowi:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __powisf2
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_fpowi(half* %p, i32 %b) #0 {
  %a = load half, half* %p, align 2
  %r = call half @llvm.powi.f16(half %a, i32 %b)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_sin:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r4]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: bl sinf
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_sin:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl sinf
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_sin(half* %p) #0 {
  %a = load half, half* %p, align 2
  %r = call half @llvm.sin.f16(half %a)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_cos:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r4]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: bl cosf
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_cos:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl cosf
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_cos(half* %p) #0 {
  %a = load half, half* %p, align 2
  %r = call half @llvm.cos.f16(half %a)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_pow:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r1]
; CHECK-FP16-NEXT: ldrh r1, [r4]
; CHECK-FP16-NEXT: vmov s2, r0
; CHECK-FP16-NEXT: vmov s0, r1
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s2, s2
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: vmov r1, s2
; CHECK-FP16-NEXT: bl powf
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_pow:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl powf
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_pow(half* %p, half* %q) #0 {
  %a = load half, half* %p, align 2
  %b = load half, half* %q, align 2
  %r = call half @llvm.pow.f16(half %a, half %b)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_exp:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r4]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: bl expf
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_exp:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl expf
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_exp(half* %p) #0 {
  %a = load half, half* %p, align 2
  %r = call half @llvm.exp.f16(half %a)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_exp2:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r4]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: bl exp2f
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_exp2:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl exp2f
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_exp2(half* %p) #0 {
  %a = load half, half* %p, align 2
  %r = call half @llvm.exp2.f16(half %a)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_log:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r4]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: bl logf
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_log:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl logf
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_log(half* %p) #0 {
  %a = load half, half* %p, align 2
  %r = call half @llvm.log.f16(half %a)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_log10:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r4]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: bl log10f
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_log10:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl log10f
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_log10(half* %p) #0 {
  %a = load half, half* %p, align 2
  %r = call half @llvm.log10.f16(half %a)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_log2:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r4]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: bl log2f
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_log2:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl log2f
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_log2(half* %p) #0 {
  %a = load half, half* %p, align 2
  %r = call half @llvm.log2.f16(half %a)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_fma:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r2]
; CHECK-FP16-NEXT: ldrh r1, [r1]
; CHECK-FP16-NEXT: ldrh r2, [r4]
; CHECK-FP16-NEXT: vmov s2, r1
; CHECK-FP16-NEXT: vmov s4, r0
; CHECK-FP16-NEXT: vmov s0, r2
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s2, s2
; CHECK-FP16-NEXT: vcvtb.f32.f16 s4, s4
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: vmov r1, s2
; CHECK-FP16-NEXT: vmov r2, s4
; CHECK-FP16-NEXT: bl fmaf
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_fma:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl fmaf
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_fma(half* %p, half* %q, half* %r) #0 {
  %a = load half, half* %p, align 2
  %b = load half, half* %q, align 2
  %c = load half, half* %r, align 2
  %v = call half @llvm.fma.f16(half %a, half %b, half %c)
  store half %v, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_fabs:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: ldrh r1, [r0]
; CHECK-FP16-NEXT: vmov s0, r1
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vabs.f32 s0, s0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r1, s0
; CHECK-FP16-NEXT: strh r1, [r0]
; CHECK-FP16-NEXT: bx lr
; CHECK-LIBCALL-LABEL: test_fabs:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bfc
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_fabs(half* %p) {
  %a = load half, half* %p, align 2
  %r = call half @llvm.fabs.f16(half %a)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_minnum:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r1]
; CHECK-FP16-NEXT: ldrh r1, [r4]
; CHECK-FP16-NEXT: vmov s2, r0
; CHECK-FP16-NEXT: vmov s0, r1
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s2, s2
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: vmov r1, s2
; CHECK-FP16-NEXT: bl fminf
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_minnum:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl fminf
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_minnum(half* %p, half* %q) #0 {
  %a = load half, half* %p, align 2
  %b = load half, half* %q, align 2
  %r = call half @llvm.minnum.f16(half %a, half %b)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_maxnum:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r1]
; CHECK-FP16-NEXT: ldrh r1, [r4]
; CHECK-FP16-NEXT: vmov s2, r0
; CHECK-FP16-NEXT: vmov s0, r1
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s2, s2
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: vmov r1, s2
; CHECK-FP16-NEXT: bl fmaxf
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_maxnum:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl fmaxf
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_maxnum(half* %p, half* %q) #0 {
  %a = load half, half* %p, align 2
  %b = load half, half* %q, align 2
  %r = call half @llvm.maxnum.f16(half %a, half %b)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_copysign:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: ldrh r1, [r1]
; CHECK-FP16-NEXT: ldrh r2, [r0]
; CHECK-FP16-NEXT: vmov.i32 d2, #0x80000000
; CHECK-FP16-NEXT: vmov s0, r2
; CHECK-FP16-NEXT: vmov s2, r1
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s2, s2
; CHECK-FP16-NEXT: vbsl d2, d1, d0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s4
; CHECK-FP16-NEXT: vmov r1, s0
; CHECK-FP16-NEXT: strh r1, [r0]
; CHECK-FP16-NEXT: bx lr
; CHECK-LIBCALL-LABEL: test_copysign:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: vbsl
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_copysign(half* %p, half* %q) #0 {
  %a = load half, half* %p, align 2
  %b = load half, half* %q, align 2
  %r = call half @llvm.copysign.f16(half %a, half %b)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_floor:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r4]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: bl floorf
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_floor:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl floorf
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_floor(half* %p) {
  %a = load half, half* %p, align 2
  %r = call half @llvm.floor.f16(half %a)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_ceil:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r4]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: bl ceilf
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_ceil:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl ceilf
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_ceil(half* %p) {
  %a = load half, half* %p, align 2
  %r = call half @llvm.ceil.f16(half %a)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_trunc:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r4]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: bl truncf
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_trunc:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl truncf
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_trunc(half* %p) {
  %a = load half, half* %p, align 2
  %r = call half @llvm.trunc.f16(half %a)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_rint:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r4]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: bl rintf
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_rint:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl rintf
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_rint(half* %p) {
  %a = load half, half* %p, align 2
  %r = call half @llvm.rint.f16(half %a)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_nearbyint:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r4]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: bl nearbyintf
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_nearbyint:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl nearbyintf
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_nearbyint(half* %p) {
  %a = load half, half* %p, align 2
  %r = call half @llvm.nearbyint.f16(half %a)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_round:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: push {r4, lr}
; CHECK-FP16-NEXT: mov r4, r0
; CHECK-FP16-NEXT: ldrh r0, [r4]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: bl roundf
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: vcvtb.f16.f32 s0, s0
; CHECK-FP16-NEXT: vmov r0, s0
; CHECK-FP16-NEXT: strh r0, [r4]
; CHECK-FP16-NEXT: pop {r4, pc}
; CHECK-LIBCALL-LABEL: test_round:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl roundf
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_round(half* %p) {
  %a = load half, half* %p, align 2
  %r = call half @llvm.round.f16(half %a)
  store half %r, half* %p
  ret void
}

; CHECK-FP16-LABEL: test_fmuladd:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: ldrh    r2, [r2]
; CHECK-FP16-NEXT: ldrh    r3, [r0]
; CHECK-FP16-NEXT: ldrh    r1, [r1]
; CHECK-FP16-NEXT: vmov    s0, r1
; CHECK-FP16-NEXT: vmov    s2, r3
; CHECK-FP16-NEXT: vmov    s4, r2
; CHECK-FP16-NEXT: vcvtb.f32.f16   s0, s0
; CHECK-FP16-NEXT: vcvtb.f32.f16   s2, s2
; CHECK-FP16-NEXT: vcvtb.f32.f16   s4, s4
; CHECK-FP16-NEXT: vmla.f32        s4, s2, s0
; CHECK-FP16-NEXT: vcvtb.f16.f32   s0, s4
; CHECK-FP16-NEXT: vmov    r1, s0
; CHECK-FP16-NEXT: strh    r1, [r0]
; CHECK-FP16-NEXT: bx      lr
; CHECK-LIBCALL-LABEL: test_fmuladd:
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: bl __gnu_h2f_ieee
; CHECK-LIBCALL: vmla.f32
; CHECK-LIBCALL: bl __gnu_f2h_ieee
define void @test_fmuladd(half* %p, half* %q, half* %r) #0 {
  %a = load half, half* %p, align 2
  %b = load half, half* %q, align 2
  %c = load half, half* %r, align 2
  %v = call half @llvm.fmuladd.f16(half %a, half %b, half %c)
  store half %v, half* %p
  ret void
}

; f16 vectors are not legal in the backend.  Vector elements are not assigned
; to the register, but are stored in the stack instead.  Hence insertelement
; and extractelement have these extra loads and stores.

; CHECK-ALL-LABEL: test_insertelement:
; CHECK-ALL-NEXT: .fnstart
; CHECK-ALL-NEXT: sub sp, sp, #8
; CHECK-ALL-NEXT: ldrh r3, [r1, #6]
; CHECK-ALL-NEXT: strh r3, [sp, #6]
; CHECK-ALL-NEXT: ldrh r3, [r1, #4]
; CHECK-ALL-NEXT: strh r3, [sp, #4]
; CHECK-ALL-NEXT: ldrh r3, [r1, #2]
; CHECK-ALL-NEXT: strh r3, [sp, #2]
; CHECK-ALL-NEXT: ldrh r3, [r1]
; CHECK-ALL-NEXT: strh r3, [sp]
; CHECK-ALL-NEXT: mov r3, sp
; CHECK-ALL-NEXT: ldrh r0, [r0]
; CHECK-ALL-NEXT: add r2, r3, r2, lsl #1
; CHECK-ALL-NEXT: strh r0, [r2]
; CHECK-ALL-NEXT: ldrh r0, [sp, #6]
; CHECK-ALL-NEXT: strh r0, [r1, #6]
; CHECK-ALL-NEXT: ldrh r0, [sp, #4]
; CHECK-ALL-NEXT: strh r0, [r1, #4]
; CHECK-ALL-NEXT: ldrh r0, [sp, #2]
; CHECK-ALL-NEXT: strh r0, [r1, #2]
; CHECK-ALL-NEXT: ldrh r0, [sp]
; CHECK-ALL-NEXT: strh r0, [r1]
; CHECK-ALL-NEXT: add sp, sp, #8
; CHECK-ALL-NEXT: bx lr
define void @test_insertelement(half* %p, <4 x half>* %q, i32 %i) #0 {
  %a = load half, half* %p, align 2
  %b = load <4 x half>, <4 x half>* %q, align 8
  %c = insertelement <4 x half> %b, half %a, i32 %i
  store <4 x half> %c, <4 x half>* %q
  ret void
}

; CHECK-ALL-LABEL: test_extractelement:
; CHECK-ALL-NEXT: .fnstart
; CHECK-ALL-NEXT: sub sp, sp, #8
; CHECK-ALL-NEXT: ldrh r12, [r1, #2]
; CHECK-ALL-NEXT: ldrh r3, [r1]
; CHECK-ALL-NEXT: orr r3, r3, r12, lsl #16
; CHECK-ALL-NEXT: str r3, [sp]
; CHECK-ALL-NEXT: ldrh r3, [r1, #6]
; CHECK-ALL-NEXT: ldrh r1, [r1, #4]
; CHECK-ALL-NEXT: orr r1, r1, r3, lsl #16
; CHECK-ALL-NEXT: str r1, [sp, #4]
; CHECK-ALL-NEXT: mov r1, sp
; CHECK-ALL-NEXT: add r1, r1, r2, lsl #1
; CHECK-ALL-NEXT: ldrh r1, [r1]
; CHECK-ALL-NEXT: strh r1, [r0]
; CHECK-ALL-NEXT: add sp, sp, #8
; CHECK-ALL-NEXT: bx lr
define void @test_extractelement(half* %p, <4 x half>* %q, i32 %i) #0 {
  %a = load <4 x half>, <4 x half>* %q, align 8
  %b = extractelement <4 x half> %a, i32 %i
  store half %b, half* %p
  ret void
}

; test struct operations

%struct.dummy = type { i32, half }

; CHECK-ALL-LABEL: test_insertvalue:
; CHECK-ALL-NEXT: .fnstart
; CHECK-ALL-NEXT: ldr r2, [r0]
; CHECK-ALL-NEXT: ldrh r1, [r1]
; CHECK-ALL-NEXT: strh r1, [r0, #4]
; CHECK-ALL-NEXT: str r2, [r0]
; CHECK-ALL-NEXT: bx lr
define void @test_insertvalue(%struct.dummy* %p, half* %q) {
  %a = load %struct.dummy, %struct.dummy* %p
  %b = load half, half* %q
  %c = insertvalue %struct.dummy %a, half %b, 1
  store %struct.dummy %c, %struct.dummy* %p
  ret void
}

; CHECK-ALL-LABEL: test_extractvalue:
; CHECK-ALL-NEXT: .fnstart
; CHECK-ALL-NEXT: ldrh r0, [r0, #4]
; CHECK-ALL-NEXT: strh r0, [r1]
; CHECK-ALL-NEXT: bx lr
define void @test_extractvalue(%struct.dummy* %p, half* %q) {
  %a = load %struct.dummy, %struct.dummy* %p
  %b = extractvalue %struct.dummy %a, 1
  store half %b, half* %q
  ret void
}

; CHECK-FP16-LABEL: test_struct_return:
; CHECK-FP16-NEXT: .fnstart
; CHECK-FP16-NEXT: ldr r2, [r0]
; CHECK-FP16-NEXT: ldrh r0, [r0, #4]
; CHECK-FP16-NEXT: vmov s0, r0
; CHECK-FP16-NEXT: mov r0, r2
; CHECK-FP16-NEXT: vcvtb.f32.f16 s0, s0
; CHECK-FP16-NEXT: vmov r1, s0
; CHECK-FP16-NEXT: bx lr
; CHECK-LIBCALL-LABEL: test_struct_return:
; CHECK-LIBCALL-NEXT: .fnstart
; CHECK-LIBCALL-NEXT: push {r4, lr}
; CHECK-LIBCALL-NEXT: ldr r4, [r0]
; CHECK-LIBCALL-NEXT: ldrh r0, [r0, #4]
; CHECK-LIBCALL-NEXT: bl __gnu_h2f_ieee
; CHECK-LIBCALL-NEXT: mov r1, r0
; CHECK-LIBCALL-NEXT: mov r0, r4
; CHECK-LIBCALL-NEXT: pop {r4, pc}
define %struct.dummy @test_struct_return(%struct.dummy* %p) {
  %a = load %struct.dummy, %struct.dummy* %p
  ret %struct.dummy %a
}

; CHECK-ALL-LABEL: test_struct_arg:
; CHECK-ALL-NEXT: .fnstart
; CHECK-ALL-NEXT: mov r0, r1
; CHECK-ALL-NEXT: bx lr
define half @test_struct_arg(%struct.dummy %p) {
  %a = extractvalue %struct.dummy %p, 1
  ret half %a
}

attributes #0 = { nounwind }
