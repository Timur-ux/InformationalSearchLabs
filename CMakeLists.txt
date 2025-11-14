cmake_minimum_required(VERSION 3.10)

project(
	YetAnotherIndexer
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(mongocxx)

file(GLOB_RECURSE ${CMAKE_PROJECT_NAME}_src LIST_DIRECTORIES FALSE ./src/*.cpp)

add_library(${CMAKE_PROJECT_NAME}_lib STATIC ${${CMAKE_PROJECT_NAME}_src})
target_include_directories(${CMAKE_PROJECT_NAME}_lib PRIVATE "./include/")

add_subdirectory("./indexer/")
