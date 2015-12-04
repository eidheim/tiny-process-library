# tiny-process-library
A tiny platform independent library making it simple to create and stop new processes in C++, as well as reading from stdin and stderr, and writing to stdin of the new processes. 

This library was created for, and is used by the C++ IDE project [juCi++](https://github.com/cppit/jucipp).

### Features
* No dependencies
* Simple to use
* Platform independent
* Read separately from stout and stderr using anonymous functions
* Write to stdin of a new process
* Kill a running process

###Usage
See [examples.cpp](http://github.com/eidheim/tiny-process-library/examples.cpp)

### Get, compile and run
```sh
git clone git clone http://github.com/eidheim/tiny-process-library
cd tiny-process-library
cmake .
make
./examples
```