set(HEADER_LIST )
find_package(fmt CONFIG REQUIRED)

set(SOURCE_LIST )

add_library(${PROJECT_NAME} ${HEADER_LIST} ${SOURCE_LIST})
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  fmt::fmt)
target_include_directories(${PROJECT_NAME} PUBLIC "")

add_executable(${PROJECT_NAME}_bin main.cpp)
set_target_properties(${PROJECT_NAME}_bin PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_bin 
  PRIVATE
  fmt::fmt
  ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}_bin PRIVATE ${PROJECT_BINARY_DIR}/include)

if (SANITIZE_ADDRESS OR SANITIZE_MEMORY OR SANITIZE_THREAD OR SANITIZE_UNDEFINED)
  add_sanitizers(${PROJECT_NAME})
  add_sanitizers(${PROJECT_NAME}_bin)

  target_compile_options(${PROJECT_NAME} PRIVATE -fno-sanitize-recover=all)
  target_compile_options(${PROJECT_NAME}_bin PRIVATE -fno-sanitize-recover=all)
endif()