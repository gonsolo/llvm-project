; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=x86_64-unknown -mattr=+avx | FileCheck %s --check-prefix=AVX --check-prefix=AVX1
; RUN: llc < %s -mtriple=x86_64-unknown -mattr=+avx2 | FileCheck %s --check-prefix=AVX --check-prefix=AVX2
; RUN: llc < %s -mtriple=x86_64-unknown -mattr=+avx512f | FileCheck %s --check-prefix=AVX --check-prefix=AVX512F

; 'signum' test cases (PR13248)

;
; generic implementation for 128-bit vectors
;

define void @signum32a(<4 x float>*) {
; AVX-LABEL: signum32a:
; AVX:       # BB#0: # %entry
; AVX-NEXT:    vmovaps (%rdi), %xmm0
; AVX-NEXT:    vxorps %xmm1, %xmm1, %xmm1
; AVX-NEXT:    vcmpltps %xmm1, %xmm0, %xmm2
; AVX-NEXT:    vcvtdq2ps %xmm2, %xmm2
; AVX-NEXT:    vcmpltps %xmm0, %xmm1, %xmm0
; AVX-NEXT:    vcvtdq2ps %xmm0, %xmm0
; AVX-NEXT:    vsubps %xmm0, %xmm2, %xmm0
; AVX-NEXT:    vmovaps %xmm0, (%rdi)
; AVX-NEXT:    retq
entry:
  %1 = load <4 x float>, <4 x float>* %0
  %2 = fcmp olt <4 x float> %1, zeroinitializer
  %3 = sitofp <4 x i1> %2 to <4 x float>
  %4 = fcmp ogt <4 x float> %1, zeroinitializer
  %5 = sitofp <4 x i1> %4 to <4 x float>
  %6 = fsub <4 x float> %3, %5
  store <4 x float> %6, <4 x float>* %0
  ret void
}

