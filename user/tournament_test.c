#include "user.h"

int main(int argc, char *argv[]){
  if(argc != 2){
    printf("Usage: tournament N\n");
    exit(1);
  }

  int n = atoi(argv[1]);
  int id = tournament_create(n);
  if(id < 0){
    printf("failed to create tournament\n");
    exit(1);
  }

  if(tournament_acquire() < 0){
    printf("Acquire failed\n");
    exit(1);
  }

  printf(">>> Process %d with PID %d acquired the lock\n", id, getpid());
  sleep(10);

  if(tournament_release() < 0){
    printf("Release failed\n");
    exit(1);
  }

  exit(0);
}

