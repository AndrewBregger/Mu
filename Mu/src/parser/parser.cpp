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
    push_restriction(Default);
    prev_res.push(Default);
}

void mu::Parser::push_restriction(mu::Restriction res) {
    restriction |= res;
    prev_res.push(res);
}

bool mu::Parser::check_restriction(mu::Restriction res) {
    return (restriction & res) == res;
}

void mu::Parser::pop_restriction() {
    if(prev_res.size() == 1) {
        restriction = 0;
    }
    else {
        restriction ^= prev_res.top();
        prev_res.pop();
    }
}

ast::ModuleFile *mu::Parser::process(io::File *file) {
    if(!scanner.init(file))
        return nullptr;
    advance();
    return parse_module();
}

void mu::Parser::passert(bool term) {
    if(!term) {
        assert(term);
    }
}

ast::ModuleFile * mu::Parser::parse_module() {

    auto pos = current().pos();
    pos.span = 0;

    auto decls = many<ast::DeclPtr>(
            [this]() {
                return parse_toplevel();
            },
            [this]() {
                remove_newlines();
                return !check(mu::Tkn_Eof);
            },
            Parser::append<ast::DeclPtr>
        );

    for(auto& d : decls)
        pos.extend(d->pos());

    auto file = interp->current_file();
    auto name = file->name();
    auto mname = interp->find_name(name);

    std::cout << "Num Decls: " << decls.size() << std::endl;
    return new ast::ModuleFile(new ast::Ident(mname, mu::Pos(0, 0, 0, file->id())), decls, pos);
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

    auto expr = parse_primary_expr();

    if(!expr)
        return expr;

    while(prec_min < current().prec()) {
        auto parser = grammar.get_infix(current());
        if(!parser) {
            report(current().pos(), "expecting binary operator found: '%s'", current().get_string().c_str());
            break;
        }
        expr = parser->lud(*this, expr, current());

        if(!expr)
            return expr;
    }

    return expr;
}

ast::ExprPtr mu::Parser::parse_primary_expr() {
    auto token = current();
    auto parser = grammar.get_prefix(token);
    if(!parser) {
        report(token.pos(), "unexpected token '%s'", token.get_string().c_str());
        return nullptr;
    }

    auto expr = parser->lud(*this, token);

    if(check(mu::Tkn_Period)) {
        auto parser = grammar.get_infix(current());
        expr = parser->lud(*this, expr, current());
    }

    return expr;
}

