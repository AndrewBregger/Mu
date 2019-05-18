cmake_minimum_required(VERSION 3.13)
project(Mu)

set(CMAKE_CXX_STANDARD 17)

include_directories(Mu/src)

add_executable(Mu
        Mu/src/parser/scanner/scanner.cpp
        Mu/src/parser/scanner/scanner.hpp
        Mu/src/parser/scanner/token.cpp
        Mu/src/parser/scanner/token.hpp
        Mu/src/utils/directory.cpp
        Mu/src/utils/directory.hpp
        Mu/src/utils/file.cpp
        Mu/src/utils/file.hpp
        Mu/src/utils/io.cpp
        Mu/src/utils/io.hpp
        Mu/src/common.hpp
        Mu/src/main.cpp Mu/src/interpreter.cpp Mu/src/interpreter.hpp)
