; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: llc < %s -mtriple=x86_64-unknown-unknown                                          | FileCheck %s --check-prefix=SSE
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -fast-isel -fast-isel-abort=1            | FileCheck %s --check-prefix=SSE
; RUN: llc < %s -mtriple=x86_64-unknown-unknown                               -mattr=avx | FileCheck %s --check-prefix=AVX
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -fast-isel -fast-isel-abort=1 -mattr=avx | FileCheck %s --check-prefix=AVX
; RUN: llc < %s -mtriple=x86_64-unknown-unknown                               -mattr=avx512f | FileCheck %s --check-prefix=AVX512 --check-prefix=AVX512SLOW
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -fast-isel -fast-isel-abort=1 -mattr=avx512f | FileCheck %s --check-prefix=AVX512 --check-prefix=AVX512FAST

; Test all cmp predicates that can be used with SSE.

define float @select_fcmp_oeq_f32(float %a, float %b, float %c, float %d) {
; SSE-LABEL: select_fcmp_oeq_f32:
; SSE:       # BB#0:
; SSE-NEXT:    cmpeqss %xmm1, %xmm0
; SSE-NEXT:    andps %xmm0, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm0
; SSE-NEXT:    orps %xmm2, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_oeq_f32:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpeqss %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_oeq_f32:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpeqss %xmm1, %xmm0, %k1
; AVX512SLOW-NEXT:    vmovss %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovaps %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_oeq_f32:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpeqss %xmm1, %xmm0, %xmm0
; AVX512FAST-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp oeq float %a, %b
  %2 = select i1 %1, float %c, float %d
  ret float %2
}

define double @select_fcmp_oeq_f64(double %a, double %b, double %c, double %d) {
; SSE-LABEL: select_fcmp_oeq_f64:
; SSE:       # BB#0:
; SSE-NEXT:    cmpeqsd %xmm1, %xmm0
; SSE-NEXT:    andps %xmm0, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm0
; SSE-NEXT:    orps %xmm2, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_oeq_f64:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpeqsd %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_oeq_f64:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpeqsd %xmm1, %xmm0, %k1
; AVX512SLOW-NEXT:    vmovsd %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovapd %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_oeq_f64:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpeqsd %xmm1, %xmm0, %xmm0
; AVX512FAST-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp oeq double %a, %b
  %2 = select i1 %1, double %c, double %d
  ret double %2
}

define float @select_fcmp_ogt_f32(float %a, float %b, float %c, float %d) {
; SSE-LABEL: select_fcmp_ogt_f32:
; SSE:       # BB#0:
; SSE-NEXT:    cmpltss %xmm0, %xmm1
; SSE-NEXT:    andps %xmm1, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm1
; SSE-NEXT:    orps %xmm2, %xmm1
; SSE-NEXT:    movaps %xmm1, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_ogt_f32:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpltss %xmm0, %xmm1, %xmm0
; AVX-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_ogt_f32:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpltss %xmm0, %xmm1, %k1
; AVX512SLOW-NEXT:    vmovss %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovaps %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_ogt_f32:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpltss %xmm0, %xmm1, %xmm0
; AVX512FAST-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp ogt float %a, %b
  %2 = select i1 %1, float %c, float %d
  ret float %2
}

define double @select_fcmp_ogt_f64(double %a, double %b, double %c, double %d) {
; SSE-LABEL: select_fcmp_ogt_f64:
; SSE:       # BB#0:
; SSE-NEXT:    cmpltsd %xmm0, %xmm1
; SSE-NEXT:    andps %xmm1, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm1
; SSE-NEXT:    orps %xmm2, %xmm1
; SSE-NEXT:    movaps %xmm1, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_ogt_f64:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpltsd %xmm0, %xmm1, %xmm0
; AVX-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_ogt_f64:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpltsd %xmm0, %xmm1, %k1
; AVX512SLOW-NEXT:    vmovsd %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovapd %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_ogt_f64:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpltsd %xmm0, %xmm1, %xmm0
; AVX512FAST-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp ogt double %a, %b
  %2 = select i1 %1, double %c, double %d
  ret double %2
}

