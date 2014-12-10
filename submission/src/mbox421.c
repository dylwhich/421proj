#include "mbox421.h"
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/semaphore.h>

static struct MBox421 mailbox;

struct Message421 *mailbox_next_message(struct MBox421 *this) {
  if (this->count < MBOX421_MSG_COUNT) {
    return (this->messages + ((this->head + this->count++) % MBOX421_MSG_COUNT));
  } else {
    return NULL;
  }
}

struct Message421 *mailbox_first_message(struct MBox421 *this, unsigned char remove) {
  struct Message421 *result = NULL;
  if (this->count > 0) {
    result = &(this->messages[this->head]);
    if (remove) {
      this->count--;
      this->head = (this->head + 1) % MBOX421_MSG_COUNT;
    }
  }
  return result;
}

long mailbox_lock(struct MBox421 *this) {
  printk("mailbox_lock: Locking mailbox...\n");
  if (down_interruptible(&this->lock)) {
    printk("mailbox_lock: FAILED TO LOCK MAILBOX\n");
    return 0;
  } else {
    printk("mailbox_lock: > MAILBOX LOCKED\n");
    return 1;
  }
}

void mailbox_unlock(struct MBox421 *this) {
  printk("mailbox_lock: Unlocking mailbox...\n");
  up(&this->lock);
  printk("mailbox_unlock: < MAILBOX UNLOCKED\n");
}

asmlinkage long sys_mkMbox421() {
  printk("sys_mkMbox421()\n");

  mailbox_lock(&mailbox);

  if (!mailbox.initialized) {
    printk("sys_mkMbox421: initializing\n");
    mailbox.head = 0;
    mailbox.count = 0;
    mailbox.initialized = 1;
  } else {
    printk("sys_mkMbox421: Already initilaized\n");
  }

  mailbox_unlock(&mailbox);
  return 0;
}

asmlinkage long sys_rmMbox421() {
  long retval = 0;

  printk("sys_rmMbox421()\n");

  mailbox_lock(&mailbox);
  /* Throw an error if the mailbox isn't initialized */
  if (mailbox.initialized) {
    printk("sys_rmMbox421: mailbox is initialized\n");

    /* Throw an error if the mailbox isn't empty */
    if (mailbox.count == 0) {
      printk("sys_rmMbox421: mailbox is empty\n");
      printk("sys_rmMbox421: de-initializing\n");
      mailbox.initialized = 0;
    } else {
      printk("sys_rmMbox421: mailbox is NOT empty\n");
      retval = ERR421_NOT_EMPTY;
    }
  } else {
    printk("sys_rmMbox421: mailbox is NOT initialized\n");
    retval = ERR421_NOT_INITIALIZED;
  }

  mailbox_unlock(&mailbox);
  return retval;
}

asmlinkage long sys_sendMsg421(char *msg, unsigned long N) {
  struct Message421 *next;
  long copied = 0;
  mailbox_lock(&mailbox);
  printk("sys_sendMsg421(%s, %lu)\n", msg, N);

  if (access_ok(VERIFY_READ, msg, N)) {
    printk("sys_sendMsg421: access_ok\n");
    
    next = mailbox_next_message(&mailbox);

    if (next != NULL) {
      copied = strncpy_from_user(next->contents, msg,  MBOX421_MSG_LENGTH);
      if (copied == -EFAULT) {
	printk("sys_sendMsg421: strncpy_from_user returned EFAULT\n");
      } else {
	if (N > MBOX421_MSG_LENGTH) {
	  next->contents[MBOX421_MSG_LENGTH-1] = '\0';
	} else {
	  next->contents[N] = '\0';
	}
	printk("sys_sendMsg421: strncpy_from_user returned %ld\n", copied);
      }
    } else {
      copied = ERR421_FULL;
    }
  } else {
    printk("sys_sendMsg421: access_ok FAILED\n");
    copied = 0;
  }
  mailbox_unlock(&mailbox);
  return copied;
}

asmlinkage long sys_receiveMsg421(char *msg, unsigned long N, unsigned char flag) {
  struct Message421 *first_msg;
  long msg_len;
  mailbox_lock(&mailbox);
  printk("sys_receiveMsg421(msg, %lu, %u)\n", N, flag);

  first_msg = mailbox_first_message(&mailbox, flag);
  if (first_msg != NULL) {
    printk("sys_receiveMsg421: first msg is %s\n", first_msg->contents);
    if (access_ok(VERIFY_WRITE, msg, N)) {
      printk("sys_receiveMsg421: access_ok\n");
      msg_len = strlen(first_msg->contents);
      copy_to_user(msg, first_msg->contents, N > msg_len ? msg_len : N);
    } else {
      printk("sys_receiveMsg421: access_ok FAILED\n");
    }
  } else {
    printk("sys_receiveMsg421: first msg is NULL\n");
  }

  mailbox_unlock(&mailbox);
  return 0;
}

asmlinkage long sys_countMsg421() {
  long result = 0;
  printk("sys_countMsg421()\n");
  mailbox_lock(&mailbox);
  result = mailbox.count;
  mailbox_unlock(&mailbox);
  return result;
}

void __init mbox421_init(void) {
  mailbox.head = 0;
  mailbox.count = 0;
  mailbox.initialized = 0;
  init_MUTEX(&mailbox.lock);
}
