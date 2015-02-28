//===-- sanitizer_symbolizer_posix_libcdep.cc -----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is shared between AddressSanitizer and ThreadSanitizer
// run-time libraries.
// POSIX-specific implementation of symbolizer parts.
//===----------------------------------------------------------------------===//

#include "sanitizer_platform.h"
#if SANITIZER_POSIX
#include "sanitizer_allocator_internal.h"
#include "sanitizer_common.h"
#include "sanitizer_flags.h"
#include "sanitizer_internal_defs.h"
#include "sanitizer_linux.h"
#include "sanitizer_placement_new.h"
#include "sanitizer_procmaps.h"
#include "sanitizer_symbolizer.h"
#include "sanitizer_symbolizer_libbacktrace.h"

#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// C++ demangling function, as required by Itanium C++ ABI. This is weak,
// because we do not require a C++ ABI library to be linked to a program
// using sanitizers; if it's not present, we'll just use the mangled name.
namespace __cxxabiv1 {
  extern "C" SANITIZER_WEAK_ATTRIBUTE
  char *__cxa_demangle(const char *mangled, char *buffer,
                                  size_t *length, int *status);
}

namespace __sanitizer {

// Attempts to demangle the name via __cxa_demangle from __cxxabiv1.
static const char *DemangleCXXABI(const char *name) {
  // FIXME: __cxa_demangle aggressively insists on allocating memory.
  // There's not much we can do about that, short of providing our
  // own demangler (libc++abi's implementation could be adapted so that
  // it does not allocate). For now, we just call it anyway, and we leak
  // the returned value.
  if (__cxxabiv1::__cxa_demangle)
    if (const char *demangled_name =
          __cxxabiv1::__cxa_demangle(name, 0, 0, 0))
      return demangled_name;

  return name;
}

// Extracts the prefix of "str" that consists of any characters not
// present in "delims" string, and copies this prefix to "result", allocating
// space for it.
// Returns a pointer to "str" after skipping extracted prefix and first
// delimiter char.
static const char *ExtractToken(const char *str, const char *delims,
                                char **result) {
  uptr prefix_len = internal_strcspn(str, delims);
  *result = (char*)InternalAlloc(prefix_len + 1);
  internal_memcpy(*result, str, prefix_len);
  (*result)[prefix_len] = '\0';
  const char *prefix_end = str + prefix_len;
  if (*prefix_end != '\0') prefix_end++;
  return prefix_end;
}

// Same as ExtractToken, but converts extracted token to integer.
static const char *ExtractInt(const char *str, const char *delims,
                              int *result) {
  char *buff;
  const char *ret = ExtractToken(str, delims, &buff);
  if (buff != 0) {
    *result = (int)internal_atoll(buff);
  }
  InternalFree(buff);
  return ret;
}

static const char *ExtractUptr(const char *str, const char *delims,
                               uptr *result) {
  char *buff;
  const char *ret = ExtractToken(str, delims, &buff);
  if (buff != 0) {
    *result = (uptr)internal_atoll(buff);
  }
  InternalFree(buff);
  return ret;
}

SymbolizerProcess::SymbolizerProcess(const char *path)
    : path_(path),
      input_fd_(kInvalidFd),
      output_fd_(kInvalidFd),
      times_restarted_(0),
      failed_to_start_(false),
      reported_invalid_path_(false) {
  CHECK(path_);
  CHECK_NE(path_[0], '\0');
}

const char *SymbolizerProcess::SendCommand(const char *command) {
  for (; times_restarted_ < kMaxTimesRestarted; times_restarted_++) {
    // Start or restart symbolizer if we failed to send command to it.
    if (const char *res = SendCommandImpl(command))
      return res;
    Restart();
  }
  if (!failed_to_start_) {
    Report("WARNING: Failed to use and restart external symbolizer!\n");
    failed_to_start_ = true;
  }
  return 0;
}

bool SymbolizerProcess::Restart() {
  if (input_fd_ != kInvalidFd)
    internal_close(input_fd_);
  if (output_fd_ != kInvalidFd)
    internal_close(output_fd_);
  return StartSymbolizerSubprocess();
}

const char *SymbolizerProcess::SendCommandImpl(const char *command) {
  if (input_fd_ == kInvalidFd || output_fd_ == kInvalidFd)
      return 0;
  if (!WriteToSymbolizer(command, internal_strlen(command)))
      return 0;
  if (!ReadFromSymbolizer(buffer_, kBufferSize))
      return 0;
  return buffer_;
}

bool SymbolizerProcess::ReadFromSymbolizer(char *buffer, uptr max_length) {
  if (max_length == 0)
    return true;
  uptr read_len = 0;
  while (true) {
    uptr just_read = internal_read(input_fd_, buffer + read_len,
                                   max_length - read_len - 1);
    // We can't read 0 bytes, as we don't expect external symbolizer to close
    // its stdout.
    if (just_read == 0 || just_read == (uptr)-1) {
      Report("WARNING: Can't read from symbolizer at fd %d\n", input_fd_);
      return false;
    }
    read_len += just_read;
    if (ReachedEndOfOutput(buffer, read_len))
      break;
  }
  buffer[read_len] = '\0';
  return true;
}

bool SymbolizerProcess::WriteToSymbolizer(const char *buffer, uptr length) {
  if (length == 0)
    return true;
  uptr write_len = internal_write(output_fd_, buffer, length);
  if (write_len == 0 || write_len == (uptr)-1) {
    Report("WARNING: Can't write to symbolizer at fd %d\n", output_fd_);
    return false;
  }
  return true;
}

bool SymbolizerProcess::StartSymbolizerSubprocess() {
  if (!FileExists(path_)) {
    if (!reported_invalid_path_) {
      Report("WARNING: invalid path to external symbolizer!\n");
      reported_invalid_path_ = true;
    }
    return false;
  }

  int *infd = NULL;
  int *outfd = NULL;
  // The client program may close its stdin and/or stdout and/or stderr
  // thus allowing socketpair to reuse file descriptors 0, 1 or 2.
  // In this case the communication between the forked processes may be
  // broken if either the parent or the child tries to close or duplicate
  // these descriptors. The loop below produces two pairs of file
  // descriptors, each greater than 2 (stderr).
  int sock_pair[5][2];
  for (int i = 0; i < 5; i++) {
    if (pipe(sock_pair[i]) == -1) {
      for (int j = 0; j < i; j++) {
        internal_close(sock_pair[j][0]);
        internal_close(sock_pair[j][1]);
      }
      Report("WARNING: Can't create a socket pair to start "
             "external symbolizer (errno: %d)\n", errno);
      return false;
    } else if (sock_pair[i][0] > 2 && sock_pair[i][1] > 2) {
      if (infd == NULL) {
        infd = sock_pair[i];
      } else {
        outfd = sock_pair[i];
        for (int j = 0; j < i; j++) {
          if (sock_pair[j] == infd) continue;
          internal_close(sock_pair[j][0]);
          internal_close(sock_pair[j][1]);
        }
        break;
      }
    }
  }
  CHECK(infd);
  CHECK(outfd);

  // Real fork() may call user callbacks registered with pthread_atfork().
  int pid = internal_fork();
  if (pid == -1) {
    // Fork() failed.
    internal_close(infd[0]);
    internal_close(infd[1]);
    internal_close(outfd[0]);
    internal_close(outfd[1]);
    Report("WARNING: failed to fork external symbolizer "
           " (errno: %d)\n", errno);
    return false;
  } else if (pid == 0) {
    // Child subprocess.
    internal_close(STDOUT_FILENO);
    internal_close(STDIN_FILENO);
    internal_dup2(outfd[0], STDIN_FILENO);
    internal_dup2(infd[1], STDOUT_FILENO);
    internal_close(outfd[0]);
    internal_close(outfd[1]);
    internal_close(infd[0]);
    internal_close(infd[1]);
    for (int fd = sysconf(_SC_OPEN_MAX); fd > 2; fd--)
      internal_close(fd);
    ExecuteWithDefaultArgs(path_);
    internal__exit(1);
  }

  // Continue execution in parent process.
  internal_close(outfd[0]);
  internal_close(infd[1]);
  input_fd_ = infd[0];
  output_fd_ = outfd[1];

  // Check that symbolizer subprocess started successfully.
  int pid_status;
  SleepForMillis(kSymbolizerStartupTimeMillis);
  int exited_pid = waitpid(pid, &pid_status, WNOHANG);
  if (exited_pid != 0) {
    // Either waitpid failed, or child has already exited.
    Report("WARNING: external symbolizer didn't start up correctly!\n");
    return false;
  }

  return true;
}


// Parses one or more two-line strings in the following format:
//   <function_name>
//   <file_name>:<line_number>[:<column_number>]
// Used by LLVMSymbolizer, Addr2LinePool and InternalSymbolizer, since all of
// them use the same output format.
static void ParseSymbolizePCOutput(const char *str, SymbolizedStack *res) {
  bool top_frame = true;
  SymbolizedStack *last = res;
  while (true) {
    char *function_name = 0;
    str = ExtractToken(str, "\n", &function_name);
    CHECK(function_name);
    if (function_name[0] == '\0') {
      // There are no more frames.
      break;
    }
    SymbolizedStack *cur;
    if (top_frame) {
      cur = res;
      top_frame = false;
    } else {
      cur = SymbolizedStack::New(res->info.address);
      cur->info.FillAddressAndModuleInfo(res->info.address, res->info.module,
                                         res->info.module_offset);
      last->next = cur;
      last = cur;
    }

    AddressInfo *info = &cur->info;
    info->function = function_name;
    // Parse <file>:<line>:<column> buffer.
    char *file_line_info = 0;
    str = ExtractToken(str, "\n", &file_line_info);
    CHECK(file_line_info);
    const char *line_info = ExtractToken(file_line_info, ":", &info->file);
    line_info = ExtractInt(line_info, ":", &info->line);
    line_info = ExtractInt(line_info, "", &info->column);
    InternalFree(file_line_info);

    // Functions and filenames can be "??", in which case we write 0
    // to address info to mark that names are unknown.
    if (0 == internal_strcmp(info->function, "??")) {
      InternalFree(info->function);
      info->function = 0;
    }
    if (0 == internal_strcmp(info->file, "??")) {
      InternalFree(info->file);
      info->file = 0;
    }
  }
}

// Parses a two-line string in the following format:
//   <symbol_name>
//   <start_address> <size>
// Used by LLVMSymbolizer and InternalSymbolizer.
static void ParseSymbolizeDataOutput(const char *str, DataInfo *info) {
  str = ExtractToken(str, "\n", &info->name);
  str = ExtractUptr(str, " ", &info->start);
  str = ExtractUptr(str, "\n", &info->size);
}

// For now we assume the following protocol:
// For each request of the form
//   <module_name> <module_offset>
// passed to STDIN, external symbolizer prints to STDOUT response:
//   <function_name>
//   <file_name>:<line_number>:<column_number>
//   <function_name>
//   <file_name>:<line_number>:<column_number>
//   ...
//   <empty line>
class LLVMSymbolizerProcess : public SymbolizerProcess {
 public:
  explicit LLVMSymbolizerProcess(const char *path) : SymbolizerProcess(path) {}

