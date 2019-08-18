#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdbool.h>

/* TODO: fix ==> errno: 22, ret: -1 */

struct stack_t {
  int size;
  uint64_t* ptr;
  uint64_t* initial_rsp;
};
struct register_state_t {
  bool done;
  uint64_t rbx;
  uint64_t rbp;
  uint64_t rsp;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;
};
struct stack_t stack;
struct register_state_t register_state;

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

static inline __attribute__((always_inline)) uint64_t* get_rsp() {
  uint64_t* rsp;
  asm volatile (
    "mov %%rsp, %0;"
    : "=r"(rsp)
    );
  return rsp;
}

static inline __attribute__((always_inline)) void restore_context() {
  register_state.done = true;
  asm volatile (
    "mov %[rbx], %%rbx;"
    "mov %[rbp], %%rbp;"
    "mov %[rsp], %%rsp;"
    "mov %[r12], %%r12;"
    "mov %[r13], %%r13;"
    "mov %[r14], %%r14;"
    "mov %[r15], %%r15;"
    "pop %%rbp;"
    "ret;"
    ::
     [rbx]"r"(register_state.rbx),
     [rbp]"r"(register_state.rbp),
     [rsp]"r"(register_state.rsp),
     [r12]"r"(register_state.r12),
     [r13]"r"(register_state.r13),
     [r14]"r"(register_state.r14),
     [r15]"r"(register_state.r15)
    );
}
static inline __attribute__((always_inline)) void evacuate_context() {
  register_state.done = false;
  asm volatile (
    "mov %%rbx, %[rbx];"
    "mov %%rbp, %[rbp];"
    "mov %%rsp, %[rsp];"
    "mov %%r12, %[r12];"
    "mov %%r13, %[r13];"
    "mov %%r14, %[r14];"
    "mov %%r15, %[r15];"
    :
     [rbx]"=r"(register_state.rbx),
     [rbp]"=r"(register_state.rbp),
     [rsp]"=r"(register_state.rsp),
     [r12]"=r"(register_state.r12),
     [r13]"=r"(register_state.r13),
     [r14]"=r"(register_state.r14),
     [r15]"=r"(register_state.r15)
    );
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
    printf("alloca: %p, curr: %p, rsp: %p\n", new_stack.ptr, stack.ptr, rsp);
    unprotect_mem_region(stack.ptr);
    memcpy(new_stack.ptr + stack.size, stack.ptr, sizeof(uint64_t) * size);
    /* update reference */
    for(uint64_t* p = new_stack.ptr + new_stack.size - 1;
        p >= new_stack.ptr + stack.size;
        p--) {
      if (*p >= (uint64_t)stack.ptr && *p < (uint64_t)(stack.ptr + stack.size)) {
        uint64_t offset = ((uint64_t)(stack.ptr + stack.size) - *p) / 8;
        uint64_t new_addr = new_stack.ptr + new_stack.size - offset;
        /* printf("offset: %llu, %p => %p\n", offset, *p, new_addr); */
        *p = new_addr;
      }
    }
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

void get_stack_size() {
  printf("REST OF STACK SIZE: %ld\n", (get_rsp() - stack.ptr));
}

void f(int v) {
  adjust_stacksize();
  int d = v; // stack allocate
  int* a = &d;
  if (d < 0)
    return;
  if (d % 10000 == 0) {
    get_stack_size();
    printf("[start] num: %d, stack_size: %d, addr: %p\n", d, stack.size, a);
  }
  f(v - 1);
  if (d % 10000 == 0) {
    get_stack_size();
    uint64_t* rsp = get_rsp();
    printf("[end] num: %d, addr: %p, rsp: %p\n", *a, a, rsp);
  }
  return;
}

void start() {
  f(100000);
}

__attribute__((noreturn)) void finish() {
  restore_context();
}

void run() {
  evacuate_context();
  if (register_state.done)
    return;
  stack.size = STACK_SIZE;
  stack.ptr = (uint64_t *)malloc(sizeof(uint64_t) * STACK_SIZE);
  stack.ptr[STACK_SIZE-2] = (uint64_t)start;
  stack.ptr[STACK_SIZE-1] = (uint64_t)finish;
  protect_mem_region(stack.ptr);
  stack.initial_rsp = (uint64_t *)&stack.ptr[STACK_SIZE-2];
  /* change stack pointer */
  asm volatile (
    "mov %0, %%rsp;"
    "ret"
    :: "r"(stack.initial_rsp)
    );
}

int main() {
  run();
  printf("finish!\n");
  exit(0);
}
