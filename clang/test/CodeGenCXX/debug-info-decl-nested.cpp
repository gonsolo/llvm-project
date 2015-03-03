// RUN: %clang_cc1 -std=c++11 -g -emit-llvm -g -triple x86_64-apple-darwin %s -o %t
// RUN: cat %t | FileCheck %s -check-prefix=CHECK0
// RUN: cat %t | FileCheck %s -check-prefix=CHECK1
// RUN: cat %t | FileCheck %s -check-prefix=CHECK2
//
// This test ensures that we associate a declaration with the
// definition of the constructor for OuterClass. The declaration is
// necessary so the backend can emit the DW_AT_specification attribute
// for the definition.
//
// rdar://problem/13116508

class Foo;
class OuterClass
{
  static class InnerClass {
  public:
    InnerClass(); // Here createContextChain() generates a limited type for OuterClass.
  } theInnerClass;
// CHECK0: ![[DECL:[0-9]+]] = !MDSubprogram(name: "OuterClass"
// CHECK0-SAME: line: [[@LINE+2]]
// CHECK0-SAME: isDefinition: false
  OuterClass(const Foo *); // line 10
};
OuterClass::InnerClass OuterClass::theInnerClass; // This toplevel decl causes InnerClass to be generated.
// CHECK0: !MDSubprogram(name: "OuterClass"
// CHECK0-SAME: line: [[@LINE+3]]
// CHECK0-SAME: isDefinition: true
// CHECK0-SAME: declaration: ![[DECL]]
OuterClass::OuterClass(const Foo *meta) { } // line 13






class Foo1;
class OuterClass1
{
  static class InnerClass1 {
  public:
    InnerClass1();
  } theInnerClass1;
// CHECK1: ![[DECL:[0-9]+]] = !MDSubprogram(name: "Bar"
// CHECK1-SAME: line: [[@LINE+2]]
// CHECK1-SAME: isDefinition: false
  void Bar(const Foo1 *);
};
OuterClass1::InnerClass1 OuterClass1::theInnerClass1;
// CHECK1: !MDSubprogram(name: "Bar"
// CHECK1-SAME: line: [[@LINE+3]]
// CHECK1-SAME: isDefinition: true
// CHECK1-SAME: declaration: ![[DECL]]
void OuterClass1::Bar(const Foo1 *meta) { }





class Foo2;
class OuterClass2
{
  static class InnerClass2 {
  public:
    InnerClass2();
  } theInnerClass2;
// CHECK2: ![[DECL:[0-9]+]] = !MDSubprogram(name: "~OuterClass2"
// CHECK2-SAME: line: [[@LINE+2]]
// CHECK2-SAME: isDefinition: false
  ~OuterClass2(); // line 10
};
OuterClass2::InnerClass2 OuterClass2::theInnerClass2;
// CHECK4: !MDSubprogram(name: "~OuterClass2"
// CHECK4-SAME: line: [[@LINE+3]]
// CHECK4-SAME: isDefinition: true
// CHECK4-SAME: declaration: ![[DECL]]
OuterClass2::~OuterClass2() { }