 private:
  bool ReachedEndOfOutput(const char *buffer, uptr length) const {
    // Empty line marks the end of llvm-symbolizer output.
    return length >= 2 && buffer[length - 1] == '\n' &&
           buffer[length - 2] == '\n';
  }

  void ExecuteWithDefaultArgs(const char *path_to_binary) const {
#if defined(__x86_64__)
    const char* const kSymbolizerArch = "--default-arch=x86_64";
#elif defined(__i386__)
    const char* const kSymbolizerArch = "--default-arch=i386";
#elif defined(__powerpc64__) && defined(__BIG_ENDIAN__)
    const char* const kSymbolizerArch = "--default-arch=powerpc64";
#elif defined(__powerpc64__) && defined(__LITTLE_ENDIAN__)
    const char* const kSymbolizerArch = "--default-arch=powerpc64le";
#else
    const char* const kSymbolizerArch = "--default-arch=unknown";
#endif

    const char *const inline_flag = common_flags()->symbolize_inline_frames
                                        ? "--inlining=true"
                                        : "--inlining=false";
    execl(path_to_binary, path_to_binary, inline_flag, kSymbolizerArch,
          (char *)0);
  }
};

class LLVMSymbolizer : public SymbolizerTool {
 public:
  explicit LLVMSymbolizer(const char *path, LowLevelAllocator *allocator)
      : symbolizer_process_(new(*allocator) LLVMSymbolizerProcess(path)) {}

