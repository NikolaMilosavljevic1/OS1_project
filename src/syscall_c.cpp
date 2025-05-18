#include "../h/syscall_c.hpp"
void* mem_alloc (size_t size) {
    void* ret;
    uint64 sizeInBlocks = size / MEM_BLOCK_SIZE + (size % MEM_BLOCK_SIZE ? 1 : 0)+1;
    __asm__ volatile ("mv a1, %0" : : "r" (sizeInBlocks));
    __asm__ volatile ("li a0, 0x01");
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

int mem_free (void* ptr) {
    int ret;

    __asm__ volatile ("mv a1, %0" : : "r" (ptr));
    __asm__ volatile ("li a0, 0x02");
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

int thread_create (thread_t* handle, void (*start_routine) (void*), void* arg) {
    int ret;
    __asm__ volatile ("mv a3, %0" : : "r" (arg));
    __asm__ volatile ("mv a2, %0" : : "r" (start_routine));
    __asm__ volatile ("mv a1, %0" : : "r" (handle));
    __asm__ volatile ("li a0, 0x11");
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %0, a0" : "=r" (ret));
//    printString("\nNapravljena nit ima handle: ");
//    printInt((uint64)*handle);
    return ret;
}

int thread_exit() {
    int ret;

    __asm__ volatile ("li a0, 0x12");
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

void thread_dispatch() {
    __asm__ volatile ("li a0, 0x13");
    __asm__ volatile ("ecall");
}


int sem_open (sem_t* handle, unsigned init) {
    int ret;

    __asm__ volatile ("mv a2, %0" : : "r" (init));
    __asm__ volatile ("mv a1, %0" : : "r" (handle));
    __asm__ volatile ("li a0, 0x21");
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

int sem_close (sem_t handle) {
    int ret;
    __asm__ volatile ("mv a1, %0" : : "r" (handle));
    __asm__ volatile ("li a0, 0x22");
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

int sem_wait (sem_t id) {
    int ret;
    __asm__ volatile ("mv a1, %0" : : "r" (id));
    __asm__ volatile ("li a0, 0x23");
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

int sem_signal (sem_t id) {
    int ret;
    __asm__ volatile ("mv a1, %0" : : "r" (id));
    __asm__ volatile ("li a0, 0x24");
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

int sem_timedwait (sem_t id, time_t timeout) {
    //int ret;
    __asm__ volatile ("mv a1, %0" : : "r" (id));
    __asm__ volatile ("mv a2, %0" : : "r" (timeout));
    __asm__ volatile ("li a0, 0x25");
    __asm__ volatile ("ecall");

    //__asm__ volatile ("mv %0, a0" : "=r" (ret));
    return 0;
}

int sem_trywait (sem_t id) {
    int ret;
    __asm__ volatile ("mv a1, %0" : : "r" (id));
    __asm__ volatile ("li a0, 0x26");
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}


int time_sleep (time_t duration) {
    //int ret;
    __asm__ volatile ("mv a1, %0" : : "r" (duration));
    __asm__ volatile ("li a0, 0x31");
    __asm__ volatile ("ecall");

    //__asm__ volatile ("mv %0, a0" : "=r" (ret));
    return 0;
}

char getc() {
    char ret;
    __asm__ volatile ("li a0, 0x41");
    __asm__ volatile("ecall");

    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

void putc (char character) {
    __asm__ volatile ("mv a1, %0" : : "r" (character));
    __asm__ volatile ("li a0, 0x42");
    __asm__ volatile("ecall");
}
