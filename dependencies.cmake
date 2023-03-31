set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})
find_package(Sanitizers)

# find_package(gtest CONFIG REQUIRED)

find_package(fmt CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)

find_path(READLINE_INCLUDE_DIR readline/readline.h)
find_path(NCURSES_INCLUDE_DIR ncurses/curses.h)
find_library(NCURSES_LIBRARY ncurses)
find_library(READLINE_LIBRARY readline)
find_library(READLINE_HISTORY_LIBRARY history)