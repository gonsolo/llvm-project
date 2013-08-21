// RUN: %clang_cc1 -triple x86_64-apple-darwin -O1 -disable-llvm-optzns %s -emit-llvm -o - | FileCheck %s
// RUN: %clang_cc1 -triple x86_64-apple-darwin -O1 -struct-path-tbaa -disable-llvm-optzns %s -emit-llvm -o - | FileCheck %s -check-prefix=PATH
// Test TBAA metadata generated by front-end.

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef struct
{
   uint16_t f16;
   uint32_t f32;
   uint16_t f16_2;
   uint32_t f32_2;
} StructA;
typedef struct
{
   uint16_t f16;
   StructA a;
   uint32_t f32;
} StructB;
typedef struct
{
   uint16_t f16;
   StructB b;
   uint32_t f32;
} StructC;
typedef struct
{
   uint16_t f16;
   StructB b;
   uint32_t f32;
   uint8_t f8;
} StructD;

typedef struct
{
   uint16_t f16;
   uint32_t f32;
} StructS;
typedef struct
{
   uint16_t f16;
   uint32_t f32;
} StructS2;

uint32_t g(uint32_t *s, StructA *A, uint64_t count) {
// CHECK: define i32 @{{.*}}(
// CHECK: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_i32:!.*]]
// CHECK: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// PATH: define i32 @{{.*}}(
// PATH: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_i32:!.*]]
// PATH: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_A_f32:!.*]]
  *s = 1;
  A->f32 = 4;
  return *s;
}

uint32_t g2(uint32_t *s, StructA *A, uint64_t count) {
// CHECK: define i32 @{{.*}}(
// CHECK: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// CHECK: store i16 4, i16* %{{.*}}, align 2, !tbaa [[TAG_i16:!.*]]
// PATH: define i32 @{{.*}}(
// PATH: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// PATH: store i16 4, i16* %{{.*}}, align 2, !tbaa [[TAG_A_f16:!.*]]
  *s = 1;
  A->f16 = 4;
  return *s;
}

uint32_t g3(StructA *A, StructB *B, uint64_t count) {
// CHECK: define i32 @{{.*}}(
// CHECK: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// CHECK: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// PATH: define i32 @{{.*}}(
// PATH: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_A_f32]]
// PATH: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_B_a_f32:!.*]]
  A->f32 = 1;
  B->a.f32 = 4;
  return A->f32;
}

uint32_t g4(StructA *A, StructB *B, uint64_t count) {
// CHECK: define i32 @{{.*}}(
// CHECK: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// CHECK: store i16 4, i16* %{{.*}}, align 2, !tbaa [[TAG_i16]]
// PATH: define i32 @{{.*}}(
// PATH: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_A_f32]]
// PATH: store i16 4, i16* %{{.*}}, align 2, !tbaa [[TAG_B_a_f16:!.*]]
  A->f32 = 1;
  B->a.f16 = 4;
  return A->f32;
}

uint32_t g5(StructA *A, StructB *B, uint64_t count) {
// CHECK: define i32 @{{.*}}(
// CHECK: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// CHECK: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// PATH: define i32 @{{.*}}(
// PATH: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_A_f32]]
// PATH: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_B_f32:!.*]]
  A->f32 = 1;
  B->f32 = 4;
  return A->f32;
}

uint32_t g6(StructA *A, StructB *B, uint64_t count) {
// CHECK: define i32 @{{.*}}(
// CHECK: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// CHECK: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// PATH: define i32 @{{.*}}(
// PATH: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_A_f32]]
// PATH: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_B_a_f32_2:!.*]]
  A->f32 = 1;
  B->a.f32_2 = 4;
  return A->f32;
}

uint32_t g7(StructA *A, StructS *S, uint64_t count) {
// CHECK: define i32 @{{.*}}(
// CHECK: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// CHECK: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// PATH: define i32 @{{.*}}(
// PATH: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_A_f32]]
// PATH: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_S_f32:!.*]]
  A->f32 = 1;
  S->f32 = 4;
  return A->f32;
}

uint32_t g8(StructA *A, StructS *S, uint64_t count) {
// CHECK: define i32 @{{.*}}(
// CHECK: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// CHECK: store i16 4, i16* %{{.*}}, align 2, !tbaa [[TAG_i16]]
// PATH: define i32 @{{.*}}(
// PATH: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_A_f32]]
// PATH: store i16 4, i16* %{{.*}}, align 2, !tbaa [[TAG_S_f16:!.*]]
  A->f32 = 1;
  S->f16 = 4;
  return A->f32;
}

