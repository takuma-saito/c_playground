#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

#define SIZE 4096

int main(int argc, char *argv[]) {
  void* addr = (void *)calloc(SIZE, sizeof(char));
  void (*func)();
  size_t c = fread(addr, 1, SIZE, stdin);
  printf("read bytes: %d\n", c);
  int ret = mprotect(addr, SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
  if (ret != 0) {
    printf("errno: %d, ret: %d\n", errno, ret);
    exit(ret);
  }
  *&func = addr;
  func();
}
