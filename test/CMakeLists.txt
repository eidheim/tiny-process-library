include_directories(..)

function(add_tiny_test name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} tiny-process-library)
  add_test(${name} ${name})
endfunction()

add_tiny_test(open_close_test)
add_tiny_test(multithread_test)
