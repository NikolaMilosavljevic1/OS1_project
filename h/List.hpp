#pragma once
#include "MemoryAllocator.hpp"
#include "../testh/printing.hpp"
template <typename T>
class List {
private:
    struct Elem {
        T* data;
        Elem* next;
        Elem(T* data, Elem* next) : data(data), next(next) {}

        void *operator new (size_t n) {
            //kprintString("\nlistnew\n");
            //kprintInteger(n);
            uint64 size = n / MEM_BLOCK_SIZE + (n % MEM_BLOCK_SIZE ? 1 : 0);
            return MemoryAllocator::getInstance().mem_alloc((size_t) size);
        }

        void *operator new [] (size_t n) {
            uint64 size = n / MEM_BLOCK_SIZE + (n % MEM_BLOCK_SIZE ? 1 : 0);
            return MemoryAllocator::getInstance().mem_alloc((size_t) size);
        }

        void operator delete(void *ptr) {
            MemoryAllocator::getInstance().mem_free(ptr);
        }

        void operator delete [](void *ptr){
            MemoryAllocator::getInstance().mem_free(ptr);
        }
    };

    Elem *head, *tail;
public:
    void *operator new (size_t n);

    void *operator new[] (size_t n);

    void operator delete(void *ptr);

    void operator delete[] (void *ptr);

    List() : head(0), tail(0) {}

    List(const List<T> &) = delete;

    List<T> &operator=(const List<T> &) = delete;

    void addFirst(T *data) {
        Elem *elem = new Elem(data, head);
        head = elem;
        if (!tail) { tail = head; }
    }

    void addLast(T *data) {
        Elem *elem = new Elem(data, 0);
        if (tail) {
            tail->next = elem;
            tail = elem;
        } else {
            head = tail = elem;
        }
    }

    T *removeFirst() {
        if (!head) { return 0; }

        Elem *elem = head;
        head = head->next;
        if (!head) { tail = 0; }

        T *ret = elem->data;
        delete elem;
        return ret;
    }

    T *peekFirst() {
        if (!head) { return 0; }
        return head->data;
    }

    T *removeLast() {
        if (!head) { return 0; }

        Elem *prev = 0;
        for (Elem *curr = head; curr && curr != tail; curr = curr->next) {
            prev = curr;
        }

        Elem *elem = tail;
        if (prev) {
            prev->next = 0;
        } else {
            head = 0;
        }
        tail = prev;

        T *ret = elem->data;
        delete elem;
        return ret;
    }

    T *peekLast() {
        if (!tail) { return 0; }
        return tail->data;
    }
};

template<typename T>
void *List<T>::operator new (size_t n) {
    uint64 size = n / MEM_BLOCK_SIZE + (n % MEM_BLOCK_SIZE ? 1 : 0);
    return MemoryAllocator::getInstance().mem_alloc((size_t) size);
}

template<typename T>
void *List<T>::operator new [] (size_t n) {
    uint64 size = n / MEM_BLOCK_SIZE + (n % MEM_BLOCK_SIZE ? 1 : 0);
    return MemoryAllocator::getInstance().mem_alloc((size_t) size);
}

template<typename T>
void List<T>::operator delete(void *ptr) {
    MemoryAllocator::getInstance().mem_free(ptr);
}

template<typename T>
void List<T>::operator delete [](void *ptr){
    MemoryAllocator::getInstance().mem_free(ptr);
}