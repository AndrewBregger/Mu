cmake_minimum_required(VERSION 3.13)
#find_package(LLVM REQUIRED CONFIG)

project(Mu)
set(CMAKE_CXX_STANDARD 17)

#message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
#message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
#
## Set your project compile flags.
## E.g. if using the C++ header files
## you will need to enable C++11 support
## for your compiler.
#
#include_directories(${LLVM_INCLUDE_DIRS})
#add_definitions(${LLVM_DEFINITIONS})


# Link against LLVM libraries



if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-security -pedantic -Wall -Wextra -g3")
endif()


include_directories()
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
        Mu/src/parser/ast/expr.cpp
        Mu/src/parser/ast/decl.cpp
        Mu/src/parser/ast/stmt.cpp
        Mu/src/parser/ast/pattern.cpp
        Mu/src/parser/ast/specs.cpp
        Mu/src/parser/ast/renderer.hpp
        Mu/src/parser/ast/renderer.cpp
        Mu/src/parser/parser.cpp
        Mu/src/parser/parser.hpp
        Mu/src/parser/grammer/grammer.cpp
        Mu/src/parser/grammer/grammer.hpp
        Mu/src/parser/grammer/parsers/infix_parser.cpp
        Mu/src/parser/grammer/parsers/infix_parser.hpp
        Mu/src/parser/grammer/parsers/prefix_parser.cpp
        Mu/src/parser/grammer/parsers/prefix_parser.hpp
        Mu/src/parser/grammer/parsers/name_parser.cpp
        Mu/src/parser/grammer/parsers/name_parser.hpp
        Mu/src/parser/grammer/parsers/prefixop_parser.cpp
        Mu/src/parser/grammer/parsers/prefixop_parser.hpp
        Mu/src/parser/grammer/parsers/literal_parser.cpp
        Mu/src/parser/grammer/parsers/literal_parser.hpp
        Mu/src/parser/grammer/parsers/block_parsers.cpp
        Mu/src/parser/grammer/parsers/block_parsers.hpp
        Mu/src/parser/grammer/parsers/period_parser.cpp
        Mu/src/parser/grammer/parsers/period_parser.hpp
        Mu/src/parser/grammer/parsers/call_parser.cpp
        Mu/src/parser/grammer/parsers/call_parser.hpp
        Mu/src/parser/grammer/parsers/parethesis_parser.cpp
        Mu/src/parser/grammer/parsers/parethesis_parser.hpp
        Mu/src/parser/grammer/parsers/compoundeliteral_parser.cpp
        Mu/src/parser/grammer/parsers/compoundeliteral_parser.hpp
        Mu/src/parser/grammer/parsers/if_parser.cpp
        Mu/src/parser/grammer/parsers/if_parser.hpp
        Mu/src/parser/grammer/parsers/while_parser.cpp
        Mu/src/parser/grammer/parsers/while_parser.hpp
        Mu/src/parser/grammer/parsers/for_parser.cpp
        Mu/src/parser/grammer/parsers/for_parser.hpp
        Mu/src/parser/grammer/parsers/lambda_parser.cpp
        Mu/src/parser/grammer/parsers/lambda_parser.hpp
        Mu/src/parser/grammer/parsers/defer_parser.cpp
        Mu/src/parser/grammer/parsers/defer_parser.hpp
        Mu/src/parser/grammer/parsers/match_parser.cpp
        Mu/src/parser/grammer/parsers/match_parser.hpp
        Mu/src/parser/grammer/parsers/return_parser.cpp
        Mu/src/parser/grammer/parsers/return_parser.hpp
        Mu/src/analysis/typer.cpp
        Mu/src/analysis/typer.hpp
        Mu/src/analysis/types/type.cpp
        Mu/src/analysis/types/type.hpp
        Mu/src/parser/grammer/parsers/range_parser.cpp
        Mu/src/parser/grammer/parsers/range_parser.hpp
        Mu/src/analysis/types/schema.cpp
        Mu/src/analysis/types/schema.hpp
        Mu/src/analysis/scope.cpp
        Mu/src/analysis/scope.hpp
        Mu/src/analysis/entity.cpp
        Mu/src/analysis/entity.hpp
        Mu/src/analysis/value.cpp
        Mu/src/analysis/value.hpp
        Mu/src/parser/grammer/parsers/cast_parser.cpp
        Mu/src/parser/grammer/parsers/cast_parser.hpp
        Mu/src/analysis/typer_op_eval.cpp
        Mu/src/analysis/typer_op_eval.hpp
        Mu/src/analysis/operand.cpp
        Mu/src/analysis/operand.hpp
        )

# Find the libraries that correspond to the LLVM components
# that we wish to use
#llvm_map_components_to_libnames(llvm_libs support core irreader)
#
#target_link_libraries(Mu ${llvm_libs})