uint32_t g9(StructS *S, StructS2 *S2, uint64_t count) {
// CHECK: define i32 @{{.*}}(
// CHECK: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// CHECK: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// PATH: define i32 @{{.*}}(
// PATH: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_S_f32]]
// PATH: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_S2_f32:!.*]]
  S->f32 = 1;
  S2->f32 = 4;
  return S->f32;
}

uint32_t g10(StructS *S, StructS2 *S2, uint64_t count) {
// CHECK: define i32 @{{.*}}(
// CHECK: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// CHECK: store i16 4, i16* %{{.*}}, align 2, !tbaa [[TAG_i16]]
// PATH: define i32 @{{.*}}(
// PATH: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_S_f32]]
// PATH: store i16 4, i16* %{{.*}}, align 2, !tbaa [[TAG_S2_f16:!.*]]
  S->f32 = 1;
  S2->f16 = 4;
  return S->f32;
}

uint32_t g11(StructC *C, StructD *D, uint64_t count) {
// CHECK: define i32 @{{.*}}(
// CHECK: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// CHECK: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// PATH: define i32 @{{.*}}(
// PATH: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_C_b_a_f32:!.*]]
// PATH: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_D_b_a_f32:!.*]]
  C->b.a.f32 = 1;
  D->b.a.f32 = 4;
  return C->b.a.f32;
}

uint32_t g12(StructC *C, StructD *D, uint64_t count) {
// CHECK: define i32 @{{.*}}(
// CHECK: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// CHECK: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// TODO: differentiate the two accesses.
// PATH: define i32 @{{.*}}(
// PATH: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_B_a_f32]]
// PATH: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_B_a_f32]]
  StructB *b1 = &(C->b);
  StructB *b2 = &(D->b);
  // b1, b2 have different context.
  b1->a.f32 = 1;
  b2->a.f32 = 4;
  return b1->a.f32;
}

// Make sure that zero-length bitfield works.
#define ATTR __attribute__ ((ms_struct))
struct five {
  char a;
  int :0;        /* ignored; prior field is not a bitfield. */
  char b;
  char c;
} ATTR;
char g13(struct five *a, struct five *b) {
  return a->b;
// CHECK: define signext i8 @{{.*}}(
// CHECK: load i8* %{{.*}}, align 1, !tbaa [[TAG_char:!.*]]
// PATH: define signext i8 @{{.*}}(
// PATH: load i8* %{{.*}}, align 1, !tbaa [[TAG_five_b:!.*]]
}

struct six {
  char a;
  int :0;
  char b;
  char c;
};
char g14(struct six *a, struct six *b) {
// CHECK: define signext i8 @{{.*}}(
// CHECK: load i8* %{{.*}}, align 1, !tbaa [[TAG_char]]
// PATH: define signext i8 @{{.*}}(
// PATH: load i8* %{{.*}}, align 1, !tbaa [[TAG_six_b:!.*]]
  return a->b;
}

// Types that differ only by name may alias.
typedef StructS StructS3;
uint32_t g15(StructS *S, StructS3 *S3, uint64_t count) {
// CHECK: define i32 @{{.*}}(
// CHECK: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// CHECK: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_i32]]
// PATH: define i32 @{{.*}}(
// PATH: store i32 1, i32* %{{.*}}, align 4, !tbaa [[TAG_S_f32]]
// PATH: store i32 4, i32* %{{.*}}, align 4, !tbaa [[TAG_S_f32]]
  S->f32 = 1;
  S3->f32 = 4;
  return S->f32;
}

// CHECK: [[TAG_char]] = metadata !{metadata !"omnipotent char", metadata [[TAG_cxx_tbaa:!.*]]}
// CHECK: [[TAG_cxx_tbaa]] = metadata !{metadata !"Simple C/C++ TBAA"}
// CHECK: [[TAG_i32]] = metadata !{metadata !"int", metadata [[TAG_char]]}
// CHECK: [[TAG_i16]] = metadata !{metadata !"short", metadata [[TAG_char]]}