define float @select_fcmp_oge_f32(float %a, float %b, float %c, float %d) {
; SSE-LABEL: select_fcmp_oge_f32:
; SSE:       # BB#0:
; SSE-NEXT:    cmpless %xmm0, %xmm1
; SSE-NEXT:    andps %xmm1, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm1
; SSE-NEXT:    orps %xmm2, %xmm1
; SSE-NEXT:    movaps %xmm1, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_oge_f32:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpless %xmm0, %xmm1, %xmm0
; AVX-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_oge_f32:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpless %xmm0, %xmm1, %k1
; AVX512SLOW-NEXT:    vmovss %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovaps %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_oge_f32:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpless %xmm0, %xmm1, %xmm0
; AVX512FAST-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp oge float %a, %b
  %2 = select i1 %1, float %c, float %d
  ret float %2
}

define double @select_fcmp_oge_f64(double %a, double %b, double %c, double %d) {
; SSE-LABEL: select_fcmp_oge_f64:
; SSE:       # BB#0:
; SSE-NEXT:    cmplesd %xmm0, %xmm1
; SSE-NEXT:    andps %xmm1, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm1
; SSE-NEXT:    orps %xmm2, %xmm1
; SSE-NEXT:    movaps %xmm1, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_oge_f64:
; AVX:       # BB#0:
; AVX-NEXT:    vcmplesd %xmm0, %xmm1, %xmm0
; AVX-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_oge_f64:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmplesd %xmm0, %xmm1, %k1
; AVX512SLOW-NEXT:    vmovsd %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovapd %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_oge_f64:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmplesd %xmm0, %xmm1, %xmm0
; AVX512FAST-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp oge double %a, %b
  %2 = select i1 %1, double %c, double %d
  ret double %2
}

define float @select_fcmp_olt_f32(float %a, float %b, float %c, float %d) {
; SSE-LABEL: select_fcmp_olt_f32:
; SSE:       # BB#0:
; SSE-NEXT:    cmpltss %xmm1, %xmm0
; SSE-NEXT:    andps %xmm0, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm0
; SSE-NEXT:    orps %xmm2, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_olt_f32:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpltss %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_olt_f32:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpltss %xmm1, %xmm0, %k1
; AVX512SLOW-NEXT:    vmovss %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovaps %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_olt_f32:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpltss %xmm1, %xmm0, %xmm0
; AVX512FAST-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp olt float %a, %b
  %2 = select i1 %1, float %c, float %d
  ret float %2
}

define double @select_fcmp_olt_f64(double %a, double %b, double %c, double %d) {
; SSE-LABEL: select_fcmp_olt_f64:
; SSE:       # BB#0:
; SSE-NEXT:    cmpltsd %xmm1, %xmm0
; SSE-NEXT:    andps %xmm0, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm0
; SSE-NEXT:    orps %xmm2, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_olt_f64:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpltsd %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_olt_f64:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpltsd %xmm1, %xmm0, %k1
; AVX512SLOW-NEXT:    vmovsd %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovapd %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_olt_f64:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpltsd %xmm1, %xmm0, %xmm0
; AVX512FAST-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp olt double %a, %b
  %2 = select i1 %1, double %c, double %d
  ret double %2
}

define float @select_fcmp_ole_f32(float %a, float %b, float %c, float %d) {
; SSE-LABEL: select_fcmp_ole_f32:
; SSE:       # BB#0:
; SSE-NEXT:    cmpless %xmm1, %xmm0
; SSE-NEXT:    andps %xmm0, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm0
; SSE-NEXT:    orps %xmm2, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_ole_f32:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpless %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_ole_f32:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpless %xmm1, %xmm0, %k1
; AVX512SLOW-NEXT:    vmovss %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovaps %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_ole_f32:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpless %xmm1, %xmm0, %xmm0
; AVX512FAST-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp ole float %a, %b
  %2 = select i1 %1, float %c, float %d
  ret float %2
}

