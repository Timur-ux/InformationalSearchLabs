add_executable(tokensCount ./TokensCount.cpp)
target_link_libraries(tokensCount PRIVATE ${CMAKE_PROJECT_NAME}_lib)

