#include "user.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(2, "Usage: tournament_test N\n");
    exit(1);
  }

  int n = atoi(argv[1]);
  if (n < 2 || n > 16) {
    fprintf(2, "N must be between 2 and 16\n");
    exit(1);
  }

  int pipefd[2];
  if (pipe(pipefd) < 0) {
    fprintf(2, "Failed to create pipe\n");
    exit(1);
  }

  int id = tournament_create(n);
  if (id < 0) {
    fprintf(2, "Failed to create tournament\n");
    exit(1);
  }

  if (tournament_acquire() < 0) {
    fprintf(2, "Acquire failed\n");
    exit(1);
  }

  int my_pid = getpid();

  if (tournament_release() < 0) {
    fprintf(2, "Release failed\n");
    exit(1);
  }

  if (id > 0) {
    char dummy;
    if (read(pipefd[0], &dummy, 1) != 1) {
      fprintf(2, "Sync failed\n");
      exit(1);
    }
  }

  fprintf(1, "Process %d (PID %d) acquired the lock\n", id, my_pid);

  if (id < n-1) {
    if (write(pipefd[1], "x", 1) != 1) {
      fprintf(2, "Sync failed\n");
      exit(1);
    }
  }

  if (id == 0) {
    int status;
    for (int i = 1; i < n; i++) {
      wait(&status);
      if (status != 0) {
        fprintf(2, "Child process exited with error\n");
        exit(1);
      }
    }
  }

  exit(0);
}