  bool SymbolizePC(uptr addr, SymbolizedStack *stack) override {
    if (const char *buf = SendCommand(/*is_data*/ false, stack->info.module,
                                      stack->info.module_offset)) {
      ParseSymbolizePCOutput(buf, stack);
      return true;
    }
    return false;
  }

  bool SymbolizeData(uptr addr, DataInfo *info) override {
    if (const char *buf =
            SendCommand(/*is_data*/ true, info->module, info->module_offset)) {
      ParseSymbolizeDataOutput(buf, info);
      return true;
    }
    return false;
  }

 private:
  const char *SendCommand(bool is_data, const char *module_name,
                          uptr module_offset) {
    CHECK(module_name);
    internal_snprintf(buffer_, kBufferSize, "%s\"%s\" 0x%zx\n",
                      is_data ? "DATA " : "", module_name, module_offset);
    return symbolizer_process_->SendCommand(buffer_);
  }

  LLVMSymbolizerProcess *symbolizer_process_;
  static const uptr kBufferSize = 16 * 1024;
  char buffer_[kBufferSize];
};

class Addr2LineProcess : public SymbolizerProcess {
 public:
  Addr2LineProcess(const char *path, const char *module_name)
      : SymbolizerProcess(path), module_name_(internal_strdup(module_name)) {}

