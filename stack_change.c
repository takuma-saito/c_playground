#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>

struct stack_t {
  int stack_size;
  uint64_t* ptr;
};

#define STACK_SIZE 0x4000
#define RED_ZONE 0x1000

void f(int v) {
  int d = v; // stack allocate
  if (d < 0)
    return;
  if (d % 100 == 0)
    printf("%d\n", d);
  f(v - 1);
  return;
}

void start() {
  f(10000);
}

int main() {
  struct stack_t stack;
  stack.stack_size = STACK_SIZE;
  stack.ptr = (uint64_t *)malloc(STACK_SIZE);
  *(uint64_t *)&stack.ptr[STACK_SIZE - 16] = (uint64_t)start;
  int ret = mprotect(stack.ptr, RED_ZONE, PROT_NONE);
  if (ret != 0) {
    printf("errno: %d, ret: %d\n", errno, ret);
    exit(ret);
  }
  /* TODO: context 退避 */
  asm volatile (
    "mov %0, %%rsp\n\t;"
    "ret"
    :: "r"(&stack.ptr[STACK_SIZE - 16])
    );
}
