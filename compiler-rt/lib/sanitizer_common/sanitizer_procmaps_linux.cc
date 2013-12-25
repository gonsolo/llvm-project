//===-- sanitizer_procmaps_linux.cc ---------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Information about the process mappings (Linux-specific parts).
//===----------------------------------------------------------------------===//

#include "sanitizer_platform.h"
#if SANITIZER_LINUX
#include "sanitizer_common.h"
#include "sanitizer_placement_new.h"
#include "sanitizer_procmaps.h"

namespace __sanitizer {

// Linker initialized.
ProcSelfMapsBuff MemoryMappingLayout::cached_proc_self_maps_;
StaticSpinMutex MemoryMappingLayout::cache_lock_;  // Linker initialized.

MemoryMappingLayout::MemoryMappingLayout(bool cache_enabled) {
  proc_self_maps_.len =
      ReadFileToBuffer("/proc/self/maps", &proc_self_maps_.data,
                       &proc_self_maps_.mmaped_size, 1 << 26);
  if (cache_enabled) {
    if (proc_self_maps_.mmaped_size == 0) {
      LoadFromCache();
      CHECK_GT(proc_self_maps_.len, 0);
    }
  } else {
    CHECK_GT(proc_self_maps_.mmaped_size, 0);
  }
  Reset();
  // FIXME: in the future we may want to cache the mappings on demand only.
  if (cache_enabled)
    CacheMemoryMappings();
}

MemoryMappingLayout::~MemoryMappingLayout() {
  // Only unmap the buffer if it is different from the cached one. Otherwise
  // it will be unmapped when the cache is refreshed.
  if (proc_self_maps_.data != cached_proc_self_maps_.data) {
    UnmapOrDie(proc_self_maps_.data, proc_self_maps_.mmaped_size);
  }
}

void MemoryMappingLayout::Reset() {
  current_ = proc_self_maps_.data;
}

// static
void MemoryMappingLayout::CacheMemoryMappings() {
  SpinMutexLock l(&cache_lock_);
  // Don't invalidate the cache if the mappings are unavailable.
  ProcSelfMapsBuff old_proc_self_maps;
  old_proc_self_maps = cached_proc_self_maps_;
  cached_proc_self_maps_.len =
      ReadFileToBuffer("/proc/self/maps", &cached_proc_self_maps_.data,
                       &cached_proc_self_maps_.mmaped_size, 1 << 26);
  if (cached_proc_self_maps_.mmaped_size == 0) {
    cached_proc_self_maps_ = old_proc_self_maps;
  } else {
    if (old_proc_self_maps.mmaped_size) {
      UnmapOrDie(old_proc_self_maps.data,
                 old_proc_self_maps.mmaped_size);
    }
  }
}

void MemoryMappingLayout::LoadFromCache() {
  SpinMutexLock l(&cache_lock_);
  if (cached_proc_self_maps_.data) {
    proc_self_maps_ = cached_proc_self_maps_;
  }
}

// Parse a hex value in str and update str.
static uptr ParseHex(char **str) {
  uptr x = 0;
  char *s;
  for (s = *str; ; s++) {
    char c = *s;
    uptr v = 0;
    if (c >= '0' && c <= '9')
      v = c - '0';
    else if (c >= 'a' && c <= 'f')
      v = c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
      v = c - 'A' + 10;
    else
      break;
    x = x * 16 + v;
  }
  *str = s;
  return x;
}

static bool IsOneOf(char c, char c1, char c2) {
  return c == c1 || c == c2;
}

static bool IsDecimal(char c) {
  return c >= '0' && c <= '9';
}

static bool IsHex(char c) {
  return (c >= '0' && c <= '9')
      || (c >= 'a' && c <= 'f');
}

static uptr ReadHex(const char *p) {
  uptr v = 0;
  for (; IsHex(p[0]); p++) {
    if (p[0] >= '0' && p[0] <= '9')
      v = v * 16 + p[0] - '0';
    else
      v = v * 16 + p[0] - 'a' + 10;
  }
  return v;
}

static uptr ReadDecimal(const char *p) {
  uptr v = 0;
  for (; IsDecimal(p[0]); p++)
    v = v * 10 + p[0] - '0';
  return v;
}

bool MemoryMappingLayout::Next(uptr *start, uptr *end, uptr *offset,
                               char filename[], uptr filename_size,
                               uptr *protection) {
  char *last = proc_self_maps_.data + proc_self_maps_.len;
  if (current_ >= last) return false;
  uptr dummy;
  if (!start) start = &dummy;
  if (!end) end = &dummy;
  if (!offset) offset = &dummy;
  char *next_line = (char*)internal_memchr(current_, '\n', last - current_);
  if (next_line == 0)
    next_line = last;
  // Example: 08048000-08056000 r-xp 00000000 03:0c 64593   /foo/bar
  *start = ParseHex(&current_);
  CHECK_EQ(*current_++, '-');
  *end = ParseHex(&current_);
  CHECK_EQ(*current_++, ' ');
  uptr local_protection = 0;
  CHECK(IsOneOf(*current_, '-', 'r'));
  if (*current_++ == 'r')
    local_protection |= kProtectionRead;
  CHECK(IsOneOf(*current_, '-', 'w'));
  if (*current_++ == 'w')
    local_protection |= kProtectionWrite;
  CHECK(IsOneOf(*current_, '-', 'x'));
  if (*current_++ == 'x')
    local_protection |= kProtectionExecute;
  CHECK(IsOneOf(*current_, 's', 'p'));
  if (*current_++ == 's')
    local_protection |= kProtectionShared;
  if (protection) {
    *protection = local_protection;
  }
  CHECK_EQ(*current_++, ' ');
  *offset = ParseHex(&current_);
  CHECK_EQ(*current_++, ' ');
  ParseHex(&current_);
  CHECK_EQ(*current_++, ':');
  ParseHex(&current_);
  CHECK_EQ(*current_++, ' ');
  while (IsDecimal(*current_))
    current_++;
  // Qemu may lack the trailing space.
  // http://code.google.com/p/address-sanitizer/issues/detail?id=160
  // CHECK_EQ(*current_++, ' ');
  // Skip spaces.
  while (current_ < next_line && *current_ == ' ')
    current_++;
  // Fill in the filename.
  uptr i = 0;
  while (current_ < next_line) {
    if (filename && i < filename_size - 1)
      filename[i++] = *current_;
    current_++;
  }
  if (filename && i < filename_size)
    filename[i] = 0;
  current_ = next_line + 1;
  return true;
}

// Gets the object name and the offset by walking MemoryMappingLayout.
bool MemoryMappingLayout::GetObjectNameAndOffset(uptr addr, uptr *offset,
                                                 char filename[],
                                                 uptr filename_size,
                                                 uptr *protection) {
  return IterateForObjectNameAndOffset(addr, offset, filename, filename_size,
                                       protection);
}

uptr MemoryMappingLayout::DumpListOfModules(LoadedModule *modules,
                                            uptr max_modules,
                                            string_predicate_t filter) {
  Reset();
  uptr cur_beg, cur_end;
  InternalScopedBuffer<char> module_name(kMaxPathLength);
  uptr n_modules = 0;
  for (uptr i = 0; n_modules < max_modules &&
                       Next(&cur_beg, &cur_end, 0, module_name.data(),
                            module_name.size(), 0);
       i++) {
    const char *cur_name = module_name.data();
    if (cur_name[0] == '\0')
      continue;
    if (filter && !filter(cur_name))
      continue;
    void *mem = &modules[n_modules];
    LoadedModule *cur_module = new(mem) LoadedModule(cur_name, cur_beg);
    cur_module->addAddressRange(cur_beg, cur_end);
    n_modules++;
  }
  return n_modules;
}

void GetMemoryProfile(fill_profile_f cb, uptr *stats, uptr stats_size) {
  char *smaps = 0;
  uptr smaps_cap = 0;
  uptr smaps_len = ReadFileToBuffer("/proc/self/smaps",
      &smaps, &smaps_cap, 64<<20);
  uptr start = 0;
  bool file = false;
  const char *pos = smaps;
  while (pos < smaps + smaps_len) {
    if (IsHex(pos[0])) {
      start = ReadHex(pos);
      for (; *pos != '/' && *pos > '\n'; pos++) {}
      file = *pos == '/';
    } else if (internal_strncmp(pos, "Rss:", 4) == 0) {
      for (; *pos < '0' || *pos > '9'; pos++) {}
      uptr rss = ReadDecimal(pos) * 1024;
      cb(start, rss, file, stats, stats_size);
    }
    while (*pos++ != '\n') {}
  }
  UnmapOrDie(smaps, smaps_cap);
}

}  // namespace __sanitizer

#endif  // SANITIZER_LINUX
