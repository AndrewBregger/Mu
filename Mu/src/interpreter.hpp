//
// Created by Andrew Bregger on 2019-05-18.
//

#ifndef MU_INTERPRETER_HPP
#define MU_INTERPRETER_HPP

#include "parser/ast/ast_common.hpp"
#include <ostream>
#include <cstdio>


#include <unordered_map>

class Interpreter {
public:

    ast::Ident* find_name(const std::string& name);

    ast::Ident* find_name(const std::string& name, const mu::Pos& pos);
    inline void set_stream(std::ostream* out) { this->out = out; }
    inline std::ostream& out_stream() { return *out;}

    template<typename... Args>
    void report_error(const mu::Pos& pos, const std::string& fmt, Args... args) {
        out_stream() << "Error: ";
        printf(fmt.c_str(), args...);
        out_stream() << std::endl;
    }
private:
    std::unordered_map<std::string, ast::Ident*> names;
    std::ostream* out;
};


#endif //MU_INTERPRETER_HPP
