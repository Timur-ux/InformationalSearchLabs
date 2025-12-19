cmake_minimum_required(VERSION 3.10)

project(
	YetAnotherIndexer
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# setup userver
find_package(userver COMPONENTS core mongo chaotic QUIET)
if(NOT userver_FOUND)
	message(STATUS "Global userver installation not found. Trying to fetch local userver at ${CMAKE_CURRENT_SOURCE_DIR}/third_party/userver directory")
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/userver)
		message(STATUS "Using local ${CMAKE_CURRENT_SOURCE_DIR}/third_party/userver framework")
		set(USERVER_FEATURE_MONGODB ON CACHE BOOL "" FORCE)
		add_subdirectory(third_party/userver)
	else()
		message(FATAL_ERROR "Userver not found not global neither locally")
	endif()
endif()

userver_setup_environment()

include(FetchContent)
# setup googletest
FetchContent_Declare(
  googletest
	URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
)

# for windows if any
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE ${CMAKE_PROJECT_NAME}_src LIST_DIRECTORIES FALSE ./src/*.cpp)
file(GLOB_RECURSE ${CMAKE_PROJECT_NAME}_tests_src LIST_DIRECTORIES FALSE ./tests/*.cpp)

add_compile_options(-Wno-reorder -Wno-unused-value -Wno-unused-parameter)
if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
	message("DEBUG ENABLED")
	# add_compile_options(-O0)
else()
	message("DEBUG DISABLED")
endif()

add_library(${CMAKE_PROJECT_NAME}_lib STATIC ${${CMAKE_PROJECT_NAME}_src})
target_include_directories(${CMAKE_PROJECT_NAME}_lib PUBLIC "./include/")
include_directories(include)

add_executable(main ./main.cpp)
add_executable(tests ${${CMAKE_PROJECT_NAME}_tests_src})

target_link_libraries(main PRIVATE ${CMAKE_PROJECT_NAME}_lib)
target_link_libraries(tests PRIVATE ${CMAKE_PROJECT_NAME}_lib GTest::gtest_main)

if(${CMAKE_BUILD_TYPE} EQUAL Release)
	add_executable(indexer ./Indexer.cpp)
	target_link_libraries(indexer PRIVATE ${CMAKE_PROJECT_NAME}_lib userver::mongo)
else()
	message(STATUS "Mongocxx library not configured under not Release build so i skip indexer that needs it")
endif()

include(GoogleTest)
gtest_discover_tests(tests)

# ------------------
# |  add services  |
# ------------------
add_subdirectory("./services/db/")
add_subdirectory("./services/tokenizer/")
add_subdirectory("./services/indexer/")
add_subdirectory("./services/searcher/")