  const char *module_name() const { return module_name_; }

 private:
  bool ReachedEndOfOutput(const char *buffer, uptr length) const {
    // Output should consist of two lines.
    int num_lines = 0;
    for (uptr i = 0; i < length; ++i) {
      if (buffer[i] == '\n')
        num_lines++;
      if (num_lines >= 2)
        return true;
    }
    return false;
  }

  void ExecuteWithDefaultArgs(const char *path_to_binary) const {
    execl(path_to_binary, path_to_binary, "-Cfe", module_name_, (char *)0);
  }

  const char *module_name_;  // Owned, leaked.
};

class Addr2LinePool : public SymbolizerTool {
 public:
  explicit Addr2LinePool(const char *addr2line_path,
                         LowLevelAllocator *allocator)
      : addr2line_path_(addr2line_path), allocator_(allocator),
        addr2line_pool_(16) {}

  bool SymbolizePC(uptr addr, SymbolizedStack *stack) override {
    if (const char *buf =
            SendCommand(stack->info.module, stack->info.module_offset)) {
      ParseSymbolizePCOutput(buf, stack);
      return true;
    }
    return false;
  }

  bool SymbolizeData(uptr addr, DataInfo *info) override {
    return false;
  }

 private:
  const char *SendCommand(const char *module_name, uptr module_offset) {
    Addr2LineProcess *addr2line = 0;
    for (uptr i = 0; i < addr2line_pool_.size(); ++i) {
      if (0 ==
          internal_strcmp(module_name, addr2line_pool_[i]->module_name())) {
        addr2line = addr2line_pool_[i];
        break;
      }
    }
    if (!addr2line) {
      addr2line =
          new(*allocator_) Addr2LineProcess(addr2line_path_, module_name);
      addr2line_pool_.push_back(addr2line);
    }
    CHECK_EQ(0, internal_strcmp(module_name, addr2line->module_name()));
    char buffer_[kBufferSize];
    internal_snprintf(buffer_, kBufferSize, "0x%zx\n", module_offset);
    return addr2line->SendCommand(buffer_);
  }

  static const uptr kBufferSize = 32;
  const char *addr2line_path_;
  LowLevelAllocator *allocator_;
  InternalMmapVector<Addr2LineProcess*> addr2line_pool_;
};

#if SANITIZER_SUPPORTS_WEAK_HOOKS
extern "C" {
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
bool __sanitizer_symbolize_code(const char *ModuleName, u64 ModuleOffset,
                                char *Buffer, int MaxLength);
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
bool __sanitizer_symbolize_data(const char *ModuleName, u64 ModuleOffset,
                                char *Buffer, int MaxLength);
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
void __sanitizer_symbolize_flush();
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
int __sanitizer_symbolize_demangle(const char *Name, char *Buffer,
                                   int MaxLength);
}  // extern "C"

class InternalSymbolizer : public SymbolizerTool {
 public:
  static InternalSymbolizer *get(LowLevelAllocator *alloc) {
    if (__sanitizer_symbolize_code != 0 &&
        __sanitizer_symbolize_data != 0) {
      return new(*alloc) InternalSymbolizer();
    }
    return 0;
  }

  bool SymbolizePC(uptr addr, SymbolizedStack *stack) override {
    bool result = __sanitizer_symbolize_code(
        stack->info.module, stack->info.module_offset, buffer_, kBufferSize);
    if (result) ParseSymbolizePCOutput(buffer_, stack);
    return result;
  }

  bool SymbolizeData(uptr addr, DataInfo *info) override {
    bool result = __sanitizer_symbolize_data(info->module, info->module_offset,
                                             buffer_, kBufferSize);
    if (result) ParseSymbolizeDataOutput(buffer_, info);
    return result;
  }

  void Flush() override {
    if (__sanitizer_symbolize_flush)
      __sanitizer_symbolize_flush();
  }

