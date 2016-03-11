; First ensure that the ThinLTO handling in the gold plugin handles
; bitcode without function summary sections gracefully.
; RUN: llvm-as %s -o %t.o
; RUN: llvm-as %p/Inputs/thinlto.ll -o %t2.o
; RUN: %gold -plugin %llvmshlibdir/LLVMgold.so \
; RUN:    --plugin-opt=thinlto \
; RUN:    --plugin-opt=thinlto-index-only \
; RUN:    -shared %t.o %t2.o -o %t3
; RUN: not test -e %t3
; RUN: %gold -plugin %llvmshlibdir/LLVMgold.so \
; RUN:    --plugin-opt=thinlto \
; RUN:    -shared %t.o %t2.o -o %t4
; RUN: llvm-nm %t4 | FileCheck %s --check-prefix=NM

; Next generate function summary sections and test gold handling.
; RUN: llvm-as -function-summary %s -o %t.o
; RUN: llvm-as -function-summary %p/Inputs/thinlto.ll -o %t2.o

; Ensure gold generates an index and not a binary if requested.
; RUN: %gold -plugin %llvmshlibdir/LLVMgold.so \
; RUN:    --plugin-opt=thinlto \
; RUN:    --plugin-opt=thinlto-index-only \
; RUN:    -shared %t.o %t2.o -o %t3
; RUN: llvm-bcanalyzer -dump %t3.thinlto.bc | FileCheck %s --check-prefix=COMBINED
; RUN: not test -e %t3

; Ensure gold generates an index as well as a binary by default in ThinLTO mode.
; First force single-threaded mode
; RUN: %gold -plugin %llvmshlibdir/LLVMgold.so \
; RUN:    --plugin-opt=thinlto \
; RUN:    --plugin-opt=jobs=1 \
; RUN:    -shared %t.o %t2.o -o %t4
; RUN: llvm-bcanalyzer -dump %t4.thinlto.bc | FileCheck %s --check-prefix=COMBINED
; RUN: llvm-nm %t4 | FileCheck %s --check-prefix=NM

; Next force multi-threaded mode
; RUN: %gold -plugin %llvmshlibdir/LLVMgold.so \
; RUN:    --plugin-opt=thinlto \
; RUN:    --plugin-opt=jobs=2 \
; RUN:    -shared %t.o %t2.o -o %t4
; RUN: llvm-bcanalyzer -dump %t4.thinlto.bc | FileCheck %s --check-prefix=COMBINED
; RUN: llvm-nm %t4 | FileCheck %s --check-prefix=NM

; Test --plugin-opt=obj-path to ensure unique object files generated.
; RUN: %gold -plugin %llvmshlibdir/LLVMgold.so \
; RUN:    --plugin-opt=thinlto \
; RUN:    --plugin-opt=jobs=2 \
; RUN:    --plugin-opt=obj-path=%t5.o \
; RUN:    -shared %t.o %t2.o -o %t4
; RUN: llvm-nm %t5.o0 | FileCheck %s --check-prefix=NM2
; RUN: llvm-nm %t5.o1 | FileCheck %s --check-prefix=NM2

; NM: T f
; NM2: T {{f|g}}

; COMBINED: <MODULE_STRTAB_BLOCK
; COMBINED-NEXT: <ENTRY {{.*}} record string = '{{.*}}/test/tools/gold/X86/Output/thinlto.ll.tmp{{.*}}.o'
; COMBINED-NEXT: <ENTRY {{.*}} record string = '{{.*}}/test/tools/gold/X86/Output/thinlto.ll.tmp{{.*}}.o'
; COMBINED-NEXT: </MODULE_STRTAB_BLOCK
; COMBINED-NEXT: <GLOBALVAL_SUMMARY_BLOCK
; COMBINED-NEXT: <COMBINED
; COMBINED-NEXT: <COMBINED
; COMBINED-NEXT: </GLOBALVAL_SUMMARY_BLOCK
; COMBINED-NEXT: <VALUE_SYMTAB
; Check that the format is: op0=valueid, op1=offset, op2=funcguid,
; where funcguid is the lower 64 bits of the function name MD5.
; COMBINED-NEXT: <COMBINED_GVDEFENTRY abbrevid={{[0-9]+}} op0={{1|2}} op1={{[0-9]+}} op2={{-3706093650706652785|-5300342847281564238}}
; COMBINED-NEXT: <COMBINED_GVDEFENTRY abbrevid={{[0-9]+}} op0={{1|2}} op1={{[0-9]+}} op2={{-3706093650706652785|-5300342847281564238}}
; COMBINED-NEXT: </VALUE_SYMTAB

declare void @g(...)

define void @f() {
entry:
  call void (...) @g()
  ret void
}
