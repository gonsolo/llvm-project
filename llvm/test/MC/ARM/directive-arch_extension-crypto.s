@ RUN: not llvm-mc -triple armv7-eabi -filetype asm -o /dev/null 2>&1 %s \
@ RUN:   | FileCheck %s -check-prefix CHECK-V7
@ RUN: not llvm-mc -triple armv8-eabi -filetype asm -o /dev/null 2>&1 %s \
@ RUN:   | FileCheck %s -check-prefix CHECK-V8
@ RUN: not llvm-mc -triple thumbv7-eabi -filetype asm -o /dev/null 2>&1 %s \
@ RUN:   | FileCheck %s -check-prefix CHECK-V7
@ RUN: not llvm-mc -triple thumbv8-eabi -filetype asm -o /dev/null 2>&1 %s \
@ RUN:   | FileCheck %s -check-prefix CHECK-V8

	.syntax unified

	.arch_extension crypto
@ CHECK-V7: error: architectural extension 'crypto' is not allowed for the current base architecture
@ CHECK-V7-NEXT: 	.arch_extension crypto
@ CHECK-V7-NEXT:                     ^

	.type crypto,%function
crypto:
	vmull.p64 q0, d0, d1
@ CHECK-V7: error: instruction requires: crypto armv8

	aesd.8 q0, q1
@ CHECK-V7: error: instruction requires: crypto armv8
	aese.8 q0, q1
@ CHECK-V7: error: instruction requires: crypto armv8
	aesimc.8 q0, q1
@ CHECK-V7: error: instruction requires: crypto armv8
	aesmc.8 q0, q1
@ CHECK-V7: error: instruction requires: crypto armv8

	sha1h.32 q0, q1
@ CHECK-V7: error: instruction requires: crypto armv8
	sha1su1.32 q0, q1
@ CHECK-V7: error: instruction requires: crypto armv8
	sha256su0.32 q0, q1
@ CHECK-V7: error: instruction requires: crypto armv8

	sha1c.32 q0, q1, q2
@ CHECK-V7: error: instruction requires: crypto armv8
	sha1m.32 q0, q1, q2
@ CHECK-V7: error: instruction requires: crypto armv8
	sha1p.32 q0, q1, q2
@ CHECK-V7: error: instruction requires: crypto armv8
	sha1su0.32 q0, q1, q2
@ CHECK-V7: error: instruction requires: crypto armv8
	sha256h.32 q0, q1, q2
@ CHECK-V7: error: instruction requires: crypto armv8
	sha256h2.32 q0, q1, q2
@ CHECK-V7: error: instruction requires: crypto armv8
	sha256su1.32 q0, q1, q2
@ CHECK-V7: error: instruction requires: crypto armv8

	.arch_extension nocrypto
@ CHECK-V7: error: architectural extension 'crypto' is not allowed for the current base architecture
@ CHECK-V7-NEXT: 	.arch_extension nocrypto
@ CHECK-V7-NEXT:                     ^

	.type nocrypto,%function
nocrypto:
	vmull.p64 q0, d0, d1
@ CHECK-V7: error: instruction requires: crypto armv8
@ CHECK-V8: error: instruction requires: crypto

	aesd.8 q0, q1
@ CHECK-V7: error: instruction requires: crypto armv8
@ CHECK-V8: error: instruction requires: crypto
	aese.8 q0, q1
@ CHECK-V7: error: instruction requires: crypto armv8
@ CHECK-V8: error: instruction requires: crypto
	aesimc.8 q0, q1
@ CHECK-V7: error: instruction requires: crypto armv8
@ CHECK-V8: error: instruction requires: crypto
	aesmc.8 q0, q1
@ CHECK-V7: error: instruction requires: crypto armv8
@ CHECK-V8: error: instruction requires: crypto

	sha1h.32 q0, q1
@ CHECK-V7: error: instruction requires: crypto armv8
@ CHECK-V8: error: instruction requires: crypto
	sha1su1.32 q0, q1
@ CHECK-V7: error: instruction requires: crypto armv8
@ CHECK-V8: error: instruction requires: crypto
	sha256su0.32 q0, q1
@ CHECK-V7: error: instruction requires: crypto armv8
@ CHECK-V8: error: instruction requires: crypto

	sha1c.32 q0, q1, q2
@ CHECK-V7: error: instruction requires: crypto armv8
@ CHECK-V8: error: instruction requires: crypto
	sha1m.32 q0, q1, q2
@ CHECK-V7: error: instruction requires: crypto armv8
@ CHECK-V8: error: instruction requires: crypto
	sha1p.32 q0, q1, q2
@ CHECK-V7: error: instruction requires: crypto armv8
@ CHECK-V8: error: instruction requires: crypto
	sha1su0.32 q0, q1, q2
@ CHECK-V7: error: instruction requires: crypto armv8
@ CHECK-V8: error: instruction requires: crypto
	sha256h.32 q0, q1, q2
@ CHECK-V7: error: instruction requires: crypto armv8
@ CHECK-V8: error: instruction requires: crypto
	sha256h2.32 q0, q1, q2
@ CHECK-V7: error: instruction requires: crypto armv8
@ CHECK-V8: error: instruction requires: crypto
	sha256su1.32 q0, q1, q2
@ CHECK-V7: error: instruction requires: crypto armv8
@ CHECK-V8: error: instruction requires: crypto

