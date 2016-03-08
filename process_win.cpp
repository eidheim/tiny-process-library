#include "process.hpp"
#include <cstring>
#include <TlHelp32.h>
#include <stdexcept>

Process::Data::Data(): id(0), handle(NULL) {}

//Based on the discussion thread: https://www.reddit.com/r/cpp/comments/3vpjqg/a_new_platform_independent_process_library_for_c11/cxq1wsj
std::mutex create_process_mutex;

//Based on the example at https://msdn.microsoft.com/en-us/library/windows/desktop/ms682499(v=vs.85).aspx.
Process::id_type Process::open(const std::string &command, const std::string &path) {
  if(open_stdin)
    stdin_fd=std::unique_ptr<fd_type>(new fd_type);
  if(read_stdout)
    stdout_fd=std::unique_ptr<fd_type>(new fd_type);
  if(read_stderr)
    stderr_fd=std::unique_ptr<fd_type>(new fd_type);

  HANDLE stdin_rd_p = NULL;
  HANDLE stdin_wr_p = NULL;
  HANDLE stdout_rd_p = NULL;
  HANDLE stdout_wr_p = NULL;
  HANDLE stderr_rd_p = NULL;
  HANDLE stderr_wr_p = NULL;

  SECURITY_ATTRIBUTES security_attributes;

  security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
  security_attributes.bInheritHandle = TRUE;
  security_attributes.lpSecurityDescriptor = NULL;

  std::lock_guard<std::mutex> hold(create_process_mutex);
  if(stdin_fd) {
    if (!CreatePipe(&stdin_rd_p, &stdin_wr_p, &security_attributes, 0)) {
      return 0;
    }
    if(!SetHandleInformation(stdin_wr_p, HANDLE_FLAG_INHERIT, 0)) {
      CloseHandle(stdin_rd_p);CloseHandle(stdin_wr_p);
      return 0;
    }
  }
  if(stdout_fd) {
    if (!CreatePipe(&stdout_rd_p, &stdout_wr_p, &security_attributes, 0)) {
      if(stdin_fd) {CloseHandle(stdin_rd_p);CloseHandle(stdin_wr_p);}
      return 0;
    }
    if(!SetHandleInformation(stdout_rd_p, HANDLE_FLAG_INHERIT, 0)) {
      if(stdin_fd) {CloseHandle(stdin_rd_p);CloseHandle(stdin_wr_p);}
      CloseHandle(stdout_rd_p);CloseHandle(stdout_wr_p);
      return 0;
    }
  }
  if(stderr_fd) {
    if (!CreatePipe(&stderr_rd_p, &stderr_wr_p, &security_attributes, 0)) {
      if(stdin_fd) {CloseHandle(stdin_rd_p);CloseHandle(stdin_wr_p);}
      if(stdout_fd) {CloseHandle(stdout_rd_p);CloseHandle(stdout_wr_p);}
      return 0;
    }
    if(!SetHandleInformation(stderr_rd_p, HANDLE_FLAG_INHERIT, 0)) {
      if(stdin_fd) {CloseHandle(stdin_rd_p);CloseHandle(stdin_wr_p);}
      if(stdout_fd) {CloseHandle(stdout_rd_p);CloseHandle(stdout_wr_p);}
      CloseHandle(stderr_rd_p);CloseHandle(stderr_wr_p);
      return 0;
    }
  }
  
  PROCESS_INFORMATION process_info;
  STARTUPINFO startup_info;
  
  ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));
  
  ZeroMemory(&startup_info, sizeof(STARTUPINFO));
  startup_info.cb = sizeof(STARTUPINFO);
  startup_info.hStdInput = stdin_fd?stdin_rd_p:INVALID_HANDLE_VALUE;
  startup_info.hStdOutput = stdout_fd?stdout_wr_p:INVALID_HANDLE_VALUE;
  startup_info.hStdError = stderr_fd?stderr_wr_p:INVALID_HANDLE_VALUE;
  if(stdin_fd || stdout_fd || stderr_fd)
    startup_info.dwFlags |= STARTF_USESTDHANDLES;
  
  char* path_cstr;
  if(path=="")
    path_cstr=NULL;
  else {
    path_cstr=new char[path.size()+1];
    std::strcpy(path_cstr, path.c_str());
  }

  char* command_cstr;
#ifdef MSYS_PROCESS_USE_SH
  size_t pos=0;
  std::string sh_command=command;
  while((pos=sh_command.find('\\', pos))!=std::string::npos) {
    sh_command.replace(pos, 1, "\\\\\\\\");
    pos+=4;
  }
  pos=0;
  while((pos=sh_command.find('\"', pos))!=std::string::npos) {
    sh_command.replace(pos, 1, "\\\"");
    pos+=2;
  }
  sh_command.insert(0, "sh -c \"");
  sh_command+="\"";
  command_cstr=new char[sh_command.size()+1];
  std::strcpy(command_cstr, sh_command.c_str());
#else
  command_cstr=new char[command.size()+1];
  std::strcpy(command_cstr, command.c_str());
