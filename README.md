# Stack Allocator toy

Here's a very simple stack allocator example, used to test performance
implications of using this technique on standard STL containers such as list
and vector.

## Building the example

Run the following

```
mkdir build
cd build/
cmake ../
make

# run with
./stack_alloc_test
```

Depending on your OS settings, you will probably need to expand the stack size.
On Linux, this is done by calling

```
ulimit -s 8388608  # this sets the stack size to 8MB
```
