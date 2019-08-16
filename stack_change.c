#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>

struct stack_t {
  int size;
  uint64_t* ptr;
  uint64_t* initial_rsp;
};
struct stack_t stack;

#define STACK_SIZE 0x2000
#define RED_ZONE 0x1000
#define YELLOW_ZONE 0x0800

void protect_mem_region(uint64_t* ptr) {
  int ret = mprotect(ptr, RED_ZONE, PROT_NONE);
  if (ret != 0) {
    printf("errno: %d, ret: %d\n", errno, ret);
    exit(ret);
  }
}
void unprotect_mem_region(uint64_t* ptr) {
  int ret = mprotect(ptr, RED_ZONE, PROT_READ | PROT_WRITE);
  if (ret != 0) {
    printf("errno: %d, ret: %d\n", errno, ret);
    exit(ret);
  }
}

/* force inline */
static inline __attribute__((always_inline)) uint64_t* get_rsp() {
  uint64_t* rsp;
  asm volatile (
    "mov %%rsp, %0;"
    : "=r"(rsp)
    );
  return rsp;
}

/* TODO: Stack references must be moved! */
void adjust_stacksize() {
  uint64_t* rsp = get_rsp();
  int size = stack.size;
  int new_size = size * 2;
  struct stack_t new_stack, pre_stack;
  uint64_t rest = (rsp - stack.ptr);
  if (rest < (YELLOW_ZONE + RED_ZONE)) {
    new_stack.ptr = malloc(sizeof(uint64_t) * new_size);
    new_stack.size = new_size;
    new_stack.initial_rsp = new_stack.ptr + new_stack.size - (stack.ptr + stack.size - rsp);
    printf("alloca: %llx, curr: %llx\n", (uint64_t)new_stack.ptr, (uint64_t)stack.ptr);
    unprotect_mem_region(stack.ptr);
    memcpy(new_stack.ptr + size, stack.ptr, sizeof(uint64_t) * size);
    pre_stack = stack;
    stack = new_stack;
    asm volatile (
      "mov %0, %%rsp;"
      :: "r"(new_stack.initial_rsp)
      );
    free(stack.ptr);
    protect_mem_region(new_stack.ptr);
  }
}

void f(int v) {
  adjust_stacksize();
  int d = v; // stack allocate
  if (d < 0)
    return;
  if (d % 100 == 0) {
    printf("num: %d, stack_size: %d\n", d, stack.size);
  }
  f(v - 1);
  return;
}

void start() {
  printf("size: %ld\n", (get_rsp() - stack.ptr));
  f(10000);
}

void finish() {
  asm volatile ("add $8, %rsp;"); /* force align 16 bytes */
  exit(0);
}

int main() {
  stack.size = STACK_SIZE;
  stack.ptr = (uint64_t *)malloc(sizeof(uint64_t) * STACK_SIZE);
  *(uint64_t *)&stack.ptr[STACK_SIZE-2] = (uint64_t)start;
  *(uint64_t *)&stack.ptr[STACK_SIZE-1] = (uint64_t)finish;
  protect_mem_region(stack.ptr);
  stack.initial_rsp = (uint64_t *)&stack.ptr[STACK_SIZE-2];
  /* TODO: context 退避 */
  asm volatile (
    "mov %0, %%rsp;"
    "ret"
    :: "r"(stack.initial_rsp)
    );
}