  const char *Demangle(const char *name) override {
    if (__sanitizer_symbolize_demangle) {
      for (uptr res_length = 1024;
           res_length <= InternalSizeClassMap::kMaxSize;) {
        char *res_buff = static_cast<char*>(InternalAlloc(res_length));
        uptr req_length =
            __sanitizer_symbolize_demangle(name, res_buff, res_length);
        if (req_length > res_length) {
          res_length = req_length + 1;
          InternalFree(res_buff);
          continue;
        }
        return res_buff;
      }
    }
    return name;
  }

 private:
  InternalSymbolizer() { }

  static const int kBufferSize = 16 * 1024;
  static const int kMaxDemangledNameSize = 1024;
  char buffer_[kBufferSize];
};
#else  // SANITIZER_SUPPORTS_WEAK_HOOKS

class InternalSymbolizer : public SymbolizerTool {
 public:
  static InternalSymbolizer *get(LowLevelAllocator *alloc) { return 0; }
};

#endif  // SANITIZER_SUPPORTS_WEAK_HOOKS

class POSIXSymbolizer : public Symbolizer {
 public:
  POSIXSymbolizer(SymbolizerTool *external_symbolizer,
                  SymbolizerTool *internal_symbolizer,
                  LibbacktraceSymbolizer *libbacktrace_symbolizer)
      : Symbolizer(),
        external_symbolizer_(external_symbolizer),
        internal_symbolizer_(internal_symbolizer),
        libbacktrace_symbolizer_(libbacktrace_symbolizer) {}

  SymbolizedStack *SymbolizePC(uptr addr) override {
    BlockingMutexLock l(&mu_);
    const char *module_name;
    uptr module_offset;
    if (!FindModuleNameAndOffsetForAddress(addr, &module_name, &module_offset))
      return SymbolizedStack::New(addr);
    // First, try to use libbacktrace symbolizer (if it's available).
    if (libbacktrace_symbolizer_ != 0) {
      mu_.CheckLocked();
      if (SymbolizedStack *res = libbacktrace_symbolizer_->SymbolizeCode(
              addr, module_name, module_offset))
        return res;
    }
    // Always fill data about module name and offset.
    SymbolizedStack *res = SymbolizedStack::New(addr);
    res->info.FillAddressAndModuleInfo(addr, module_name, module_offset);
    if (SymbolizerTool *tool = GetSymbolizerTool()) {
      SymbolizerScope sym_scope(this);
      tool->SymbolizePC(addr, res);
    }
    return res;
  }

  bool SymbolizeData(uptr addr, DataInfo *info) override {
    BlockingMutexLock l(&mu_);
    LoadedModule *module = FindModuleForAddress(addr);
    if (module == 0)
      return false;
    const char *module_name = module->full_name();
    uptr module_offset = addr - module->base_address();
    info->Clear();
    info->module = internal_strdup(module_name);
    info->module_offset = module_offset;
    // First, try to use libbacktrace symbolizer (if it's available).
    if (libbacktrace_symbolizer_ != 0) {
      mu_.CheckLocked();
      if (libbacktrace_symbolizer_->SymbolizeData(addr, info))
        return true;
    }
    if (SymbolizerTool *tool = GetSymbolizerTool()) {
      SymbolizerScope sym_scope(this);
      tool->SymbolizeData(addr, info);
    }
    info->start += module->base_address();
    return true;
  }

  bool GetModuleNameAndOffsetForPC(uptr pc, const char **module_name,
                                   uptr *module_address) override {
    BlockingMutexLock l(&mu_);
    return FindModuleNameAndOffsetForAddress(pc, module_name, module_address);
  }

  bool CanReturnFileLineInfo() override {
    return internal_symbolizer_ != 0 || external_symbolizer_ != 0 ||
           libbacktrace_symbolizer_ != 0;
  }

  void Flush() override {
    BlockingMutexLock l(&mu_);
    if (internal_symbolizer_ != 0) {
      SymbolizerScope sym_scope(this);
      internal_symbolizer_->Flush();
    }
  }

  const char *Demangle(const char *name) override {
    BlockingMutexLock l(&mu_);
    // Run hooks even if we don't use internal symbolizer, as cxxabi
    // demangle may call system functions.
    SymbolizerScope sym_scope(this);
    // Try to use libbacktrace demangler (if available).
    if (libbacktrace_symbolizer_ != 0) {
      if (const char *demangled = libbacktrace_symbolizer_->Demangle(name))
        return demangled;
    }
    if (internal_symbolizer_ != 0)
      return internal_symbolizer_->Demangle(name);
    return DemangleCXXABI(name);
  }

