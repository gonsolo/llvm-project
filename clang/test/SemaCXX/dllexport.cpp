// RUN: %clang_cc1 -triple i686-win32     -fsyntax-only -verify -std=c++11 -DMS %s
// RUN: %clang_cc1 -triple x86_64-win32   -fsyntax-only -verify -std=c++1y -DMS %s
// RUN: %clang_cc1 -triple i686-mingw32   -fsyntax-only -verify -std=c++1y %s
// RUN: %clang_cc1 -triple x86_64-mingw32 -fsyntax-only -verify -std=c++11 %s

// Helper structs to make templates more expressive.
struct ImplicitInst_Exported {};
struct ExplicitDecl_Exported {};
struct ExplicitInst_Exported {};
struct ExplicitSpec_Exported {};
struct ExplicitSpec_Def_Exported {};
struct ExplicitSpec_InlineDef_Exported {};
struct ExplicitSpec_NotExported {};
namespace { struct Internal {}; }
struct External { int v; };


// Invalid usage.
__declspec(dllexport) typedef int typedef1; // expected-warning{{'dllexport' attribute only applies to variables, functions and classes}}
typedef __declspec(dllexport) int typedef2; // expected-warning{{'dllexport' attribute only applies to variables, functions and classes}}
typedef int __declspec(dllexport) typedef3; // expected-warning{{'dllexport' attribute only applies to variables, functions and classes}}
typedef __declspec(dllexport) void (*FunTy)(); // expected-warning{{'dllexport' attribute only applies to variables, functions and classes}}
enum __declspec(dllexport) Enum {}; // expected-warning{{'dllexport' attribute only applies to variables, functions and classes}}
#if __has_feature(cxx_strong_enums)
  enum class __declspec(dllexport) EnumClass {}; // expected-warning{{'dllexport' attribute only applies to variables, functions and classes}}
#endif



//===----------------------------------------------------------------------===//
// Globals
//===----------------------------------------------------------------------===//

// Export declaration.
__declspec(dllexport) extern int ExternGlobalDecl;

// dllexport implies a definition.
__declspec(dllexport) int GlobalDef;

// Export definition.
__declspec(dllexport) int GlobalInit1 = 1;
int __declspec(dllexport) GlobalInit2 = 1;

// Declare, then export definition.
__declspec(dllexport) extern int GlobalDeclInit;
int GlobalDeclInit = 1;

// Redeclarations
__declspec(dllexport) extern int GlobalRedecl1;
__declspec(dllexport)        int GlobalRedecl1;

__declspec(dllexport) extern int GlobalRedecl2;
                             int GlobalRedecl2;

                      extern int GlobalRedecl3; // expected-note{{previous declaration is here}}
__declspec(dllexport) extern int GlobalRedecl3; // expected-error{{redeclaration of 'GlobalRedecl3' cannot add 'dllexport' attribute}}

// External linkage is required.
__declspec(dllexport) static int StaticGlobal; // expected-error{{'StaticGlobal' must have external linkage when declared 'dllexport'}}
__declspec(dllexport) Internal InternalTypeGlobal; // expected-error{{'InternalTypeGlobal' must have external linkage when declared 'dllexport'}}
namespace    { __declspec(dllexport) int InternalGlobal; } // expected-error{{'(anonymous namespace)::InternalGlobal' must have external linkage when declared 'dllexport'}}
namespace ns { __declspec(dllexport) int ExternalGlobal; }

__declspec(dllexport) auto InternalAutoTypeGlobal = Internal(); // expected-error{{'InternalAutoTypeGlobal' must have external linkage when declared 'dllexport'}}
__declspec(dllexport) auto ExternalAutoTypeGlobal = External();

// Export in local scope.
void functionScope() {
  __declspec(dllexport)        int LocalVarDecl; // expected-error{{'LocalVarDecl' must have external linkage when declared 'dllexport'}}
  __declspec(dllexport)        int LocalVarDef = 1; // expected-error{{'LocalVarDef' must have external linkage when declared 'dllexport'}}
  __declspec(dllexport) extern int ExternLocalVarDecl;
  __declspec(dllexport) static int StaticLocalVar; // expected-error{{'StaticLocalVar' must have external linkage when declared 'dllexport'}}
}



//===----------------------------------------------------------------------===//
// Variable templates
//===----------------------------------------------------------------------===//
#if __has_feature(cxx_variable_templates)

// Export declaration.
template<typename T> __declspec(dllexport) extern int ExternVarTmplDecl;

// dllexport implies a definition.
template<typename T> __declspec(dllexport) int VarTmplDef;

// Export definition.
template<typename T> __declspec(dllexport) int VarTmplInit1 = 1;
template<typename T> int __declspec(dllexport) VarTmplInit2 = 1;

// Declare, then export definition.
template<typename T> __declspec(dllexport) extern int VarTmplDeclInit;
template<typename T>                              int VarTmplDeclInit = 1;

// Redeclarations
template<typename T> __declspec(dllexport) extern int VarTmplRedecl1;
template<typename T> __declspec(dllexport)        int VarTmplRedecl1 = 1;

template<typename T> __declspec(dllexport) extern int VarTmplRedecl2;
template<typename T>                              int VarTmplRedecl2 = 1;

template<typename T>                       extern int VarTmplRedecl3; // expected-note{{previous declaration is here}}
template<typename T> __declspec(dllexport) extern int VarTmplRedecl3; // expected-error{{redeclaration of 'VarTmplRedecl3' cannot add 'dllexport' attribute}}

// External linkage is required.
template<typename T> __declspec(dllexport) static int StaticVarTmpl; // expected-error{{'StaticVarTmpl' must have external linkage when declared 'dllexport'}}
template<typename T> __declspec(dllexport) Internal InternalTypeVarTmpl; // expected-error{{'InternalTypeVarTmpl' must have external linkage when declared 'dllexport'}}
namespace    { template<typename T> __declspec(dllexport) int InternalVarTmpl; } // expected-error{{'(anonymous namespace)::InternalVarTmpl' must have external linkage when declared 'dllexport'}}
namespace ns { template<typename T> __declspec(dllexport) int ExternalVarTmpl = 1; }

template<typename T> __declspec(dllexport) auto InternalAutoTypeVarTmpl = Internal(); // expected-error{{'InternalAutoTypeVarTmpl' must have external linkage when declared 'dllexport'}}
template<typename T> __declspec(dllexport) auto ExternalAutoTypeVarTmpl = External();
template External ExternalAutoTypeVarTmpl<ExplicitInst_Exported>;


template<typename T> int VarTmpl = 1;
template<typename T> __declspec(dllexport) int ExportedVarTmpl = 1;

// Export implicit instantiation of an exported variable template.
int useVarTmpl() { return ExportedVarTmpl<ImplicitInst_Exported>; }