define double @select_fcmp_ole_f64(double %a, double %b, double %c, double %d) {
; SSE-LABEL: select_fcmp_ole_f64:
; SSE:       # BB#0:
; SSE-NEXT:    cmplesd %xmm1, %xmm0
; SSE-NEXT:    andps %xmm0, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm0
; SSE-NEXT:    orps %xmm2, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_ole_f64:
; AVX:       # BB#0:
; AVX-NEXT:    vcmplesd %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_ole_f64:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmplesd %xmm1, %xmm0, %k1
; AVX512SLOW-NEXT:    vmovsd %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovapd %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_ole_f64:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmplesd %xmm1, %xmm0, %xmm0
; AVX512FAST-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp ole double %a, %b
  %2 = select i1 %1, double %c, double %d
  ret double %2
}

define float @select_fcmp_ord_f32(float %a, float %b, float %c, float %d) {
; SSE-LABEL: select_fcmp_ord_f32:
; SSE:       # BB#0:
; SSE-NEXT:    cmpordss %xmm1, %xmm0
; SSE-NEXT:    andps %xmm0, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm0
; SSE-NEXT:    orps %xmm2, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_ord_f32:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpordss %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_ord_f32:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpordss %xmm1, %xmm0, %k1
; AVX512SLOW-NEXT:    vmovss %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovaps %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_ord_f32:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpordss %xmm1, %xmm0, %xmm0
; AVX512FAST-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp ord float %a, %b
  %2 = select i1 %1, float %c, float %d
  ret float %2
}

define double @select_fcmp_ord_f64(double %a, double %b, double %c, double %d) {
; SSE-LABEL: select_fcmp_ord_f64:
; SSE:       # BB#0:
; SSE-NEXT:    cmpordsd %xmm1, %xmm0
; SSE-NEXT:    andps %xmm0, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm0
; SSE-NEXT:    orps %xmm2, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_ord_f64:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpordsd %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_ord_f64:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpordsd %xmm1, %xmm0, %k1
; AVX512SLOW-NEXT:    vmovsd %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovapd %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_ord_f64:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpordsd %xmm1, %xmm0, %xmm0
; AVX512FAST-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp ord double %a, %b
  %2 = select i1 %1, double %c, double %d
  ret double %2
}

define float @select_fcmp_uno_f32(float %a, float %b, float %c, float %d) {
; SSE-LABEL: select_fcmp_uno_f32:
; SSE:       # BB#0:
; SSE-NEXT:    cmpunordss %xmm1, %xmm0
; SSE-NEXT:    andps %xmm0, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm0
; SSE-NEXT:    orps %xmm2, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_uno_f32:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpunordss %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_uno_f32:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpunordss %xmm1, %xmm0, %k1
; AVX512SLOW-NEXT:    vmovss %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovaps %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_uno_f32:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpunordss %xmm1, %xmm0, %xmm0
; AVX512FAST-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp uno float %a, %b
  %2 = select i1 %1, float %c, float %d
  ret float %2
}

define double @select_fcmp_uno_f64(double %a, double %b, double %c, double %d) {
; SSE-LABEL: select_fcmp_uno_f64:
; SSE:       # BB#0:
; SSE-NEXT:    cmpunordsd %xmm1, %xmm0
; SSE-NEXT:    andps %xmm0, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm0
; SSE-NEXT:    orps %xmm2, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_uno_f64:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpunordsd %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_uno_f64:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpunordsd %xmm1, %xmm0, %k1
; AVX512SLOW-NEXT:    vmovsd %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovapd %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_uno_f64:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpunordsd %xmm1, %xmm0, %xmm0
; AVX512FAST-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp uno double %a, %b
  %2 = select i1 %1, double %c, double %d
  ret double %2
}

define float @select_fcmp_ugt_f32(float %a, float %b, float %c, float %d) {
; SSE-LABEL: select_fcmp_ugt_f32:
; SSE:       # BB#0:
; SSE-NEXT:    cmpnless %xmm1, %xmm0
; SSE-NEXT:    andps %xmm0, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm0
; SSE-NEXT:    orps %xmm2, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_ugt_f32:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpnless %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_ugt_f32:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpnless %xmm1, %xmm0, %k1
; AVX512SLOW-NEXT:    vmovss %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovaps %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_ugt_f32:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpnless %xmm1, %xmm0, %xmm0
; AVX512FAST-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp ugt float %a, %b
  %2 = select i1 %1, float %c, float %d
  ret float %2
}

