message("examples CMakeLists begin")
project(examples)



#configuration
message("CMAKE_PROJECT_NAME: ${CMAKE_PROJECT_NAME}")
message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

message("PROJECT_NAME: ${PROJECT_NAME}")
message("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message("PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")









#directory and target

#alternate_test
add_executable(alternate_test
    alternate_test.cpp
)

target_link_libraries(alternate_test
    PRIVATE
        io_context
)

#acceptor_test
add_executable(acceptor_test
    acceptor_test.cpp
)

target_link_libraries(acceptor_test
    PRIVATE
        io_context
)

message("examples CMakeLists end")