// Export explicit instantiation declaration of an exported variable template.
extern template int ExportedVarTmpl<ExplicitDecl_Exported>;
       template int ExportedVarTmpl<ExplicitDecl_Exported>;

// Export explicit instantiation definition of an exported variable template.
template __declspec(dllexport) int ExportedVarTmpl<ExplicitInst_Exported>;

// Export specialization of an exported variable template.
template<> __declspec(dllexport) int ExportedVarTmpl<ExplicitSpec_Exported>;
template<> __declspec(dllexport) int ExportedVarTmpl<ExplicitSpec_Def_Exported> = 1;

// Not exporting specialization of an exported variable template without
// explicit dllexport.
template<> int ExportedVarTmpl<ExplicitSpec_NotExported>;


// Export explicit instantiation declaration of a non-exported variable template.
extern template __declspec(dllexport) int VarTmpl<ExplicitDecl_Exported>;
       template __declspec(dllexport) int VarTmpl<ExplicitDecl_Exported>;

// Export explicit instantiation definition of a non-exported variable template.
template __declspec(dllexport) int VarTmpl<ExplicitInst_Exported>;

// Export specialization of a non-exported variable template.
template<> __declspec(dllexport) int VarTmpl<ExplicitSpec_Exported>;
template<> __declspec(dllexport) int VarTmpl<ExplicitSpec_Def_Exported> = 1;

#endif // __has_feature(cxx_variable_templates)



//===----------------------------------------------------------------------===//
// Functions
//===----------------------------------------------------------------------===//

// Export function declaration. Check different placements.
__attribute__((dllexport)) void decl1A(); // Sanity check with __attribute__
__declspec(dllexport)      void decl1B();

void __attribute__((dllexport)) decl2A();
void __declspec(dllexport)      decl2B();

// Export function definition.
__declspec(dllexport) void def() {}

// extern "C"
extern "C" __declspec(dllexport) void externC() {}

// Export inline function.
__declspec(dllexport) inline void inlineFunc1() {}
inline void __attribute__((dllexport)) inlineFunc2() {}

__declspec(dllexport) inline void inlineDecl();
                             void inlineDecl() {}

__declspec(dllexport) void inlineDef();
               inline void inlineDef() {}

// Redeclarations
__declspec(dllexport) void redecl1();
__declspec(dllexport) void redecl1() {}

__declspec(dllexport) void redecl2();
                      void redecl2() {}

                      void redecl3(); // expected-note{{previous declaration is here}}
__declspec(dllexport) void redecl3(); // expected-error{{redeclaration of 'redecl3' cannot add 'dllexport' attribute}}

                      void redecl4(); // expected-note{{previous declaration is here}}
__declspec(dllexport) inline void redecl4() {} // expected-error{{redeclaration of 'redecl4' cannot add 'dllexport' attribute}}

// Friend functions
struct FuncFriend {
  friend __declspec(dllexport) void friend1();
  friend __declspec(dllexport) void friend2();
  friend                       void friend3(); // expected-note{{previous declaration is here}}
  friend                       void friend4(); // expected-note{{previous declaration is here}}
};
__declspec(dllexport) void friend1() {}
                      void friend2() {}
__declspec(dllexport) void friend3() {} // expected-error{{redeclaration of 'friend3' cannot add 'dllexport' attribute}}
__declspec(dllexport) inline void friend4() {} // expected-error{{redeclaration of 'friend4' cannot add 'dllexport' attribute}}

// Implicit declarations can be redeclared with dllexport.
__declspec(dllexport) void* operator new(__SIZE_TYPE__ n);

// External linkage is required.
__declspec(dllexport) static int staticFunc(); // expected-error{{'staticFunc' must have external linkage when declared 'dllexport'}}
__declspec(dllexport) Internal internalRetFunc(); // expected-error{{'internalRetFunc' must have external linkage when declared 'dllexport'}}
namespace    { __declspec(dllexport) void internalFunc() {} } // expected-error{{'(anonymous namespace)::internalFunc' must have external linkage when declared 'dllexport'}}
namespace ns { __declspec(dllexport) void externalFunc() {} }

// Export deleted function.
__declspec(dllexport) void deletedFunc() = delete; // expected-error{{attribute 'dllexport' cannot be applied to a deleted function}}
__declspec(dllexport) inline void deletedInlineFunc() = delete; // expected-error{{attribute 'dllexport' cannot be applied to a deleted function}}



//===----------------------------------------------------------------------===//
// Function templates
//===----------------------------------------------------------------------===//

// Export function template declaration. Check different placements.
template<typename T> __declspec(dllexport) void funcTmplDecl1();
template<typename T> void __declspec(dllexport) funcTmplDecl2();

// Export function template definition.
template<typename T> __declspec(dllexport) void funcTmplDef() {}

// Export inline function template.
template<typename T> __declspec(dllexport) inline void inlineFuncTmpl1() {}
template<typename T> inline void __attribute__((dllexport)) inlineFuncTmpl2() {}

template<typename T> __declspec(dllexport) inline void inlineFuncTmplDecl();
template<typename T>                              void inlineFuncTmplDecl() {}

template<typename T> __declspec(dllexport) void inlineFuncTmplDef();
template<typename T>                inline void inlineFuncTmplDef() {}

// Redeclarations
template<typename T> __declspec(dllexport) void funcTmplRedecl1();
template<typename T> __declspec(dllexport) void funcTmplRedecl1() {}

template<typename T> __declspec(dllexport) void funcTmplRedecl2();
template<typename T>                       void funcTmplRedecl2() {}

template<typename T>                       void funcTmplRedecl3(); // expected-note{{previous declaration is here}}
template<typename T> __declspec(dllexport) void funcTmplRedecl3(); // expected-error{{redeclaration of 'funcTmplRedecl3' cannot add 'dllexport' attribute}}

template<typename T>                       void funcTmplRedecl4(); // expected-note{{previous declaration is here}}
template<typename T> __declspec(dllexport) inline void funcTmplRedecl4() {} // expected-error{{redeclaration of 'funcTmplRedecl4' cannot add 'dllexport' attribute}}

// Function template friends
struct FuncTmplFriend {
  template<typename T> friend __declspec(dllexport) void funcTmplFriend1();
  template<typename T> friend __declspec(dllexport) void funcTmplFriend2();
  template<typename T> friend                       void funcTmplFriend3(); // expected-note{{previous declaration is here}}
  template<typename T> friend                       void funcTmplFriend4(); // expected-note{{previous declaration is here}}
};
template<typename T> __declspec(dllexport) void funcTmplFriend1() {}
template<typename T>                       void funcTmplFriend2() {}
template<typename T> __declspec(dllexport) void funcTmplFriend3() {} // expected-error{{redeclaration of 'funcTmplFriend3' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport) inline void funcTmplFriend4() {} // expected-error{{redeclaration of 'funcTmplFriend4' cannot add 'dllexport' attribute}}