ast::ExprPtr mu::Parser::parse_call(ast::ExprPtr& name, mu::Token token, ast::ExprPtr operand) {
    advance();
    std::vector<ast::SpecPtr> type_parameters;
    std::vector<ast::ExprPtr> actuals;
    auto pos = name->pos();

    if(allow(mu::Tkn_OpenBrace)) {
        type_parameters = many<ast::SpecPtr>(
                    [this]() {
                        return parse_spec(false);
                    },
                    [this]() {
                        bool val = allow(mu::Tkn_Comma);
                        if(val and check(mu::Tkn_CloseBrace)) {
                            report(current().pos(), "expecting type following ','");
                            return false;
                        }
                        return val and !check(mu::Tkn_CloseBrace);
                    },
                    Parser::append<ast::SpecPtr>
                );
        expect(mu::Tkn_CloseBrace);
    }

    if(check(mu::Tkn_OpenParen)) {
        actuals = many<ast::ExprPtr>([this]() {
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
    }
    else {
        report(current().pos(), "expecting '('");
        return operand;
    }

    auto [_, valid] = expect(mu::Tkn_CloseParen);
    if(!valid)
        return nullptr;

    if(name) {
        auto p = operand->pos();
        p.extend(pos);
        return ast::make_expr<ast::Method>(operand, name, actuals, p);
    }
    else {
        auto p = operand->pos();
        p.extend(pos);
        return ast::make_expr<ast::Call>(operand, actuals, p);
    }
}

ast::ExprPtr mu::Parser::parse_expr_spec(bool is_spec) {
    auto token = current();
    ast::ExprPtr expr;
    if(check(mu::Tkn_Identifier))
        expr = parse_name();
    else
        expr = ast::make_expr<ast::Self>(current().pos());

    if(check(mu::Tkn_Period)) {
        auto parser = grammar.get_infix(current());
        expr = parser->lud(*this, expr, current());
    }
    return expr;
}

ast::ExprPtr mu::Parser::parse_name() {
    passert(current().kind() == mu::Tkn_Identifier);

    auto [name, valid] = expect(mu::Tkn_Identifier);

    if(valid) {
        auto pos = name.pos();
        std::vector<ast::SpecPtr> type_params;
        if(check(mu::Tkn_OpenBrace)) {
            pos.extend(current().pos());
            expect(mu::Tkn_OpenBrace);
            type_params = many<ast::SpecPtr>(
                    [this]() {
                        return parse_spec(false);
                    },
                    [this]() {
                        bool val = allow(mu::Tkn_Comma);
                        if(val and check(mu::Tkn_CloseBrace)) {
                            report(current().pos(), "expecting type following ',' found ']'");
                            return false;
                        }
                        return val;
                    },
                    Parser::append<ast::SpecPtr>
            );

            expect(mu::Tkn_CloseBrace);
            for(auto& p : type_params)
                pos.extend(p->pos());

            return ast::make_expr<ast::NameGeneric>(name.ident, type_params, pos);
        }
        else
            return ast::make_expr<ast::Name>(name.ident, name.pos());
    }
    return ast::ExprPtr();
}

ast::ExprPtr mu::Parser::parse_suffix(ast::ExprPtr& expr, bool is_spec) {
    advance();

    if(check(mu::Tkn_Identifier)) {
        auto name = current().ident;
        auto element = parse_name();

        if(check(mu::Tkn_OpenParen)) {
//            if(is_spec) {
//                report(expr->pos(), "unexpected '(' in type expression");
//            }
            return parse_call(element, current(), expr);
        }
        else {
//            if(!is_spec and expr->kind == ast::ast_name_generic) {
//                if(!check(mu::Tkn_OpenBracket))
//                    report(expr->pos(), "");
//            }

            auto pos = expr->pos();
            pos.extend(name->pos);
            return ast::make_expr<ast::Accessor>(expr, name, pos);
        }
    }
    else if(check(mu::Tkn_IntLiteral)) {
        if(is_spec) {
            report(current().pos(), "expecting identifier in type expression");
            return expr;
        }
        else {
            auto index = current();
            advance();

            auto pos = expr->pos();
            pos.extend(index.pos());
            return ast::make_expr<ast::TupleAcessor>(expr, index.integer, pos);
        }
    }
    else {
        report(current().pos(), "expecting identifier or an integer, found: '%s'", current().get_string().c_str());
        return expr;
    }
}

ast::StmtPtr mu::Parser::parse_stmt() {
    if(check(mu::Tkn_Let) or check(mu::Tkn_Mut) or check(mu::Tkn_At) or
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
    ast::AttributeList attrs({});
    if(check(mu::Tkn_At)) {
        attrs = parse_attributes();
        auto[_, valid] = expect(mu::Tkn_NewLine);
        if (!valid)
            return nullptr;
    }

    auto vis = parse_visability();

    switch(token.kind()) {
        case mu::Tkn_Identifier: {
            auto [name, _] = expect(mu::Tkn_Identifier);
            auto [token, valid] = expect(mu::Tkn_Colon);
            if(valid)
                decl = parse_type_decl(name.ident, attrs, vis);
            else {
                report(token.pos(), "expecting ':' following identifier in type declaration");
                break;
            }
        } break;
        case mu::Tkn_Use: {
            decl = parse_use(vis);
        } break;
        case mu::Tkn_Let:
        case mu::Tkn_Mut:
            if(toplevel)
                decl = parse_global(token.kind(), vis);
            else {
                if(vis == ast::Public) {
                    report<false>(current().pos(), "'let' and 'mut' following 'pub' is only allowed in global scope");
                }
                decl = parse_variable(token.kind());
            } break;
        default:
            report(token.pos(), "invalid declaration, expecting '@', 'let', 'mut', 'use', or identifier. Found '%s'",
                    token.get_string().c_str());
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
    auto pos = current().pos();
    advance();

    auto pattern = parse_pattern(false);
    pos.extend(pattern->pos());
    auto type = parse_spec(false);
    pos.extend(type->pos());

    ast::ExprPtr init;
    if(allow(mu::Tkn_Equal)) {
        init = parse_expr();
        passert(init.get());
        pos.extend(init->pos());
    }
    if(kind == mu::Tkn_Let) {
        return ast::make_decl<ast::Local>(pattern, type, init, pos);
    }
    else {
        return ast::make_decl<ast::Mutable>(pattern, type, init, pos);
    }
}

ast::DeclPtr mu::Parser::parse_global(mu::TokenKind kind, ast::Visibility vis) {
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
//        if(!check(mu::Tkn_Equal)) {
        spec = parse_spec(false);
        if(spec)
            pos.extend(spec->pos());
//        }

        // let x f32 = 1.0
        if(allow(mu::Tkn_Equal)) {
            init = parse_expr();
            if(init)
                pos.extend(init->pos());
        }
        if(kind == mu::Tkn_Let)
            return ast::make_decl<ast::Global>(name.ident, spec, init, vis, pos);
        else
            return ast::make_decl<ast::GlobalMut>(name.ident, spec, init, vis, pos);
    }
    else return nullptr;
}

ast::DeclPtr mu::Parser::parse_type_decl(ast::Ident *name, const ast::AttributeList &attributes, ast::Visibility vis) {
    auto token = current();
    switch(token.kind()) {
        case mu::Tkn_Struct:
            return parse_struct(name, vis);
        case mu::Tkn_Type:
            return parse_type(name, vis);
        case mu::Tkn_Impl:
            return parse_impl(name, vis);
        case mu::Tkn_Trait:
            return parse_trait(name, vis);
        case mu::Tkn_Alias: {
            advance(false);
            auto spec = parse_spec(false);
            auto pos = name->pos;
            pos.extend(token.pos());
            pos.extend(spec->pos());

            return ast::make_decl<ast::Alias>(name, spec, vis, pos);
        }
        default:
            return parse_procedure(name, attributes, vis);
    }
}

ast::DeclPtr mu::Parser::parse_trait(ast::Ident *name, ast::Visibility vis) {
    auto pos = name->pos;
    passert(current().kind() == mu::Tkn_Trait);
    pos.extend(current().pos());
    advance();

    ast::DeclPtr generics;
    if(check(mu::Tkn_OpenBrace)) {
        generics = parse_generic_group();
        pos.extend(generics->pos());
    }

    remove_newlines();
    expect(mu::Tkn_OpenBracket);
    remove_newlines();

    std::vector<ast::DeclPtr> members;
    if(!check(mu::Tkn_CloseBracket)) {
        members = many<ast::DeclPtr>(
                [this]() {
                    if (check(mu::Tkn_At)) {
                        report(current().pos(), "'@' function modifiers are not allowed on trait functions");
                        return ast::DeclPtr();
                    } else {
                        auto[token, valid] = expect(mu::Tkn_Identifier);
                        if (valid) {
                            expect(mu::Tkn_Colon);
                            if(allow(mu::Tkn_TypeLit)) {
                                ast::SpecPtr def;
                                auto pos = token.pos();
                                pos.span += 1;
                                if(check(mu::Tkn_Equal)) {
                                    def = parse_spec(false);
                                    pos.extend(def->pos());
                                }

                                return ast::make_decl<ast::TraitElementType>(token.ident, def, pos);
                            }
                            else {
                                return parse_procedure(token.ident, ast::AttributeList({}), ast::Public);
                            }
                        }
                    }
                },
                [this]() {
                    remove_newlines();
                    return !check(mu::Tkn_CloseBracket);
                },
                Parser::append<ast::DeclPtr>
        );
    }
    expect(mu::Tkn_CloseBracket);
    return ast::make_decl<ast::TypeClass>(name, members, generics, vis, pos);
}

ast::AttributeList mu::Parser::parse_attributes() {
    auto [at, valid] = expect(mu::Tkn_At);
    if(valid) {
        auto attributes = many<ast::Attribute>(
                    [this]() {
                        auto [token, val] = expect(mu::Tkn_Identifier);
                        if(!val) {
                            report(current().pos(), "expecting an identifier for the attribute name, found: '%s'",
                                    current().get_string().c_str());
                            return ast::Attribute(nullptr, "");
                        }
                        if(allow(mu::Tkn_OpenParen)) {
                            auto [value, exp] = expect(mu::Tkn_StringLiteral);
                            if(exp) {
                                return ast::Attribute(token.ident, value.str);
                            }
                            else {
                                report(current().pos(), "expecting a string as an attribute parameter, found: '%s'",
                                       current().get_string().c_str());
                                return ast::Attribute(token.ident, "");
                            }
                        }
                        else return ast::Attribute(token.ident, "");
                    },
                    [this]() {
                        return allow(mu::Tkn_Comma);
                    },
                    Parser::append_value<ast::Attribute>
                );
        expect(mu::Tkn_CloseParen);
        return ast::AttributeList(attributes);
    }
    else
        return ast::AttributeList({});
}

ast::DeclPtr mu::Parser::parse_procedure_parameter() {
    if(check(mu::Tkn_Self)) {
        auto self = current();
        advance();
        return ast::make_decl<ast::SelfParameter>(self.pos());
    }
    else {
        auto pattern = parse_pattern(false);
        auto pos = pattern->pos();
        auto type = parse_spec(false);
        pos.extend(type->pos());
        ast::ExprPtr init;

        if (allow(mu::Tkn_Equal)) {
            init = parse_expr();
            pos.extend(init->pos());
        }

        return ast::make_decl<ast::ProcedureParameter>(pattern, type, init, pos);
    }
}

std::shared_ptr<ast::ProcedureSigniture> mu::Parser::parse_procedure_signiture() {
    ast::DeclPtr generics;
    if(check(mu::Tkn_OpenBrace))
        generics = parse_generic_group();
    if(allow(mu::Tkn_OpenParen)) {
        std::vector<ast::DeclPtr> parameters;
        if(!check(mu::Tkn_CloseParen)) {
            parameters = many<ast::DeclPtr>(
                    [this]() {
                        return parse_procedure_parameter();
                    },
                    [this]() {
                        return allow(mu::Tkn_Comma);
                    },
                    Parser::append<ast::DeclPtr>
            );
        }
        expect(mu::Tkn_CloseParen);

        auto ret = parse_spec(false);
        return std::make_shared<ast::ProcedureSigniture>(parameters, ret, generics);
    }
    else {
        report(current().pos(), "expecting '(' or '[' in procedure declaration");
        return nullptr;
    }
}

ast::DeclPtr mu::Parser::parse_procedure(ast::Ident *name, const ast::AttributeList &attributes, ast::Visibility vis) {
    std::vector<ast::Modifier> modifiers;
    auto pos = name->pos;
    bool pub_added = false;
    bool stop = false;
    while(!stop) {
        switch(current().kind()) {
            case mu::Tkn_Inline:
                modifiers.push_back(ast::Mod_Inline);
                pos.extend(current().pos());
                advance();
                break;
            default:
                stop = true;
        }
    }

//    if(!pub_added)
//        modifiers.push_back(ast::Mod_Private);

    auto sig = parse_procedure_signiture();

    if(sig->generics)
        pos.extend(sig->generics->pos());
    pos.extend(sig->ret->pos());
    for(auto p : sig->parameters)
        pos.extend(p->pos());

    if(allow(mu::Tkn_Equal) || check(mu::Tkn_OpenBracket)) {
        auto body = parse_expr();
        pos.extend(body->pos());
        return ast::make_decl<ast::Procedure>(name, sig, body, attributes, modifiers, vis, pos);
    }
    else {
        ast::ExprPtr body;
        return ast::make_decl<ast::Procedure>(name, sig, body, attributes, modifiers, vis, pos);
    }
}

ast::DeclPtr mu::Parser::parse_struct(ast::Ident *name, ast::Visibility vis) {
    passert(current().kind() == mu::Tkn_Struct);
    advance();

    auto pos = name->pos;
    std::vector<ast::SpecPtr> bounds;

    ast::DeclPtr generics;

    if(check(mu::Tkn_OpenBrace)) {
        generics = parse_generic_group();
        // this could fail
        if(generics)
            pos.extend(generics->pos());
    }

    if(allow(mu::Tkn_Less)) {
        bool error = false;
        many<ast::SpecPtr>(

                [this]() {
                    return parse_spec(false);
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

    return ast::make_decl<ast::Structure>(name, bounds, members, generics, vis, pos);
}

void mu::Parser::remove_newlines() {
    if (check(mu::Tkn_NewLine))
        advance(true);
}

ast::DeclPtr mu::Parser::parse_type(ast::Ident *name, ast::Visibility vis) {
//    auto generics = parse_generic_group();
    passert(current().kind() == mu::Tkn_Type);
    advance();

    auto pos = name->pos;
    std::vector<ast::SpecPtr> bounds;
    ast::DeclPtr generics;
    if(check(mu::Tkn_OpenBrace)) {
        generics = parse_generic_group();
        pos.extend(generics->pos());
    }

    if(allow(mu::Tkn_Less)) {
        bool error = false;
        many<ast::SpecPtr>(
                [this]() {
                    return parse_spec(false);
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
                    return parse_type_member();
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

    return ast::make_decl<ast::Type>(name, bounds, members, generics, vis, pos);
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
        type = parse_spec(false);
    }

    if(allow(mu::Tkn_Equal)) {
        auto expr = parse_expr();
        if(expr->kind == ast::ast_assign) {
            report(expr->pos(), "cannot use an assignment as default initialization");
            return nullptr;
        }
        init.push_back(expr);
        // init = many<ast::ExprPtr>(
        //         [this]() {
        //             return parse_expr();
        //         },
        //         [this]() {
        //             return check(mu::Tkn_Comma);
        //         },
        //         [&pos](std::vector<ast::ExprPtr>& results, ast::ExprPtr res) {
        //             Parser::append(results, res);
        //             if(res)
        //                 pos.extend(res->pos());
        //         }
        // );

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
                    return parse_spec(false);
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

ast::PatternPtr mu::Parser::parse_pattern(bool bind_pattern) {

    auto pattern_parser = [this, &bind_pattern]() {
        auto many_patterns = [this](bool bind) {
            auto pos = current().pos();
            auto elements = many<ast::PatternPtr>(
                    [this, &bind]() {
                        return parse_pattern(bind);
                    },
                    [this]() {
                        return allow(mu::Tkn_Comma);
                    },
                    Parser::append<ast::PatternPtr>
            );

            for (auto &element : elements)
                pos.extend(element->pos());

            return std::make_pair(elements, pos);
        };


        auto token = current();
        switch (token.kind()) {
            case mu::Tkn_Underscore: {
                advance();
                return ast::make_pattern<ast::IgnorePattern>(token.pos());
            }
            case mu::Tkn_Minus: {
                advance();
                switch (current().kind()) {
                    case mu::Tkn_IntLiteral: {
                        auto p = ast::make_pattern<ast::IntPattern>(-((i64) current().integer), current().pos());
                        advance();
                        return p;
                    }
                    case mu::Tkn_FloatLiteral: {
                        auto p = ast::make_pattern<ast::FloatPattern>(-current().floating, current().pos());
                        advance();
                        return p;
                    }
                    default:
                        report(current().pos(), "expecting a float or integer following '-' in a pattern");
                        return ast::PatternPtr();
                }
            }
            case mu::Tkn_IntLiteral:
                advance();
                return ast::make_pattern<ast::IntPattern>(((i64) token.integer), token.pos());
            case mu::Tkn_FloatLiteral:
                advance();
                return ast::make_pattern<ast::FloatPattern>(token.floating, token.pos());
            case mu::Tkn_CharLiteral:
                advance();
                return ast::make_pattern<ast::CharPattern>(token.character, token.pos());
            case mu::Tkn_StringLiteral:
                advance();
                return ast::make_pattern<ast::StringPattern>(token.str, token.pos());
            case mu::Tkn_True:
            case mu::Tkn_False:
                advance();
                return ast::make_pattern<ast::BoolPattern>(token.kind() == mu::Tkn_True, token.pos());
            case mu::Tkn_OpenParen: {
                expect(mu::Tkn_OpenParen);
                auto[elements, pos] = many_patterns(false);
                expect(mu::Tkn_CloseParen);
                return ast::make_pattern<ast::TuplePattern>(elements, pos);
            }
            case mu::Tkn_OpenBrace: {
                expect(mu::Tkn_OpenParen);
                auto[elements, pos] = many_patterns(false);
                expect(mu::Tkn_CloseBrace);
                return ast::make_pattern<ast::ListPattern>(elements, pos);
            }
            case mu::Tkn_Identifier: {
                auto name = token.ident;
                ast::SpecPtr type;

                switch (peek().kind()) {
                    case mu::Tkn_Period:
                    case mu::Tkn_OpenBrace:
                    case mu::Tkn_OpenParen:
                    case mu::Tkn_OpenBracket: {
                        type = parse_spec(false);

                        // this must be ( or {
                        auto last = current();

                        remove_newlines();
                        // if it is neither the expected
                        if (last.kind() != mu::Tkn_OpenParen and
                            last.kind() != mu::Tkn_OpenBracket) {
                            report(last.pos(), "expecting '(', or '{'");
                            return ast::PatternPtr();
                        }
                        remove_newlines();

                        auto[elements, pos] = many_patterns(last.kind() == mu::Tkn_OpenBracket);

                        if (last.kind() == mu::Tkn_OpenBracket)
                            expect(mu::Tkn_CloseBracket);
                        else
                            expect(mu::Tkn_CloseParen);

                        auto p = type->pos();
                        pos = p.extend(pos);
                        if (last.kind() == mu::Tkn_OpenBracket)
                            return ast::make_pattern<ast::StructPattern>(type, elements, pos);
                        else
                            return ast::make_pattern<ast::TypePattern>(type, elements, pos);
                    }
                    default:
                        advance();

                        if (allow(mu::Tkn_Colon)) {
                            if (bind_pattern) {
                                auto p = parse_pattern();
                                auto pos = token.pos();
                                pos.extend(p->pos());
                                return ast::make_pattern<ast::BindPattern>(token.ident, p, pos);
                            } else {
                                report(current().pos(), "unexpected ':' in pattern");
                                return ast::PatternPtr();
                            }
                        }
                        return ast::make_pattern<ast::IdentPattern>(name, name->pos);
                }
            }
            default:
                report(token.pos(), "invalid pattern, expecting identifier, '[', '{'. Found: '%s'",
                       current().get_string().c_str());
        }
        return ast::PatternPtr();
    };

    auto pattern = pattern_parser();
    if(!pattern)
        return pattern;

    if(allow(mu::Tkn_PeriodPeriod)) {
        auto end = pattern_parser();
        if(!end)
            return end;

        auto pos = pattern->pos();
        pos.span += 1;
        pos.extend(end->pos());
        return ast::make_pattern<ast::RangePattern>(pattern, end, pos);
    }
    else
        return pattern;
}

ast::SpecPtr mu::Parser::parse_spec(bool allow_infer) {
    auto token = current();
    switch(token.kind()) {
        case mu::Tkn_TypeLit: {
            advance();
            return ast::make_spec<ast::TypeLitSpec>(token.pos());
        }
        case mu::Tkn_UnitName: {
            advance();
            return ast::make_spec<ast::UnitSpec>(token.pos());
        }
        case mu::Tkn_Identifier: {
            auto expr = parse_expr_spec(true);
            return ast::make_spec<ast::ExprSpec>(expr, expr->pos());
        }
        case mu::Tkn_OpenParen: {
            auto pos = token.pos();
            auto params = many<ast::SpecPtr>(
                    [this]() {
                        return parse_spec(false);
                    },
                    [this] {
                        bool val = allow(mu::Tkn_Comma);
                        if(val and check(mu::Tkn_CloseParen)) {
                            report(current().pos(), "expecting type following comma");
                            return false;
                        }
                        return val || !check(mu::Tkn_CloseParen);
                    },
                    Parser::append<ast::SpecPtr>
            );
            for(auto& p : params)
                if(p)
                    pos.extend(p->pos());
            expect(mu::Tkn_CloseParen);
            pos.span++;

            if(allow(mu::Tkn_Arrow)) {
                auto ret = parse_spec(false);
                pos.extend(ret->pos());

                return ast::make_spec<ast::ProcedureSpec>(params, ret, pos);
            }
            else return ast::make_spec<ast::TupleSpec>(params, pos);
        } break;
        case mu::Tkn_OpenBrace: {
            advance();
            auto pos = token.pos();
            auto type = parse_spec(false);
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
            auto type = parse_spec(false);
            pos.extend(type->pos());
            return ast::make_spec<ast::PtrSpec>(type, pos);
        } break;
        case mu::Tkn_Ampersand: {
            advance();
            auto pos = token.pos();
            auto type = parse_spec(false);
            pos.extend(type->pos());
            return ast::make_spec<ast::RefSpec>(type, pos);
        } break;
        case mu::Tkn_Mut: {
            advance();
            auto pos = token.pos();
            auto type = parse_spec(false);
            pos.extend(type->pos());
            return ast::make_spec<ast::MutSpec>(type, pos);
        } break;
        case mu::Tkn_SelfType: {
            advance();
            return ast::make_spec<ast::SelfSpec>(token.pos());
        } break;
        default:
            return ast::make_spec<ast::InferSpec>(current().pos());
    }
}

ast::DeclPtr mu::Parser::parse_impl(ast::Ident *name, ast::Visibility vis) {
    auto pos = name->pos;
    pos.extend(current().pos());
    advance();

    ast::DeclPtr generics;
    if(check(mu::Tkn_OpenBrace)) {
        generics = parse_generic_group();
        pos.extend(generics->pos());
    }


    remove_newlines();
    auto [_, valid] = expect(mu::Tkn_OpenBracket);
    remove_newlines();

    auto members = many<ast::DeclPtr>(
            [this]() {
                ast::AttributeList attributes({});
                if(check(mu::Tkn_At)) {
                    attributes = parse_attributes();
                    expect(mu::Tkn_NewLine);
                }
                //
                auto vis = parse_visability();
                auto [token, valid] = expect(mu::Tkn_Identifier);
                expect(mu::Tkn_Colon);
                if(valid) {
                    return parse_procedure(token.ident, attributes, vis);
                }
                else {
                    ast::DeclPtr ret;
                    report(current().pos(), "expecting identifier, found: '%s'", current().get_string().c_str());
                    return ret;
                }
            },
            [this]() {
                remove_newlines();
                return !check(mu::Tkn_CloseBracket);
            },
            Parser::append<ast::DeclPtr>
            );
    for(auto& m : members)
        if(m)
            pos.extend(m->pos());
    expect(mu::Tkn_CloseBracket);
    pos.span += 1;

    return ast::make_decl<ast::Impl>(name, members, generics, pos);
}

ast::DeclPtr mu::Parser::parse_type_member() {
    auto [token, valid] = expect(mu::Tkn_Identifier);
    if(valid) {
        auto pos = token.pos();
        if(allow(mu::Tkn_OpenParen)) {
            auto members = many<ast::SpecPtr>(
                        [this]() {
                            return parse_spec(false);
                        },
                        [this]() {
                            return allow(mu::Tkn_Comma);
                        },
                        Parser::append<ast::SpecPtr>
                    );
            for(auto& m : members)
                if(m)
                    pos.extend(m->pos());
            expect(mu::Tkn_CloseParen);
            return ast::make_decl<ast::TypeMember>(token.ident, members, pos);
        }
    }
    return ast::DeclPtr();
}

ast::Visibility mu::Parser::parse_visability() {
    if(allow(mu::Tkn_Pub))
        return ast::Public;
    else
        return ast::Private;
}

bool mu::Parser::sync_after_error() {
    while(!check(mu::Tkn_NewLine)) {
        switch(current().kind()) {
            case mu::Tkn_Let:
            case mu::Tkn_Mut:
            case mu::Tkn_Use:
            case mu::Tkn_Eof:
                return true;
            default:
                advance(true);
        }
    }
    return true;
}

ast::DeclPtr mu::Parser::parse_usepath() {
    auto parse_path = [this]() {
        ast::SPath path;
        bool expect_follow = false;
        do {
            auto token = current();
            if(allow(mu::Tkn_Identifier)) {
                path.push_back(token.ident);
            } else if(check(mu::Tkn_OpenBracket)) {
                expect_follow = true;
                break;
            }
            else if(check(mu::Tkn_Astrick)) {
                expect_follow = true;
                break;
            }
            else {
                report(token.pos(), "unexpected '%s' in path", token.get_string().c_str());
                return std::make_pair(ast::SPath(), expect_follow);
            }
        } while(allow(mu::Tkn_Period));
        return std::make_pair(path, expect_follow);
    };

    auto [path, expect_follow] = parse_path();
    if(path.empty())
        return ast::DeclPtr();

    auto pos = path.front()->pos;
    pos.span = 0;

    for(auto& p : path)
        pos.extend(p->pos);

    // this handles the span of the periods.
    pos.span += (path.size() - 1);

    if(expect_follow) {
        auto token = current();
        advance();
        switch (token.kind()) {
            case mu::Tkn_OpenBracket: {
                pos.span += 1;
                auto elements = many<ast::DeclPtr>(
                        [this]() {
                            return parse_usepath();
                        },
                        [this, &pos]() {
                            remove_newlines();
                            bool val = allow(mu::Tkn_Comma);
                            remove_newlines();
                            if(val)
                                pos.span += 1;
                            return val and !check(mu::Tkn_CloseBracket);
                        },
                        Parser::append<ast::DeclPtr>
                );
                expect(mu::Tkn_CloseBracket);

                for(auto& e : elements)
                    pos.extend(e->pos());
                pos.span += 1;

                return ast::make_decl<ast::UsePathList>(path, elements, pos);
            }
            case mu::Tkn_Astrick: {
                pos.span += 1;
                return ast::make_decl<ast::UsePath>(path, true, pos);
            }
            case mu::Tkn_Colon: {
                auto [name, valid] = expect(mu::Tkn_Identifier);

                if(valid) {
                    pos.extend(name.pos());
                    pos.span += 1;
                    return ast::make_decl<ast::UsePathAlias>(path, name.ident, pos);
                }
            }
            default:
                break;
        }
    }
    else {
        return ast::make_decl<ast::UsePath>(path, false, pos);
    }
}

ast::DeclPtr mu::Parser::parse_use(ast::Visibility vis) {
    expect(mu::Tkn_Use);

    auto path = parse_usepath();

    return ast::make_decl<ast::Use>(path, vis, path->pos());
}
