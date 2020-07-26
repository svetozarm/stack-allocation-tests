#include <iostream>
#include <chrono>
#include <list>
#include <vector>
#include <type_traits>
#include "stack_allocator.hpp"

const size_t ALLOCATOR_SIZE = 1024 * 1024;
const int REPS = 10;

// The following two functions use false_type and true_type to let the user
// choose a version of the workload

// Makes a list and fills it with numbers.
// This relies on T having a push_back method, i.e. won't work on sets et al.
template <typename T>
void test_container(size_t container_size, std::false_type validation)
{
    T l;
    for (int i = 0; i < container_size; i++)
    {
        l.push_back(i);
    }
}

// Version of this function that does one additional traversal to validate the
// contents of the data structure. The effect of this is the added cycles to
// the "workload", and a somewhat lower proportion of the run time taken on
// allocation
template <typename T>
void test_container(size_t container_size, std::true_type validation)
{
    T l;
    for (int i = 0; i < container_size; i++)
    {
        l.push_back(i);
    }
    int value = 0;
    for (auto i : l)
    {
        if (i != value++)
        {
            std::cerr << "Value invalid!" << std::endl;
            return;
        }
    }
}

// Main measurement function
// AllocSize is the size of memory to preallocate with the stack allocator.
// ContainerSize is the number of elements to push into the container using this allocator.
// These are different because having them be the same number breaks std::vector, due to the way vectors allocate data.
//
// The Validate parameter is either false_type or true_type, and controls the
// template type deduction for the functions above.
template <template <typename, typename> typename T, int AllocSize, int ContainerSize, typename Validate>
void measure_stack_heap()
{
    using std::chrono::system_clock;

    std::chrono::duration<double, std::milli> stack_duration, heap_duration;

    auto start_time = system_clock::now();
    for (int i = 0; i < REPS; i++)
    {
        test_container<T<int, StackAllocator<int, AllocSize>>>(ContainerSize, Validate());
    }
    auto end_time = system_clock::now();
    stack_duration = end_time - start_time;

    start_time = system_clock::now();
    for (int i = 0; i < REPS; i++)
    {
        test_container<T<int, std::allocator<int>>>(ContainerSize, Validate());
    }
    end_time = system_clock::now();
    heap_duration = end_time - start_time;

    std::cout << "Stack allocatior: " << stack_duration.count() << std::endl
              << "Heap allocator: " << heap_duration.count() << std::endl;
}

int main(int argc, char **argv)
{
    std::cout << "============" << std::endl;
    std::cout << "Testing list with validation" << std::endl;
    std::cout << "============" << std::endl;
    measure_stack_heap<std::list, ALLOCATOR_SIZE, ALLOCATOR_SIZE, std::true_type>();

    std::cout << "============" << std::endl;
    std::cout << "Testing vector with validation" << std::endl;
    std::cout << "============" << std::endl;
    measure_stack_heap<std::vector, ALLOCATOR_SIZE, ALLOCATOR_SIZE / 4, std::true_type>();

    std::cout << "============" << std::endl;
    std::cout << "Testing list without validation" << std::endl;
    std::cout << "============" << std::endl;
    measure_stack_heap<std::list, ALLOCATOR_SIZE, ALLOCATOR_SIZE, std::false_type>();

    std::cout << "============" << std::endl;
    std::cout << "Testing vector without validation" << std::endl;
    std::cout << "============" << std::endl;
    measure_stack_heap<std::vector, ALLOCATOR_SIZE, ALLOCATOR_SIZE / 4, std::false_type>();
    return 0;
}