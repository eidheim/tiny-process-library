#include "process.hpp"
#include <iostream>

using namespace std;

int main() {
  bool stdout_error=false;
  for(size_t c=0;c<10000;c++) {
    Process process("echo Hello World", "", [&stdout_error](const char *bytes, size_t n) {
      if(std::string(bytes, n)!="Hello World\n") 
        stdout_error=true;
    });
    auto exit_code=process.get_exit_code();
    if(exit_code!=0) {
      cerr << "Process returned failure." << endl;
      return 1;
    }
    if(stdout_error) {
      cerr << "Wrong output to stdout." << endl;
      return 1;
    }
  }
 
  return 0;
}