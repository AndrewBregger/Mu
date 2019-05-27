//
// Created by Andrew Bregger on 2019-05-18.
//

#ifndef MU_MODULE_HPP
#define MU_MODULE_HPP

#include "ast_common.hpp"

namespace mu {
    class Module {
    public:
        Module(ast::Ident* name, std::vector<ast::DeclPtr>& items);
    private:
        ast::Ident* name;
        std::vector<ast::DeclPtr> items;
    };
}


#endif //MU_MODULE_HPP
