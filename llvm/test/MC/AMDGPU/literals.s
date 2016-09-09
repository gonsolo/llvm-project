// RUN: not llvm-mc -arch=amdgcn -show-encoding %s | FileCheck %s --check-prefix=GCN --check-prefix=SI --check-prefix=SICI
// RUN: not llvm-mc -arch=amdgcn -mcpu=SI -show-encoding %s | FileCheck %s --check-prefix=GCN --check-prefix=SI --check-prefix=SICI
// RUN: not llvm-mc -arch=amdgcn -mcpu=bonaire -show-encoding %s | FileCheck %s --check-prefix=GCN --check-prefix=SICI --check-prefix=CIVI
// RUN: not llvm-mc -arch=amdgcn -mcpu=tonga -show-encoding %s | FileCheck %s --check-prefix=GCN --check-prefix=CIVI --check-prefix=VI

// RUN: not llvm-mc -arch=amdgcn -show-encoding %s 2>&1 | FileCheck %s --check-prefix=NOSI --check-prefix=NOSICI
// RUN: not llvm-mc -arch=amdgcn -mcpu=SI -show-encoding %s 2>&1 | FileCheck %s --check-prefix=NOSI --check-prefix=NOSICI
// RUN: not llvm-mc -arch=amdgcn -mcpu=bonaire -show-encoding %s 2>&1 | FileCheck %s --check-prefix=NOSICI
// RUN: not llvm-mc -arch=amdgcn -mcpu=tonga -show-encoding %s 2>&1 | FileCheck %s -check-prefix=NOVI

//---------------------------------------------------------------------------//
// fp literal, expected fp operand
//---------------------------------------------------------------------------//

// SICI: v_fract_f64_e32 v[0:1], 0.5 ; encoding: [0xf0,0x7c,0x00,0x7e]
// VI: v_fract_f64_e32 v[0:1], 0.5 ; encoding: [0xf0,0x64,0x00,0x7e]
v_fract_f64 v[0:1], 0.5

// SICI: v_sqrt_f64_e32 v[0:1], -4.0 ; encoding: [0xf7,0x68,0x00,0x7e]
// VI: v_sqrt_f64_e32 v[0:1], -4.0 ; encoding: [0xf7,0x50,0x00,0x7e]
v_sqrt_f64 v[0:1], -4.0

// SICI: v_log_clamp_f32_e32 v1, 0.5 ; encoding: [0xf0,0x4c,0x02,0x7e]
// NOVI: error: instruction not supported on this GPU
v_log_clamp_f32 v1, 0.5

// SICI: v_fract_f64_e32 v[0:1], 0.5 ; encoding: [0xf0,0x7c,0x00,0x7e]
// VI: v_fract_f64_e32 v[0:1], 0.5 ; encoding: [0xf0,0x64,0x00,0x7e]
v_fract_f64 v[0:1], 0.5

// SICI: v_trunc_f32_e32 v0, 0.5 ; encoding: [0xf0,0x42,0x00,0x7e]
// VI: v_trunc_f32_e32 v0, 0.5 ; encoding: [0xf0,0x38,0x00,0x7e]
v_trunc_f32 v0, 0.5

// SICI: v_fract_f64_e32 v[0:1], -1.0 ; encoding: [0xf3,0x7c,0x00,0x7e]
// VI: v_fract_f64_e32 v[0:1], -1.0 ; encoding: [0xf3,0x64,0x00,0x7e]
v_fract_f64 v[0:1], -1.0

// SICI: v_trunc_f32_e32 v0, -1.0 ; encoding: [0xf3,0x42,0x00,0x7e]
// VI: v_trunc_f32_e32 v0, -1.0 ; encoding: [0xf3,0x38,0x00,0x7e]
v_trunc_f32 v0, -1.0

// SICI: v_fract_f64_e32 v[0:1], 4.0 ; encoding: [0xf6,0x7c,0x00,0x7e]
// VI: v_fract_f64_e32 v[0:1], 4.0 ; encoding: [0xf6,0x64,0x00,0x7e]
v_fract_f64 v[0:1], 4.0

// SICI: v_trunc_f32_e32 v0, 4.0 ; encoding: [0xf6,0x42,0x00,0x7e]
// VI: v_trunc_f32_e32 v0, 4.0 ; encoding: [0xf6,0x38,0x00,0x7e]
v_trunc_f32 v0, 4.0

// SICI: v_fract_f64_e32 v[0:1], 0 ; encoding: [0x80,0x7c,0x00,0x7e]
// VI: v_fract_f64_e32 v[0:1], 0 ; encoding: [0x80,0x64,0x00,0x7e]
v_fract_f64 v[0:1], 0.0

