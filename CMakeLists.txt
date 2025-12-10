cmake_minimum_required(VERSION 3.10)

project(
	YetAnotherIndexer
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(mongocxx)
find_package(userver COMPONENTS core mongo REQUIRED)
if(NOT userver_FOUND)
	message(STATUS "Global userver installation not found. Trying to fetch userver from ${CMAKE_CURRENT_SOURCE_DIR}/third_party/userver directory")
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/userver)
		message(STATUS "Using local ${CMAKE_CURRENT_SOURCE_DIR}/third_party/userver framework")
		add_subdirectory(third_party/userver)
		userver_setup_environment()
	else()
		message(FATAL_ERROR "Userver not found not global neither locally")
	endif()
else()
	message(STATUS "Using global userver framework")
endif()

# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
file(GLOB_RECURSE ${CMAKE_PROJECT_NAME}_src LIST_DIRECTORIES FALSE ./src/*.cpp)
add_compile_options(-g -O0)

add_library(${CMAKE_PROJECT_NAME}_lib STATIC ${${CMAKE_PROJECT_NAME}_src})
target_include_directories(${CMAKE_PROJECT_NAME}_lib PRIVATE "./include/")
include_directories(include)

add_subdirectory("./indexer/")
add_executable(tokenizer ./Tokenizer.cpp)
add_executable(main ./main.cpp)

target_link_libraries(main PRIVATE ${CMAKE_PROJECT_NAME}_lib)
target_link_libraries(tokenizer PRIVATE ${CMAKE_PROJECT_NAME}_lib)

