function(set_wall target)
  if(NOT MSVC)
    target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic -Werror)
  endif()
endfunction()