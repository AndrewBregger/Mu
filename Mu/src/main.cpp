#include <iostream>
#include "interpreter.hpp"
#include "parser/scanner/scanner.hpp"
#include "parser/scanner/token.hpp"
#include "utils/file.hpp"

int main() {
    Interpreter interp;
    interp.set_stream(&std::cout);

    mu::Scanner scanner(&interp);
    io::File file(io::Path("tests/scan_test.mu"));
    scanner.init(&file);
    scanner.advance();

    mu::Token token;
    do {
        token = scanner.token();
        std::cout << token << std::endl;
        scanner.advance();

    } while(token.kind() != mu::Tkn_Eof or token.kind() == mu::Tkn_Error);
}