// SICI: v_trunc_f32_e32 v0, 0 ; encoding: [0x80,0x42,0x00,0x7e]
// VI: v_trunc_f32_e32 v0, 0 ; encoding: [0x80,0x38,0x00,0x7e]
v_trunc_f32 v0, 0.0

// SICI: v_fract_f64_e32 v[0:1], 0x3ff80000 ; encoding: [0xff,0x7c,0x00,0x7e,0x00,0x00,0xf8,0x3f]
// VI: v_fract_f64_e32 v[0:1], 0x3ff80000 ; encoding: [0xff,0x64,0x00,0x7e,0x00,0x00,0xf8,0x3f]
v_fract_f64 v[0:1], 1.5

// SICI: v_trunc_f32_e32 v0, 0x3fc00000 ; encoding: [0xff,0x42,0x00,0x7e,0x00,0x00,0xc0,0x3f]
// VI: v_trunc_f32_e32 v0, 0x3fc00000 ; encoding: [0xff,0x38,0x00,0x7e,0x00,0x00,0xc0,0x3f]
v_trunc_f32 v0, 1.5

// SICI: v_fract_f64_e32 v[0:1], 0xc00921ca ; encoding: [0xff,0x7c,0x00,0x7e,0xca,0x21,0x09,0xc0]
// VI: v_fract_f64_e32 v[0:1], 0xc00921ca ; encoding: [0xff,0x64,0x00,0x7e,0xca,0x21,0x09,0xc0]
v_fract_f64 v[0:1], -3.1415

// SICI: v_trunc_f32_e32 v0, 0xc0490e56 ; encoding: [0xff,0x42,0x00,0x7e,0x56,0x0e,0x49,0xc0]
// VI: v_trunc_f32_e32 v0, 0xc0490e56 ; encoding: [0xff,0x38,0x00,0x7e,0x56,0x0e,0x49,0xc0]
v_trunc_f32 v0, -3.1415

// SICI: v_fract_f64_e32 v[0:1], 0x44b52d02 ; encoding: [0xff,0x7c,0x00,0x7e,0x02,0x2d,0xb5,0x44]
// VI: v_fract_f64_e32 v[0:1], 0x44b52d02 ; encoding: [0xff,0x64,0x00,0x7e,0x02,0x2d,0xb5,0x44]
v_fract_f64 v[0:1], 100000000000000000000000.0

// SICI: v_trunc_f32_e32 v0, 0x65a96816 ; encoding: [0xff,0x42,0x00,0x7e,0x16,0x68,0xa9,0x65]
// VI: v_trunc_f32_e32 v0, 0x65a96816 ; encoding: [0xff,0x38,0x00,0x7e,0x16,0x68,0xa9,0x65]
v_trunc_f32 v0, 100000000000000000000000.0

// SICI: v_fract_f64_e32 v[0:1], 0x416312d0 ; encoding: [0xff,0x7c,0x00,0x7e,0xd0,0x12,0x63,0x41]
// VI: v_fract_f64_e32 v[0:1], 0x416312d0 ; encoding: [0xff,0x64,0x00,0x7e,0xd0,0x12,0x63,0x41]
v_fract_f64 v[0:1], 10000000.0

// SICI: v_trunc_f32_e32 v0, 0x4b189680 ; encoding: [0xff,0x42,0x00,0x7e,0x80,0x96,0x18,0x4b]
// VI: v_trunc_f32_e32 v0, 0x4b189680 ; encoding: [0xff,0x38,0x00,0x7e,0x80,0x96,0x18,0x4b]
v_trunc_f32 v0, 10000000.0

// SICI: v_fract_f64_e32 v[0:1], 0x47efffff ; encoding: [0xff,0x7c,0x00,0x7e,0xff,0xff,0xef,0x47]
// VI: v_fract_f64_e32 v[0:1], 0x47efffff ; encoding: [0xff,0x64,0x00,0x7e,0xff,0xff,0xef,0x47]
v_fract_f64 v[0:1], 3.402823e+38

// SICI: v_trunc_f32_e32 v0, 0x7f7ffffd ; encoding: [0xff,0x42,0x00,0x7e,0xfd,0xff,0x7f,0x7f]
// VI: v_trunc_f32_e32 v0, 0x7f7ffffd ; encoding: [0xff,0x38,0x00,0x7e,0xfd,0xff,0x7f,0x7f]
v_trunc_f32 v0, 3.402823e+38

