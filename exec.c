#include <unistd.h>

int main() {
  char *const argv[] = {"/bin/ls", "-al"};
  char *const env[] = {NULL};
  execve(argv[0], argv, env);
  return 0;
}
