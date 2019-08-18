#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <assert.h>

#define SIZE 4096

char SHELLCODE[] = "\x48\x8d\x3d\x41\x00\x00\x00\xe8\x00\x00\x00\x00\x48\x89\xf8\x48\xff\xc0\x48\x8b\x10\x48\x81\xe2\xff\x00\x00\x00\x48\x83\xfa\x00\x75\xed\x48\x8d\x35\x1f\x00\x00\x00\x48\x29\xf0\x48\x89\xc2\xb8\x04\x00\x00\x02\xbf\x01\x00\x00\x00\x0f\x05\xb8\x01\x00\x00\x02\xbf\x00\x00\x00\x00\x0f\x05\x00\x48\x65\x6c\x6c\x6f\x2c\x20\x77\x6f\x72\x6c\x64\x21\x0a\x00";

struct invoke_t {
  void (*fn)();
};

void ok() {
  printf("ok\n");
}

int main() {
  size_t size = sizeof(struct invoke_t);
  struct invoke_t* m = (struct invoke_t*) malloc(size);
  m->fn = ok;
  printf("%p\n", m);
  m->fn();
  free(m);
  void** d = (void **)malloc(size);
  void* payload = (void *)malloc(SIZE);
  printf("%p\n", d);
  memcpy(payload, SHELLCODE, SIZE);
  int ret = mprotect(payload, SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
  assert((uint64_t)m == (uint64_t)d); /* m and d points to same address */
  if (ret != 0) {
    printf("addr: %p, ret: %d, errono: %d\n", payload, ret, errno);
    exit(2);
  }
  *d = payload;
  m->fn();                      /* use after free */
}