define void @signum64a(<2 x double>*) {
; AVX1-LABEL: signum64a:
; AVX1:       # BB#0: # %entry
; AVX1-NEXT:    vmovapd (%rdi), %xmm0
; AVX1-NEXT:    vxorpd %xmm1, %xmm1, %xmm1
; AVX1-NEXT:    vcmpltpd %xmm1, %xmm0, %xmm2
; AVX1-NEXT:    vpextrq $1, %xmm2, %rax
; AVX1-NEXT:    vcvtsi2sdq %rax, %xmm3, %xmm3
; AVX1-NEXT:    vmovq %xmm2, %rax
; AVX1-NEXT:    vcvtsi2sdq %rax, %xmm4, %xmm2
; AVX1-NEXT:    vunpcklpd {{.*#+}} xmm2 = xmm2[0],xmm3[0]
; AVX1-NEXT:    vcmpltpd %xmm0, %xmm1, %xmm0
; AVX1-NEXT:    vpextrq $1, %xmm0, %rax
; AVX1-NEXT:    vcvtsi2sdq %rax, %xmm4, %xmm1
; AVX1-NEXT:    vmovq %xmm0, %rax
; AVX1-NEXT:    vcvtsi2sdq %rax, %xmm4, %xmm0
; AVX1-NEXT:    vunpcklpd {{.*#+}} xmm0 = xmm0[0],xmm1[0]
; AVX1-NEXT:    vsubpd %xmm0, %xmm2, %xmm0
; AVX1-NEXT:    vmovapd %xmm0, (%rdi)
; AVX1-NEXT:    retq
;
; AVX2-LABEL: signum64a:
; AVX2:       # BB#0: # %entry
; AVX2-NEXT:    vmovapd (%rdi), %xmm0
; AVX2-NEXT:    vxorpd %xmm1, %xmm1, %xmm1
; AVX2-NEXT:    vcmpltpd %xmm1, %xmm0, %xmm2
; AVX2-NEXT:    vpextrq $1, %xmm2, %rax
; AVX2-NEXT:    vcvtsi2sdq %rax, %xmm3, %xmm3
; AVX2-NEXT:    vmovq %xmm2, %rax
; AVX2-NEXT:    vcvtsi2sdq %rax, %xmm4, %xmm2
; AVX2-NEXT:    vunpcklpd {{.*#+}} xmm2 = xmm2[0],xmm3[0]
; AVX2-NEXT:    vcmpltpd %xmm0, %xmm1, %xmm0
; AVX2-NEXT:    vpextrq $1, %xmm0, %rax
; AVX2-NEXT:    vcvtsi2sdq %rax, %xmm4, %xmm1
; AVX2-NEXT:    vmovq %xmm0, %rax
; AVX2-NEXT:    vcvtsi2sdq %rax, %xmm4, %xmm0
; AVX2-NEXT:    vunpcklpd {{.*#+}} xmm0 = xmm0[0],xmm1[0]
; AVX2-NEXT:    vsubpd %xmm0, %xmm2, %xmm0
; AVX2-NEXT:    vmovapd %xmm0, (%rdi)
; AVX2-NEXT:    retq
;
; AVX512F-LABEL: signum64a:
; AVX512F:       # BB#0: # %entry
; AVX512F-NEXT:    vmovapd (%rdi), %xmm0
; AVX512F-NEXT:    vxorpd %xmm1, %xmm1, %xmm1
; AVX512F-NEXT:    vcmpltpd %xmm1, %xmm0, %xmm2
; AVX512F-NEXT:    vpermilps {{.*#+}} xmm2 = xmm2[0,2,2,3]
; AVX512F-NEXT:    vcvtdq2pd %xmm2, %xmm2
; AVX512F-NEXT:    vcmpltpd %xmm0, %xmm1, %xmm0
; AVX512F-NEXT:    vpermilps {{.*#+}} xmm0 = xmm0[0,2,2,3]
; AVX512F-NEXT:    vcvtdq2pd %xmm0, %xmm0
; AVX512F-NEXT:    vsubpd %xmm0, %xmm2, %xmm0
; AVX512F-NEXT:    vmovapd %xmm0, (%rdi)
; AVX512F-NEXT:    retq
entry:
  %1 = load <2 x double>, <2 x double>* %0
  %2 = fcmp olt <2 x double> %1, zeroinitializer
  %3 = sitofp <2 x i1> %2 to <2 x double>
  %4 = fcmp ogt <2 x double> %1, zeroinitializer
  %5 = sitofp <2 x i1> %4 to <2 x double>
  %6 = fsub <2 x double> %3, %5
  store <2 x double> %6, <2 x double>* %0
  ret void
}

;
; generic implementation for 256-bit vectors
;

define void @signum32b(<8 x float>*) {
; AVX1-LABEL: signum32b:
; AVX1:       # BB#0: # %entry
; AVX1-NEXT:    vmovaps (%rdi), %ymm0
; AVX1-NEXT:    vxorps %ymm1, %ymm1, %ymm1
; AVX1-NEXT:    vcmpltps %ymm1, %ymm0, %ymm2
; AVX1-NEXT:    vextractf128 $1, %ymm2, %xmm3
; AVX1-NEXT:    vpacksswb %xmm3, %xmm2, %xmm2
; AVX1-NEXT:    vpsllw $15, %xmm2, %xmm2
; AVX1-NEXT:    vpsraw $15, %xmm2, %xmm2
; AVX1-NEXT:    vpmovsxwd %xmm2, %xmm3
; AVX1-NEXT:    vpshufd {{.*#+}} xmm2 = xmm2[2,3,0,1]
; AVX1-NEXT:    vpmovsxwd %xmm2, %xmm2
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm3, %ymm2
; AVX1-NEXT:    vcvtdq2ps %ymm2, %ymm2
; AVX1-NEXT:    vcmpltps %ymm0, %ymm1, %ymm0
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm1
; AVX1-NEXT:    vpacksswb %xmm1, %xmm0, %xmm0
; AVX1-NEXT:    vpsllw $15, %xmm0, %xmm0
; AVX1-NEXT:    vpsraw $15, %xmm0, %xmm0
; AVX1-NEXT:    vpmovsxwd %xmm0, %xmm1
; AVX1-NEXT:    vpshufd {{.*#+}} xmm0 = xmm0[2,3,0,1]
; AVX1-NEXT:    vpmovsxwd %xmm0, %xmm0
; AVX1-NEXT:    vinsertf128 $1, %xmm0, %ymm1, %ymm0
; AVX1-NEXT:    vcvtdq2ps %ymm0, %ymm0
; AVX1-NEXT:    vsubps %ymm0, %ymm2, %ymm0
; AVX1-NEXT:    vmovaps %ymm0, (%rdi)
; AVX1-NEXT:    vzeroupper
; AVX1-NEXT:    retq
;
; AVX2-LABEL: signum32b:
; AVX2:       # BB#0: # %entry
; AVX2-NEXT:    vmovaps (%rdi), %ymm0
; AVX2-NEXT:    vxorps %ymm1, %ymm1, %ymm1
; AVX2-NEXT:    vcmpltps %ymm1, %ymm0, %ymm2
; AVX2-NEXT:    vextractf128 $1, %ymm2, %xmm3
; AVX2-NEXT:    vpacksswb %xmm3, %xmm2, %xmm2
; AVX2-NEXT:    vpsllw $15, %xmm2, %xmm2
; AVX2-NEXT:    vpsraw $15, %xmm2, %xmm2
; AVX2-NEXT:    vpmovsxwd %xmm2, %ymm2
; AVX2-NEXT:    vcvtdq2ps %ymm2, %ymm2
; AVX2-NEXT:    vcmpltps %ymm0, %ymm1, %ymm0
; AVX2-NEXT:    vextractf128 $1, %ymm0, %xmm1
; AVX2-NEXT:    vpacksswb %xmm1, %xmm0, %xmm0
; AVX2-NEXT:    vpsllw $15, %xmm0, %xmm0
; AVX2-NEXT:    vpsraw $15, %xmm0, %xmm0
; AVX2-NEXT:    vpmovsxwd %xmm0, %ymm0
; AVX2-NEXT:    vcvtdq2ps %ymm0, %ymm0
; AVX2-NEXT:    vsubps %ymm0, %ymm2, %ymm0
; AVX2-NEXT:    vmovaps %ymm0, (%rdi)
; AVX2-NEXT:    vzeroupper
; AVX2-NEXT:    retq
;
; AVX512F-LABEL: signum32b:
; AVX512F:       # BB#0: # %entry
; AVX512F-NEXT:    vmovaps (%rdi), %ymm0
; AVX512F-NEXT:    vxorps %ymm1, %ymm1, %ymm1
; AVX512F-NEXT:    vcmpltps %zmm1, %zmm0, %k1
; AVX512F-NEXT:    vpternlogd $255, %zmm2, %zmm2, %zmm2
; AVX512F-NEXT:    vmovdqa64 %zmm2, %zmm3 {%k1} {z}
; AVX512F-NEXT:    vpmovqd %zmm3, %ymm3
; AVX512F-NEXT:    vcvtdq2ps %ymm3, %ymm3
; AVX512F-NEXT:    vcmpltps %zmm0, %zmm1, %k1
; AVX512F-NEXT:    vmovdqa64 %zmm2, %zmm0 {%k1} {z}
; AVX512F-NEXT:    vpmovqd %zmm0, %ymm0
; AVX512F-NEXT:    vcvtdq2ps %ymm0, %ymm0
; AVX512F-NEXT:    vsubps %ymm0, %ymm3, %ymm0
; AVX512F-NEXT:    vmovaps %ymm0, (%rdi)
; AVX512F-NEXT:    retq
entry:
  %1 = load <8 x float>, <8 x float>* %0
  %2 = fcmp olt <8 x float> %1, zeroinitializer
  %3 = sitofp <8 x i1> %2 to <8 x float>
  %4 = fcmp ogt <8 x float> %1, zeroinitializer
  %5 = sitofp <8 x i1> %4 to <8 x float>
  %6 = fsub <8 x float> %3, %5
  store <8 x float> %6, <8 x float>* %0
  ret void
}

define void @signum64b(<4 x double>*) {
; AVX1-LABEL: signum64b:
; AVX1:       # BB#0: # %entry
; AVX1-NEXT:    vmovapd (%rdi), %ymm0
; AVX1-NEXT:    vxorpd %ymm1, %ymm1, %ymm1
; AVX1-NEXT:    vcmpltpd %ymm1, %ymm0, %ymm2
; AVX1-NEXT:    vextractf128 $1, %ymm2, %xmm3
; AVX1-NEXT:    vpacksswb %xmm3, %xmm2, %xmm2
; AVX1-NEXT:    vpslld $31, %xmm2, %xmm2
; AVX1-NEXT:    vpsrad $31, %xmm2, %xmm2
; AVX1-NEXT:    vcvtdq2pd %xmm2, %ymm2
; AVX1-NEXT:    vcmpltpd %ymm0, %ymm1, %ymm0
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm1
; AVX1-NEXT:    vpacksswb %xmm1, %xmm0, %xmm0
; AVX1-NEXT:    vpslld $31, %xmm0, %xmm0
; AVX1-NEXT:    vpsrad $31, %xmm0, %xmm0
; AVX1-NEXT:    vcvtdq2pd %xmm0, %ymm0
; AVX1-NEXT:    vsubpd %ymm0, %ymm2, %ymm0
; AVX1-NEXT:    vmovapd %ymm0, (%rdi)
; AVX1-NEXT:    vzeroupper
; AVX1-NEXT:    retq
;
; AVX2-LABEL: signum64b:
; AVX2:       # BB#0: # %entry
; AVX2-NEXT:    vmovapd (%rdi), %ymm0
; AVX2-NEXT:    vxorpd %ymm1, %ymm1, %ymm1
; AVX2-NEXT:    vcmpltpd %ymm1, %ymm0, %ymm2
; AVX2-NEXT:    vextractf128 $1, %ymm2, %xmm3
; AVX2-NEXT:    vpacksswb %xmm3, %xmm2, %xmm2
; AVX2-NEXT:    vpslld $31, %xmm2, %xmm2
; AVX2-NEXT:    vpsrad $31, %xmm2, %xmm2
; AVX2-NEXT:    vcvtdq2pd %xmm2, %ymm2
; AVX2-NEXT:    vcmpltpd %ymm0, %ymm1, %ymm0
; AVX2-NEXT:    vextractf128 $1, %ymm0, %xmm1
; AVX2-NEXT:    vpacksswb %xmm1, %xmm0, %xmm0
; AVX2-NEXT:    vpslld $31, %xmm0, %xmm0
; AVX2-NEXT:    vpsrad $31, %xmm0, %xmm0
; AVX2-NEXT:    vcvtdq2pd %xmm0, %ymm0
; AVX2-NEXT:    vsubpd %ymm0, %ymm2, %ymm0
; AVX2-NEXT:    vmovapd %ymm0, (%rdi)
; AVX2-NEXT:    vzeroupper
; AVX2-NEXT:    retq
;
; AVX512F-LABEL: signum64b:
; AVX512F:       # BB#0: # %entry
; AVX512F-NEXT:    vmovapd (%rdi), %ymm0
; AVX512F-NEXT:    vxorpd %ymm1, %ymm1, %ymm1
; AVX512F-NEXT:    vcmpltpd %ymm1, %ymm0, %ymm2
; AVX512F-NEXT:    vpmovqd %zmm2, %ymm2
; AVX512F-NEXT:    vpslld $31, %xmm2, %xmm2
; AVX512F-NEXT:    vpsrad $31, %xmm2, %xmm2
; AVX512F-NEXT:    vcvtdq2pd %xmm2, %ymm2
; AVX512F-NEXT:    vcmpltpd %ymm0, %ymm1, %ymm0
; AVX512F-NEXT:    vpmovqd %zmm0, %ymm0
; AVX512F-NEXT:    vpslld $31, %xmm0, %xmm0
; AVX512F-NEXT:    vpsrad $31, %xmm0, %xmm0
; AVX512F-NEXT:    vcvtdq2pd %xmm0, %ymm0
; AVX512F-NEXT:    vsubpd %ymm0, %ymm2, %ymm0
; AVX512F-NEXT:    vmovapd %ymm0, (%rdi)
; AVX512F-NEXT:    retq
entry:
  %1 = load <4 x double>, <4 x double>* %0
  %2 = fcmp olt <4 x double> %1, zeroinitializer
  %3 = sitofp <4 x i1> %2 to <4 x double>
  %4 = fcmp ogt <4 x double> %1, zeroinitializer
  %5 = sitofp <4 x i1> %4 to <4 x double>
  %6 = fsub <4 x double> %3, %5
  store <4 x double> %6, <4 x double>* %0
  ret void
}

;
; implementation using AVX intrinsics for 256-bit vectors
;

define void @signum32c(<8 x float>*) {
; AVX1-LABEL: signum32c:
; AVX1:       # BB#0: # %entry
; AVX1-NEXT:    vmovaps (%rdi), %ymm0
; AVX1-NEXT:    vxorps %ymm1, %ymm1, %ymm1
; AVX1-NEXT:    vcmpltps %ymm1, %ymm0, %ymm2
; AVX1-NEXT:    vcvtdq2ps %ymm2, %ymm2
; AVX1-NEXT:    vcmpltps %ymm0, %ymm1, %ymm0
; AVX1-NEXT:    vcvtdq2ps %ymm0, %ymm0
; AVX1-NEXT:    vsubps %ymm0, %ymm2, %ymm0
; AVX1-NEXT:    vmovaps %ymm0, (%rdi)
; AVX1-NEXT:    vzeroupper
; AVX1-NEXT:    retq
;
; AVX2-LABEL: signum32c:
; AVX2:       # BB#0: # %entry
; AVX2-NEXT:    vmovaps (%rdi), %ymm0
; AVX2-NEXT:    vxorps %ymm1, %ymm1, %ymm1
; AVX2-NEXT:    vcmpltps %ymm1, %ymm0, %ymm2
; AVX2-NEXT:    vcvtdq2ps %ymm2, %ymm2
; AVX2-NEXT:    vcmpltps %ymm0, %ymm1, %ymm0
; AVX2-NEXT:    vcvtdq2ps %ymm0, %ymm0
; AVX2-NEXT:    vsubps %ymm0, %ymm2, %ymm0
; AVX2-NEXT:    vmovaps %ymm0, (%rdi)
; AVX2-NEXT:    vzeroupper
; AVX2-NEXT:    retq
;
; AVX512F-LABEL: signum32c:
; AVX512F:       # BB#0: # %entry
; AVX512F-NEXT:    vmovaps (%rdi), %ymm0
; AVX512F-NEXT:    vxorps %ymm1, %ymm1, %ymm1
; AVX512F-NEXT:    vcmpltps %ymm1, %ymm0, %ymm2
; AVX512F-NEXT:    vcvtdq2ps %ymm2, %ymm2
; AVX512F-NEXT:    vcmpltps %ymm0, %ymm1, %ymm0
; AVX512F-NEXT:    vcvtdq2ps %ymm0, %ymm0
; AVX512F-NEXT:    vsubps %ymm0, %ymm2, %ymm0
; AVX512F-NEXT:    vmovaps %ymm0, (%rdi)
; AVX512F-NEXT:    retq
entry:
  %1 = load <8 x float>, <8 x float>* %0
  %2 = tail call <8 x float> @llvm.x86.avx.cmp.ps.256(<8 x float> %1, <8 x float> zeroinitializer, i8 1)
  %3 = bitcast <8 x float> %2 to <8 x i32>
  %4 = sitofp <8 x i32> %3 to <8 x float>
  %5 = tail call <8 x float> @llvm.x86.avx.cmp.ps.256(<8 x float> zeroinitializer, <8 x float> %1, i8 1)
  %6 = bitcast <8 x float> %5 to <8 x i32>
  %7 = sitofp <8 x i32> %6 to <8 x float>
  %8 = fsub <8 x float> %4, %7
  store <8 x float> %8, <8 x float>* %0
  ret void
}

define void @signum64c(<4 x double>*) {
; AVX1-LABEL: signum64c:
; AVX1:       # BB#0: # %entry
; AVX1-NEXT:    vmovapd (%rdi), %ymm0
; AVX1-NEXT:    vxorpd %ymm1, %ymm1, %ymm1
; AVX1-NEXT:    vcmpltpd %ymm1, %ymm0, %ymm2
; AVX1-NEXT:    vcmpltpd %ymm0, %ymm1, %ymm0
; AVX1-NEXT:    vpsubd %xmm0, %xmm2, %xmm1
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm0
; AVX1-NEXT:    vextractf128 $1, %ymm2, %xmm2
; AVX1-NEXT:    vpsubd %xmm0, %xmm2, %xmm0
; AVX1-NEXT:    vpshufd {{.*#+}} xmm0 = xmm0[0,1,0,2]
; AVX1-NEXT:    vpshufd {{.*#+}} xmm1 = xmm1[0,2,2,3]
; AVX1-NEXT:    vpblendw {{.*#+}} xmm0 = xmm1[0,1,2,3],xmm0[4,5,6,7]
; AVX1-NEXT:    vcvtdq2pd %xmm0, %ymm0
; AVX1-NEXT:    vmovaps %ymm0, (%rdi)
; AVX1-NEXT:    vzeroupper
; AVX1-NEXT:    retq
;
; AVX2-LABEL: signum64c:
; AVX2:       # BB#0: # %entry
; AVX2-NEXT:    vmovapd (%rdi), %ymm0
; AVX2-NEXT:    vxorpd %ymm1, %ymm1, %ymm1
; AVX2-NEXT:    vcmpltpd %ymm1, %ymm0, %ymm2
; AVX2-NEXT:    vcmpltpd %ymm0, %ymm1, %ymm0
; AVX2-NEXT:    vpsubd %ymm0, %ymm2, %ymm0
; AVX2-NEXT:    vextracti128 $1, %ymm0, %xmm1
; AVX2-NEXT:    vpshufd {{.*#+}} xmm1 = xmm1[0,1,0,2]
; AVX2-NEXT:    vpshufd {{.*#+}} xmm0 = xmm0[0,2,2,3]
; AVX2-NEXT:    vpblendd {{.*#+}} xmm0 = xmm0[0,1],xmm1[2,3]
; AVX2-NEXT:    vcvtdq2pd %xmm0, %ymm0
; AVX2-NEXT:    vmovaps %ymm0, (%rdi)
; AVX2-NEXT:    vzeroupper
; AVX2-NEXT:    retq
;
; AVX512F-LABEL: signum64c:
; AVX512F:       # BB#0: # %entry
; AVX512F-NEXT:    vmovapd (%rdi), %ymm0
; AVX512F-NEXT:    vxorpd %ymm1, %ymm1, %ymm1
; AVX512F-NEXT:    vcmpltpd %ymm1, %ymm0, %ymm2
; AVX512F-NEXT:    vcmpltpd %ymm0, %ymm1, %ymm0
; AVX512F-NEXT:    vpsubd %ymm0, %ymm2, %ymm0
; AVX512F-NEXT:    vextracti128 $1, %ymm0, %xmm1
; AVX512F-NEXT:    vpshufd {{.*#+}} xmm1 = xmm1[0,1,0,2]
; AVX512F-NEXT:    vpshufd {{.*#+}} xmm0 = xmm0[0,2,2,3]
; AVX512F-NEXT:    vpblendd {{.*#+}} xmm0 = xmm0[0,1],xmm1[2,3]
; AVX512F-NEXT:    vcvtdq2pd %xmm0, %ymm0
; AVX512F-NEXT:    vmovaps %ymm0, (%rdi)
; AVX512F-NEXT:    retq
entry:
  %x = load <4 x double>, <4 x double>* %0
  %xgt = tail call <4 x double> @llvm.x86.avx.cmp.pd.256(<4 x double> %x, <4 x double> zeroinitializer, i8 1)
  %igt = bitcast <4 x double> %xgt to <8 x i32>
  %xlt = tail call <4 x double> @llvm.x86.avx.cmp.pd.256(<4 x double> zeroinitializer, <4 x double> %x, i8 1)
  %ilt = bitcast <4 x double> %xlt to <8 x i32>
  ; it is important to use %igt twice as source in order to make LLVM use a shuffle operation
  %isign = sub <8 x i32> %igt, %ilt
  %ssign = shufflevector <8 x i32> %isign, <8 x i32> %isign, <4 x i32> <i32 0, i32 2, i32 12, i32 14>
  %sign = tail call <4 x double> @llvm.x86.avx.cvtdq2.pd.256(<4 x i32> %ssign)
  store <4 x double> %sign, <4 x double>* %0
  ret void
}

declare <8 x float> @llvm.x86.avx.cmp.ps.256(<8 x float>, <8 x float>, i8) nounwind readnone

declare <4 x double> @llvm.x86.avx.cmp.pd.256(<4 x double>, <4 x double>, i8) nounwind readnone

declare <4 x double> @llvm.x86.avx.cvtdq2.pd.256(<4 x i32>) nounwind readnone