// External linkage is required.
template<typename T> __declspec(dllexport) static int staticFuncTmpl(); // expected-error{{'staticFuncTmpl' must have external linkage when declared 'dllexport'}}
template<typename T> __declspec(dllexport) Internal internalRetFuncTmpl(); // expected-error{{'internalRetFuncTmpl' must have external linkage when declared 'dllexport'}}
namespace    { template<typename T> __declspec(dllexport) void internalFuncTmpl(); } // expected-error{{'(anonymous namespace)::internalFuncTmpl' must have external linkage when declared 'dllexport'}}
namespace ns { template<typename T> __declspec(dllexport) void externalFuncTmpl(); }


template<typename T> void funcTmpl() {}
template<typename T> __declspec(dllexport) void exportedFuncTmplDecl();
template<typename T> __declspec(dllexport) void exportedFuncTmpl() {}

// Export implicit instantiation of an exported function template.
void useFunTmplDecl() { exportedFuncTmplDecl<ImplicitInst_Exported>(); }
void useFunTmplDef() { exportedFuncTmpl<ImplicitInst_Exported>(); }

// Export explicit instantiation declaration of an exported function template.
extern template void exportedFuncTmpl<ExplicitDecl_Exported>();
       template void exportedFuncTmpl<ExplicitDecl_Exported>();

// Export explicit instantiation definition of an exported function template.
template void exportedFuncTmpl<ExplicitInst_Exported>();

// Export specialization of an exported function template.
template<> __declspec(dllexport) void exportedFuncTmpl<ExplicitSpec_Exported>();
template<> __declspec(dllexport) void exportedFuncTmpl<ExplicitSpec_Def_Exported>() {}
template<> __declspec(dllexport) inline void exportedFuncTmpl<ExplicitSpec_InlineDef_Exported>() {}

// Not exporting specialization of an exported function template without
// explicit dllexport.
template<> void exportedFuncTmpl<ExplicitSpec_NotExported>() {}


// Export explicit instantiation declaration of a non-exported function template.
extern template __declspec(dllexport) void funcTmpl<ExplicitDecl_Exported>();
       template __declspec(dllexport) void funcTmpl<ExplicitDecl_Exported>();

// Export explicit instantiation definition of a non-exported function template.
template __declspec(dllexport) void funcTmpl<ExplicitInst_Exported>();

// Export specialization of a non-exported function template.
template<> __declspec(dllexport) void funcTmpl<ExplicitSpec_Exported>();
template<> __declspec(dllexport) void funcTmpl<ExplicitSpec_Def_Exported>() {}
template<> __declspec(dllexport) inline void funcTmpl<ExplicitSpec_InlineDef_Exported>() {}



//===----------------------------------------------------------------------===//
// Classes
//===----------------------------------------------------------------------===//

class __declspec(dllexport) ClassDecl;

class __declspec(dllexport) ClassDef { };

#ifdef MS
// expected-warning@+3{{'dllexport' attribute ignored}}
#endif
template <typename T> struct PartiallySpecializedClassTemplate {};
template <typename T> struct __declspec(dllexport) PartiallySpecializedClassTemplate<T*> { void f() {} };

template <typename T> struct ExpliciallySpecializedClassTemplate {};
template <> struct __declspec(dllexport) ExpliciallySpecializedClassTemplate<int> { void f() {} };



//===----------------------------------------------------------------------===//
// Precedence
//===----------------------------------------------------------------------===//

// dllexport takes precedence over dllimport if both are specified.
__attribute__((dllimport, dllexport))       extern int PrecedenceExternGlobal1A; // expected-warning{{'dllimport' attribute ignored}}
__declspec(dllimport) __declspec(dllexport) extern int PrecedenceExternGlobal1B; // expected-warning{{'dllimport' attribute ignored}}

__attribute__((dllexport, dllimport))       extern int PrecedenceExternGlobal2A; // expected-warning{{'dllimport' attribute ignored}}
__declspec(dllexport) __declspec(dllimport) extern int PrecedenceExternGlobal2B; // expected-warning{{'dllimport' attribute ignored}}

__attribute__((dllimport, dllexport))       int PrecedenceGlobal1A; // expected-warning{{'dllimport' attribute ignored}}
__declspec(dllimport) __declspec(dllexport) int PrecedenceGlobal1B; // expected-warning{{'dllimport' attribute ignored}}

__attribute__((dllexport, dllimport))       int PrecedenceGlobal2A; // expected-warning{{'dllimport' attribute ignored}}
__declspec(dllexport) __declspec(dllimport) int PrecedenceGlobal2B; // expected-warning{{'dllimport' attribute ignored}}

__declspec(dllexport) extern int PrecedenceExternGlobalRedecl1;
__declspec(dllimport) extern int PrecedenceExternGlobalRedecl1; // expected-warning{{'dllimport' attribute ignored}}

__declspec(dllimport) extern int PrecedenceExternGlobalRedecl2; // expected-warning{{'dllimport' attribute ignored}}
__declspec(dllexport) extern int PrecedenceExternGlobalRedecl2;

__declspec(dllexport) extern int PrecedenceGlobalRedecl1;
__declspec(dllimport)        int PrecedenceGlobalRedecl1; // expected-warning{{'dllimport' attribute ignored}}

__declspec(dllimport) extern int PrecedenceGlobalRedecl2; // expected-warning{{'dllimport' attribute ignored}}
__declspec(dllexport)        int PrecedenceGlobalRedecl2;

void __attribute__((dllimport, dllexport))       precedence1A() {} // expected-warning{{'dllimport' attribute ignored}}
void __declspec(dllimport) __declspec(dllexport) precedence1B() {} // expected-warning{{'dllimport' attribute ignored}}

void __attribute__((dllexport, dllimport))       precedence2A() {} // expected-warning{{'dllimport' attribute ignored}}
void __declspec(dllexport) __declspec(dllimport) precedence2B() {} // expected-warning{{'dllimport' attribute ignored}}

void __declspec(dllimport) precedenceRedecl1(); // expected-warning{{'dllimport' attribute ignored}}
void __declspec(dllexport) precedenceRedecl1() {}

void __declspec(dllexport) precedenceRedecl2();
void __declspec(dllimport) precedenceRedecl2() {} // expected-warning{{'dllimport' attribute ignored}}



//===----------------------------------------------------------------------===//
// Class members
//===----------------------------------------------------------------------===//

// Export individual members of a class.
struct ExportMembers {
  struct Nested {
    __declspec(dllexport) void normalDef();
  };

