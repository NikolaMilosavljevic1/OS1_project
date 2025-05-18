#include "../h/MemoryAllocator.hpp"

MemoryAllocator::list* MemoryAllocator::free_list_head = nullptr;
MemoryAllocator::list* MemoryAllocator::allocated_list_head = nullptr;

void* MemoryAllocator::mem_alloc(size_t size) {
    //kprintInteger(size);
    //kprintString("\nmem_alloc\n");
    if (size <= 0) return nullptr;

    //first time allocating memory
    if (!free_list_head) {
        uint64 start_addr = (uint64)HEAP_START_ADDR + ((uint64)HEAP_END_ADDR - (uint64)HEAP_START_ADDR) / 2;
        free_list_head = (list*)start_addr;
        free_list_head->next = nullptr;
        free_list_head->prev = nullptr;
        free_list_head->size = ((size_t)HEAP_END_ADDR - (size_t)HEAP_START_ADDR) / MEM_BLOCK_SIZE;
    }

    list* ftmp;
    for (ftmp = free_list_head; ftmp; ftmp = ftmp->next) {
        if (ftmp->size >= size) break;
    }

    //not enough free blocks in heap
    if (!ftmp) {
        return nullptr;
    }

    //newly allocated memory
    list* allocat = ftmp;

    //updating free_list
    if (ftmp->size == size) { //'deallocates' free_list structure
        if (ftmp->prev) {
            ftmp->prev->next = ftmp->next;
        }
        else {
            free_list_head = ftmp->next;
        }
        if (ftmp->next) {
            ftmp->next->prev = ftmp->prev;
        }
    }
    else { //updates free_list structure
        // Split the free block
        list* fprev, *fnext;
        uint64 blocks;
        if (ftmp == free_list_head) {
            fprev = free_list_head->prev;
            fnext = free_list_head->next;
            blocks = free_list_head->size - size;
        }
        else {
            fprev = ftmp->prev;
            fnext = ftmp->next;
            blocks = ftmp->size - size;
        }
        ftmp = (list*)((char*)ftmp + size * MEM_BLOCK_SIZE);
        ftmp->prev = fprev;
        ftmp->next = fnext;
        ftmp->size = blocks;

        if (ftmp->prev == nullptr) {
            free_list_head = ftmp;
        } else {
            ftmp->prev->next = ftmp;
        }
        if (ftmp->next) {
            ftmp->next->prev = ftmp;
        }
    }

    list* allocating = allocat;
    allocating->next = nullptr;
    allocating->prev = nullptr;
    allocating->size = size;

    //if this is first time calling mem_alloc
    if (!allocated_list_head) {
        allocated_list_head = allocating;
        allocated_list_head->prev = nullptr;
        allocated_list_head->next = nullptr;
        allocated_list_head->size = allocating->size;
    }

    else {
        list* atmp;
        for (atmp = allocated_list_head; atmp->next; atmp = atmp->next) {
            if (!atmp->next) break;
        }//ovde se desava illegal address read exception
        atmp->next = allocating;
        allocating->prev = atmp;
        allocating->next = nullptr;
    }
    return (void*) ((char*)allocating + sizeof(list));
};

int MemoryAllocator::mem_free(void *ptr) {
    if (!ptr) return -1;
    //kprintInteger((uint64)ptr);
    //kprintString("\n");

    list* atmp;
    for (atmp = allocated_list_head; atmp; atmp = atmp->next) {
        if ((char*)atmp + sizeof(list) == (char*)ptr) break;
    }

    //can't free memory starting from ptr address, cause mem_alloc didn't return ptr address when allocating memory
    if (atmp == nullptr) {
        //printString("\natmp == nullptr\n");
        return -2;
    }

    uint64 blocks = atmp->size;

    if (atmp == allocated_list_head) {
        allocated_list_head = atmp->next;
        if (allocated_list_head != nullptr) {
            allocated_list_head->prev = nullptr;
            allocated_list_head->next = atmp->next->next;
        }
    }
    if (atmp->prev) {
        atmp->prev->next = atmp->next;
    }
    if (atmp->next) {
        atmp->next->prev = atmp->prev;
    }

    //updating free_list
    list* ftmp;
    for (ftmp = free_list_head; ftmp->next; ftmp = ftmp->next);
    list* new_free_block = (list*)((char*)ptr - sizeof(list));
    new_free_block->size = blocks;
    new_free_block->prev = ftmp;
    new_free_block->next = nullptr;
    ftmp->next = new_free_block;

    tryToJoin(new_free_block);
    tryToJoin(ftmp);
    return 0;
}

MemoryAllocator& MemoryAllocator::getInstance() {
    static MemoryAllocator instance;
    return instance;
}

int MemoryAllocator::tryToJoin(MemoryAllocator::list *ftmp) {
    if (!ftmp) return 0;
    if (ftmp->next && (char*)(ftmp->next) == (char*)ftmp + ftmp->size*MEM_BLOCK_SIZE) {
        ftmp->size += ftmp->next->size;
        ftmp->next = ftmp->next->next;
        if (ftmp->next) {
            ftmp->next->prev = ftmp;
        }
        return 1;
    }
    else return 0;
}
