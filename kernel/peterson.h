
#ifndef PETERSONLOCK_H
#define PETERSONLOCK_H

#define MAX_PETERSON_LOCKS 15

struct peterson_lock {
  int barrier[2];
  int turn;
  int active;          // is this lock active
};

int peterson_create();
int peterson_acquire(int lock_id, int role);
int peterson_release(int lock_id, int role);
int peterson_destroy(int lock_id);

void init_peterson_locks(void);

#endif //PETERSONLOCK_H
