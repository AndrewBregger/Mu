//
// Created by Andrew Bregger on 2019-05-18.
//

#ifndef MU_MODULE_HPP
#define MU_MODULE_HPP

#include "ast_common.hpp"
#include <vector>

namespace ast {
    class ModuleFile : public AstNode {
    public:
        ModuleFile(ast::Ident *name, std::vector<ast::DeclPtr> &items, const mu::Pos &pos);

        inline ast::Ident* get_name() { return name; }

        inline std::vector<ast::DeclPtr>::iterator begin() { return items.begin(); }
        inline std::vector<ast::DeclPtr>::iterator end() { return items.end(); }

    private:
        ast::Ident* name;
        std::vector<ast::DeclPtr> items;
    };

    class ModuleDirectory : public AstNode {
    public:
        ModuleDirectory(ast::Ident* name,
                const std::vector<ModuleFile*>& files,
                const std::vector<ModuleDirectory*> directories);

        inline ast::Ident* get_name() { return name; }
    private:
        ast::Ident* name;
        std::vector<ModuleFile*> files;
        std::vector<ModuleDirectory*> directories;
    };
}

#endif //MU_MODULE_HPP
