#include <unistd.h>
#include <syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char **argv){
  char msg[512];
  unsigned long len;
  unsigned char flag;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s <max length> <delete>\n", argv[0]);
    return 1;
  } else {
    errno = 0;
    len = strtoul(argv[1], NULL, 10);
    if (errno != 0) {
      fprintf(stderr, "Invalid value '%s' for argument 'max length'\n", argv[1]);
      return 1;
    }

    errno = 0;
    flag = (unsigned char) atoi(argv[2]);
    if (errno != 0) {
      fprintf(stderr, "Invalid value '%s' for argument 'delete'\n", argv[2]);
      return 1;
    }
  }

  syscall(__NR_mkMbox421);
  syscall(__NR_receiveMsg421, msg, len, flag);

  printf("%.*s\n", (int)len, msg);
  return 0;
}
