#include "user.h"

// Helper: convert integer to string in buffer
int printint_to_buf(char *dst, int num) {
  char tmp[16];
  int i = 0, len = 0;
  int neg = 0;

  if (num < 0) {
    neg = 1;
    num = -num;
  }

  do {
    tmp[i++] = num % 10 + '0';
    num /= 10;
  } while (num > 0);

  if (neg) tmp[i++] = '-';

  for (int j = i - 1; j >= 0; j--) {
    *dst++ = tmp[j];
    len++;
  }

  return len;
}

// Helper: copy string to buffer
int sprint(char *dst, const char *src) {
  int len = 0;
  while (*src) {
    *dst++ = *src++;
    len++;
  }
  return len;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: tournament_test N\n");
    exit(1);
  }

  int n = atoi(argv[1]);
  int id = tournament_create(n);
  if (id < 0) {
    printf("Failed to create tournament\n");
    exit(1);
  }

  if (tournament_acquire() < 0) {
    printf("Acquire failed\n");
    exit(1);
  }

  // Critical section: write output atomically while holding lock
  char buf[100];
  int len = 0;
  len += sprint(buf + len, ">>> Process ");
  len += printint_to_buf(buf + len, id);
  len += sprint(buf + len, " with PID ");
  len += printint_to_buf(buf + len, getpid());
  len += sprint(buf + len, " acquired the lock\n");

  for (int i = 0; i < len; i++) {
    if (write(1, &buf[i], 1) != 1) {
      // optional error handling
    }
  }

  // Add a tiny delay to reduce contention on the console
  for (volatile int i = 0; i < 1000000; i++);

  if (tournament_release() < 0) {
    printf("Release failed\n");
    exit(1);
  }

  exit(0);
}
