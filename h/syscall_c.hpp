#include "../lib/hw.h"
#include "../testh/printing.hpp"
#define EOF -1

class TCB;
class kSemaphore;
typedef TCB* thread_t;
typedef kSemaphore* sem_t;

void* mem_alloc (size_t size);

int mem_free (void*);
int thread_create (
        thread_t* handle,
        void (*start_routine) (void*),
        void* arg
);

int thread_exit();

void thread_dispatch();

int sem_open (
        sem_t* handle,
        unsigned init
);

int sem_close (sem_t handle);

int sem_wait (sem_t id);

int sem_signal (sem_t id);

int sem_timedwait (
        sem_t id,
        time_t timeout
);

int sem_trywait (sem_t id);

typedef unsigned long time_t;

int time_sleep (time_t);


char getc();

void putc (char);

