//
// Created by Andrew Bregger on 2019-05-19.
//

#include <iostream>

#include "parser.hpp"

#include "interpreter.hpp"
#include "ast/expr.hpp"
#include "ast/stmt.hpp"
#include "ast/specs.hpp"
#include "ast/pattern.hpp"
#include "ast/decl.hpp"

namespace ast {
    std::vector<std::string> node_names = {
#define ToString(tok) #tok
            ToString(integer),
            ToString(fl),
            ToString(ch),
            ToString(str),
            ToString(list),
            ToString(map),
            ToString(lambda),
            ToString(name),
            ToString(unary),
            ToString(binary),
            ToString(accessor),
            ToString(tuple_accessor),
            ToString(method),
            ToString(call),
            ToString(if_expr),
            ToString(while_expr),
            ToString(match_expr),
            ToString(for_expr),
            ToString(defer_expr),
            ToString(block),
            ToString(expr_binding),

            ToString(expr),
            ToString(decl),
            ToString(empty),

            ToString(local),
            ToString(mutable),
            ToString(global),
            ToString(procedure),
            ToString(structure),
            ToString(type),
            ToString(type_class),
            ToString(use),
            ToString(alias),
            ToString(generic),
            ToString(ident),
            ToString(multi),
            ToString(tuple_desc),
            ToString(struct_desc),
            ToString(list_desc),
            ToString(type_desc),
            ToString(named),
            ToString(named_generic),
            ToString(tuple),
            ToString(list_spec),
            ToString(ptr),
            ToString(ref),
            ToString(mut),
#undef ToString
    };
}

mu::Parser::Parser(Interpreter* interp) : interp(interp), scanner(interp) {
}

mu::Module *mu::Parser::process(io::File *file) {
    scanner.init(file);
    advance();
    return parse_module();
}

void mu::Parser::passert(bool term) {
    if(!term) {
        assert(term);
    }
}

mu::Module *mu::Parser::parse_module() {

    auto decls = many<ast::DeclPtr>(
            [this]() {
                return parse_toplevel();
            },
            [this]() {
                return !check(mu::Tkn_Eof);
            },
            Parser::append<ast::DeclPtr>
        );

    auto file = interp->current_file();
    auto name = file->name();
    auto mname = interp->find_name(name, Pos(0, 0, 0, file->id()));

    std::cout << "Num Decls: " << decls.size() << std::endl;
    return new Module(mname, decls);
}

bool mu::Parser::advance(bool ignore_newline) {
    do {
        scanner.advance();
        t = scanner.token();
    } while(t.kind() == mu::Tkn_Comment ||
            (ignore_newline and t.kind() == Tkn_NewLine)
    );
    return t.kind() == mu::Tkn_Error;
}


std::pair<mu::Token, bool> mu::Parser::expect(mu::TokenKind tok) {
    auto curr = current();
    if(curr.kind() == tok) {
        advance();
        return std::make_pair(curr, true);
    } else {
        report(curr.pos(), "expecting '%s', found '%s'",
                Token::get_string(tok).c_str(), curr.get_string().c_str());
        return std::make_pair(curr, false);
    }
}

bool mu::Parser::allow(mu::TokenKind tok) {
    if(current().kind() == tok) {
        advance();
        return true;
    }
    return false;
}

bool mu::Parser::check(mu::TokenKind tok) {
    return current().kind() == tok;
}

ast::ExprPtr mu::Parser::parse_expr() {
    return parse_expr(1);
}


ast::ExprPtr mu::Parser::parse_expr(i32 prec_min) {
    auto token = current();
    auto parser = grammar.get_prefix(token);
    if(!parser) {
        report(token.pos(), "unexpected token '%s'", token.get_string().c_str());
        return nullptr;
    }

    auto expr = parser->lud(*this, token);

    // this handles period accessor and method expression.
    while(check(mu::Tkn_Period)) {
        auto parser = grammar.get_infix(current());
        expr = parser->lud(*this, expr, current());
    }

    // this handles function calls.
    while(check(mu::Tkn_OpenParen)) {
        auto parser = grammar.get_infix(current());
        expr = parser->lud(*this, expr, current());
    }

    while(prec_min < current().prec()) {
        auto parser = grammar.get_infix(current());
        if(!parser) {
            report(current().pos(), "expecting binary operator found: '%s'", current().get_string().c_str());
            break;
        }
        expr = parser->lud(*this, expr, current());
    }

    return expr;
}