// SICI: v_fract_f64_e32 v[0:1], 0x381fffff ; encoding: [0xff,0x7c,0x00,0x7e,0xff,0xff,0x1f,0x38]
// VI: v_fract_f64_e32 v[0:1], 0x381fffff ; encoding: [0xff,0x64,0x00,0x7e,0xff,0xff,0x1f,0x38]
v_fract_f64 v[0:1], 2.3509886e-38

// SICI: v_trunc_f32_e32 v0, 0xffffff ; encoding: [0xff,0x42,0x00,0x7e,0xff,0xff,0xff,0x00]
// VI: v_trunc_f32_e32 v0, 0xffffff ; encoding: [0xff,0x38,0x00,0x7e,0xff,0xff,0xff,0x00]
v_trunc_f32 v0, 2.3509886e-38

// SICI: v_fract_f64_e32 v[0:1], 0x3179f623 ; encoding: [0xff,0x7c,0x00,0x7e,0x23,0xf6,0x79,0x31]
// VI: v_fract_f64_e32 v[0:1], 0x3179f623 ; encoding: [0xff,0x64,0x00,0x7e,0x23,0xf6,0x79,0x31]
v_fract_f64 v[0:1], 2.3509886e-70

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
v_trunc_f32 v0, 2.3509886e-70

//---------------------------------------------------------------------------//
// fp literal, expected int operand
//---------------------------------------------------------------------------//

// SICI: s_mov_b64 s[0:1], 0.5 ; encoding: [0xf0,0x04,0x80,0xbe]
// VI: s_mov_b64 s[0:1], 0.5 ; encoding: [0xf0,0x01,0x80,0xbe]
s_mov_b64_e32 s[0:1], 0.5

// SICI: v_and_b32_e32 v0, 0.5, v1 ; encoding: [0xf0,0x02,0x00,0x36]
// VI: v_and_b32_e32 v0, 0.5, v1 ; encoding: [0xf0,0x02,0x00,0x26]
v_and_b32_e32 v0, 0.5, v1

// SICI: v_and_b32_e64 v0, 0.5, v1 ; encoding: [0x00,0x00,0x36,0xd2,0xf0,0x02,0x02,0x00]
// VI: v_and_b32_e64 v0, 0.5, v1 ; encoding: [0x00,0x00,0x13,0xd1,0xf0,0x02,0x02,0x00]
v_and_b32_e64 v0, 0.5, v1

// SICI: s_mov_b64 s[0:1], -1.0 ; encoding: [0xf3,0x04,0x80,0xbe]
// VI: s_mov_b64 s[0:1], -1.0 ; encoding: [0xf3,0x01,0x80,0xbe]
s_mov_b64_e32 s[0:1], -1.0

// SICI: v_and_b32_e32 v0, -1.0, v1 ; encoding: [0xf3,0x02,0x00,0x36]
// VI: v_and_b32_e32 v0, -1.0, v1 ; encoding: [0xf3,0x02,0x00,0x26]
v_and_b32_e32 v0, -1.0, v1

// SICI: v_and_b32_e64 v0, -1.0, v1 ; encoding: [0x00,0x00,0x36,0xd2,0xf3,0x02,0x02,0x00]
// VI: v_and_b32_e64 v0, -1.0, v1 ; encoding: [0x00,0x00,0x13,0xd1,0xf3,0x02,0x02,0x00]
v_and_b32_e64 v0, -1.0, v1

// SICI: s_mov_b64 s[0:1], 4.0 ; encoding: [0xf6,0x04,0x80,0xbe]
// VI: s_mov_b64 s[0:1], 4.0 ; encoding: [0xf6,0x01,0x80,0xbe]
s_mov_b64_e32 s[0:1], 4.0

// SICI: v_and_b32_e32 v0, 4.0, v1 ; encoding: [0xf6,0x02,0x00,0x36]
// VI: v_and_b32_e32 v0, 4.0, v1 ; encoding: [0xf6,0x02,0x00,0x26]
v_and_b32_e32 v0, 4.0, v1

// SICI: v_and_b32_e64 v0, 4.0, v1 ; encoding: [0x00,0x00,0x36,0xd2,0xf6,0x02,0x02,0x00]
// VI: v_and_b32_e64 v0, 4.0, v1 ; encoding: [0x00,0x00,0x13,0xd1,0xf6,0x02,0x02,0x00]
v_and_b32_e64 v0, 4.0, v1

// SICI: s_mov_b64 s[0:1], 0 ; encoding: [0x80,0x04,0x80,0xbe]
// VI: s_mov_b64 s[0:1], 0 ; encoding: [0x80,0x01,0x80,0xbe]
s_mov_b64_e32 s[0:1], 0.0

