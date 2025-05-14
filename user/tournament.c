#include "user.h"
#include <stdbool.h>

#define MAX_PROCESSES 16
#define MAX_LOCKS 31 // the number of nodes in preferct tree is 2^4-1=31

int process_idx = -1;
int num_levels = 0;
int total_locks = 0;
int locks[MAX_LOCKS];

struct level{
  int lock_pids[MAX_LOCKS];
  int roles[MAX_LOCKS];
  } process_info;

bool is_power_of_2(int x){
  return x > 0 && (x & (x - 1 )) == 0;
}

void process_path(int idx, int levels){
  for(int k = 0; k < levels; k++){
    int role = (idx >> (levels - k - 1)) & 1;
    int level_lock = idx >> (levels - k);
    int locks_idx = level_lock + ((1 << k) - 1);

    process_info.lock_pids[k] = locks[locks_idx];
    process_info.roles[k] = role;
  }
}

int tournament_create(int processes){
  if(processes < 2 || processes > MAX_PROCESSES || !is_power_of_2(processes))
    return -1;

  num_levels = 0;
  int temp = processes;
  while(temp > 1){
    temp >>=1; // bitwise right shift
    num_levels++;
  }

  total_locks = processes - 1;

  for(int i=0; i < total_locks; i++){
    locks[i] = peterson_create();
    if(locks[i] < 0)
      return -1;
  }

  for(int i=0; i < processes - 1; i++){
    int pid = fork();
    if(pid == 0){
      process_idx = i + 1;
      process_path(process_idx, num_levels);
      return process_idx;
    }
  }

  //parent process
  process_idx = 0;
  process_path(process_idx, num_levels);
  return process_idx;
}

int tournament_acquire(){
  for(int k = 0; k < num_levels; k++){
    if(peterson_acquire(process_info.lock_pids[k], process_info.roles[k]) < 0)
      return -1;
  }
  return 0;
}

int tournament_release(){
  for(int k = num_levels - 1; k >= 0; k--){
    if(peterson_release(process_info.lock_pids[k], process_info.roles[k]) < 0)
      return -1;
  }
  return 0;
}