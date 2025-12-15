set(serviceName dbService)

add_executable(${serviceName} ./main.cpp)


file(GLOB_RECURSE ${serviceName}_src LIST_DIRECTORIES FALSE ./src/*.cpp)
add_library(${serviceName}_lib STATIC ${${serviceName}_src})

target_link_libraries(dbService PRIVATE userver::core ${serviceName}_lib)
target_include_directories(dbService PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include")

unset(serviceName)

