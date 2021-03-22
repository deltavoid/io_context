


add_executable(alternate_test
    alternate_test.cpp
)

target_link_libraries(alternate_test
    PRIVATE
        io_context
)