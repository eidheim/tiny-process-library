set(test_source_files open_close_test.cc ../process.cc)

if(MSYS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMSYS_PROCESS_USE_SH")
  list(APPEND test_source_files ../process_win.cc)
else()
  list(APPEND test_source_files ../process_unix.cc)
endif()

add_executable(open_close_test ${test_source_files})

target_link_libraries(open_close_test ${CMAKE_THREAD_LIBS_INIT})

include_directories(..)

add_test(open_close_test open_close_test)