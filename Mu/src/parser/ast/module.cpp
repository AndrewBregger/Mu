//
// Created by Andrew Bregger on 2019-05-18.
//

#include "module.hpp"
#include "renderer.hpp"

ast::ModuleFile::ModuleFile(ast::Ident *name, std::vector<ast::DeclPtr> &items, const mu::Pos &pos)
        : AstNode(ast_module_file, pos), name(name), items(std::move(items)) {
}

void ast::ModuleFile::renderer(AstRenderer* renderer) {
        renderer->render(this);
}

ast::ModuleDirectory::ModuleDirectory(ast::Ident *name, const std::vector<ast::ModuleFile *> &files,
                                      const std::vector<ast::ModuleDirectory *>& directories) :
                                      AstNode(ast_module_dir, mu::Pos()){
    for(auto f : files)
        all.push_back(f);

    for(auto f : directories)
        all.push_back(f);
}

void ast::ModuleDirectory::renderer(AstRenderer* renderer) {
        renderer->render(this);
}
