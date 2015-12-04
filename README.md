# tiny-process-library
A small platform independent library making it simple to create and stop new processes in C++, as well as writing to stdin and reading from stdout and stderr of the new process.

This library was created for, and is used by the C++ IDE project [juCi++](https://github.com/cppit/jucipp).

### Features
* No dependencies
* Simple to use
* Platform independent
* Read separately from stout and stderr using anonymous functions
* Write to stdin
* Kill a running process (SIGTERM is supported on Unix-like systems)

###Usage
See [examples.cpp](https://github.com/eidheim/tiny-process-library/blob/master/examples.cpp).

### Get, compile and run
```sh
git clone git clone http://github.com/eidheim/tiny-process-library
cd tiny-process-library
cmake .
make
./examples
```

####If using MSYS2 (https://msys2.github.io/)
Replace `cmake .` above with
```sh
cmake -G"MSYS Makefiles" .
```