// SICI: v_and_b32_e32 v0, 0, v1 ; encoding: [0x80,0x02,0x00,0x36]
// VI: v_and_b32_e32 v0, 0, v1 ; encoding: [0x80,0x02,0x00,0x26]
v_and_b32_e32 v0, 0.0, v1

// SICI: v_and_b32_e64 v0, 0, v1 ; encoding: [0x00,0x00,0x36,0xd2,0x80,0x02,0x02,0x00]
// VI: v_and_b32_e64 v0, 0, v1 ; encoding: [0x00,0x00,0x13,0xd1,0x80,0x02,0x02,0x00]
v_and_b32_e64 v0, 0.0, v1

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
s_mov_b64_e32 s[0:1], 1.5

// SICI: v_and_b32_e32 v0, 0x3fc00000, v1 ; encoding: [0xff,0x02,0x00,0x36,0x00,0x00,0xc0,0x3f]
// VI: v_and_b32_e32 v0, 0x3fc00000, v1 ; encoding: [0xff,0x02,0x00,0x26,0x00,0x00,0xc0,0x3f]
v_and_b32_e32 v0, 1.5, v1

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
s_mov_b64_e32 s[0:1], -3.1415

// SICI: v_and_b32_e32 v0, 0xc0490e56, v1 ; encoding: [0xff,0x02,0x00,0x36,0x56,0x0e,0x49,0xc0]
// VI: v_and_b32_e32 v0, 0xc0490e56, v1 ; encoding: [0xff,0x02,0x00,0x26,0x56,0x0e,0x49,0xc0]
v_and_b32_e32 v0, -3.1415, v1

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
s_mov_b64_e32 s[0:1], 100000000000000000000000.0

// SICI: v_and_b32_e32 v0, 0x65a96816, v1 ; encoding: [0xff,0x02,0x00,0x36,0x16,0x68,0xa9,0x65]
// VI: v_and_b32_e32 v0, 0x65a96816, v1 ; encoding: [0xff,0x02,0x00,0x26,0x16,0x68,0xa9,0x65]
v_and_b32_e32 v0, 100000000000000000000000.0, v1

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
s_mov_b64_e32 s[0:1], 10000000.0

// SICI: v_and_b32_e32 v0, 0x4b189680, v1 ; encoding: [0xff,0x02,0x00,0x36,0x80,0x96,0x18,0x4b]
// VI: v_and_b32_e32 v0, 0x4b189680, v1 ; encoding: [0xff,0x02,0x00,0x26,0x80,0x96,0x18,0x4b]
v_and_b32_e32 v0, 10000000.0, v1

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
s_mov_b64_e32 s[0:1], 3.402823e+38

// SICI: v_and_b32_e32 v0, 0x7f7ffffd, v1 ; encoding: [0xff,0x02,0x00,0x36,0xfd,0xff,0x7f,0x7f]
// VI: v_and_b32_e32 v0, 0x7f7ffffd, v1 ; encoding: [0xff,0x02,0x00,0x26,0xfd,0xff,0x7f,0x7f]
v_and_b32_e32 v0, 3.402823e+38, v1

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
s_mov_b64_e32 s[0:1], 2.3509886e-38

// SICI: v_and_b32_e32 v0, 0xffffff, v1 ; encoding: [0xff,0x02,0x00,0x36,0xff,0xff,0xff,0x00]
// VI: v_and_b32_e32 v0, 0xffffff, v1 ; encoding: [0xff,0x02,0x00,0x26,0xff,0xff,0xff,0x00]
v_and_b32_e32 v0, 2.3509886e-38, v1

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
s_mov_b64_e32 s[0:1], 2.3509886e-70

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
v_and_b32_e32 v0, 2.3509886e-70, v1

//---------------------------------------------------------------------------//
// int literal, expected fp operand
//---------------------------------------------------------------------------//

// SICI: v_trunc_f32_e32 v0, 0 ; encoding: [0x80,0x42,0x00,0x7e]
// VI: v_trunc_f32_e32 v0, 0 ; encoding: [0x80,0x38,0x00,0x7e]
v_trunc_f32_e32 v0, 0

// SICI: v_fract_f64_e32 v[0:1], 0 ; encoding: [0x80,0x7c,0x00,0x7e]
// VI: v_fract_f64_e32 v[0:1], 0 ; encoding: [0x80,0x64,0x00,0x7e]
v_fract_f64_e32 v[0:1], 0

