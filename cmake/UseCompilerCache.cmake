function(useCompilerCache)
  find_program(CCACHE_EXECUTABLE ccache)
  if(NOT CCACHE_EXECUTABLE)
    return()
  endif()

  foreach(lang IN ITEMS C CXX OBJC OBJCXX)
    if(CMAKE_${lang}_COMPILER_ID MATCHES "Clang")
      add_compile_options(
        "$<$<COMPILE_LANGUAGE:${lang}>:SHELL:-Xclang -fno-pch-timestamp>")
    endif()
  endforeach()

  set(CCACHE_ENV
      CCACHE_SLOPPINESS=pch_defines,time_macros
      CACHE STRING
            "List of environment variables for ccache, each in key=value form")

  if(CMAKE_GENERATOR MATCHES "Ninja|Makefiles")
    foreach(lang IN ITEMS C CXX OBJC OBJCXX CUDA)
      set(CMAKE_${lang}_COMPILER_LAUNCHER
          ${CMAKE_COMMAND} -E env ${CCACHE_ENV} ${CCACHE_EXECUTABLE}
          PARENT_SCOPE)
    endforeach()
  endif()
endfunction()
