#include "../h/Riscv.hpp"
void Riscv::handleSupervisorTrap() {
    uint64 scause = r_scause();
    uint64 sstatus = r_sstatus();
    uint64 sepc = r_sepc();
    switch (scause) {
        case 0x8000000000000001UL:
            //kprintString("timer interrupt\n");
            mc_sip(Riscv::SIP_SSIP);
            break;
        case 0x8000000000000009UL:
            //kprintString("enivornment interrupt from console\n");
            console_handler();
            break;
        case 0x0000000000000002:
            printString("\nillegal instruction exception\n");
            printInt(sepc);
            break;
        case 0x0000000000000005:
            printInt(sepc);
            printString("illegal address read exception\n");
            break;
        case 0x0000000000000007:
            printString("illegal address write exception\n");
            printInt(sepc);
            break;
        case 0x0000000000000008:
        case 0x0000000000000009:{
            uint64 opcode;
            __asm__ volatile ("mv %[opcode], a0" : [opcode] "=r"(opcode));
            uint64 param1, param2, param3, param4;
            __asm__ volatile ("mv %0, a1" : "=r" (param1));
            __asm__ volatile ("mv %0, a2" : "=r" (param2));
            __asm__ volatile ("mv %0, a3" : "=r" (param3));
            __asm__ volatile ("mv %0, a4" : "=r" (param4));
            //kprintString("ecall from user mode\n");
            sepc += 4;
            //switch jump to some system call implementation based on operation code stored in a0 register in C API
            switch (opcode) {
                case 0x01: {
                    //mem_alloc
                    void *ret;
                    size_t size = (size_t) param1;
                    ret = MemoryAllocator::getInstance().mem_alloc(size);
                    __asm__ volatile ("mv a0, %0" : : "r" (ret));
                    break;
                }
                case 0x02: {
                    //mem_free
                    void *ptr = (void *) param1;
                    int ret = MemoryAllocator::getInstance().mem_free(ptr);
                    __asm__ volatile ("mv a0, %0" : : "r" (ret));
                    break;
                }
                case 0x11: {
                    //thread_create
                    int ret;
                    TCB** handle = (TCB**)param1;
                    TCB::Body body = (TCB::Body)param2;
                    void* arg = (void*) param3;
                    void* stack = MemoryAllocator::getInstance().mem_alloc(4*DEFAULT_STACK_SIZE / MEM_BLOCK_SIZE);

                    ret = TCB::createThread(handle, body, arg, stack);
                    __asm__ volatile ("mv a0, %0" : : "r" (ret));
                    break;
                }
                case 0x12: {
                    //thread_exit
                    int ret;
                    if (TCB::running == nullptr) {
                         ret = -1;//there is no running thread;
                    }
                    else {
                        TCB::running->setFinished(true);
                        TCB* old = TCB::running;
                        TCB::dispatch();
                        delete old;
                        w_sstatus(sstatus);
                        w_sepc(sepc);
                        ret = 0;
                    }
                    __asm__ volatile ("mv a0, %0" : : "r" (ret));
                    break;
                }
                case 0x13: {
                    //thread_dispatch
                    TCB::dispatch();
                    break;
                }

                case 0x21: {
                    //sem_open
                    int ret;
                    uint64 value = param2;
                    kSemaphore** handle = (kSemaphore**)param1;
                    *handle = new kSemaphore(value);
                    if (!(*handle)) { ret = -1; }
                    else { ret = 0; }

                    __asm__ volatile ("mv a0, %0" : : "r" (ret));
                    break;
                }
                case 0x22: {
                    //sem_close
                    int ret;
                    kSemaphore* sem = (kSemaphore*)param1;
                    if (sem == nullptr) {
                        ret = -1;
                    }
                    else {
                        ret = 0;
                    }
                    delete sem;

                    __asm__ volatile ("mv a0, %0" : : "r" (ret));
                    break;
                }
                case 0x23: {
                    //sem_wait
                    int ret;
                    kSemaphore* sem = (kSemaphore*)param1;
                    if (sem == nullptr) {
                        ret = -1;
                    }
                    else {
                        ret = sem->wait();
                    }
                    __asm__ volatile ("mv a0, %0" : : "r" (ret));
                    break;
                }
                case 0x24: {
                    //sem_signal
                    int ret;
                    kSemaphore* sem = (kSemaphore*)param1;
                    if (sem == nullptr) {
                        ret = -1;
                    }
                    else {
                        ret = sem->signal();
                    }
                    __asm__ volatile ("mv a0, %0" : : "r" (ret));
                    break;
                }
                case 0x25: {
                    //sem_timedwait
                    break;
                }
                case 0x26: {
                    //sem_trywait
                    int ret;
                    kSemaphore* sem = (kSemaphore*)param1;
                    if (sem == nullptr) {
                        ret = -1;
                    }
                    else {
                        ret = sem->trywait();
                    }
                    __asm__ volatile ("mv a0, %0" : : "r" (ret));
                    break;
                }

                case 0x31: {
                    //time_sleep
                    break;
                }

                case 0x41: {
                    //getc
                    char ret;
                    ret = __getc();
                    __asm__ volatile ("mv a0, %0" : : "r" (ret));
                    break;
                }
                case 0x42: {
                    //putc
                    char param = (char)param1;
                    __putc(param);
                    break;
                }

            }
            //rewriting the value of register a0 on stack, because we need it for return value;
            //__asm__ volatile ("sd a0, %0(fp)"  : : "i"(8*10) : "memory");
            break;
        }
//        case 0x0000000000000009: {
//            sepc += 4;
//            TCB::dispatch();
//            break;
//        }
    }
    w_sepc(sepc);
    w_sstatus(sstatus);
}

void Riscv::popSppSpie() {
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}