ast::ExprPtr mu::Parser::parse_call(ast::Ident *name, mu::Token token, ast::ExprPtr operand) {
    advance();
    std::vector<ast::SpecPtr> type_parameters;
    auto pos = token.pos();
    auto actuals = many<ast::ExprPtr>([this]() {
        if(check(mu::Tkn_Identifier)) {
            if(check(mu::Tkn_Colon)) {
                auto ident = current();
                auto pos = ident.pos();

                advance(); // identifier
                pos.extend(current().pos());

                advance(); // colon

                auto expr = parse_expr();
                pos.extend(expr->pos());

                return ast::make_expr<ast::BindingExpr>(ident.ident, expr, pos);
            }
        }

        auto expr = parse_expr();
        return expr;
    }, [this]() {
        return !check(mu::Tkn_CloseParen);
    }, [&pos](auto& results, ast::ExprPtr expr) {
        Parser::append<ast::ExprPtr>(results, expr);
        pos.extend(expr->pos());
    });

    auto [_, valid] = expect(mu::Tkn_CloseParen);

    if(name) {
        auto p = operand->pos();
        p.extend(pos);
        return ast::make_expr<ast::Method>(operand, name, type_parameters, actuals, p);
    }
    else {
        auto p = operand->pos();
        p.extend(pos);
        return ast::make_expr<ast::Call>(operand, type_parameters, actuals, p);
    }
}

ast::StmtPtr mu::Parser::parse_stmt() {
    if(check(mu::Tkn_Let) or check(mu::Tkn_Mut) or
       (check(mu::Tkn_Identifier) and check(mu::Tkn_Colon))) {
        report(current().pos(), "declarations are not implemented");
         auto decl = parse_decl(current());
         return ast::make_stmt<ast::DeclStmt>(decl, decl->pos());
    }
    else {
        auto token = current();
        if(allow(mu::Tkn_NewLine))
            return ast::make_stmt<ast::EmptyStmt>(token.pos());

        auto expr = parse_expr();
        auto [tok, valid] = expect(Tkn_NewLine);
        if(valid)
            return ast::make_stmt<ast::ExprStmt>(expr, expr->pos());
        else
            return ast::make_stmt<ast::EmptyStmt>(token.pos());
    }
}

ast::DeclPtr mu::Parser::parse_toplevel() {
    return parse_decl(current(), true);
}

ast::DeclPtr mu::Parser::parse_decl(mu::Token token, bool toplevel) {
    if(token.kind() ==  mu::Tkn_Eof)
        return nullptr;

    if(token.kind() == mu::Tkn_NewLine) {
        passert(toplevel);
        advance();
        return parse_decl(current(), toplevel);
    }
    ast::DeclPtr decl = nullptr;
    switch(token.kind()) {
        case mu::Tkn_Identifier: {
            advance();
            auto [token, valid] = expect(mu::Tkn_Colon);
            if(valid)
                decl = parse_type_decl(token.ident);
            else {
                report(token.pos(), "expecting ':' following identifier in type declaration");
                break;
            }
        } break;
        case mu::Tkn_Let:
        case mu::Tkn_Mut:
            if(toplevel)
                decl = parse_global(token.kind());
            else
                decl = parse_variable(token.kind());
    }
    // if we are at the end of the file do not expect a new line
    // however, consume them if some are there.
    if(!check(mu::Tkn_Eof))
        expect(mu::Tkn_NewLine);
    else
        remove_newlines();
    return decl;
}

ast::DeclPtr mu::Parser::parse_variable(mu::TokenKind kind) {
    return ast::DeclPtr();
}

ast::DeclPtr mu::Parser::parse_global(mu::TokenKind kind) {
    passert(current().kind() == kind);

    auto pos = current().pos();
    advance();

    auto [name, valid] = expect(mu::Tkn_Identifier);
    pos.extend(name.pos());

    ast::ExprPtr init;
    ast::SpecPtr spec;

    if(valid) {
        if(check(mu::Tkn_NewLine)) {
            report(current().pos(), "expecting a type or '=' following identifier, found a newline");
            return nullptr;
        }

        // let x f32
        if(!check(mu::Tkn_Equal)) {
            spec = parse_spec();
            if(spec)
                pos.extend(spec->pos());
        }

        // let x f32 = 1.0
        if(allow(mu::Tkn_Equal)) {
            init = parse_expr();
            if(init)
                pos.extend(init->pos());
        }
        if(kind == mu::Tkn_Let)
            return ast::make_decl<ast::Global>(name.ident, spec, init, pos);
        else
            return ast::make_decl<ast::GlobalMut>(name.ident, spec, init, pos);
    }
    else return nullptr;
}