// SICI: v_trunc_f32_e64 v0, 0 ; encoding: [0x00,0x00,0x42,0xd3,0x80,0x00,0x00,0x00]
// VI: v_trunc_f32_e64 v0, 0 ; encoding: [0x00,0x00,0x5c,0xd1,0x80,0x00,0x00,0x00]
v_trunc_f32_e64 v0, 0

// SICI: v_fract_f64_e64 v[0:1], 0 ; encoding: [0x00,0x00,0x7c,0xd3,0x80,0x00,0x00,0x00]
// VI: v_fract_f64_e64 v[0:1], 0 ; encoding: [0x00,0x00,0x72,0xd1,0x80,0x00,0x00,0x00]
v_fract_f64_e64 v[0:1], 0

// SICI: v_trunc_f32_e32 v0, -13 ; encoding: [0xcd,0x42,0x00,0x7e]
// VI: v_trunc_f32_e32 v0, -13 ; encoding: [0xcd,0x38,0x00,0x7e]
v_trunc_f32_e32 v0, -13

// SICI: v_fract_f64_e32 v[0:1], -13 ; encoding: [0xcd,0x7c,0x00,0x7e]
// VI: v_fract_f64_e32 v[0:1], -13 ; encoding: [0xcd,0x64,0x00,0x7e]
v_fract_f64_e32 v[0:1], -13

// SICI: v_trunc_f32_e64 v0, -13 ; encoding: [0x00,0x00,0x42,0xd3,0x8d,0x00,0x00,0x20]
// VI: v_trunc_f32_e64 v0, -13 ; encoding: [0x00,0x00,0x5c,0xd1,0x8d,0x00,0x00,0x20]
v_trunc_f32_e64 v0, -13

// SICI: v_fract_f64_e64 v[0:1], -13 ; encoding: [0x00,0x00,0x7c,0xd3,0x8d,0x00,0x00,0x20]
// VI: v_fract_f64_e64 v[0:1], -13 ; encoding: [0x00,0x00,0x72,0xd1,0x8d,0x00,0x00,0x20]
v_fract_f64_e64 v[0:1], -13

// SICI: v_trunc_f32_e32 v0, 35 ; encoding: [0xa3,0x42,0x00,0x7e]
// VI: v_trunc_f32_e32 v0, 35 ; encoding: [0xa3,0x38,0x00,0x7e]
v_trunc_f32_e32 v0, 35

// SICI: v_fract_f64_e32 v[0:1], 35 ; encoding: [0xa3,0x7c,0x00,0x7e]
// VI: v_fract_f64_e32 v[0:1], 35 ; encoding: [0xa3,0x64,0x00,0x7e]
v_fract_f64_e32 v[0:1], 35

// SICI: v_trunc_f32_e64 v0, 35 ; encoding: [0x00,0x00,0x42,0xd3,0xa3,0x00,0x00,0x00]
// VI: v_trunc_f32_e64 v0, 35 ; encoding: [0x00,0x00,0x5c,0xd1,0xa3,0x00,0x00,0x00]
v_trunc_f32_e64 v0, 35

// SICI: v_fract_f64_e64 v[0:1], 35 ; encoding: [0x00,0x00,0x7c,0xd3,0xa3,0x00,0x00,0x00]
// VI: v_fract_f64_e64 v[0:1], 35 ; encoding: [0x00,0x00,0x72,0xd1,0xa3,0x00,0x00,0x00]
v_fract_f64_e64 v[0:1], 35

// SICI: v_trunc_f32_e32 v0, 0x4d2 ; encoding: [0xff,0x42,0x00,0x7e,0xd2,0x04,0x00,0x00]
// VI: v_trunc_f32_e32 v0, 0x4d2 ; encoding: [0xff,0x38,0x00,0x7e,0xd2,0x04,0x00,0x00]
v_trunc_f32_e32 v0, 1234

// SICI: v_fract_f64_e32 v[0:1], 0x4d2 ; encoding: [0xff,0x7c,0x00,0x7e,0xd2,0x04,0x00,0x00]
// VI: v_fract_f64_e32 v[0:1], 0x4d2 ; encoding: [0xff,0x64,0x00,0x7e,0xd2,0x04,0x00,0x00]
v_fract_f64_e32 v[0:1], 1234

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
v_trunc_f32_e64 v0, 1234

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
v_fract_f64_e64 v[0:1], 1234

// SICI: v_trunc_f32_e32 v0, 0xffff2bcf ; encoding: [0xff,0x42,0x00,0x7e,0xcf,0x2b,0xff,0xff]
// VI: v_trunc_f32_e32 v0, 0xffff2bcf ; encoding: [0xff,0x38,0x00,0x7e,0xcf,0x2b,0xff,0xff]
v_trunc_f32_e32 v0, -54321

