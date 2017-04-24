cmake_minimum_required(VERSION 2.8)

project(tiny-process-library)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wno-unused-parameter")
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
target_include_directories(tiny-process-library PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/.>)
target_include_directories(tiny-process-library SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)

# Installation
install(TARGETS tiny-process-library 
  EXPORT tiny-process-library-targets
  DESTINATION lib)
install(FILES process.hpp DESTINATION include)

set(CONFIG_PACKAGE_INSTALL_DIR lib/cmake/tiny-process-library)
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/tiny-process-library-config-version.cmake
  VERSION 1.0.7
  COMPATIBILITY SameMajorVersion
)

install(EXPORT tiny-process-library-targets
  DESTINATION
    ${CONFIG_PACKAGE_INSTALL_DIR}
)

install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/tiny-process-library-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/tiny-process-library-config-version.cmake
  DESTINATION
    ${CONFIG_PACKAGE_INSTALL_DIR} )

# Build examples
add_executable(examples examples.cpp)
target_link_libraries(examples tiny-process-library)

option(BUILD_TESTING Off)

# To enable tests: cmake -DBUILD_TESTING=1 .
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()
