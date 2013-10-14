// RUN: %clang_cc1 -fno-rtti -emit-llvm-only -triple i686-pc-win32 -fdump-record-layouts -fsyntax-only -cxx-abi microsoft %s 2>&1 \
// RUN:            | FileCheck %s

extern "C" int printf(const char *fmt, ...);

struct __declspec(align(8)) B0 { B0() {printf("B0 : %p\n", this);} };
struct __declspec(align(8)) B1 { B1() {printf("B1 : %p\n", this);} };
struct __declspec(align(8)) B2 { B2() {printf("B2 : %p\n", this);} };
struct __declspec(align(8)) B3 { B3() {printf("B3 : %p\n", this);} };
struct __declspec(align(8)) B4 { B4() {printf("B4 : %p\n", this);} };

struct C0 { int a; C0() : a(0xf00000C0) {printf("C0 : %p\n", this);} };
struct C1 { int a; C1() : a(0xf00000C1) {printf("C1 : %p\n", this);} };
struct C2 { int a; C2() : a(0xf00000C2) {printf("C2 : %p\n", this);} };
struct C3 { int a; C3() : a(0xf00000C3) {printf("C3 : %p\n", this);} };
struct C4 { int a; C4() : a(0xf00000C4) {printf("C4 : %p\n", this);} };

struct A : B0 {
	int a;
	A() : a(0xf000000A) {printf("X : %p\n", this);}
};

// CHECK: *** Dumping AST Record Layout
// CHECK:    0 | struct A
// CHECK:    0 |   struct B0 (base) (empty)
// CHECK:    0 |   int a
// CHECK:      | [sizeof=8, align=8
// CHECK:      |  nvsize=8, nvalign=8]

struct B : B0 {
	B0 b0;
	int a;
	B() : a(0xf000000B) {printf("X : %p\n", this);}
};

// CHECK: *** Dumping AST Record Layout
// CHECK:    0 | struct B
// CHECK:    0 |   struct B0 (base) (empty)
// CHECK:    0 |   struct B0 b0 (empty)
// CHECK:      |   [sizeof=8, align=8
// CHECK:      |    nvsize=0, nvalign=1]
// CHECK:    8 |   int a
// CHECK:      | [sizeof=16, align=8
// CHECK:      |  nvsize=16, nvalign=8]

struct C : B0, B1, B2, B3, B4 {
	int a;
	C() : a(0xf000000C) {printf("X : %p\n", this);}
};

// CHECK: *** Dumping AST Record Layout
// CHECK:    0 | struct C
// CHECK:    0 |   struct B0 (base) (empty)
// CHECK:    8 |   struct B1 (base) (empty)
// CHECK:   16 |   struct B2 (base) (empty)
// CHECK:   24 |   struct B3 (base) (empty)
// CHECK:   32 |   struct B4 (base) (empty)
// CHECK:   32 |   int a
// CHECK:      | [sizeof=40, align=8
// CHECK:      |  nvsize=40, nvalign=8]

struct D {
	B0 b0;
	C0 c0;
	C1 c1;
	C2 c2;
	B1 b1;
	int a;
	D() : a(0xf000000D) {printf("X : %p\n", this);}
};

// CHECK: *** Dumping AST Record Layout
// CHECK:    0 | struct D
// CHECK:    0 |   struct B0 b0 (empty)
// CHECK:      |   [sizeof=8, align=8
// CHECK:      |    nvsize=0, nvalign=1]
// CHECK:    8 |   struct C0 c0
// CHECK:    8 |     int a
// CHECK:      |   [sizeof=4, align=4
// CHECK:      |    nvsize=4, nvalign=4]
// CHECK:   12 |   struct C1 c1
// CHECK:   12 |     int a
// CHECK:      |   [sizeof=4, align=4
// CHECK:      |    nvsize=4, nvalign=4]
// CHECK:   16 |   struct C2 c2
// CHECK:   16 |     int a
// CHECK:      |   [sizeof=4, align=4
// CHECK:      |    nvsize=4, nvalign=4]
// CHECK:   24 |   struct B1 b1 (empty)
// CHECK:      |   [sizeof=8, align=8
// CHECK:      |    nvsize=0, nvalign=1]
// CHECK:   32 |   int a
// CHECK:      | [sizeof=40, align=8
// CHECK:      |  nvsize=40, nvalign=8]

struct E : B0, C0, C1, C2, B1 {
	int a;
	E() : a(0xf000000E) {printf("X : %p\n", this);}
};

// CHECK: *** Dumping AST Record Layout
// CHECK:    0 | struct E
// CHECK:    0 |   struct B0 (base) (empty)
// CHECK:    0 |   struct C0 (base)
// CHECK:    0 |     int a
// CHECK:    4 |   struct C1 (base)
// CHECK:    4 |     int a
// CHECK:    8 |   struct C2 (base)
// CHECK:    8 |     int a
// CHECK:   16 |   struct B1 (base) (empty)
// CHECK:   16 |   int a
// CHECK:      | [sizeof=24, align=8
// CHECK:      |  nvsize=24, nvalign=8]

struct F : C0, B0, B1, C1 {
	int a;
	F() : a(0xf000000F) {printf("X : %p\n", this);}
};

// CHECK: *** Dumping AST Record Layout
// CHECK:    0 | struct F
// CHECK:    0 |   struct C0 (base)
// CHECK:    0 |     int a
// CHECK:    8 |   struct B0 (base) (empty)
// CHECK:   16 |   struct B1 (base) (empty)
// CHECK:   16 |   struct C1 (base)
// CHECK:   16 |     int a
// CHECK:   20 |   int a
// CHECK:      | [sizeof=24, align=8
// CHECK:      |  nvsize=24, nvalign=8]

struct G : B0, B1, B2, B3, B4 {
	__declspec(align(32)) int a;
	G() : a(0xf0000011) {printf("X : %p\n", this);}
};

// CHECK: *** Dumping AST Record Layout
// CHECK:    0 | struct G
// CHECK:    0 |   struct B0 (base) (empty)
// CHECK:    8 |   struct B1 (base) (empty)
// CHECK:   16 |   struct B2 (base) (empty)
// CHECK:   24 |   struct B3 (base) (empty)
// CHECK:   32 |   struct B4 (base) (empty)
// CHECK:   32 |   int a
// CHECK:      | [sizeof=64, align=32
// CHECK:      |  nvsize=64, nvalign=32]

struct __declspec(align(32)) H : B0, B1, B2, B3, B4 {
	int a;
	H() : a(0xf0000011) {printf("X : %p\n", this);}
};

// CHECK: *** Dumping AST Record Layout
// CHECK:    0 | struct H
// CHECK:    0 |   struct B0 (base) (empty)
// CHECK:    8 |   struct B1 (base) (empty)
// CHECK:   16 |   struct B2 (base) (empty)
// CHECK:   24 |   struct B3 (base) (empty)
// CHECK:   32 |   struct B4 (base) (empty)
// CHECK:   32 |   int a
// CHECK:      | [sizeof=64, align=32
// CHECK:      |  nvsize=40, nvalign=8]

int a[
sizeof(A)+
sizeof(B)+
sizeof(C)+
sizeof(D)+
sizeof(E)+
sizeof(F)+
sizeof(G)+
sizeof(H)];
