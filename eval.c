#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

#define PAYLOAD_SIZE 2000

int main() {
  char* payload = "\x48\x8d\x3d\x35\x00\x00\x00\x48\x89\xf8\x48\xff\xc0\x48\x8b\x10\x48\x83\xfa\x00\x75\xf4\x48\x8d\x35\x1f\x00\x00\x00\x48\x29\xf0\x48\x89\xc2\xb8\x04\x00\x00\x02\xbf\x01\x00\x00\x00\x0f\x05\xb8\x01\x00\x00\x02\xbf\x00\x00\x00\x00\x0f\x05\x00\x48\x65\x6c\x6c\x6f\x2c\x20\x77\x6f\x72\x6c\x64\x21\x0a\x00";
  void* addr = (void *)malloc(PAYLOAD_SIZE); // TODO
  void (*func)();
  memcpy(addr, payload, PAYLOAD_SIZE);
  int ret = mprotect(addr, PAYLOAD_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
  if (ret != 0) {
    printf("ret: %d", ret);
    exit(ret);
  }
  *((void **)&func) = addr; // 謎: https://stackoverflow.com/questions/13696918/c-cast-void-pointer-to-function-pointer
  func();
}
