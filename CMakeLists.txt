cmake_minimum_required(VERSION 3.13)
project(Mu)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-security -pedantic -Wall -Wextra")

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
        Mu/src/main.cpp
        Mu/src/interpreter.cpp
        Mu/src/interpreter.hpp
        Mu/src/parser/ast/expr.hpp
        Mu/src/parser/ast/decl.hpp
        Mu/src/parser/ast/stmt.hpp
        Mu/src/parser/ast/module.cpp
        Mu/src/parser/ast/module.hpp
        Mu/src/parser/ast/pattern.hpp
        Mu/src/parser/ast/specs.hpp
        Mu/src/parser/parser.cpp Mu/src/parser/parser.hpp Mu/src/parser/grammer/grammer.cpp Mu/src/parser/grammer/grammer.hpp Mu/src/parser/grammer/parsers/infix_parser.cpp Mu/src/parser/grammer/parsers/infix_parser.hpp Mu/src/parser/grammer/parsers/prefix_parser.cpp Mu/src/parser/grammer/parsers/prefix_parser.hpp Mu/src/parser/grammer/parsers/name_parser.cpp Mu/src/parser/grammer/parsers/name_parser.hpp Mu/src/parser/grammer/parsers/prefixop_parser.cpp Mu/src/parser/grammer/parsers/prefixop_parser.hpp Mu/src/parser/grammer/parsers/literal_parser.cpp Mu/src/parser/grammer/parsers/literal_parser.hpp Mu/src/parser/grammer/parsers/block_parsers.cpp Mu/src/parser/grammer/parsers/block_parsers.hpp Mu/src/parser/grammer/parsers/period_parser.cpp Mu/src/parser/grammer/parsers/period_parser.hpp Mu/src/parser/grammer/parsers/call_parser.cpp Mu/src/parser/grammer/parsers/call_parser.hpp Mu/src/parser/grammer/parsers/parethesis_parser.cpp Mu/src/parser/grammer/parsers/parethesis_parser.hpp Mu/src/parser/grammer/parsers/compoundeliteral_parser.cpp Mu/src/parser/grammer/parsers/compoundeliteral_parser.hpp Mu/src/parser/grammer/parsers/if_parser.cpp Mu/src/parser/grammer/parsers/if_parser.hpp Mu/src/parser/grammer/parsers/while_parser.cpp Mu/src/parser/grammer/parsers/while_parser.hpp Mu/src/parser/grammer/parsers/for_parser.cpp Mu/src/parser/grammer/parsers/for_parser.hpp)