define double @select_fcmp_ugt_f64(double %a, double %b, double %c, double %d) {
; SSE-LABEL: select_fcmp_ugt_f64:
; SSE:       # BB#0:
; SSE-NEXT:    cmpnlesd %xmm1, %xmm0
; SSE-NEXT:    andps %xmm0, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm0
; SSE-NEXT:    orps %xmm2, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_ugt_f64:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpnlesd %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_ugt_f64:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpnlesd %xmm1, %xmm0, %k1
; AVX512SLOW-NEXT:    vmovsd %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovapd %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_ugt_f64:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpnlesd %xmm1, %xmm0, %xmm0
; AVX512FAST-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp ugt double %a, %b
  %2 = select i1 %1, double %c, double %d
  ret double %2
}

define float @select_fcmp_uge_f32(float %a, float %b, float %c, float %d) {
; SSE-LABEL: select_fcmp_uge_f32:
; SSE:       # BB#0:
; SSE-NEXT:    cmpnltss %xmm1, %xmm0
; SSE-NEXT:    andps %xmm0, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm0
; SSE-NEXT:    orps %xmm2, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_uge_f32:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpnltss %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_uge_f32:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpnltss %xmm1, %xmm0, %k1
; AVX512SLOW-NEXT:    vmovss %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovaps %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_uge_f32:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpnltss %xmm1, %xmm0, %xmm0
; AVX512FAST-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp uge float %a, %b
  %2 = select i1 %1, float %c, float %d
  ret float %2
}

define double @select_fcmp_uge_f64(double %a, double %b, double %c, double %d) {
; SSE-LABEL: select_fcmp_uge_f64:
; SSE:       # BB#0:
; SSE-NEXT:    cmpnltsd %xmm1, %xmm0
; SSE-NEXT:    andps %xmm0, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm0
; SSE-NEXT:    orps %xmm2, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_uge_f64:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpnltsd %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_uge_f64:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpnltsd %xmm1, %xmm0, %k1
; AVX512SLOW-NEXT:    vmovsd %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovapd %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_uge_f64:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpnltsd %xmm1, %xmm0, %xmm0
; AVX512FAST-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp uge double %a, %b
  %2 = select i1 %1, double %c, double %d
  ret double %2
}

define float @select_fcmp_ult_f32(float %a, float %b, float %c, float %d) {
; SSE-LABEL: select_fcmp_ult_f32:
; SSE:       # BB#0:
; SSE-NEXT:    cmpnless %xmm0, %xmm1
; SSE-NEXT:    andps %xmm1, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm1
; SSE-NEXT:    orps %xmm2, %xmm1
; SSE-NEXT:    movaps %xmm1, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_ult_f32:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpnless %xmm0, %xmm1, %xmm0
; AVX-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_ult_f32:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpnless %xmm0, %xmm1, %k1
; AVX512SLOW-NEXT:    vmovss %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovaps %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_ult_f32:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpnless %xmm0, %xmm1, %xmm0
; AVX512FAST-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp ult float %a, %b
  %2 = select i1 %1, float %c, float %d
  ret float %2
}

define double @select_fcmp_ult_f64(double %a, double %b, double %c, double %d) {
; SSE-LABEL: select_fcmp_ult_f64:
; SSE:       # BB#0:
; SSE-NEXT:    cmpnlesd %xmm0, %xmm1
; SSE-NEXT:    andps %xmm1, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm1
; SSE-NEXT:    orps %xmm2, %xmm1
; SSE-NEXT:    movaps %xmm1, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_ult_f64:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpnlesd %xmm0, %xmm1, %xmm0
; AVX-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_ult_f64:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpnlesd %xmm0, %xmm1, %k1
; AVX512SLOW-NEXT:    vmovsd %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovapd %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_ult_f64:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpnlesd %xmm0, %xmm1, %xmm0
; AVX512FAST-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp ult double %a, %b
  %2 = select i1 %1, double %c, double %d
  ret double %2
}

