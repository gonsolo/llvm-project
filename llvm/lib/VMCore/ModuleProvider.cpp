//===-- ModuleProvider.cpp - Base implementation for module providers -----===//
//
// Minimal implementation of the abstract interface for providing a module.
//
//===----------------------------------------------------------------------===//

#include "llvm/ModuleProvider.h"
#include "llvm/Module.h"

/// ctor - always have a valid Module
///
ModuleProvider::ModuleProvider() : TheModule(0) { }

/// dtor - when we leave, we take our Module with us
///
ModuleProvider::~ModuleProvider() {
  delete TheModule;
}

/// materializeFunction - make sure the given function is fully read.
///
void ModuleProvider::materializeModule() {
  if (!TheModule) return;

  for (Module::iterator i = TheModule->begin(), e = TheModule->end();
       i != e; ++i)
    materializeFunction(i);
}