  __declspec(dllexport)                void normalDecl();
  __declspec(dllexport)                void normalDef();
  __declspec(dllexport)                void normalInclass() {}
  __declspec(dllexport)                void normalInlineDef();
  __declspec(dllexport)         inline void normalInlineDecl();
  __declspec(dllexport) virtual        void virtualDecl();
  __declspec(dllexport) virtual        void virtualDef();
  __declspec(dllexport) virtual        void virtualInclass() {}
  __declspec(dllexport) virtual        void virtualInlineDef();
  __declspec(dllexport) virtual inline void virtualInlineDecl();
  __declspec(dllexport) static         void staticDecl();
  __declspec(dllexport) static         void staticDef();
  __declspec(dllexport) static         void staticInclass() {}
  __declspec(dllexport) static         void staticInlineDef();
  __declspec(dllexport) static  inline void staticInlineDecl();

protected:
  __declspec(dllexport)                void protectedDef();
private:
  __declspec(dllexport)                void privateDef();
public:

  __declspec(dllexport)                int  Field; // expected-warning{{'dllexport' attribute only applies to variables, functions and classes}}
  __declspec(dllexport) static         int  StaticField;
  __declspec(dllexport) static         int  StaticFieldDef;
  __declspec(dllexport) static  const  int  StaticConstField;
  __declspec(dllexport) static  const  int  StaticConstFieldDef;
  __declspec(dllexport) static  const  int  StaticConstFieldEqualInit = 1;
  __declspec(dllexport) static  const  int  StaticConstFieldBraceInit{1};
  __declspec(dllexport) constexpr static int ConstexprField = 1;
  __declspec(dllexport) constexpr static int ConstexprFieldDef = 1;
};

       void ExportMembers::Nested::normalDef() {}
       void ExportMembers::normalDef() {}
inline void ExportMembers::normalInlineDef() {}
       void ExportMembers::normalInlineDecl() {}
       void ExportMembers::virtualDef() {}
inline void ExportMembers::virtualInlineDef() {}
       void ExportMembers::virtualInlineDecl() {}
       void ExportMembers::staticDef() {}
inline void ExportMembers::staticInlineDef() {}
       void ExportMembers::staticInlineDecl() {}
       void ExportMembers::protectedDef() {}
       void ExportMembers::privateDef() {}

       int  ExportMembers::StaticFieldDef;
const  int  ExportMembers::StaticConstFieldDef = 1;
constexpr int ExportMembers::ConstexprFieldDef;


// Export on member definitions.
struct ExportMemberDefs {
  __declspec(dllexport)                void normalDef();
  __declspec(dllexport)                void normalInlineDef();
  __declspec(dllexport)         inline void normalInlineDecl();
  __declspec(dllexport) virtual        void virtualDef();
  __declspec(dllexport) virtual        void virtualInlineDef();
  __declspec(dllexport) virtual inline void virtualInlineDecl();
  __declspec(dllexport) static         void staticDef();
  __declspec(dllexport) static         void staticInlineDef();
  __declspec(dllexport) static  inline void staticInlineDecl();

  __declspec(dllexport) static         int  StaticField;
  __declspec(dllexport) static  const  int  StaticConstField;
  __declspec(dllexport) constexpr static int ConstexprField = 1;
};

__declspec(dllexport)        void ExportMemberDefs::normalDef() {}
__declspec(dllexport) inline void ExportMemberDefs::normalInlineDef() {}
__declspec(dllexport)        void ExportMemberDefs::normalInlineDecl() {}
__declspec(dllexport)        void ExportMemberDefs::virtualDef() {}
__declspec(dllexport) inline void ExportMemberDefs::virtualInlineDef() {}
__declspec(dllexport)        void ExportMemberDefs::virtualInlineDecl() {}
__declspec(dllexport)        void ExportMemberDefs::staticDef() {}
__declspec(dllexport) inline void ExportMemberDefs::staticInlineDef() {}
__declspec(dllexport)        void ExportMemberDefs::staticInlineDecl() {}

__declspec(dllexport)        int  ExportMemberDefs::StaticField;
__declspec(dllexport) const  int  ExportMemberDefs::StaticConstField = 1;
__declspec(dllexport) constexpr int ExportMemberDefs::ConstexprField;


// Export special member functions.
struct ExportSpecials {
  __declspec(dllexport) ExportSpecials() {}
  __declspec(dllexport) ~ExportSpecials();
  __declspec(dllexport) inline ExportSpecials(const ExportSpecials&);
  __declspec(dllexport) ExportSpecials& operator=(const ExportSpecials&);
  __declspec(dllexport) ExportSpecials(ExportSpecials&&);
  __declspec(dllexport) ExportSpecials& operator=(ExportSpecials&&);
};

ExportSpecials::~ExportSpecials() {}
ExportSpecials::ExportSpecials(const ExportSpecials&) {}
inline ExportSpecials& ExportSpecials::operator=(const ExportSpecials&) { return *this; }
ExportSpecials::ExportSpecials(ExportSpecials&&) {}
ExportSpecials& ExportSpecials::operator=(ExportSpecials&&) { return *this; }


// Export allocation functions.
extern "C" void* malloc(__SIZE_TYPE__ size);
extern "C" void free(void* p);
struct ExportAlloc {
  __declspec(dllexport) void* operator new(__SIZE_TYPE__);
  __declspec(dllexport) void* operator new[](__SIZE_TYPE__);
  __declspec(dllexport) void operator delete(void*);
  __declspec(dllexport) void operator delete[](void*);
};
void* ExportAlloc::operator new(__SIZE_TYPE__ n) { return malloc(n); }
void* ExportAlloc::operator new[](__SIZE_TYPE__ n) { return malloc(n); }
void ExportAlloc::operator delete(void* p) { free(p); }
void ExportAlloc::operator delete[](void* p) { free(p); }


// Export deleted member functions.
struct ExportDeleted {
  __declspec(dllexport) ExportDeleted() = delete; // expected-error{{attribute 'dllexport' cannot be applied to a deleted function}}
  __declspec(dllexport) ~ExportDeleted() = delete; // expected-error{{attribute 'dllexport' cannot be applied to a deleted function}}
  __declspec(dllexport) ExportDeleted(const ExportDeleted&) = delete; // expected-error{{attribute 'dllexport' cannot be applied to a deleted function}}
  __declspec(dllexport) ExportDeleted& operator=(const ExportDeleted&) = delete; // expected-error{{attribute 'dllexport' cannot be applied to a deleted function}}
  __declspec(dllexport) ExportDeleted(ExportDeleted&&) = delete; // expected-error{{attribute 'dllexport' cannot be applied to a deleted function}}
  __declspec(dllexport) ExportDeleted& operator=(ExportDeleted&&) = delete; // expected-error{{attribute 'dllexport' cannot be applied to a deleted function}}
  __declspec(dllexport) void deleted() = delete; // expected-error{{attribute 'dllexport' cannot be applied to a deleted function}}
};


// Export defaulted member functions.
struct ExportDefaulted {
  __declspec(dllexport) ExportDefaulted() = default;
  __declspec(dllexport) ~ExportDefaulted() = default;
  __declspec(dllexport) ExportDefaulted(const ExportDefaulted&) = default;
  __declspec(dllexport) ExportDefaulted& operator=(const ExportDefaulted&) = default;
  __declspec(dllexport) ExportDefaulted(ExportDefaulted&&) = default;
  __declspec(dllexport) ExportDefaulted& operator=(ExportDefaulted&&) = default;
};


