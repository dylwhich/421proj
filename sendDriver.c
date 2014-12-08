#include <unistd.h>
#include <syscall.h>
#include <stdio.h>

int main() {
  char in[512];
  char buf = 0;
  int read = 0, sent = 0;

  printf("Message: ");
  while (read < 511) {
    buf = getchar();
    if (buf == EOF) break;
    in[read++] = buf;
  }

  /* null-terminate the input string */
  in[read] = '\0';

  /* discard the rest of stdin */
  while (buf != EOF) {
    buf = getchar();
  }

  /* ensure the mailbox exists */
  syscall(__NR_mkMbox421);
  sent = syscall(__NR_sendMsg421, in, read);

  fprintf(stderr, "%d\n", sent);
  printf("%*s", sent, in);

  return 0;
}
