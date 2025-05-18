#include "../h/kSemaphore.hpp"

int kSemaphore::wait() {
    if (val < 0 || TCB::running == nullptr) {
        return -1;
    }
    if (val > 0) {
        val--;
        return 0;
    }
    block();
    return 0;
}

int kSemaphore::signal() {
    if (val < 0 || TCB::running == nullptr) {
        return -1;
    }
    val++;
    unblock();
    return 0;
}

int kSemaphore::trywait() {
    if (val < 0 || TCB::running == nullptr) {
        return -1;
    }
    if (val > 0) {
        val--;
        return 0;
    }
    else return 1;
}

void kSemaphore::block() {
    TCB* oldRunning = TCB::running;
    add(oldRunning);
    TCB::running = Scheduler::get();
    TCB::contextSwitch(&oldRunning->context, &TCB::running->context);
}

void kSemaphore::add (TCB* thread){
    Elem *elem = (Elem*)MemoryAllocator::getInstance().mem_alloc(1);
    elem->data = thread;
    elem->next = nullptr;
    if (tail) {
        tail->next = elem;
        tail = elem;
    } else {
        head = tail = elem;
    }
}

TCB* kSemaphore::remove() {
    if (!head) { return 0; }

    Elem *elem = head;
    head = head->next;
    if (!head) { tail = 0; }

    TCB *ret = elem->data;
    MemoryAllocator::getInstance().mem_free((void*)elem);
    return ret;
}

void kSemaphore::unblock() {
    TCB* thread = remove();
    if (thread) {
        Scheduler::put(thread);
    }
}

kSemaphore::~kSemaphore() {
    // putting all threads that were waiting on this semaphore in ready queue
    while (1) {
        TCB* first = (head != nullptr) ? head->data : nullptr;
        if (first == nullptr) {
            break;
        }
        Scheduler::put(first);
        remove();
    }
}