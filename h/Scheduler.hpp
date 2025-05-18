#pragma once
#include "MemoryAllocator.hpp"
class TCB;

class Scheduler {
public:
    static TCB *get ();
    static void put (TCB* tcb);
private:
    struct Elem {
        TCB *data;
        Elem *next;

        //Elem(TCB *data, Elem *next) : data(data), next(next) {}
    };
    static Elem* head;
    static Elem* tail;

    static void add(TCB *data);
    static TCB *remove();

    Scheduler() {};

    Scheduler& operator=(const Scheduler&) = delete;

    Scheduler(const Scheduler&) = delete;
};