// Export defaulted member function definitions.
struct ExportDefaultedDefs {
  __declspec(dllexport) ExportDefaultedDefs();
  __declspec(dllexport) ~ExportDefaultedDefs();

  __declspec(dllexport) inline ExportDefaultedDefs(const ExportDefaultedDefs&);
  __declspec(dllexport) ExportDefaultedDefs& operator=(const ExportDefaultedDefs&);

  __declspec(dllexport) ExportDefaultedDefs(ExportDefaultedDefs&&);
  __declspec(dllexport) ExportDefaultedDefs& operator=(ExportDefaultedDefs&&);
};

// Export definitions.
__declspec(dllexport) ExportDefaultedDefs::ExportDefaultedDefs() = default;
ExportDefaultedDefs::~ExportDefaultedDefs() = default;

// Export inline declaration and definition.
__declspec(dllexport) ExportDefaultedDefs::ExportDefaultedDefs(const ExportDefaultedDefs&) = default;
inline ExportDefaultedDefs& ExportDefaultedDefs::operator=(const ExportDefaultedDefs&) = default;

__declspec(dllexport) ExportDefaultedDefs::ExportDefaultedDefs(ExportDefaultedDefs&&) = default;
ExportDefaultedDefs& ExportDefaultedDefs::operator=(ExportDefaultedDefs&&) = default;


// Redeclarations cannot add dllexport.
struct MemberRedecl {
                 void normalDef();         // expected-note{{previous declaration is here}}
                 void normalInlineDef();   // expected-note{{previous declaration is here}}
          inline void normalInlineDecl();  // expected-note{{previous declaration is here}}
  virtual        void virtualDef();        // expected-note{{previous declaration is here}}
  virtual        void virtualInlineDef();  // expected-note{{previous declaration is here}}
  virtual inline void virtualInlineDecl(); // expected-note{{previous declaration is here}}
  static         void staticDef();         // expected-note{{previous declaration is here}}
  static         void staticInlineDef();   // expected-note{{previous declaration is here}}
  static  inline void staticInlineDecl();  // expected-note{{previous declaration is here}}

  static         int  StaticField;         // expected-note{{previous declaration is here}}
  static  const  int  StaticConstField;    // expected-note{{previous declaration is here}}
  constexpr static int ConstexprField = 1; // expected-note{{previous declaration is here}}
};

__declspec(dllexport)        void MemberRedecl::normalDef() {}         // expected-error{{redeclaration of 'MemberRedecl::normalDef' cannot add 'dllexport' attribute}}
__declspec(dllexport) inline void MemberRedecl::normalInlineDef() {}   // expected-error{{redeclaration of 'MemberRedecl::normalInlineDef' cannot add 'dllexport' attribute}}
__declspec(dllexport)        void MemberRedecl::normalInlineDecl() {}  // expected-error{{redeclaration of 'MemberRedecl::normalInlineDecl' cannot add 'dllexport' attribute}}
__declspec(dllexport)        void MemberRedecl::virtualDef() {}        // expected-error{{redeclaration of 'MemberRedecl::virtualDef' cannot add 'dllexport' attribute}}
__declspec(dllexport) inline void MemberRedecl::virtualInlineDef() {}  // expected-error{{redeclaration of 'MemberRedecl::virtualInlineDef' cannot add 'dllexport' attribute}}
__declspec(dllexport)        void MemberRedecl::virtualInlineDecl() {} // expected-error{{redeclaration of 'MemberRedecl::virtualInlineDecl' cannot add 'dllexport' attribute}}
__declspec(dllexport)        void MemberRedecl::staticDef() {}         // expected-error{{redeclaration of 'MemberRedecl::staticDef' cannot add 'dllexport' attribute}}
__declspec(dllexport) inline void MemberRedecl::staticInlineDef() {}   // expected-error{{redeclaration of 'MemberRedecl::staticInlineDef' cannot add 'dllexport' attribute}}
__declspec(dllexport)        void MemberRedecl::staticInlineDecl() {}  // expected-error{{redeclaration of 'MemberRedecl::staticInlineDecl' cannot add 'dllexport' attribute}}

__declspec(dllexport)        int  MemberRedecl::StaticField = 1;       // expected-error{{redeclaration of 'MemberRedecl::StaticField' cannot add 'dllexport' attribute}}
__declspec(dllexport) const  int  MemberRedecl::StaticConstField = 1;  // expected-error{{redeclaration of 'MemberRedecl::StaticConstField' cannot add 'dllexport' attribute}}
__declspec(dllexport) constexpr int MemberRedecl::ConstexprField;      // expected-error{{redeclaration of 'MemberRedecl::ConstexprField' cannot add 'dllexport' attribute}}



//===----------------------------------------------------------------------===//
// Class member templates
//===----------------------------------------------------------------------===//

struct ExportMemberTmpl {
  template<typename T> __declspec(dllexport)               void normalDecl();
  template<typename T> __declspec(dllexport)               void normalDef();
  template<typename T> __declspec(dllexport)               void normalInclass() {}
  template<typename T> __declspec(dllexport)               void normalInlineDef();
  template<typename T> __declspec(dllexport)        inline void normalInlineDecl();
  template<typename T> __declspec(dllexport) static        void staticDecl();
  template<typename T> __declspec(dllexport) static        void staticDef();
  template<typename T> __declspec(dllexport) static        void staticInclass() {}
  template<typename T> __declspec(dllexport) static        void staticInlineDef();
  template<typename T> __declspec(dllexport) static inline void staticInlineDecl();

#if __has_feature(cxx_variable_templates)
  template<typename T> __declspec(dllexport) static        int  StaticField;
  template<typename T> __declspec(dllexport) static        int  StaticFieldDef;
  template<typename T> __declspec(dllexport) static const  int  StaticConstField;
  template<typename T> __declspec(dllexport) static const  int  StaticConstFieldDef;
  template<typename T> __declspec(dllexport) static const  int  StaticConstFieldEqualInit = 1;
  template<typename T> __declspec(dllexport) static const  int  StaticConstFieldBraceInit{1};
  template<typename T> __declspec(dllexport) constexpr static int ConstexprField = 1;
  template<typename T> __declspec(dllexport) constexpr static int ConstexprFieldDef = 1;
#endif // __has_feature(cxx_variable_templates)
};

template<typename T>        void ExportMemberTmpl::normalDef() {}
template<typename T> inline void ExportMemberTmpl::normalInlineDef() {}
template<typename T>        void ExportMemberTmpl::normalInlineDecl() {}
template<typename T>        void ExportMemberTmpl::staticDef() {}
template<typename T> inline void ExportMemberTmpl::staticInlineDef() {}
template<typename T>        void ExportMemberTmpl::staticInlineDecl() {}

