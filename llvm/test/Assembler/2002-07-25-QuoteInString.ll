; Test double quotes in strings work correctly!
; RUN: llvm-as < %s | llvm-dis | llvm-as | llvm-dis
;
; RUN: verify-uselistorder %s -preserve-bc-use-list-order -num-shuffles=5
@str = internal global [6 x i8] c"\22foo\22\00"         ; <[6 x i8]*> [#uses=0]