// PATH: [[TYPE_CHAR:!.*]] = metadata !{metadata !"omnipotent char", metadata
// PATH: [[TAG_i32]] = metadata !{metadata [[TYPE_INT:!.*]], metadata [[TYPE_INT]], i64 0}
// PATH: [[TYPE_INT]] = metadata !{metadata !"int", metadata [[TYPE_CHAR]]
// PATH: [[TAG_A_f32]] = metadata !{metadata [[TYPE_A:!.*]], metadata [[TYPE_INT]], i64 4}
// PATH: [[TYPE_A]] = metadata !{metadata !"_ZTS7StructA", metadata [[TYPE_SHORT:!.*]], i64 0, metadata [[TYPE_INT]], i64 4, metadata [[TYPE_SHORT]], i64 8, metadata [[TYPE_INT]], i64 12}
// PATH: [[TYPE_SHORT:!.*]] = metadata !{metadata !"short", metadata [[TYPE_CHAR]]
// PATH: [[TAG_A_f16]] = metadata !{metadata [[TYPE_A]], metadata [[TYPE_SHORT]], i64 0}
// PATH: [[TAG_B_a_f32]] = metadata !{metadata [[TYPE_B:!.*]], metadata [[TYPE_INT]], i64 8}
// PATH: [[TYPE_B]] = metadata !{metadata !"_ZTS7StructB", metadata [[TYPE_SHORT]], i64 0, metadata [[TYPE_A]], i64 4, metadata [[TYPE_INT]], i64 20}
// PATH: [[TAG_B_a_f16]] = metadata !{metadata [[TYPE_B]], metadata [[TYPE_SHORT]], i64 4}
// PATH: [[TAG_B_f32]] = metadata !{metadata [[TYPE_B]], metadata [[TYPE_INT]], i64 20}
// PATH: [[TAG_B_a_f32_2]] = metadata !{metadata [[TYPE_B]], metadata [[TYPE_INT]], i64 16}
// PATH: [[TAG_S_f32]] = metadata !{metadata [[TYPE_S:!.*]], metadata [[TYPE_INT]], i64 4}
// PATH: [[TYPE_S]] = metadata !{metadata !"_ZTS7StructS", metadata [[TYPE_SHORT]], i64 0, metadata [[TYPE_INT]], i64 4}
// PATH: [[TAG_S_f16]] = metadata !{metadata [[TYPE_S]], metadata [[TYPE_SHORT]], i64 0}
// PATH: [[TAG_S2_f32]] = metadata !{metadata [[TYPE_S2:!.*]], metadata [[TYPE_INT]], i64 4}
// PATH: [[TYPE_S2]] = metadata !{metadata !"_ZTS8StructS2", metadata [[TYPE_SHORT]], i64 0, metadata [[TYPE_INT]], i64 4}
// PATH: [[TAG_S2_f16]] = metadata !{metadata [[TYPE_S2]], metadata [[TYPE_SHORT]], i64 0}
// PATH: [[TAG_C_b_a_f32]] = metadata !{metadata [[TYPE_C:!.*]], metadata [[TYPE_INT]], i64 12}
// PATH: [[TYPE_C]] = metadata !{metadata !"_ZTS7StructC", metadata [[TYPE_SHORT]], i64 0, metadata [[TYPE_B]], i64 4, metadata [[TYPE_INT]], i64 28}
// PATH: [[TAG_D_b_a_f32]] = metadata !{metadata [[TYPE_D:!.*]], metadata [[TYPE_INT]], i64 12}
// PATH: [[TYPE_D]] = metadata !{metadata !"_ZTS7StructD", metadata [[TYPE_SHORT]], i64 0, metadata [[TYPE_B]], i64 4, metadata [[TYPE_INT]], i64 28, metadata [[TYPE_CHAR]], i64 32}
// PATH: [[TAG_five_b]] = metadata !{metadata [[TYPE_five:!.*]], metadata [[TYPE_CHAR]], i64 1}
// PATH: [[TYPE_five]] = metadata !{metadata !"_ZTS4five", metadata [[TYPE_CHAR]], i64 0, metadata [[TYPE_INT]], i64 1, metadata [[TYPE_CHAR]], i64 1, metadata [[TYPE_CHAR]], i64 2}
// PATH: [[TAG_six_b]] = metadata !{metadata [[TYPE_six:!.*]], metadata [[TYPE_CHAR]], i64 4}
// PATH: [[TYPE_six]] = metadata !{metadata !"_ZTS3six", metadata [[TYPE_CHAR]], i64 0, metadata [[TYPE_INT]], i64 4, metadata [[TYPE_CHAR]], i64 4, metadata [[TYPE_CHAR]], i64 5}
