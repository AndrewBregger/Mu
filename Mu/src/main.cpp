#include <iostream>
#include "interpreter.hpp"
#include "parser/scanner/scanner.hpp"
#include "parser/scanner/token.hpp"
#include "utils/file.hpp"

int main(i32 argc, const char** argv) {
    std::vector<std::string> args;
    args.reserve(argc);

    for(i32 i = 0; i < argc; ++i)
        args.emplace_back(argv[i]);

    Interpreter interp(args);
    interp.set_stream(&std::cout);

    io::File file(io::Path("tests/simple_expr.mu"));

    mu::Token token;
    mu::Scanner scanner(&interp);
    scanner.init(&file);
    scanner.advance();

    do {
        token = scanner.token();
        std::cout << token << std::endl;
        scanner.advance();
    } while(token.kind() != mu::Tkn_Eof or token.kind() == mu::Tkn_Error);

    interp.process(&file);


}