ast::DeclPtr mu::Parser::parse_type_decl(ast::Ident *name) {
    auto token = current();
    switch(token.kind()) {
        case mu::Tkn_Struct:
            return parse_struct(name);
        case mu::Tkn_Type:
            return parse_type(name);
        case mu::Tkn_OpenParen:
        case mu::Tkn_OpenBrace:
            return parse_procedure(name);
        default:
            return nullptr;
    }
}

ast::DeclPtr mu::Parser::parse_procedure(ast::Ident *name) {
//    auto generics = parse_generic_group();

    return ast::DeclPtr();
}

ast::DeclPtr mu::Parser::parse_struct(ast::Ident *name) {
    passert(current().kind() == mu::Tkn_Struct);
    advance();

    auto pos = name->pos;
    std::vector<ast::SpecPtr> bounds;

    auto generics = parse_generic_group();
    pos.extend(generics->pos());

    if(allow(mu::Tkn_Less)) {
        bool error = false;
        many<ast::SpecPtr>(

                [this]() {
                    return parse_spec();
                },
                [this, &error]() {
                    return allow(mu::Tkn_Plus) and !error;
                },
                [&bounds, &pos, &error](std::vector<ast::SpecPtr>&, ast::SpecPtr res) {
                    if(res)
                        bounds.push_back(res);
                    else
                        error = true;
                    pos.extend(pos);
                }
        );
        if(error) {
            report(current().pos(), "expecting type, found: '%s'", current().get_string().c_str());
        }
    }

    remove_newlines();
    expect(mu::Tkn_OpenBracket);
    remove_newlines();

    // explicitly check for an empty block.
    std::vector<ast::DeclPtr> members;
    if(!check(mu::Tkn_CloseBracket)) {
        members = many<ast::DeclPtr>(
                [this]() {
                    return parse_member_variable();
                },
                [this]() {
                    bool val = allow(mu::Tkn_Comma);
                    remove_newlines();
                    return val and !check(mu::Tkn_CloseBracket);
                },
                [&pos](std::vector<ast::DeclPtr> &results, ast::DeclPtr res) {
                    Parser::append<ast::DeclPtr>(results, res);
                    pos.extend(res->pos());
                }
        );
    }

    expect(mu::Tkn_CloseBracket);

    return ast::make_decl<ast::Structure>(name, bounds, members, generics, pos);
}

void mu::Parser::remove_newlines() {
    if (this->check(mu::Tkn_NewLine))
        this->advance(true);
}

ast::DeclPtr mu::Parser::parse_type(ast::Ident *name) {
//    auto generics = parse_generic_group();
    return ast::DeclPtr();
}

ast::DeclPtr mu::Parser::parse_member_variable() {

    ast::SpecPtr type;
    std::vector<ast::ExprPtr> init;
    ast::Visibility vis = ast::Visibility::Private;

    auto pos = current().pos();
    pos.span = 0;

    if(allow(mu::Tkn_Pub))
        vis = ast::Visibility::Public;

    std::vector<ast::Ident*> names = many<ast::Ident*>(
            [this]() {
                auto [token, valid] = expect(mu::Tkn_Identifier);
                if(valid)
                    return token.ident;
                else
                    return (ast::Ident*) nullptr;
            },
            [this]() {
                return allow(mu::Tkn_Comma);
            },
            [&pos](std::vector<ast::Ident*>& results, ast::Ident* res) {
                Parser::append(results, res);
                pos.extend(res->pos);
            }
        );
    if(!check(mu::Tkn_Equal)) {
        type = parse_spec();
    }

    if(allow(mu::Tkn_Equal)) {
        init = many<ast::ExprPtr>(
                [this]() {
                    return parse_expr();
                },
                [this]() {
                    return check(mu::Tkn_Comma);
                },
                [&pos](std::vector<ast::ExprPtr>& results, ast::ExprPtr res) {
                    Parser::append(results, res);
                    pos.extend(res->pos());
                }
        );

    }
    return ast::make_decl<ast::MemberVariable>(names, type, init, vis, pos);
}

