#!/bin/bash

#===- lib/asan/scripts/gen_asm_instrumentation.sh -------------------------===#
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
# Emit x86 instrumentation functions for asan.
#
#===-----------------------------------------------------------------------===#

check() {
  test $# -eq 2 || (echo "Incorrent number of arguments: $#" 1>&2 && exit 1)
  case "$1" in
    store) ;;
     load) ;;
        *) echo "Incorrect first argument: $1" 1>&2 && exit 1 ;;
  esac
  case "$2" in
    [0-9]*) ;;
         *) echo "Incorrect second argument: $2" 1>&2 && exit 1 ;;
  esac
}

func_name() {
  check $1 $2
  echo "__sanitizer_sanitize_$1$2"
}

func_label() {
  check $1 $2
  echo ".sanitize_$1$2_done"
}

func_report() {
  check $1 $2
  echo "__asan_report_$1$2"
}

emit_call_report() {
cat <<EOF
  cld
  emms
  call $(func_report $1 $2)
EOF
}

emit_stack_align() {
cat <<EOF
  subq \$8, %rsp
  andq \$-16, %rsp
EOF
}

cat <<EOF
// This file was generated by $(basename $0). Please, do not edit
// manually.
EOF

echo ".section .text"

echo "#if defined(__x86_64__) || defined(__i386__)"
for as in 1 2 4 8 16
do
  for at in store load
  do
    echo ".globl $(func_report $at $as)"
  done
done
echo "#endif //  defined(__x86_64__) || defined(__i386__)"

echo "#if defined(__i386__)"

# Functions for i386 1-, 2- and 4-byte accesses.
for as in 1 2 4
do
  for at in store load
  do
cat <<EOF
// Sanitize $as-byte $at. Takes one 4-byte address as an argument on
// stack, nothing is returned.
.globl $(func_name $at $as)
.type $(func_name $at $as), @function
$(func_name $at $as):
  pushl %ebp
  movl %esp, %ebp
  pushl %eax
  pushl %ecx
  pushl %edx
  pushfl
  movl 8(%ebp), %eax
  movl %eax, %ecx
  shrl \$0x3, %ecx
  movb 0x20000000(%ecx), %cl
  testb %cl, %cl
  je $(func_label $at $as)
  movl %eax, %edx
  andl \$0x7, %edx
EOF

    case $as in
    1) ;;
    2) echo '  incl %edx' ;;
    4) echo '  addl $0x3, %edx' ;;
    *) echo "Incorrect access size: $as" 1>&2; exit 1 ;;
    esac

cat <<EOF
  movsbl %cl, %ecx
  cmpl %ecx, %edx
  jl $(func_label $at $as)
  pushl %eax
$(emit_call_report $at $as)
$(func_label $at $as):
  popfl
  popl %edx
  popl %ecx
  popl %eax
  leave
  ret
EOF
  done
done

# Functions for i386 8- and 16-byte accesses.
for as in 8 16
do
  for at in store load
  do
cat <<EOF
// Sanitize $as-byte $at. Takes one 4-byte address as an argument on
// stack, nothing is returned.
.globl $(func_name $at $as)
.type $(func_name $at $as), @function
$(func_name $at $as):
  pushl %ebp
  movl %esp, %ebp
  pushl %eax
  pushl %ecx
  pushfl
  movl 8(%ebp), %eax
  movl %eax, %ecx
  shrl \$0x3, %ecx
EOF

    case ${as} in
      8) echo '  cmpb $0x0, 0x20000000(%ecx)' ;;
     16) echo '  cmpw $0x0, 0x20000000(%ecx)' ;;
      *) echo "Incorrect access size: ${as}" 1>&2; exit 1 ;;
    esac

cat <<EOF
  je $(func_label $at $as)
  pushl %eax
$(emit_call_report $at $as)
$(func_label $at $as):
  popfl
  popl %ecx
  popl %eax
  leave
  ret
EOF
  done
done

echo "#endif // defined(__i386__)"

echo "#if defined(__x86_64__)"

# Functions for x86-64 1-, 2- and 4-byte accesses.
for as in 1 2 4
do
  for at in store load
  do
cat <<EOF
// Sanitize $as-byte $at. Takes one 8-byte address as an argument in %rdi,
// nothing is returned.
.globl $(func_name $at $as)
.type $(func_name $at $as), @function
$(func_name $at $as):
  subq \$128, %rsp
  pushq %rax
  pushq %rcx
  pushfq
  movq %rdi, %rax
  shrq \$0x3, %rax
  movb 0x7fff8000(%rax), %al
  test %al, %al
  je $(func_label $at $as)
  movl %edi, %ecx
  andl \$0x7, %ecx
EOF

    case ${as} in
    1) ;;
    2) echo '  incl %ecx' ;;
    4) echo '  addl $0x3, %ecx' ;;
    *) echo "Incorrect access size: ${as}" 1>&2; exit 1 ;;
    esac

cat <<EOF
  movsbl %al, %eax
  cmpl %eax, %ecx
  jl $(func_label $at $as)
$(emit_stack_align)
$(emit_call_report $at $as)
$(func_label $at $as):
  popfq
  popq %rcx
  popq %rax
  addq \$128, %rsp
  ret
EOF
  done
done

# Functions for x86-64 8- and 16-byte accesses.
for as in 8 16
do
  for at in store load
  do
cat <<EOF
// Sanitize $as-byte $at. Takes one 8-byte address as an argument in %rdi,
// nothing is returned.
.globl $(func_name $at $as)
.type $(func_name $at $as), @function
$(func_name $at $as):
  subq \$128, %rsp
  pushq %rax
  pushfq
  movq %rdi, %rax
  shrq \$0x3, %rax
EOF

    case ${as} in
      8) echo '  cmpb $0x0, 0x7fff8000(%rax)' ;;
     16) echo '  cmpw $0x0, 0x7fff8000(%rax)' ;;
      *) echo "Incorrect access size: ${as}" 1>&2; exit 1 ;;
    esac

cat <<EOF
  je $(func_label $at $as)
$(emit_stack_align)
$(emit_call_report $at $as)
$(func_label $at $as):
  popfq
  popq %rax
  addq \$128, %rsp
  ret
EOF
  done
done
echo "#endif // defined(__x86_64__)"

cat <<EOF
#if defined(__linux__)
/* We do not need executable stack. */
.section        .note.GNU-stack,"",@progbits
#endif // defined(__linux__)
EOF