// SICI: v_fract_f64_e32 v[0:1], 0xffff2bcf ; encoding: [0xff,0x7c,0x00,0x7e,0xcf,0x2b,0xff,0xff]
// VI: v_fract_f64_e32 v[0:1], 0xffff2bcf ; encoding: [0xff,0x64,0x00,0x7e,0xcf,0x2b,0xff,0xff]
v_fract_f64_e32 v[0:1], -54321

// SICI: v_trunc_f32_e32 v0, 0xdeadbeef ; encoding: [0xff,0x42,0x00,0x7e,0xef,0xbe,0xad,0xde]
// VI: v_trunc_f32_e32 v0, 0xdeadbeef ; encoding: [0xff,0x38,0x00,0x7e,0xef,0xbe,0xad,0xde]
v_trunc_f32_e32 v0, 0xdeadbeef

// SICI: v_fract_f64_e32 v[0:1], 0xdeadbeef ; encoding: [0xff,0x7c,0x00,0x7e,0xef,0xbe,0xad,0xde]
// VI: v_fract_f64_e32 v[0:1], 0xdeadbeef ; encoding: [0xff,0x64,0x00,0x7e,0xef,0xbe,0xad,0xde]
v_fract_f64_e32 v[0:1], 0xdeadbeef

// SICI: v_trunc_f32_e32 v0, -1 ; encoding: [0xc1,0x42,0x00,0x7e]
// VI: v_trunc_f32_e32 v0, -1 ; encoding: [0xc1,0x38,0x00,0x7e]
v_trunc_f32_e32 v0, 0xffffffff

// SICI: v_fract_f64_e32 v[0:1], 0xffffffff ; encoding: [0xff,0x7c,0x00,0x7e,0xff,0xff,0xff,0xff]
// VI: v_fract_f64_e32 v[0:1], 0xffffffff ; encoding: [0xff,0x64,0x00,0x7e,0xff,0xff,0xff,0xff]
v_fract_f64_e32 v[0:1], 0xffffffff

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
v_trunc_f32_e32 v0, 0x123456789abcdef0

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
v_fract_f64_e32 v[0:1], 0x123456789abcdef0

// SICI: v_trunc_f32_e32 v0, -1 ; encoding: [0xc1,0x42,0x00,0x7e]
// VI: v_trunc_f32_e32 v0, -1 ; encoding: [0xc1,0x38,0x00,0x7e]
v_trunc_f32_e32 v0, 0xffffffffffffffff

// SICI: v_fract_f64_e32 v[0:1], -1 ; encoding: [0xc1,0x7c,0x00,0x7e]
// VI: v_fract_f64_e32 v[0:1], -1 ; encoding: [0xc1,0x64,0x00,0x7e]
v_fract_f64_e32 v[0:1], 0xffffffffffffffff

//---------------------------------------------------------------------------//
// int literal, expected int operand
//---------------------------------------------------------------------------//

// SICI: s_mov_b64 s[0:1], 0 ; encoding: [0x80,0x04,0x80,0xbe]
// VI: s_mov_b64 s[0:1], 0 ; encoding: [0x80,0x01,0x80,0xbe]
s_mov_b64_e32 s[0:1], 0

// SICI: v_and_b32_e32 v0, 0, v1 ; encoding: [0x80,0x02,0x00,0x36]
// VI: v_and_b32_e32 v0, 0, v1 ; encoding: [0x80,0x02,0x00,0x26]
v_and_b32_e32 v0, 0, v1

// SICI: v_and_b32_e64 v0, 0, v1 ; encoding: [0x00,0x00,0x36,0xd2,0x80,0x02,0x02,0x00]
// VI: v_and_b32_e64 v0, 0, v1 ; encoding: [0x00,0x00,0x13,0xd1,0x80,0x02,0x02,0x00]
v_and_b32_e64 v0, 0, v1

// SICI: s_mov_b64 s[0:1], -13 ; encoding: [0xcd,0x04,0x80,0xbe]
// VI: s_mov_b64 s[0:1], -13 ; encoding: [0xcd,0x01,0x80,0xbe]
s_mov_b64_e32 s[0:1], -13

// SICI: v_and_b32_e32 v0, -13, v1 ; encoding: [0xcd,0x02,0x00,0x36]
// VI: v_and_b32_e32 v0, -13, v1 ; encoding: [0xcd,0x02,0x00,0x26]
v_and_b32_e32 v0, -13, v1