ast::DeclPtr mu::Parser::parse_generic_group() {
    passert(current().kind() == mu::Tkn_OpenBrace);
    auto pos = current().pos();
    advance();

    auto group = many<ast::DeclPtr>(
                [this]() {
                    return parse_generic_and_bounds();
                },
                [this]() {
                    return allow(mu::Tkn_Comma);
                },
                Parser::append<ast::DeclPtr>
            );

    for(auto& x : group)
        pos.extend(x->pos());
    pos.extend(current().pos());
    expect(mu::Tkn_CloseBrace);

    return ast::make_decl<ast::GenericGroup>(group, pos);
}

ast::DeclPtr mu::Parser::parse_generic() {
    if(check(mu::Tkn_Identifier)) {
        auto token = current();
        advance();
        return ast::make_decl<ast::Generic>(token.ident, token.pos());
    }
    else {
        report(current().pos(), "expecting an identifier, found: '%s'", current().get_string().c_str());
        return nullptr;
    }
}

ast::DeclPtr mu::Parser::parse_generic_and_bounds() {
    auto generic = parse_generic();
    if(!generic) {
        return nullptr;
    }
    std::vector<ast::SpecPtr> bounds;
    auto pos = generic->pos();
    bool error = false;

    // T < ...
    if(allow(mu::Tkn_Less)) {
        many<ast::SpecPtr>(
                [this]() {
                    return parse_spec();
                },
                [this]() {
                    return allow(mu::Tkn_Plus);
                },
                [&bounds, &pos, &error](std::vector<ast::SpecPtr>&, ast::SpecPtr res) {
                    if(res)
                        bounds.push_back(res);
                    else
                        error = true;
                    pos.extend(pos);
                }
            );
        if(error)
            return nullptr;

        // @TODO: Refactor this objects construction.
        auto name = std::static_pointer_cast<ast::Generic>(generic)->name;
        auto bounded_generic = ast::make_decl<ast::BoundedGeneric>(name, ast::GenericBounds(bounds, nullptr), pos);
        // we just constructed this object so we know this is safe to do.
        auto bounded_generic_ptr = static_cast<ast::BoundedGeneric*>(bounded_generic.get());
        bounded_generic_ptr->bounds.parent = bounded_generic_ptr;
        return bounded_generic;
    }
    else {
        return generic;
    }

}

ast::SpecPtr mu::Parser::parse_spec() {
    auto token = current();

    switch(token.kind()) {
        case mu::Tkn_Identifier: {
            auto name = token.ident;
            advance();
            if(check(mu::Tkn_OpenBrace)) {
                // parse generic name
                report(current().pos(), "generics are not implemented at this time");
            }
            return ast::make_spec<ast::Named>(name, name->pos);
        }
        case mu::Tkn_OpenParen: {

        } break;
        case mu::Tkn_OpenBrace: {
            advance();
            auto pos = token.pos();
            auto type = parse_spec();
            pos.extend(type->pos());

            if(allow(mu::Tkn_Semicolon)) {
                pos.span += 1;

                auto size = parse_expr();
                pos.extend(size->pos());

                pos.extend(current().pos());
                expect(mu::Tkn_CloseBrace);
                return ast::make_spec<ast::ListSpec>(type, size, pos);
            }
            else {
                pos.extend(current().pos());
                expect(mu::Tkn_CloseBrace);
                return ast::make_spec<ast::DynListSpec>(type, pos);
            }
        } break;
        case mu::Tkn_Astrick: {
            advance();
            auto pos = token.pos();
            auto type = parse_spec();
            pos.extend(type->pos());
            return ast::make_spec<ast::PtrSpec>(type, pos);
        } break;
        case mu::Tkn_Ampersand: {
            advance();
            auto pos = token.pos();
            auto type = parse_spec();
            pos.extend(type->pos());
            return ast::make_spec<ast::RefSpec>(type, pos);
        } break;
        case mu::Tkn_Mut: {
            advance();
            auto pos = token.pos();
            auto type = parse_spec();
            pos.extend(type->pos());
            return ast::make_spec<ast::MutSpec>(type, pos);
        } break;
    }
}
