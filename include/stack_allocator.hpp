#ifndef STACK_ALLOCATOR
#define STACK_ALLOCATOR

#include <iostream>

/**
 * The stack allocator class.
 * Templated on the type of element T, and number of elements ELEMENTS. This
 * means you need to know in advance the maximum number of objects you will be
 * allocating through this allocator. This may seem like a huge limitation, but
 * stack allocators are not meant to completely replace heap allocators.
 * 
 * It is beneficial to use this class to allocate container items when:
 *  - You make a large number of allocations. Heap allocators are typically
 *  optimized for allocation speed, but you will get penalized once your
 *  collection goes out of scope, and you need to deallocate everything.
 *  - You know the upper limit to the number of allocated objects.
 *  - You use the allocated objects in an isolated part of your program. Once
 *  this allocator object goes out of scope, every object it created gets
 *  deleted instantly.
 */
template <typename T, int ELEMENTS>
class StackAllocator
{
public:
    typedef T value_type; // std::allocator interface requirement
    T data[ELEMENTS];     // This is the data store we will be using
    std::size_t next;     // Denotes the next item to return from allocate

    StackAllocator() : next{0} {};

    // Rebind is neede here because our allocator is actually a two-parameter
    // template class
    template <typename U>
    struct rebind
    {
        typedef StackAllocator<U, ELEMENTS> other;
    };

    // Just return the next pointer into the preallocated array and update the
    // next index
    T *
    allocate(std::size_t size)
    {
        if (next + size > ELEMENTS)
        {
            return nullptr;
        }

        T *retval = &(data[next]);
        next += size;
        return retval;
    }

    // deallocate() does literally nothing, and is inlined
    inline void deallocate(T *ptr, std::size_t)
    {
    }
};

#endif