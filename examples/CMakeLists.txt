

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