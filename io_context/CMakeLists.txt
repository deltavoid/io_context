
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

