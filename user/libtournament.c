#include "user.h"

#define NULL ((void*)0)
#define MAX_PROCESSES 16
#define MAX_LEVELS 4
#define MAX_LOCKS 31

static int lock_ids[MAX_LOCKS];
static int my_roles[MAX_LEVELS];
static int my_lock_ids[MAX_LEVELS];
static int process_idx = -1;
static int num_levels = 0;

int tournament_create(int processes) {
    if (processes < 2 || processes > MAX_PROCESSES || (processes & (processes - 1)) != 0)
        return -1;

    num_levels = 0;
    int tmp = processes;
    while (tmp >>= 1) num_levels++;

    // Initialize lock arrays
    for (int i = 0; i < MAX_LOCKS; i++) lock_ids[i] = -1;
    for (int i = 0; i < MAX_LEVELS; i++) {
        my_roles[i] = -1;
        my_lock_ids[i] = -1;
    }

    for (int i = 0; i < processes - 1; i++) {
        int lid = peterson_create();
        if (lid < 0) return -1;
        lock_ids[i] = lid;
    }

    __sync_synchronize();

    for (int i = 1; i < processes; i++) {
        int pid = fork();
        if (pid < 0) return -1;
        if (pid == 0) {
            process_idx = i;
            break;
        }
    }

    if (process_idx == -1) process_idx = 0;

    for (int l = 0; l < num_levels; l++) {
        int role = (process_idx >> (num_levels - l - 1)) & 1;
        int node = (process_idx >> (num_levels - l)) + ((1 << l) - 1);
        my_roles[l] = role;
        my_lock_ids[l] = lock_ids[node];
    }

    __sync_synchronize();
    return process_idx;
}

int tournament_acquire(void) {
    __sync_synchronize();

    for (int i = 0; i < num_levels; i++) {
        if (peterson_acquire(my_lock_ids[i], my_roles[i]) < 0)
            return -1;
        __sync_synchronize();
    }

    __sync_synchronize();
    return 0;
}

int tournament_release(void) {
    __sync_synchronize();

    for (int i = num_levels - 1; i >= 0; i--) {
        if (peterson_release(my_lock_ids[i], my_roles[i]) < 0)
            return -1;
        __sync_synchronize();
    }

    __sync_synchronize();
    return 0;
}