define float @select_fcmp_ule_f32(float %a, float %b, float %c, float %d) {
; SSE-LABEL: select_fcmp_ule_f32:
; SSE:       # BB#0:
; SSE-NEXT:    cmpnltss %xmm0, %xmm1
; SSE-NEXT:    andps %xmm1, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm1
; SSE-NEXT:    orps %xmm2, %xmm1
; SSE-NEXT:    movaps %xmm1, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_ule_f32:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpnltss %xmm0, %xmm1, %xmm0
; AVX-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_ule_f32:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpnltss %xmm0, %xmm1, %k1
; AVX512SLOW-NEXT:    vmovss %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovaps %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_ule_f32:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpnltss %xmm0, %xmm1, %xmm0
; AVX512FAST-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp ule float %a, %b
  %2 = select i1 %1, float %c, float %d
  ret float %2
}

define double @select_fcmp_ule_f64(double %a, double %b, double %c, double %d) {
; SSE-LABEL: select_fcmp_ule_f64:
; SSE:       # BB#0:
; SSE-NEXT:    cmpnltsd %xmm0, %xmm1
; SSE-NEXT:    andps %xmm1, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm1
; SSE-NEXT:    orps %xmm2, %xmm1
; SSE-NEXT:    movaps %xmm1, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_ule_f64:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpnltsd %xmm0, %xmm1, %xmm0
; AVX-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_ule_f64:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpnltsd %xmm0, %xmm1, %k1
; AVX512SLOW-NEXT:    vmovsd %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovapd %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_ule_f64:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpnltsd %xmm0, %xmm1, %xmm0
; AVX512FAST-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp ule double %a, %b
  %2 = select i1 %1, double %c, double %d
  ret double %2
}

define float @select_fcmp_une_f32(float %a, float %b, float %c, float %d) {
; SSE-LABEL: select_fcmp_une_f32:
; SSE:       # BB#0:
; SSE-NEXT:    cmpneqss %xmm1, %xmm0
; SSE-NEXT:    andps %xmm0, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm0
; SSE-NEXT:    orps %xmm2, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_une_f32:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpneqss %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_une_f32:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpneqss %xmm1, %xmm0, %k1
; AVX512SLOW-NEXT:    vmovss %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovaps %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_une_f32:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpneqss %xmm1, %xmm0, %xmm0
; AVX512FAST-NEXT:    vblendvps %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp une float %a, %b
  %2 = select i1 %1, float %c, float %d
  ret float %2
}

define double @select_fcmp_une_f64(double %a, double %b, double %c, double %d) {
; SSE-LABEL: select_fcmp_une_f64:
; SSE:       # BB#0:
; SSE-NEXT:    cmpneqsd %xmm1, %xmm0
; SSE-NEXT:    andps %xmm0, %xmm2
; SSE-NEXT:    andnps %xmm3, %xmm0
; SSE-NEXT:    orps %xmm2, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: select_fcmp_une_f64:
; AVX:       # BB#0:
; AVX-NEXT:    vcmpneqsd %xmm1, %xmm0, %xmm0
; AVX-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX-NEXT:    retq
;
; AVX512SLOW-LABEL: select_fcmp_une_f64:
; AVX512SLOW:       # BB#0:
; AVX512SLOW-NEXT:    vcmpneqsd %xmm1, %xmm0, %k1
; AVX512SLOW-NEXT:    vmovsd %xmm2, %xmm0, %xmm3 {%k1}
; AVX512SLOW-NEXT:    vmovapd %xmm3, %xmm0
; AVX512SLOW-NEXT:    retq
;
; AVX512FAST-LABEL: select_fcmp_une_f64:
; AVX512FAST:       # BB#0:
; AVX512FAST-NEXT:    vcmpneqsd %xmm1, %xmm0, %xmm0
; AVX512FAST-NEXT:    vblendvpd %xmm0, %xmm2, %xmm3, %xmm0
; AVX512FAST-NEXT:    retq
;
  %1 = fcmp une double %a, %b
  %2 = select i1 %1, double %c, double %d
  ret double %2
}

