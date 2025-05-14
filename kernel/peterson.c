#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "peterson.h"

struct peterson_lock peterson_locks[MAX_PETERSON_LOCKS];

// Initialize
void
init_peterson_locks(void)
{
    for (int i = 0; i < MAX_PETERSON_LOCKS; i++) {
        peterson_locks[i].barrier[0] = 0;
        peterson_locks[i].barrier[1] = 0;
        peterson_locks[i].turn = 0;
        peterson_locks[i].active = 0;
    }
}

int
peterson_create(void) {
    for (int i = 0; i < MAX_PETERSON_LOCKS; i++) {
        if (__sync_lock_test_and_set(&peterson_locks[i].active, 1) == 0) {
            __sync_synchronize();  //for setting the barrier
            peterson_locks[i].barrier[0] = 0;
            peterson_locks[i].barrier[1] = 0;
            peterson_locks[i].turn = 0;
            return i;
        }
    }
    return -1; // No free lock
}

int
peterson_acquire(int lock_id, int role) {
    if (lock_id < 0 || lock_id >= MAX_PETERSON_LOCKS)
        return -1;
    if (role != 0 && role != 1)
        return -1;
    if (!peterson_locks[lock_id].active)
        return -1;

    int other = 1 - role;
    peterson_locks[lock_id].barrier[role] = 1;
    __sync_synchronize();
    peterson_locks[lock_id].turn = other;
    __sync_synchronize();

    // other process is intrested and it is not our turn - the opposite from the original algorithem
    while (peterson_locks[lock_id].barrier[other] &&
           peterson_locks[lock_id].turn == other) {
        yield();  // Don't busy-wait
           }

    return 0;
}

int
peterson_release(int lock_id, int role) {
    if (lock_id < 0 || lock_id >= MAX_PETERSON_LOCKS)
        return -1;
    if (role != 0 && role != 1)
        return -1;
    if (!peterson_locks[lock_id].active)
        return -1;

    peterson_locks[lock_id].barrier[role] = 0;
    __sync_synchronize();
    return 0;
}

int
peterson_destroy(int lock_id) {
    if (lock_id < 0 || lock_id >= MAX_PETERSON_LOCKS)
        return -1;
    if (!peterson_locks[lock_id].active)
        return -1;

    __sync_lock_release(&peterson_locks[lock_id].active);
    return 0;
}