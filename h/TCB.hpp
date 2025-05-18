#pragma once
#include "MemoryAllocator.hpp"
#include "Scheduler.hpp"
#include "Riscv.hpp"
#include "kSemaphore.hpp"
#include "../lib/console.h"

class TCB {
private:

public:
    using Body = void (*) (void*);
    void* operator new (size_t n) {
        //kprintString("\nnew from TCB\n");
        //kprintInteger(n);
        //kprintString("\n");
        uint64 size = n / MEM_BLOCK_SIZE + (n % MEM_BLOCK_SIZE ? 1 : 0);
        return MemoryAllocator::getInstance().mem_alloc((size_t)size);
    }
    void* operator new[] (size_t n) {
        uint64 size = n / MEM_BLOCK_SIZE + (n % MEM_BLOCK_SIZE ? 1 : 0);
        return MemoryAllocator::getInstance().mem_alloc((size_t) size);
    }

    void operator delete (void* ptr) {
        MemoryAllocator::getInstance().mem_free(ptr);
    }
    void operator delete[] (void* ptr) {
        MemoryAllocator::getInstance().mem_free(ptr);
    }

    ~TCB() {
        MemoryAllocator::getInstance().mem_free(stack);
    }
    static int createThread(TCB** handle, Body body, void* arg, void* stack_space) {
        TCB* thread = (TCB*)MemoryAllocator::getInstance().mem_alloc((sizeof(TCB)+MEM_BLOCK_SIZE-1)/MEM_BLOCK_SIZE+1);
//        kprintString("Before setting arguments: ");
//        kprintInteger((uint64)handle);
//        kprintString("\n");
//        kprintString("After allocation: ");
//        kprintInteger((uint64)thread);
//        kprintString("\n");
        if (!thread) return -1;
        if (!body) {
            thread->body = nullptr;
            thread->arg = nullptr;
            thread->stack = nullptr;
            thread->context.ra = (uint64)&ThreadWrapper;
            thread->context.sp = 0;
            thread->finished = false;
            thread->mode = 0;
        }
        else {
            thread->body = body;
            thread->arg = arg;
            thread->stack = (uint64*)stack_space;
            thread->context.ra = (uint64)&ThreadWrapper;
            thread->context.sp = (uint64)&((thread->stack)[DEFAULT_STACK_SIZE]);
            thread->finished = false;
            thread->mode = 1;
            Scheduler::put(thread);

        }
//        kprintString("tcreate: ");
//        kprintInteger((uint64)thread);
//        kprintString("\n");
          *handle = thread;
//        kprintInteger((uint64)*handle);
//        kprintString("\n");
        return 0;
    }

    static TCB* running;

    bool isFinished() const {
        return finished;
    }

    void setFinished(bool finished) {
        this->finished = finished;
    }

    static void yield();

    static void dispatch();
private:
//    TCB (Body body, void* arg, void* stack_space) :
//    body(body),
//    arg(arg),
//    stack(body != nullptr ? (uint64*)stack_space : nullptr),
//    context ({
//        (uint64)&ThreadWrapper,
//        body != nullptr ? (uint64)&stack[DEFAULT_STACK_SIZE] : 0
//    })
//    {
//        //kprintInteger((uint64)this);
//        this->mode = body != nullptr ? 1 : 0;
//        this->finished = false;
//        //kprintString("usao u konstruktor\nFinished je: ");
//        //kprintInteger((uint64)this->finished);
//        //kprintString("\n");
//        if (body != nullptr) { Scheduler::put(this); }
//    }
    TCB() {};

    Body body;
    void* arg;
    uint64* stack;
    struct Context {
        uint64 ra;
        uint64 sp;
    };
    Context context;
    bool finished = false;
    bool mode = 0; //0 system, 1 user
    friend class kSemaphore;
    static void contextSwitch(Context* oldContext, Context* newContext);

    static void ThreadWrapper();

};