// SICI: v_and_b32_e64 v0, -13, v1 ; encoding: [0x00,0x00,0x36,0xd2,0xcd,0x02,0x02,0x00]
// VI: v_and_b32_e64 v0, -13, v1 ; encoding: [0x00,0x00,0x13,0xd1,0xcd,0x02,0x02,0x00]
v_and_b32_e64 v0, -13, v1

// SICI: s_mov_b64 s[0:1], 35 ; encoding: [0xa3,0x04,0x80,0xbe]
// VI: s_mov_b64 s[0:1], 35 ; encoding: [0xa3,0x01,0x80,0xbe]
s_mov_b64_e32 s[0:1], 35

// SICI: v_and_b32_e32 v0, 35, v1 ; encoding: [0xa3,0x02,0x00,0x36]
// VI: v_and_b32_e32 v0, 35, v1 ; encoding: [0xa3,0x02,0x00,0x26]
v_and_b32_e32 v0, 35, v1

// SICI: v_and_b32_e64 v0, 35, v1 ; encoding: [0x00,0x00,0x36,0xd2,0xa3,0x02,0x02,0x00]
// VI: v_and_b32_e64 v0, 35, v1 ; encoding: [0x00,0x00,0x13,0xd1,0xa3,0x02,0x02,0x00]
v_and_b32_e64 v0, 35, v1

// SICI: s_mov_b64 s[0:1], 0x4d2 ; encoding: [0xff,0x04,0x80,0xbe,0xd2,0x04,0x00,0x00]
// VI: s_mov_b64 s[0:1], 0x4d2 ; encoding: [0xff,0x01,0x80,0xbe,0xd2,0x04,0x00,0x00]
s_mov_b64_e32 s[0:1], 1234

// SICI: v_and_b32_e32 v0, 0x4d2, v1 ; encoding: [0xff,0x02,0x00,0x36,0xd2,0x04,0x00,0x00]
// VI: v_and_b32_e32 v0, 0x4d2, v1 ; encoding: [0xff,0x02,0x00,0x26,0xd2,0x04,0x00,0x00]
v_and_b32_e32 v0, 1234, v1

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
v_and_b32_e64 v0, 1234, v1

// SICI: s_mov_b64 s[0:1], 0xffff2bcf ; encoding: [0xff,0x04,0x80,0xbe,0xcf,0x2b,0xff,0xff]
// VI: s_mov_b64 s[0:1], 0xffff2bcf ; encoding: [0xff,0x01,0x80,0xbe,0xcf,0x2b,0xff,0xff]
s_mov_b64_e32 s[0:1], -54321

// SICI: v_and_b32_e32 v0, 0xffff2bcf, v1 ; encoding: [0xff,0x02,0x00,0x36,0xcf,0x2b,0xff,0xff]
// VI: v_and_b32_e32 v0, 0xffff2bcf, v1 ; encoding: [0xff,0x02,0x00,0x26,0xcf,0x2b,0xff,0xff]
v_and_b32_e32 v0, -54321, v1

// SICI: s_mov_b64 s[0:1], 0xdeadbeef ; encoding: [0xff,0x04,0x80,0xbe,0xef,0xbe,0xad,0xde]
// VI: s_mov_b64 s[0:1], 0xdeadbeef ; encoding: [0xff,0x01,0x80,0xbe,0xef,0xbe,0xad,0xde]
s_mov_b64_e32 s[0:1], 0xdeadbeef

// SICI: v_and_b32_e32 v0, 0xdeadbeef, v1 ; encoding: [0xff,0x02,0x00,0x36,0xef,0xbe,0xad,0xde]
// VI: v_and_b32_e32 v0, 0xdeadbeef, v1 ; encoding: [0xff,0x02,0x00,0x26,0xef,0xbe,0xad,0xde]
v_and_b32_e32 v0, 0xdeadbeef, v1

// SICI: s_mov_b64 s[0:1], 0xffffffff ; encoding: [0xff,0x04,0x80,0xbe,0xff,0xff,0xff,0xff]
// VI: s_mov_b64 s[0:1], 0xffffffff ; encoding: [0xff,0x01,0x80,0xbe,0xff,0xff,0xff,0xff]
s_mov_b64_e32 s[0:1], 0xffffffff

// SICI: v_and_b32_e32 v0, -1, v1 ; encoding: [0xc1,0x02,0x00,0x36]
// VI: v_and_b32_e32 v0, -1, v1 ; encoding: [0xc1,0x02,0x00,0x26]
v_and_b32_e32 v0, 0xffffffff, v1

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
s_mov_b64_e32 s[0:1], 0x123456789abcdef0

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
v_and_b32_e32 v0, 0x123456789abcdef0, v1