#if __has_feature(cxx_variable_templates)
template<typename T>        int  ExportMemberTmpl::StaticFieldDef;
template<typename T> const  int  ExportMemberTmpl::StaticConstFieldDef = 1;
template<typename T> constexpr int ExportMemberTmpl::ConstexprFieldDef;
#endif // __has_feature(cxx_variable_templates)


// Redeclarations cannot add dllexport.
struct MemTmplRedecl {
  template<typename T>               void normalDef();         // expected-note{{previous declaration is here}}
  template<typename T>               void normalInlineDef();   // expected-note{{previous declaration is here}}
  template<typename T>        inline void normalInlineDecl();  // expected-note{{previous declaration is here}}
  template<typename T> static        void staticDef();         // expected-note{{previous declaration is here}}
  template<typename T> static        void staticInlineDef();   // expected-note{{previous declaration is here}}
  template<typename T> static inline void staticInlineDecl();  // expected-note{{previous declaration is here}}

#if __has_feature(cxx_variable_templates)
  template<typename T> static        int  StaticField;         // expected-note{{previous declaration is here}}
  template<typename T> static const  int  StaticConstField;    // expected-note{{previous declaration is here}}
  template<typename T> constexpr static int ConstexprField = 1; // expected-note{{previous declaration is here}}
#endif // __has_feature(cxx_variable_templates)
};

template<typename T> __declspec(dllexport)        void MemTmplRedecl::normalDef() {}        // expected-error{{redeclaration of 'MemTmplRedecl::normalDef' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport) inline void MemTmplRedecl::normalInlineDef() {}  // expected-error{{redeclaration of 'MemTmplRedecl::normalInlineDef' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport)        void MemTmplRedecl::normalInlineDecl() {} // expected-error{{redeclaration of 'MemTmplRedecl::normalInlineDecl' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport)        void MemTmplRedecl::staticDef() {}        // expected-error{{redeclaration of 'MemTmplRedecl::staticDef' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport) inline void MemTmplRedecl::staticInlineDef() {}  // expected-error{{redeclaration of 'MemTmplRedecl::staticInlineDef' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport)        void MemTmplRedecl::staticInlineDecl() {} // expected-error{{redeclaration of 'MemTmplRedecl::staticInlineDecl' cannot add 'dllexport' attribute}}

#if __has_feature(cxx_variable_templates)
template<typename T> __declspec(dllexport)        int  MemTmplRedecl::StaticField = 1;      // expected-error{{redeclaration of 'MemTmplRedecl::StaticField' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport) const  int  MemTmplRedecl::StaticConstField = 1; // expected-error{{redeclaration of 'MemTmplRedecl::StaticConstField' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport) constexpr int MemTmplRedecl::ConstexprField;     // expected-error{{redeclaration of 'MemTmplRedecl::ConstexprField' cannot add 'dllexport' attribute}}
#endif // __has_feature(cxx_variable_templates)



struct MemFunTmpl {
  template<typename T>                              void normalDef() {}
  template<typename T> __declspec(dllexport)        void exportedNormal() {}
  template<typename T>                       static void staticDef() {}
  template<typename T> __declspec(dllexport) static void exportedStatic() {}
};

// Export implicit instantiation of an exported member function template.
void useMemFunTmpl() {
  MemFunTmpl().exportedNormal<ImplicitInst_Exported>();
  MemFunTmpl().exportedStatic<ImplicitInst_Exported>();
}

// Export explicit instantiation declaration of an exported member function
// template.
extern template void MemFunTmpl::exportedNormal<ExplicitDecl_Exported>();
       template void MemFunTmpl::exportedNormal<ExplicitDecl_Exported>();

extern template void MemFunTmpl::exportedStatic<ExplicitDecl_Exported>();
       template void MemFunTmpl::exportedStatic<ExplicitDecl_Exported>();

// Export explicit instantiation definition of an exported member function
// template.
template void MemFunTmpl::exportedNormal<ExplicitInst_Exported>();
template void MemFunTmpl::exportedStatic<ExplicitInst_Exported>();

// Export specialization of an exported member function template.
template<> __declspec(dllexport) void MemFunTmpl::exportedNormal<ExplicitSpec_Exported>();
template<> __declspec(dllexport) void MemFunTmpl::exportedNormal<ExplicitSpec_Def_Exported>() {}
template<> __declspec(dllexport) inline void MemFunTmpl::exportedNormal<ExplicitSpec_InlineDef_Exported>() {}

template<> __declspec(dllexport) void MemFunTmpl::exportedStatic<ExplicitSpec_Exported>();
template<> __declspec(dllexport) void MemFunTmpl::exportedStatic<ExplicitSpec_Def_Exported>() {}
template<> __declspec(dllexport) inline void MemFunTmpl::exportedStatic<ExplicitSpec_InlineDef_Exported>() {}

// Not exporting specialization of an exported member function template without
// explicit dllexport.
template<> void MemFunTmpl::exportedNormal<ExplicitSpec_NotExported>() {}
template<> void MemFunTmpl::exportedStatic<ExplicitSpec_NotExported>() {}


// Export explicit instantiation declaration of a non-exported member function
// template.
extern template __declspec(dllexport) void MemFunTmpl::normalDef<ExplicitDecl_Exported>();
       template __declspec(dllexport) void MemFunTmpl::normalDef<ExplicitDecl_Exported>();

extern template __declspec(dllexport) void MemFunTmpl::staticDef<ExplicitDecl_Exported>();
       template __declspec(dllexport) void MemFunTmpl::staticDef<ExplicitDecl_Exported>();

// Export explicit instantiation definition of a non-exported member function
// template.
template __declspec(dllexport) void MemFunTmpl::normalDef<ExplicitInst_Exported>();
template __declspec(dllexport) void MemFunTmpl::staticDef<ExplicitInst_Exported>();

// Export specialization of a non-exported member function template.
template<> __declspec(dllexport) void MemFunTmpl::normalDef<ExplicitSpec_Exported>();
template<> __declspec(dllexport) void MemFunTmpl::normalDef<ExplicitSpec_Def_Exported>() {}
template<> __declspec(dllexport) inline void MemFunTmpl::normalDef<ExplicitSpec_InlineDef_Exported>() {}

template<> __declspec(dllexport) void MemFunTmpl::staticDef<ExplicitSpec_Exported>();
template<> __declspec(dllexport) void MemFunTmpl::staticDef<ExplicitSpec_Def_Exported>() {}
template<> __declspec(dllexport) inline void MemFunTmpl::staticDef<ExplicitSpec_InlineDef_Exported>() {}



#if __has_feature(cxx_variable_templates)
struct MemVarTmpl {
  template<typename T>                       static const int StaticVar = 1;
  template<typename T> __declspec(dllexport) static const int ExportedStaticVar = 1;
};
template<typename T> const int MemVarTmpl::StaticVar;
template<typename T> const int MemVarTmpl::ExportedStaticVar;

