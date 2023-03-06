find_package(fmt CONFIG REQUIRED)

find_path(READLINE_INCLUDE_DIR readline/readline.h)
find_path(NCURSES_INCLUDE_DIR ncurses/curses.h)
find_library(NCURSES_LIBRARY ncurses)
find_library(READLINE_LIBRARY readline)
find_library(READLINE_HISTORY_LIBRARY history)

add_executable(${PROJECT_NAME}_bin
  main.cpp 
  ReadLine.cpp
  ../include/config.hpp)

target_include_directories(${PROJECT_NAME}_bin
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${NCURSES_INCLUDE_DIR}
  ${READLINE_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME}_bin 
  PRIVATE
  fmt::fmt
  ${READLINE_LIBRARY}
  ${READLINE_HISTORY_LIBRARY}
  ${NCURSES_LIBRARY}
  ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME}_bin
  PROPERTIES
  OUTPUT_NAME ${PROJECT_NAME}
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (SANITIZE_ADDRESS OR SANITIZE_MEMORY OR SANITIZE_THREAD OR SANITIZE_UNDEFINED)
  add_sanitizers(${PROJECT_NAME}_bin)
  target_compile_options(${PROJECT_NAME}_bin PRIVATE -fno-sanitize-recover=all)
endif()