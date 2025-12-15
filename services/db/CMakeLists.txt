set(serviceName dbService)
set(configName static_config_yml)

file(GLOB_RECURSE ${serviceName}_src LIST_DIRECTORIES FALSE ./src/*.cpp)
userver_embed_file(${serviceName}_config NAME ${configName} FILEPATH ${CMAKE_CURRENT_SOURCE_DIR}/static_config.yaml)


file(GLOB_RECURSE ${serviceName}_schemas ${CMAKE_CURRENT_SOURCE_DIR}/schemas/*.yaml)
userver_target_generate_chaotic(
		${serviceName}-chgen
    # Generate serializers for responses
    GENERATE_SERIALIZERS
    # Map '/components/schemas/*' JSONSchema types to C++ types in 'samples::STEM' namespace
		LAYOUT "/components/schemas/([^/]*)/=${serviceName}::{stem}::{0}"
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/include
    SCHEMAS ${${serviceName}_schemas}
    RELATIVE_TO ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(${serviceName}_lib OBJECT ${${serviceName}_src})
target_link_libraries(${serviceName}_lib PUBLIC userver::core ${serviceName}-chgen)
target_include_directories(${serviceName}_lib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_BINARY_DIR}/include")

add_executable(${serviceName} ./main.cpp)
target_link_libraries(${serviceName} PRIVATE ${serviceName}_config PRIVATE ${serviceName}_lib PRIVATE ${CMAKE_PROJECT_NAME}_lib)
target_include_directories(${serviceName} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_compile_definitions(${serviceName} PRIVATE CONFIG_NAME="${configName}")

unset(serviceName)
unset(configName)

