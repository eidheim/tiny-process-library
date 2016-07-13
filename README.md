# tiny-process-library [![Build Status](https://travis-ci.org/cppit/jucipp.svg?branch=master)](https://travis-ci.org/eidheim/tiny-process-library)
A small platform independent library making it simple to create and stop new processes in C++, as well as writing to stdin and reading from stdout and stderr of a new process.

This library was created for, and is used by the C++ IDE project [juCi++](https://github.com/cppit/jucipp).

### Features
* No external dependencies
* Simple to use
* Platform independent
* Read separately from stout and stderr using anonymous functions
* Write to stdin
* Kill a running process (SIGTERM is supported on Unix-like systems)
* Correctly closes file descriptors/handles

### Usage
See [examples.cpp](https://github.com/eidheim/tiny-process-library/blob/master/examples.cpp).

### Get, compile and run

#### Unix-like systems
```sh
git clone http://github.com/eidheim/tiny-process-library
cd tiny-process-library
cmake .
make
./examples
```

#### Windows with MSYS2 (https://msys2.github.io/)
```sh
git clone http://github.com/eidheim/tiny-process-library
cd tiny-process-library
cmake -G"MSYS Makefiles" .
make
./examples
```
