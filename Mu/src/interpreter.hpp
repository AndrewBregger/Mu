//
// Created by Andrew Bregger on 2019-05-18.
//

#ifndef MU_INTERPRETER_HPP
#define MU_INTERPRETER_HPP

#include "parser/ast/ast_common.hpp"
#include "parser/ast/module.hpp"

#include "utils/io.hpp"
#include "utils/file.hpp"
#include "utils/directory.hpp"

#include <ostream>
#include <cstdio>


#include <unordered_map>


class Interpreter {
public:
    struct Context {
        io::File* current_file{nullptr};
        io::Directory* dir{nullptr};
        // io::Directory* standard; // Directory containing standard library
        // command line argument arguments are stored here.
        std::vector<std::string> args;

        Context(const std::vector<std::string>& args);
    };

    Interpreter(const std::vector<std::string>& args);

    void setup();

    mu::Module* process(io::File* file);

    ast::Ident* find_name(const std::string& name);

    ast::Ident* find_name(const std::string& name, const mu::Pos& pos);
    inline void set_stream(std::ostream* out) { this->out = out; }
    inline std::ostream& out_stream() { return *out;}

    inline io::File* current_file() { return context.current_file; }

    template<typename... Args>
    void report_error(const mu::Pos& pos, const std::string& fmt, Args... args) {
        out_stream() << "Error: ";
        printf(fmt.c_str(), args...);
        out_stream() << std::endl;
    }

    void fatal(const std::string& msg);

    void quit();
private:
    Context context;

    std::unordered_map<std::string, ast::Ident*> names;
    std::ostream* out;
};


#endif //MU_INTERPRETER_HPP
