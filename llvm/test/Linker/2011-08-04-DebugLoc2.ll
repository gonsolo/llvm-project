; This file is used by 2011-08-04-DebugLoc.ll, so it doesn't actually do anything itself
;
; RUN: true


target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64"
target triple = "x86_64-apple-macosx10.7.0"

define i32 @bar() nounwind ssp {
  ret i32 21, !dbg !6
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!11}
!llvm.dbg.sp = !{!1}

!0 = !MDCompileUnit(language: DW_LANG_C99, producer: "Apple clang version 3.0 (tags/Apple/clang-209.11) (based on LLVM 3.0svn)", isOptimized: true, emissionKind: 0, file: !8, enums: !9, retainedTypes: !9, subprograms: !10)
!1 = !MDSubprogram(name: "bar", line: 1, isLocal: false, isDefinition: true, virtualIndex: 6, isOptimized: false, file: !8, scope: !2, type: !3, function: i32 ()* @bar)
!2 = !MDFile(filename: "b.c", directory: "/private/tmp")
!3 = !MDSubroutineType(types: !4)
!4 = !{!5}
!5 = !MDBasicType(tag: DW_TAG_base_type, name: "int", size: 32, align: 32, encoding: DW_ATE_signed)
!6 = !MDLocation(line: 1, column: 13, scope: !7)
!7 = distinct !MDLexicalBlock(line: 1, column: 11, file: !8, scope: !1)
!8 = !MDFile(filename: "b.c", directory: "/private/tmp")
!9 = !{i32 0}
!10 = !{!1}
!11 = !{i32 1, !"Debug Info Version", i32 3}
