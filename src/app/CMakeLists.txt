add_executable(cpplox main.cpp ReadLine.cpp
                      ${CMAKE_SOURCE_DIR}/include/config.hpp)

target_include_directories(
  cpplox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${NCURSES_INCLUDE_DIR}
                 ${READLINE_INCLUDE_DIR})

target_link_libraries(
  cpplox PRIVATE fmt::fmt ${READLINE_LIBRARY} ${READLINE_HISTORY_LIBRARY}
                 ${NCURSES_LIBRARY} cpplox::lox)

add_sanitizers(cpplox)

# set_target_properties(plox::cpplox PROPERTIES OUTPUT_NAME cpplox)
# RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# if (SANITIZE_ADDRESS OR SANITIZE_MEMORY OR SANITIZE_THREAD OR
# SANITIZE_UNDEFINED) target_compile_options(${PROJECT_NAME}_bin PRIVATE
# -fno-sanitize-recover=all) endif()
