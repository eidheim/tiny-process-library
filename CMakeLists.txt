cmake_minimum_required(VERSION 2.8)

project(tiny-process-library)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
endif()

find_package(Threads REQUIRED)

set(process_source_files process.cpp)

if(WIN32)
  list(APPEND process_source_files process_win.cpp)
  #If compiled using MSYS2, use sh to run commands
  if(MSYS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMSYS_PROCESS_USE_SH")
  endif()
else()
  list(APPEND process_source_files process_unix.cpp)
endif()

add_library(tiny-process-library ${process_source_files})

target_link_libraries(tiny-process-library ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(tiny-process-library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

option(BUILD_TESTING OFF)

# if tiny-process-library is not a sub-project:
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
  add_executable(examples examples.cpp)
  target_link_libraries(examples tiny-process-library)
  set(BUILD_TESTING ON)
endif()

if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

install(TARGETS tiny-process-library DESTINATION lib)
install(FILES process.hpp DESTINATION include)
