#include <unistd.h>

int main() {
  char *const argv[] = {"/bin/sh"};
  char *const env[] = {NULL};
  execve(argv[0], argv, env);
  return 0;
}
