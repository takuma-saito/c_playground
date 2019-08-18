#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>

// https://opensource.apple.com/source/dyld/dyld-239.3/include/mach-o/dyld-interposing.h
#define DYLD_INTERPOSE(_replacement,_replacee) \
   __attribute__((used)) static struct{ const void* replacement; const void* replacee; } _interpose_##_replacee \
   __attribute__ ((section ("__DATA,__interpose"))) = { (const void*)(unsigned long)&_replacement, (const void*)(unsigned long)&_replacee };

#define pagesize 4096
#define align(size, pagesize) (((size / pagesize) + 1) * pagesize)

void* executable_malloc(size_t size) {
  write(STDOUT_FILENO, "This is a wrapper function for malloc.\n", 40);
  size_t new_size = align(size, pagesize);
  void* addr = malloc(new_size);
  int ret = mprotect(addr, new_size, PROT_READ | PROT_WRITE | PROT_EXEC);
  if (ret != 0) {
    exit(ret);
  }
  return addr;
}

DYLD_INTERPOSE(executable_malloc, malloc)

