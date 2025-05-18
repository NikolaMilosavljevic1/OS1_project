#pragma once
#include "../lib/hw.h"
#include "../testh/printing.hpp"
#include "../lib/console.h"
class MemoryAllocator {
public:
    void* mem_alloc (size_t size);

    int mem_free (void* ptr);

    static MemoryAllocator& getInstance();

private:
    struct list {
        list* next;
        list* prev;
        size_t size;
    };

    static list* free_list_head;
    static list* allocated_list_head;

    int tryToJoin(list*);

    MemoryAllocator() {};

    MemoryAllocator& operator=(const MemoryAllocator&) = delete;

    MemoryAllocator(const MemoryAllocator&) = delete;
};