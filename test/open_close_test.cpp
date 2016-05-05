#include "process.hpp"
#include <iostream>

using namespace std;

#if !defined(_WIN32) || defined(MSYS_PROCESS_USE_SH)
const string PREFIX = "";
const char* EOL = "\n";
#else
const string PREFIX = "cmd /C ";
const char* EOL = "\r\n";
#endif

int main() {
  bool stdout_error=false;
  for(size_t c=0;c<10000;c++) {
    Process process(PREFIX+"echo Hello World "+std::to_string(c), "", [&stdout_error, c](const char *bytes, size_t n) {
      if(std::string(bytes, n)!="Hello World "+std::to_string(c)+EOL)
        stdout_error=true;
    }, [](const char *bytes, size_t n) {
    }, true);
    auto exit_status=process.get_exit_status();
    if(exit_status!=0) {
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