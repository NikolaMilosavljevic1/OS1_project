#pragma once
#include "TCB.hpp"
#include "Scheduler.hpp"
#include "../testh/printing.hpp"
#include "MemoryAllocator.hpp"
class kSemaphore {
public:
    void* operator new (uint64 n) {
        //printString("usao u sem");
        uint64 size = n / MEM_BLOCK_SIZE + (n % MEM_BLOCK_SIZE ? 1 : 0);
        return MemoryAllocator::getInstance().mem_alloc((size_t) size);
    }
    void* operator new[] (uint64 n) {
        uint64 size = n / MEM_BLOCK_SIZE + (n % MEM_BLOCK_SIZE ? 1 : 0);
        return MemoryAllocator::getInstance().mem_alloc((size_t) size);
    }

    void operator delete (void* ptr) {
        MemoryAllocator::getInstance().mem_free(ptr);
    }
    void operator delete[] (void* ptr) {
        MemoryAllocator::getInstance().mem_free(ptr);
    }

    int wait();
    int signal();
    int trywait();

    int value () const { return val; }
    kSemaphore (unsigned init = 1) : val(init) {printString("usao u sem\n");}
    ~kSemaphore();

private:
    struct Elem {
        TCB *data;
        Elem *next;

        Elem(TCB *data, Elem *next) : data(data), next(next) {}
    };

    Elem* head = nullptr;
    Elem* tail = nullptr;

    unsigned int val;
    void block();
    void unblock();
    void add(TCB* thread);
    TCB* remove();
};