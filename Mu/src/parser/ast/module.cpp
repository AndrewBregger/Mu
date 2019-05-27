//
// Created by Andrew Bregger on 2019-05-18.
//

#include "module.hpp"

mu::Module::Module(ast::Ident *name, std::vector<ast::DeclPtr> &items) : name(name), items(std::move(items)) {
}
