#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <dlfcn.h>

static void* (*real_connect)(int, struct sockaddr *, socklen_t) = NULL;
static FILE* log;
static char* filename = "connect_logs.txt";

int hello() {
  return 128;
}
__attribute__((constructor))
static void init() {
  real_connect = (void* (*)(int, struct sockaddr *, socklen_t)) dlsym(RTLD_NEXT, "connect");
  if (NULL == real_connect) {
    fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
    exit(2);
  }
  log = fopen(filename, "a");
  if (NULL == log) {
    fprintf(stderr, "Error in `fopen`: %s\n", filename);
    exit(2);    
  }
}
__attribute__((destructor))
static void finish() {
  fclose(log);
}

connect(int socket, const struct sockaddr *address,
       socklen_t address_len) {
  if (address->sa_family == AF_INET) {
    struct sockaddr_in *sin = (struct sockaddr_in *) address;
    char *ip = inet_ntoa(sin->sin_addr);
    fprintf(stderr, "connect: %s\n", ip);
    fprintf(log, "connect: %s\n", ip);
  }
  return real_connect(socket, address, address_len);
}
