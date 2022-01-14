FROM deltavoid/debian_cpp_basic:2022.01.13


ADD ./ /repos/io_context

RUN cd /repos/io_context && make build
