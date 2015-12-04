#include "process.h"
#include <iostream>

using namespace std;

int main() {
  cout << "Example 1 - simple echo process" << endl;
  Process process1("echo \"Hello World\"", "", [](const char *bytes, size_t n) {
    cout << "Output example 1 process: " << std::string(bytes, n);
  });
  
  auto exit_code=process1.get_exit_code();
  cout << "Example 1 process returned: " << exit_code << endl;
  
  
  cout << endl << "Example 2 - async sleep process" << endl;
  std::thread thread2([]() {
    Process process2("sleep 5");
    auto exit_code=process2.get_exit_code();
    cout << "Example 2 process returned (" << (exit_code==0?"success":"failure") << "): " << exit_code << endl;
  });
  thread2.detach();
  
  std::this_thread::sleep_for(std::chrono::seconds(10));
  
  
  cout << endl << "Example 3 - killing async sleep process after 5 seconds" << endl;
  auto process3=std::make_shared<Process>("sleep 10");
  std::thread thread3([process3]() {
    auto exit_code=process3->get_exit_code();
    cout << "Example 3 process returned (" << (exit_code==0?"success":"failure") << "): " << exit_code << endl;
  });
  thread3.detach();
  
  std::this_thread::sleep_for(std::chrono::seconds(5));
  Process::kill(process3->get_id());
  std::this_thread::sleep_for(std::chrono::seconds(5));
  
  
  //Example 4 and 5 only works when having access to the sh command
  //On Windows: install MSYS2 (https://msys2.github.io)
#if !defined(_WIN32) || defined(MSYS_PROCESS_USE_SH)
  cout << endl << "Example 4 - multiple commands, stdout and stderr" << endl;
  Process process4("echo something && ls an_incorrect_path", "", [](const char *bytes, size_t n) {
    cout << "Output from stdout: " << std::string(bytes, n);
  }, [](const char *bytes, size_t n) {
    cout << "Output from stderr: " << std::string(bytes, n);
  });
  exit_code=process4.get_exit_code();
  cout << "Example 4 process returned: " << exit_code << endl;
  std::this_thread::sleep_for(std::chrono::seconds(5));
  
  
  cout << endl << "Example 5 - stdout and stdin" << endl;
  Process process5("bash", "", [](const char *bytes, size_t n) {
    cout << "Output from stdout: " << std::string(bytes, n);
  }, nullptr, true);
  process5.write_stdin("echo something_from_bash\n", 26);
  process5.write_stdin("exit\n", 6);
  exit_code=process5.get_exit_code();
  cout << "Example 5 process returned: " << exit_code << endl;
  std::this_thread::sleep_for(std::chrono::seconds(5));
#endif
  return 0;
}
