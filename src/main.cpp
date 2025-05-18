#include "../h/MemoryAllocator.hpp"
#include "../h/Riscv.hpp"
#include "../h/TCB.hpp"
#include "../testh/printing.hpp"
#include "../lib/console.h"

void userMain();

void userWrapper(void* arg) {
    userMain();
}

int main() {
    MemoryAllocator& instance = MemoryAllocator::getInstance();
    Riscv::w_stvec((uint64)&Riscv::supervisorTrap);
    TCB* mainThread = nullptr, *userThread = nullptr;
    TCB::createThread(&mainThread, nullptr, nullptr, nullptr);
    TCB::running = mainThread;
    void* userStack = instance.mem_alloc( 4*DEFAULT_STACK_SIZE / MEM_BLOCK_SIZE);
    TCB::createThread(&userThread, &userWrapper, nullptr, userStack);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
    while (!(userThread->isFinished())) {
        TCB::yield();
    }

    delete userThread;
    delete mainThread;

//    //zaustavljanje programa;
//    uint64 stop = 0x100000;
//    __asm__ volatile ("li t0, 0x5555");
//    __asm__ volatile ("sw t0, 0(%0)" : : "r"(stop) : "memory");
    return 0;
}