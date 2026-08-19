CMAKE_MINIMUM_REQUIRED(VERSION 4.0)

PROJECT(gs_lib)

ADD_LIBRARY(gs_lib INTERFACE
        uint512_t.hpp
        argument_parser.hpp)

set_target_properties(gs_lib PROPERTIES CMAKE_CXX_STANDARD 20)

install(FILES
        uint512_t.hpp
        argument_parser.hpp
        DESTINATION .)