#endif

  BOOL bSuccess = CreateProcess(NULL, command_cstr, NULL, NULL, TRUE, 0,
                                NULL, path_cstr, &startup_info, &process_info);
  delete[] path_cstr;
  delete[] command_cstr;
  
  if(!bSuccess) {
    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);
    if(stdin_fd) {CloseHandle(stdin_rd_p);CloseHandle(stdin_wr_p);}
    if(stdout_fd) {CloseHandle(stdout_rd_p);CloseHandle(stdout_wr_p);}
    if(stderr_fd) {CloseHandle(stderr_rd_p);CloseHandle(stderr_wr_p);}
    return 0;
  }
  else {
    CloseHandle(process_info.hThread);
    if(stdin_fd) CloseHandle(stdin_rd_p);
    if(stdout_fd) CloseHandle(stdout_wr_p);
    if(stderr_fd) CloseHandle(stderr_wr_p);
  }

  if(stdin_fd) *stdin_fd=stdin_wr_p;
  if(stdout_fd) *stdout_fd=stdout_rd_p;
  if(stderr_fd) *stderr_fd=stderr_rd_p;
  
  closed=false;
  data.id=process_info.dwProcessId;
  data.handle=process_info.hProcess;
  return process_info.dwProcessId;
}

void Process::async_read() {
  if(data.id==0)
    return;
  if(stdout_fd) {
    stdout_thread=std::thread([this](){
      DWORD n;
      char buffer[buffer_size];
      for (;;) {
        BOOL bSuccess = ReadFile(*stdout_fd, static_cast<CHAR*>(buffer), static_cast<DWORD>(buffer_size), &n, NULL);
        if(!bSuccess || n == 0)
          break;
        read_stdout(buffer, static_cast<size_t>(n));
      }
    });
  }
  if(stderr_fd) {
    stderr_thread=std::thread([this](){
      DWORD n;
      char buffer[buffer_size];
      for (;;) {
        BOOL bSuccess = ReadFile(*stderr_fd, static_cast<CHAR*>(buffer), static_cast<DWORD>(buffer_size), &n, NULL);
        if(!bSuccess || n == 0)
          break;
        read_stderr(buffer, static_cast<size_t>(n));
      }
    });
  }
}

int Process::get_exit_status() {
  if(data.id==0)
    return -1;
  DWORD exit_status;
  WaitForSingleObject(data.handle, INFINITE);
  if(!GetExitCodeProcess(data.handle, &exit_status))
    exit_status=-1;
  {
    std::lock_guard<std::mutex> hold(close_mutex);
    CloseHandle(data.handle);
    closed=true;
  }
  close_fds();

  return static_cast<int>(exit_status);
}

void Process::close_fds() {
  if(stdout_thread.joinable())
    stdout_thread.join();
  if(stderr_thread.joinable())
    stderr_thread.join();
  
  if(stdin_fd)
    close_stdin();
  if(stdout_fd) {
    CloseHandle(*stdout_fd);
    stdout_fd.reset();
  }
  if(stderr_fd) {
    CloseHandle(*stderr_fd);
    stderr_fd.reset();
  }
}

bool Process::write(const char *bytes, size_t n) {
  if(!open_stdin)
    throw std::invalid_argument("Can't write to an unopened stdin pipe. Please set open_stdin=true when constructing the process.");

  std::lock_guard<std::mutex> hold(stdin_mutex);
  if(stdin_fd) {
    DWORD written;
    BOOL bSuccess=WriteFile(*stdin_fd, bytes, static_cast<DWORD>(n), &written, NULL);
    if(!bSuccess || written==0) {
      return false;
    }
    else {
      return true;
    }
  }
  return false;
}

void Process::close_stdin() {
  std::lock_guard<std::mutex> hold(stdin_mutex);
  if(stdin_fd) {
    CloseHandle(*stdin_fd);
    stdin_fd.reset();
  }
}

//Based on http://stackoverflow.com/a/1173396
void Process::kill(bool force) {
  std::lock_guard<std::mutex> hold(close_mutex);
  if(data.id>0 && !closed) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(snapshot) {
      PROCESSENTRY32 process;
      ZeroMemory(&process, sizeof(process));
      process.dwSize = sizeof(process);
      if(Process32First(snapshot, &process)) {
        do {
          if(process.th32ParentProcessID==data.id) {
            HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process.th32ProcessID);
            if(process_handle) TerminateProcess(process_handle, 2);
          }
        } while (Process32Next(snapshot, &process));
      }
    }
    TerminateProcess(data.handle, 2);
  }
}

//Based on http://stackoverflow.com/a/1173396
void Process::kill(id_type id, bool force) {
  if(id==0)
    return;
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if(snapshot) {
    PROCESSENTRY32 process;
    ZeroMemory(&process, sizeof(process));
    process.dwSize = sizeof(process);
    if(Process32First(snapshot, &process)) {
      do {
        if(process.th32ParentProcessID==id) {
          HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process.th32ProcessID);
          if(process_handle) TerminateProcess(process_handle, 2);
        }
      } while (Process32Next(snapshot, &process));
    }
  }
  HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
  if(process_handle) TerminateProcess(process_handle, 2);
}