  void PrepareForSandboxing() override {
#if SANITIZER_LINUX && !SANITIZER_ANDROID
    BlockingMutexLock l(&mu_);
    // Cache /proc/self/exe on Linux.
    CacheBinaryName();
#endif
  }

 private:
  SymbolizerTool *GetSymbolizerTool() {
    mu_.CheckLocked();
    if (internal_symbolizer_) return internal_symbolizer_;
    if (external_symbolizer_) return external_symbolizer_;
    return nullptr;
  }

  LoadedModule *FindModuleForAddress(uptr address) {
    mu_.CheckLocked();
    bool modules_were_reloaded = false;
    if (modules_ == 0 || !modules_fresh_) {
      modules_ = (LoadedModule*)(symbolizer_allocator_.Allocate(
          kMaxNumberOfModuleContexts * sizeof(LoadedModule)));
      CHECK(modules_);
      n_modules_ = GetListOfModules(modules_, kMaxNumberOfModuleContexts,
                                    /* filter */ 0);
      CHECK_GT(n_modules_, 0);
      CHECK_LT(n_modules_, kMaxNumberOfModuleContexts);
      modules_fresh_ = true;
      modules_were_reloaded = true;
    }
    for (uptr i = 0; i < n_modules_; i++) {
      if (modules_[i].containsAddress(address)) {
        return &modules_[i];
      }
    }
    // Reload the modules and look up again, if we haven't tried it yet.
    if (!modules_were_reloaded) {
      // FIXME: set modules_fresh_ from dlopen()/dlclose() interceptors.
      // It's too aggressive to reload the list of modules each time we fail
      // to find a module for a given address.
      modules_fresh_ = false;
      return FindModuleForAddress(address);
    }
    return 0;
  }

  bool FindModuleNameAndOffsetForAddress(uptr address, const char **module_name,
                                         uptr *module_offset) {
    mu_.CheckLocked();
    LoadedModule *module = FindModuleForAddress(address);
    if (module == 0)
      return false;
    *module_name = module->full_name();
    *module_offset = address - module->base_address();
    return true;
  }

  // 16K loaded modules should be enough for everyone.
  static const uptr kMaxNumberOfModuleContexts = 1 << 14;
  LoadedModule *modules_;  // Array of module descriptions is leaked.
  uptr n_modules_;
  // If stale, need to reload the modules before looking up addresses.
  bool modules_fresh_;
  BlockingMutex mu_;

  SymbolizerTool *const external_symbolizer_;         // Leaked.
  SymbolizerTool *const internal_symbolizer_;         // Leaked.
  LibbacktraceSymbolizer *libbacktrace_symbolizer_;   // Leaked.
};

Symbolizer *Symbolizer::PlatformInit() {
  if (!common_flags()->symbolize) {
    return new(symbolizer_allocator_) POSIXSymbolizer(0, 0, 0);
  }
  InternalSymbolizer* internal_symbolizer =
      InternalSymbolizer::get(&symbolizer_allocator_);
  SymbolizerTool *external_symbolizer = 0;
  LibbacktraceSymbolizer *libbacktrace_symbolizer = 0;

  if (!internal_symbolizer) {
    libbacktrace_symbolizer =
        LibbacktraceSymbolizer::get(&symbolizer_allocator_);
    if (!libbacktrace_symbolizer) {
      const char *path_to_external = common_flags()->external_symbolizer_path;
      if (path_to_external && path_to_external[0] == '\0') {
        // External symbolizer is explicitly disabled. Do nothing.
      } else {
        // Find path to llvm-symbolizer if it's not provided.
        if (!path_to_external)
          path_to_external = FindPathToBinary("llvm-symbolizer");
        if (path_to_external) {
          external_symbolizer = new(symbolizer_allocator_)
              LLVMSymbolizer(path_to_external, &symbolizer_allocator_);
        } else if (common_flags()->allow_addr2line) {
          // If llvm-symbolizer is not found, try to use addr2line.
          if (const char *addr2line_path = FindPathToBinary("addr2line")) {
            external_symbolizer = new(symbolizer_allocator_)
                Addr2LinePool(addr2line_path, &symbolizer_allocator_);
          }
        }
      }
    }
  }

  return new(symbolizer_allocator_) POSIXSymbolizer(
      external_symbolizer, internal_symbolizer, libbacktrace_symbolizer);
}

}  // namespace __sanitizer

#endif  // SANITIZER_POSIX