// SICI: s_mov_b64 s[0:1], -1 ; encoding: [0xc1,0x04,0x80,0xbe]
// VI: s_mov_b64 s[0:1], -1 ; encoding: [0xc1,0x01,0x80,0xbe]
s_mov_b64_e32 s[0:1], 0xffffffffffffffff

// SICI: v_and_b32_e32 v0, -1, v1 ; encoding: [0xc1,0x02,0x00,0x36]
// VI: v_and_b32_e32 v0, -1, v1 ; encoding: [0xc1,0x02,0x00,0x26]
v_and_b32_e32 v0, 0xffffffffffffffff, v1

//---------------------------------------------------------------------------//
// 1/(2*PI)
//---------------------------------------------------------------------------//

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
v_trunc_f32_e32 v0, 0x3fc45f306dc9c882

// NOSICI: error: invalid operand for instruction
// VI: v_fract_f64_e32 v[0:1], 0x3fc45f306dc9c882 ; encoding: [0xf8,0x64,0x00,0x7e]
v_fract_f64_e32 v[0:1], 0x3fc45f306dc9c882

// SICI: v_trunc_f32_e32 v0, 0x3e22f983 ; encoding: [0xff,0x42,0x00,0x7e,0x83,0xf9,0x22,0x3e]
// VI: v_trunc_f32_e32 v0, 0x3e22f983 ; encoding: [0xf8,0x38,0x00,0x7e]
v_trunc_f32_e32 v0, 0x3e22f983

// SICI: v_fract_f64_e32 v[0:1], 0x3e22f983 ; encoding: [0xff,0x7c,0x00,0x7e,0x83,0xf9,0x22,0x3e]
// VI: v_fract_f64_e32 v[0:1], 0x3e22f983 ; encoding: [0xff,0x64,0x00,0x7e,0x83,0xf9,0x22,0x3e]
v_fract_f64_e32 v[0:1], 0x3e22f983

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
v_trunc_f32_e64 v0, 0x3fc45f306dc9c882

// NOSICI: error: invalid operand for instruction
// VI: v_fract_f64_e64 v[0:1], 0x3fc45f306dc9c882 ; encoding: [0x00,0x00,0x72,0xd1,0xf8,0x00,0x00,0x00]
v_fract_f64_e64 v[0:1], 0x3fc45f306dc9c882

// NOSICI: error: invalid operand for instruction
// VI: v_trunc_f32_e64 v0, 0x3e22f983 ; encoding: [0x00,0x00,0x5c,0xd1,0xf8,0x00,0x00,0x00]
v_trunc_f32_e64 v0, 0x3e22f983

// NOSICI: error: invalid operand for instruction
// NOVI: error: invalid operand for instruction
v_fract_f64_e64 v[0:1], 0x3e22f983

// NOSICI: error: invalid operand for instruction
// VI: s_mov_b64 s[0:1], 0x3fc45f306dc9c882 ; encoding: [0xf8,0x01,0x80,0xbe]
s_mov_b64_e32 s[0:1], 0.159154943091895317852646485335

// SICI: v_and_b32_e32 v0, 0x3e22f983, v1 ; encoding: [0xff,0x02,0x00,0x36,0x83,0xf9,0x22,0x3e]
// VI: v_and_b32_e32 v0, 0x3e22f983, v1 ; encoding: [0xf8,0x02,0x00,0x26]
v_and_b32_e32 v0, 0.159154943091895317852646485335, v1

// NOSICI: error: invalid operand for instruction
// VI: v_and_b32_e64 v0, 0x3e22f983, v1 ; encoding: [0x00,0x00,0x13,0xd1,0xf8,0x02,0x02,0x00]
v_and_b32_e64 v0, 0.159154943091895317852646485335, v1

// SICI: v_fract_f64_e32 v[0:1], 0x3fc45f30 ; encoding: [0xff,0x7c,0x00,0x7e,0x30,0x5f,0xc4,0x3f]
// VI: v_fract_f64_e32 v[0:1], 0x3fc45f306dc9c882 ; encoding: [0xf8,0x64,0x00,0x7e]
v_fract_f64 v[0:1], 0.159154943091895317852646485335

// SICI: v_trunc_f32_e32 v0, 0x3e22f983 ; encoding: [0xff,0x42,0x00,0x7e,0x83,0xf9,0x22,0x3e]
// VI: v_trunc_f32_e32 v0, 0x3e22f983 ; encoding: [0xf8,0x38,0x00,0x7e]
v_trunc_f32 v0, 0.159154943091895317852646485335