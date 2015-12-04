#include "process.h"
#include <iostream>

using namespace std;

int main() {
  Process hello_world_process("echo \"Hello World\"", "", [](const char *bytes, size_t n) {
    cout << "From hello_world_process: " << std::string(bytes, n);
  });
  
  auto exit_code=hello_world_process.get_exit_code();
  cout << "hello_world_process returned " << exit_code << endl;
  
  return 0;
}