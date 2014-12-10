#ifndef MBOX421_H
#define MBOX421_H

#include <linux/semaphore.h>

#define ERR421_NOT_INITIALIZED -1
#define ERR421_NOT_EMPTY -2
#define ERR421_FULL -3

#define MBOX421_MSG_LENGTH 512
#define MBOX421_MSG_COUNT 128

struct Message421 {
  char contents[MBOX421_MSG_LENGTH];
};

struct MBox421 {
  unsigned char initialized;
  unsigned char head;
  unsigned char count;
  struct semaphore lock;
  struct Message421 messages[MBOX421_MSG_COUNT];
};

struct Message421 *mailbox_next_message(struct MBox421 *this);
struct Message421 *mailbox_first_message(struct MBox421 *this, unsigned char rm);
long mailbox_lock(struct MBox421 *this);
void mailbox_unlock(struct MBox421 *this);

void __init mbox421_init(void);

#endif
