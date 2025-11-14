cmake_minimum_required(VERSION 3.10)

add_executable(indexer ./main.cpp)
target_link_libraries(indexer PRIVATE mongo::mongocxx_static PRIVATE ${CMAKE_PROJECT_NAME}_lib)
target_include_directories(indexer PUBLIC "../include")
