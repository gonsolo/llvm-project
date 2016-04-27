; RUN: llvm-as %s -o %t.o
; RUN: %ld64 -lto_library %llvmshlibdir/libLTO.dylib -dylib -arch x86_64 -macosx_version_min 10.10.0 -lSystem -o %t.dylib %t.o -save-temps  -undefined dynamic_lookup -exported_symbol _c -exported_symbol _b

; RUN: llvm-dis %t.dylib.lto.opt.bc -o - | FileCheck --check-prefix=IR %s
; check that @a is no longer a linkonce_odr definition
; IR-NOT: define linkonce_odr void @a()
; check that @b is turned into weak because it is exported
; IR: define weak_odr void @b() #1 {

; RUN: llvm-nm %t.dylib | FileCheck --check-prefix=NM %s
; check that the linker can hide @a but not @b
; NM: 0000000000000f10 t _a
; NM: 0000000000000f20 T _b
; NM: 0000000000000f00 T _c

target triple = "x86_64-apple-macosx10.10.0"

declare void @external()

define linkonce_odr void @a() noinline {
  call void @external()
  ret void
}

define linkonce_odr void @b() {
  ret void
}

define void()* @c() {
  call void @a()
  ret void()* @b
}
