#include <stdlib.h>
#include <stdio.h>
int main() {
  void* m = malloc(128);
  printf("%p\n", m);
  exit(0);
}
