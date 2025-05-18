#include "../h/Scheduler.hpp"
#include "../lib/console.h"

Scheduler::Elem* Scheduler::head = nullptr;
Scheduler::Elem* Scheduler::tail = nullptr;

TCB* Scheduler:: get() {
    return remove();
}

void Scheduler:: put(TCB* tcb) {
    add(tcb);
}

void Scheduler::add(TCB *data) {
//    printString("sizeof(Elem): ");
//    printInt(sizeof(Elem));
//    printString("\n");
    Elem *elem = (Elem*)MemoryAllocator::getInstance().mem_alloc(1);
    elem->data = data;
    elem->next = nullptr;
    if (tail) {
        tail->next = elem;
        tail = elem;
    } else {
        head = tail = elem;
    }
}

TCB* Scheduler::remove() {
    if (!head) { return 0; }

    Elem *elem = head;
    head = head->next;
    if (!head) { tail = 0; }

    TCB *ret = elem->data;
    MemoryAllocator::getInstance().mem_free(elem);
    return ret;
}