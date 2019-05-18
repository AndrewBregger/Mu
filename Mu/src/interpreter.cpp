//
// Created by Andrew Bregger on 2019-05-18.
//

#include "interpreter.hpp"

ast::Ident *Interpreter::find_name(const std::string &name) {
    auto iter = names.find(name);
    if(iter == names.end())
        return nullptr;
    else
        return iter->second;
}

ast::Ident *Interpreter::find_name(const std::string &name, const mu::Pos &pos) {
    auto iter = names.find(name);
    if(iter == names.end())
        return names.emplace(name, new ast::Ident(name, pos)).first->second;
    else
        return iter->second;
}
