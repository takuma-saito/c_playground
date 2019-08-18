#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

// https://opensource.apple.com/source/dyld/dyld-239.3/include/mach-o/dyld-interposing.h
#define DYLD_INTERPOSE(_replacement,_replacee) \
   __attribute__((used)) static struct{ const void* replacement; const void* replacee; } _interpose_##_replacee \
   __attribute__ ((section ("__DATA,__interpose"))) = { (const void*)(unsigned long)&_replacement, (const void*)(unsigned long)&_replacee };

wrap_connect(int socket, const struct sockaddr *address,
       socklen_t address_len) {
  if (address->sa_family == AF_INET) {
    struct sockaddr_in *sin = (struct sockaddr_in *) address;
    char *ip = inet_ntoa(sin->sin_addr);
    printf("connect to: %s\n", ip);
  }
  return connect(socket, address, address_len);
}

DYLD_INTERPOSE(wrap_connect, connect);
