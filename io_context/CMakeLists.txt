message("io_context CMakeLists begin")
project(io_context)



#configuration
message("CMAKE_PROJECT_NAME: ${CMAKE_PROJECT_NAME}")
message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

message("PROJECT_NAME: ${PROJECT_NAME}")
message("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message("PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")


# message("CURRENT_SOURCE_DIR: ${CURRENT_SOURCE_DIR}")
# message("CURRENT_BINARY_DIR: ${CURRENT_BINARY_DIR}")






#directory and target

#io_context
aux_source_directory(. IO_CONTEXT_SRCS)

add_library(io_context ${IO_CONTEXT_SRCS})

target_include_directories(io_context 
    PUBLIC
        .
)

target_link_libraries(io_context 
    PUBLIC
        -lpthread
)

message("io_context CMakeLists end")