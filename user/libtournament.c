#include "user.h"
#include <stdbool.h>

#define MAX_PROCESSES 16
#define MAX_LOCKS 31 // number of nodes in perfect binary tree for 16 leaves

int process_idx = -1;
int num_levels = 0;
int total_locks = 0;
int locks[MAX_LOCKS];

// ✅ These are now file-scoped, per-process after fork
static int process_lock_ids[MAX_LOCKS];
static int process_roles[MAX_LOCKS];
static int global_locks_snapshot[MAX_LOCKS];  // copy of locks from parent

bool is_power_of_2(int x){
  return x > 0 && (x & (x - 1 )) == 0;
}

void assign_path(int idx, int levels){
  for (int l = 0; l < levels; l++) {
    int role = (idx >> (levels - l - 1)) & 1;
    int lock_num = (idx >> (levels - l)) + ((1 << l) - 1);
    process_lock_ids[l] = global_locks_snapshot[lock_num];
    process_roles[l] = role;
  }
}

int tournament_create(int processes){
  if (processes < 2 || processes > MAX_PROCESSES || !is_power_of_2(processes))
    return -1;

  num_levels = 0;
  int tmp = processes;
  while (tmp >>= 1) num_levels++;

  total_locks = processes - 1;
  for (int i = 0; i < total_locks; i++) {
    locks[i] = peterson_create();
    if (locks[i] < 0) return -1;
  }

  for (int i = 0; i < total_locks; i++)
    global_locks_snapshot[i] = locks[i];

  // Fork N - 1 children, each gets a unique index
  for (int i = 1; i < processes; i++) {
    int pid = fork();
    if (pid < 0) return -1;
    if (pid == 0) {
      process_idx = i;
      assign_path(process_idx, num_levels);
      return process_idx;
    }
  }

  // Parent process gets index 0
  process_idx = 0;
  assign_path(process_idx, num_levels);
  return process_idx;
}

int tournament_acquire(void) {
  for (int i = 0; i < num_levels; i++) {
    if (peterson_acquire(process_lock_ids[i], process_roles[i]) < 0)
      return -1;
    __sync_synchronize();
  }
  __sync_synchronize();
  return 0;
}

int tournament_release(void) {
  __sync_synchronize();
  for (int i = num_levels - 1; i >= 0; i--) {
    if (peterson_release(process_lock_ids[i], process_roles[i]) < 0)
      return -1;
    __sync_synchronize();
  }
  return 0;
}
