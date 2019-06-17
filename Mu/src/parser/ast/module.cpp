//
// Created by Andrew Bregger on 2019-05-18.
//

#include "module.hpp"

ast::ModuleFile::ModuleFile(ast::Ident *name, std::vector<ast::DeclPtr> &items, const mu::Pos &pos)
        : AstNode(ast_module_file, pos), name(name), items(std::move(items)) {
}

ast::ModuleDirectory::ModuleDirectory(ast::Ident *name, const std::vector<ast::ModuleFile *> &files,
                                      const std::vector<ast::ModuleDirectory *> directories) :
                                      AstNode(ast_module_dir, mu::Pos()){
}