// Export implicit instantiation of an exported member variable template.
int useMemVarTmpl() { return MemVarTmpl::ExportedStaticVar<ImplicitInst_Exported>; }

// Export explicit instantiation declaration of an exported member variable
// template.
extern template const int MemVarTmpl::ExportedStaticVar<ExplicitDecl_Exported>;
       template const int MemVarTmpl::ExportedStaticVar<ExplicitDecl_Exported>;

// Export explicit instantiation definition of an exported member variable
// template.
template const int MemVarTmpl::ExportedStaticVar<ExplicitInst_Exported>;

// Export specialization of an exported member variable template.
template<> __declspec(dllexport) const int MemVarTmpl::ExportedStaticVar<ExplicitSpec_Exported>;
template<> __declspec(dllexport) const int MemVarTmpl::ExportedStaticVar<ExplicitSpec_Def_Exported> = 1;

// Not exporting specialization of an exported member variable template without
// explicit dllexport.
template<> const int MemVarTmpl::ExportedStaticVar<ExplicitSpec_NotExported>;


// Export explicit instantiation declaration of a non-exported member variable
// template.
extern template __declspec(dllexport) const int MemVarTmpl::StaticVar<ExplicitDecl_Exported>;
       template __declspec(dllexport) const int MemVarTmpl::StaticVar<ExplicitDecl_Exported>;

// Export explicit instantiation definition of a non-exported member variable
// template.
template __declspec(dllexport) const int MemVarTmpl::StaticVar<ExplicitInst_Exported>;

// Export specialization of a non-exported member variable template.
template<> __declspec(dllexport) const int MemVarTmpl::StaticVar<ExplicitSpec_Exported>;
template<> __declspec(dllexport) const int MemVarTmpl::StaticVar<ExplicitSpec_Def_Exported> = 1;

#endif // __has_feature(cxx_variable_templates)



//===----------------------------------------------------------------------===//
// Class template members
//===----------------------------------------------------------------------===//

// Export individual members of a class template.
template<typename T>
struct ExportClassTmplMembers {
  __declspec(dllexport)                void normalDecl();
  __declspec(dllexport)                void normalDef();
  __declspec(dllexport)                void normalInclass() {}
  __declspec(dllexport)                void normalInlineDef();
  __declspec(dllexport)         inline void normalInlineDecl();
  __declspec(dllexport) virtual        void virtualDecl();
  __declspec(dllexport) virtual        void virtualDef();
  __declspec(dllexport) virtual        void virtualInclass() {}
  __declspec(dllexport) virtual        void virtualInlineDef();
  __declspec(dllexport) virtual inline void virtualInlineDecl();
  __declspec(dllexport) static         void staticDecl();
  __declspec(dllexport) static         void staticDef();
  __declspec(dllexport) static         void staticInclass() {}
  __declspec(dllexport) static         void staticInlineDef();
  __declspec(dllexport) static  inline void staticInlineDecl();

protected:
  __declspec(dllexport)                void protectedDef();
private:
  __declspec(dllexport)                void privateDef();
public:

  __declspec(dllexport)                int  Field; // expected-warning{{'dllexport' attribute only applies to variables, functions and classes}}
  __declspec(dllexport) static         int  StaticField;
  __declspec(dllexport) static         int  StaticFieldDef;
  __declspec(dllexport) static  const  int  StaticConstField;
  __declspec(dllexport) static  const  int  StaticConstFieldDef;
  __declspec(dllexport) static  const  int  StaticConstFieldEqualInit = 1;
  __declspec(dllexport) static  const  int  StaticConstFieldBraceInit{1};
  __declspec(dllexport) constexpr static int ConstexprField = 1;
  __declspec(dllexport) constexpr static int ConstexprFieldDef = 1;
};

template<typename T>        void ExportClassTmplMembers<T>::normalDef() {}
template<typename T> inline void ExportClassTmplMembers<T>::normalInlineDef() {}
template<typename T>        void ExportClassTmplMembers<T>::normalInlineDecl() {}
template<typename T>        void ExportClassTmplMembers<T>::virtualDef() {}
template<typename T> inline void ExportClassTmplMembers<T>::virtualInlineDef() {}
template<typename T>        void ExportClassTmplMembers<T>::virtualInlineDecl() {}
template<typename T>        void ExportClassTmplMembers<T>::staticDef() {}
template<typename T> inline void ExportClassTmplMembers<T>::staticInlineDef() {}
template<typename T>        void ExportClassTmplMembers<T>::staticInlineDecl() {}
template<typename T>        void ExportClassTmplMembers<T>::protectedDef() {}
template<typename T>        void ExportClassTmplMembers<T>::privateDef() {}

template<typename T>        int  ExportClassTmplMembers<T>::StaticFieldDef;
template<typename T> const  int  ExportClassTmplMembers<T>::StaticConstFieldDef = 1;
template<typename T> constexpr int ExportClassTmplMembers<T>::ConstexprFieldDef;

template struct ExportClassTmplMembers<ImplicitInst_Exported>;


// Redeclarations cannot add dllexport.
template<typename T>
struct CTMR /*ClassTmplMemberRedecl*/ {
                 void normalDef();         // expected-note{{previous declaration is here}}
                 void normalInlineDef();   // expected-note{{previous declaration is here}}
          inline void normalInlineDecl();  // expected-note{{previous declaration is here}}
  virtual        void virtualDef();        // expected-note{{previous declaration is here}}
  virtual        void virtualInlineDef();  // expected-note{{previous declaration is here}}
  virtual inline void virtualInlineDecl(); // expected-note{{previous declaration is here}}
  static         void staticDef();         // expected-note{{previous declaration is here}}
  static         void staticInlineDef();   // expected-note{{previous declaration is here}}
  static  inline void staticInlineDecl();  // expected-note{{previous declaration is here}}

  static         int  StaticField;         // expected-note{{previous declaration is here}}
  static  const  int  StaticConstField;    // expected-note{{previous declaration is here}}
  constexpr static int ConstexprField = 1; // expected-note{{previous declaration is here}}
};

template<typename T> __declspec(dllexport)        void CTMR<T>::normalDef() {}         // expected-error{{redeclaration of 'CTMR::normalDef' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport) inline void CTMR<T>::normalInlineDef() {}   // expected-error{{redeclaration of 'CTMR::normalInlineDef' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport)        void CTMR<T>::normalInlineDecl() {}  // expected-error{{redeclaration of 'CTMR::normalInlineDecl' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport)        void CTMR<T>::virtualDef() {}        // expected-error{{redeclaration of 'CTMR::virtualDef' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport) inline void CTMR<T>::virtualInlineDef() {}  // expected-error{{redeclaration of 'CTMR::virtualInlineDef' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport)        void CTMR<T>::virtualInlineDecl() {} // expected-error{{redeclaration of 'CTMR::virtualInlineDecl' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport)        void CTMR<T>::staticDef() {}         // expected-error{{redeclaration of 'CTMR::staticDef' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport) inline void CTMR<T>::staticInlineDef() {}   // expected-error{{redeclaration of 'CTMR::staticInlineDef' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport)        void CTMR<T>::staticInlineDecl() {}  // expected-error{{redeclaration of 'CTMR::staticInlineDecl' cannot add 'dllexport' attribute}}

