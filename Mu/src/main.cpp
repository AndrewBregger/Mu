#include <iostream>
#include "interpreter.hpp"
#include "parser/scanner/scanner.hpp"
#include "parser/scanner/token.hpp"
#include "utils/directory.hpp"
#include "utils/file.hpp"


int main(i32 argc, const char** argv) {
    std::vector<std::string> args;
    args.reserve(argc);

    // the first argument is always the executable.
    for(i32 i = 1; i < argc; ++i)
        args.emplace_back(argv[i]);

    Interpreter interp(args);
    interp.set_stream(&std::cout);

    if(args.size() == 0)
        interp.fatal("missing input file");
    else {
        interp.compile();
    }
}