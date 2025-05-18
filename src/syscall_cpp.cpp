#include "../h/syscall_cpp.hpp"

void* operator new (size_t size) {
    return mem_alloc (size);
}

void operator delete (void* ptr) noexcept {
    mem_free (ptr);
}

void* operator new [] (size_t size) {
    return mem_alloc (size);
}

void operator delete [](void* ptr) noexcept {
    mem_free (ptr);
}

Thread::Thread(void (*body)(void *), void *arg) {
    this->body = body;
    this->arg = arg;
}

Thread::~Thread() {
    myHandle->setFinished(true);
    delete myHandle;
    //thread_exit();
}

int Thread::start() {
    if (this->body != nullptr) {
        this->body = Thread::runWrapper;
    }
    return thread_create(&this->myHandle, this->body, this);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t duration) {
    return time_sleep(duration);
}

Thread::Thread() {
    this->myHandle = nullptr;
}

Semaphore::Semaphore(unsigned int init) {
    sem_open (&this->myHandle, init);
}


Semaphore::~Semaphore() {
    sem_close(this->myHandle);
}

int Semaphore::wait() {
    return sem_wait(this->myHandle);
}

int Semaphore::signal() {
    return sem_signal (this->myHandle);
}

int Semaphore::timedWait(time_t timeout) {
    return sem_timedwait(this->myHandle, timeout);
}

int Semaphore::tryWait() {
    return sem_trywait(this->myHandle);
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char character) {
    ::putc(character);
}