template<typename T> __declspec(dllexport)        int  CTMR<T>::StaticField = 1;       // expected-error{{redeclaration of 'CTMR::StaticField' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport) const  int  CTMR<T>::StaticConstField = 1;  // expected-error{{redeclaration of 'CTMR::StaticConstField' cannot add 'dllexport' attribute}}
template<typename T> __declspec(dllexport) constexpr int CTMR<T>::ConstexprField;      // expected-error{{redeclaration of 'CTMR::ConstexprField' cannot add 'dllexport' attribute}}



//===----------------------------------------------------------------------===//
// Class template member templates
//===----------------------------------------------------------------------===//

template<typename T>
struct ExportClsTmplMemTmpl {
  template<typename U> __declspec(dllexport)               void normalDecl();
  template<typename U> __declspec(dllexport)               void normalDef();
  template<typename U> __declspec(dllexport)               void normalInclass() {}
  template<typename U> __declspec(dllexport)               void normalInlineDef();
  template<typename U> __declspec(dllexport)        inline void normalInlineDecl();
  template<typename U> __declspec(dllexport) static        void staticDecl();
  template<typename U> __declspec(dllexport) static        void staticDef();
  template<typename U> __declspec(dllexport) static        void staticInclass() {}
  template<typename U> __declspec(dllexport) static        void staticInlineDef();
  template<typename U> __declspec(dllexport) static inline void staticInlineDecl();

#if __has_feature(cxx_variable_templates)
  template<typename U> __declspec(dllexport) static        int  StaticField;
  template<typename U> __declspec(dllexport) static        int  StaticFieldDef;
  template<typename U> __declspec(dllexport) static const  int  StaticConstField;
  template<typename U> __declspec(dllexport) static const  int  StaticConstFieldDef;
  template<typename U> __declspec(dllexport) static const  int  StaticConstFieldEqualInit = 1;
  template<typename U> __declspec(dllexport) static const  int  StaticConstFieldBraceInit{1};
  template<typename U> __declspec(dllexport) constexpr static int ConstexprField = 1;
  template<typename U> __declspec(dllexport) constexpr static int ConstexprFieldDef = 1;
#endif // __has_feature(cxx_variable_templates)
};

template<typename T> template<typename U>        void ExportClsTmplMemTmpl<T>::normalDef() {}
template<typename T> template<typename U> inline void ExportClsTmplMemTmpl<T>::normalInlineDef() {}
template<typename T> template<typename U>        void ExportClsTmplMemTmpl<T>::normalInlineDecl() {}
template<typename T> template<typename U>        void ExportClsTmplMemTmpl<T>::staticDef() {}
template<typename T> template<typename U> inline void ExportClsTmplMemTmpl<T>::staticInlineDef() {}
template<typename T> template<typename U>        void ExportClsTmplMemTmpl<T>::staticInlineDecl() {}

#if __has_feature(cxx_variable_templates)
template<typename T> template<typename U>        int  ExportClsTmplMemTmpl<T>::StaticFieldDef;
template<typename T> template<typename U> const  int  ExportClsTmplMemTmpl<T>::StaticConstFieldDef = 1;
template<typename T> template<typename U> constexpr int ExportClsTmplMemTmpl<T>::ConstexprFieldDef;
#endif // __has_feature(cxx_variable_templates)


// Redeclarations cannot add dllexport.
template<typename T>
struct CTMTR /*ClassTmplMemberTmplRedecl*/ {
  template<typename U>               void normalDef();         // expected-note{{previous declaration is here}}
  template<typename U>               void normalInlineDef();   // expected-note{{previous declaration is here}}
  template<typename U>        inline void normalInlineDecl();  // expected-note{{previous declaration is here}}
  template<typename U> static        void staticDef();         // expected-note{{previous declaration is here}}
  template<typename U> static        void staticInlineDef();   // expected-note{{previous declaration is here}}
  template<typename U> static inline void staticInlineDecl();  // expected-note{{previous declaration is here}}

#if __has_feature(cxx_variable_templates)
  template<typename U> static        int  StaticField;         // expected-note{{previous declaration is here}}
  template<typename U> static const  int  StaticConstField;    // expected-note{{previous declaration is here}}
  template<typename U> constexpr static int ConstexprField = 1; // expected-note{{previous declaration is here}}
#endif // __has_feature(cxx_variable_templates)
};

template<typename T> template<typename U> __declspec(dllexport)        void CTMTR<T>::normalDef() {}         // expected-error{{redeclaration of 'CTMTR::normalDef' cannot add 'dllexport' attribute}}
template<typename T> template<typename U> __declspec(dllexport) inline void CTMTR<T>::normalInlineDef() {}   // expected-error{{redeclaration of 'CTMTR::normalInlineDef' cannot add 'dllexport' attribute}}
template<typename T> template<typename U> __declspec(dllexport)        void CTMTR<T>::normalInlineDecl() {}  // expected-error{{redeclaration of 'CTMTR::normalInlineDecl' cannot add 'dllexport' attribute}}
template<typename T> template<typename U> __declspec(dllexport)        void CTMTR<T>::staticDef() {}         // expected-error{{redeclaration of 'CTMTR::staticDef' cannot add 'dllexport' attribute}}
template<typename T> template<typename U> __declspec(dllexport) inline void CTMTR<T>::staticInlineDef() {}   // expected-error{{redeclaration of 'CTMTR::staticInlineDef' cannot add 'dllexport' attribute}}
template<typename T> template<typename U> __declspec(dllexport)        void CTMTR<T>::staticInlineDecl() {}  // expected-error{{redeclaration of 'CTMTR::staticInlineDecl' cannot add 'dllexport' attribute}}

#if __has_feature(cxx_variable_templates)
template<typename T> template<typename U> __declspec(dllexport)        int  CTMTR<T>::StaticField = 1;       // expected-error{{redeclaration of 'CTMTR::StaticField' cannot add 'dllexport' attribute}}
template<typename T> template<typename U> __declspec(dllexport) const  int  CTMTR<T>::StaticConstField = 1;  // expected-error{{redeclaration of 'CTMTR::StaticConstField' cannot add 'dllexport' attribute}}
template<typename T> template<typename U> __declspec(dllexport) constexpr int CTMTR<T>::ConstexprField;      // expected-error{{redeclaration of 'CTMTR::ConstexprField' cannot add 'dllexport' attribute}}
#endif // __has_feature(cxx_variable_templates)

// FIXME: Precedence rules seem to be different for classes.
