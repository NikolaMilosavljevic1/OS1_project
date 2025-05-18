#include "../h/TCB.hpp"

TCB* TCB::running = nullptr;

void TCB::yield() {
    __asm__ volatile ("li a0, 0x13");
    __asm__ volatile ("ecall");
}

void TCB::dispatch() {
    TCB* oldThread = TCB::running;
    if (!(oldThread->isFinished())) {
        Scheduler::put(oldThread);
    }
    TCB::running = Scheduler::get();
//    kprintString("\nTrenutna running nit je: ");
//    kprintInteger((uint64)TCB::running);
//    kprintString("\n");
    TCB::contextSwitch(&oldThread->context, &TCB::running->context);
}

void TCB::ThreadWrapper() {
    //kprintString("threadWrapper");
    //main nit nece da ulazi u ThreadWrapper, ali neka druga sistemska nit ce uci ovde, pa zbog toga proveravamo mode
    if (running->mode == 0) { //thread is executed in system environment
        Riscv::ms_sstatus(Riscv::SSTATUS_SPP);
    }
    else { //thread is executed in user environment
        Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    }
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    TCB::yield();
}