//
// Created by Andrew Bregger on 2019-06-07.
//
#include "interpreter.hpp"

#include "typer.hpp"

#include "typer_op_eval.hpp"


#define report(pos, msg, ...) { \
    interp->report_error(pos, msg, __VA_ARGS__); \
    increment_error(); \
}

#define report_str(pos, msg) { \
    interp->report_error(pos, msg); \
    increment_error(); \
}

// these are not well defined. they owuld only be used
// once in a single scope.
#define push_context_state(var, new_state) \
    auto _old_##var = context.var; \
    context.var = new_state;


#define pop_context_state(var) context.var = _old_##var;

u64 next_multiple(u64 old, u64 multi_of) {
    return old + multi_of - (old % multi_of);
}



extern mu::types::Type* type_u8;
extern mu::types::Type* type_u16;
extern mu::types::Type* type_u32;
extern mu::types::Type* type_u64;
extern mu::types::Type* type_i8;
extern mu::types::Type* type_i16;
extern mu::types::Type* type_i32;
extern mu::types::Type* type_i64;
extern mu::types::Type* type_f32;
extern mu::types::Type* type_f64;
extern mu::types::Type* type_char;
extern mu::types::Type* type_bool;
extern mu::types::Type* type_unit;

// this is not implemented yet
extern mu::types::Type* type_string;

namespace mu {


    Typer::Typer(Interpreter *interp) : interp(interp), prelude(interp->get_prelude()), renderer(true, interp->out_stream()) {
    }

    void Typer::increment_error() {
        errors_num++;
    }

    ScopePtr Typer::active_scope() {
        return context.current_scope;
    }

    AddressType Typer::get_addressing_by_type(types::Type* type) {
		if(!type) return AddressType::Value;
        if(type->is_array() || type->is_ptr() || type->is_ref() || type->is_function()) 
            return AddressType::Reference;
        else
            return AddressType::Value;
    }

    bool Typer::is_redeclaration(ast::Ident* name) {
        auto [_, valid] = active_scope()->find(name);
        return valid;
    }

    bool Typer::compatible_types(types::Type *expected, types::Type *given, bool casting) {
        if(!casting and interp->equivalent_types(expected, given)) {
            return true;
        }
        else {
            // check for compatibility.
            return false;
        }
    }

    Entity* Typer::search_active_scope(ast::Ident* name) {
        auto e = search_scope(active_scope(), name);
        if(e) {
            return e;
        }
        else {
            e = search_scope(prelude, name);
            if(e)
                return e;

            report(name->pos, "use of undeclared identifier '%s'", name->val->value.c_str())
            return nullptr;
        }
    }

    Entity* Typer::search_scope(ScopePtr  scope, ast::Ident* name) {
        auto curr = scope;

        while(curr) {
            auto [entity, valid] = curr->find(name);
            if(valid) {
                return entity;
            }
            curr = curr->get_parent();
        }

        return nullptr;
    }

    Local* Typer::new_padding(const std::string& name, u32 size) {
        auto padding_atom = interp->find_name(name);
        auto padding_type = interp->checked_new_type<types::Array>(type_u8, size);
        auto entity = interp->new_entity<mu::Local>(new ast::Ident(padding_atom, mu::Pos((u64) 0, (u64) 0, (u64) 0, interp->current_file()->id())), padding_type,
            Reference, active_scope(), context.active_entity->get_decl());

        entity->resolve_to(padding_type);
        entity->set_member(size);

        return entity;
    }

    Module * Typer::resolve_main_module(ast::ModuleFile *main_module) {
        // this will become the executable name.
        auto name = main_module->get_name();

        auto module_scope = make_scope<ModuleScope>(name, main_module, active_scope());

        push_scope(module_scope);


        std::vector<Entity*> entities;
        std::vector<ast::DeclPtr> impl_blocks;
        for(auto& decl : *main_module) {
            if(decl->kind == ast::ast_impl)
                impl_blocks.push_back(decl);
            else {
                auto entity = build_top_level_entity(decl);
                add_entity(entity);
                entities.emplace_back(entity);
            }
        }

        for(auto& block : impl_blocks) {
            auto name = block->as<ast::Impl>()->name;
            auto type_entity = search_active_scope(name);

            if(type_entity->is_type()) {
                auto type = type_entity->as<Type>();
                type->add_impl(block);
            }
            else {
                report(name->pos, "'%s' doesn't return to a type", name->value().c_str());
                // there is an error, no point in continuing.
                return nullptr;
            }
        }

        for(auto entity : entities) {
            auto e = resolve_entity(entity);
    
            if(e) e->debug_print(interp->out_stream());
        }

        pop_scope();

//        auto main_module_type = interp->new_type<ModuleType>();

        return nullptr;
    }

    void Typer::add_entity(Entity *entity) {
        active_scope()->insert(entity->get_name(), entity);
    }

    void Typer::push_scope(ScopePtr scope) {
        if(active_scope()) active_scope()->add_child(scope);
        if(scope->get_parent() != active_scope())
            interp->fatal("Compiler Error: Pushing a scope where the parent isn't the active scope");

        context.current_scope = scope;
    }

    void Typer::pop_scope() {
        context.current_scope = active_scope()->get_parent(); 
    }

    Entity *Typer::build_top_level_entity(ast::DeclPtr decl) {
        switch(decl->kind) {
            case ast::ast_global: {
                auto g = decl->as<ast::Global>();
                return interp->new_entity<Global>(g->name, active_scope(), decl);
            }
            case ast::ast_global_mut: {
                auto g = decl->as<ast::GlobalMut>();
               return interp->new_entity<Global>(g->name, active_scope(), decl);
            } break;
            case ast::ast_procedure: {
                auto p = decl->as<ast::Procedure>();
                return interp->new_entity<Function>(p->name, active_scope(), decl);
            } break;
            case ast::ast_structure: {
                auto s = decl->as<ast::Structure>();
                return interp->new_entity<Type>(s->name, active_scope(), decl);
            } break;
            case ast::ast_type: {
                auto s = decl->as<ast::Type>();
                return interp->new_entity<Type>(s->name, active_scope(), decl);
            } break;
            case ast::ast_type_class: {
                auto s = decl->as<ast::TypeClass>();
                return interp->new_entity<Type>(s->name, active_scope(), decl);
            } break;
            case ast::ast_use: {
                auto s = decl->as<ast::Use>();
                ast::Ident* name = nullptr;
                switch(s->use_path->kind) {
                    case ast::ast_use_path:
                    case ast::ast_use_path_list:
                    case ast::ast_use_path_alias:
                    default:
                        report_str(s->use_path->pos(), "Compiler Error: parser bug in use declaration");
                }
                interp->fatal("Use are not implemented at this moment");
                return interp->new_entity<Module>(name, active_scope(), decl);
            } break;
            case ast::ast_alias: {
                auto a = decl->as<ast::Alias>();
                return interp->new_entity<Alias>(a->name, (types::Type*) nullptr, active_scope(), decl);
            } break;
            case ast::ast_impl:  {
                // think about how this should be handled.
                // maybe what I can do is search for the struct this impl block is associated with and
                // attach it so that it is resolved when the structure is.
            } break;
            default:
                report_str(decl->pos(), "invalid global declaration");
                return nullptr;
        }
    }

    Entity *Typer::resolve_entity(Entity *entity) {
        interp->message("Resolving: %s", entity->str().c_str());
        if(entity->is_resolved()) {
            interp->message("\tAlready Resolved: Returning");
            return entity;
        }

        // If an entity is resolved while it is
        // actively being resolved.
        if(entity->status() == Resolving) {
            report(entity->node()->pos(), "'%s' has ciclic dependency", entity->str().c_str());

            // return null if this happens.
            return nullptr;
        }

        entity->update_status(Resolving);
        context.active_entity = entity;

        auto e = entity->resolve(this);

        if(e != entity)
            interp->message("The Entity was changed during resolution");

        context.active_entity = nullptr;
        return e;
    }

    Entity *Typer::resolve(Global *global) {
        
        ast::Ident* name{nullptr};
        ast::Spec* spec{nullptr};
        ast::Expr* init{nullptr};

        if(global->is_mutable()) {
            auto declaration = global->get_decl_as<ast::GlobalMut>();

            name = declaration->name;
            spec = declaration->type.get();
            init = declaration->init.get();

            if(!spec and !init) {
                report(name->pos, "global '%s' must have a type or be initialized", name->value().c_str());
            }
        }
        else {
            auto declaration = global->get_decl_as<ast::Global>();

            name = declaration->name;
            spec = declaration->type.get();
            init = declaration->init.get();

            if(!init) {
                report(name->pos, "constant global '%s' must be initialized", name->value().c_str());
                return nullptr;
            }
        }

        types::Type* resolved_type{nullptr};
        Operand expr_type(init);

        if(spec)
            resolved_type = resolve_spec(spec);

        if(init) {
            expr_type = resolve_expr(init, resolved_type);
            if(expr_type.error) {
                return nullptr;
            }
        }

        if(resolved_type and expr_type.type) {
            interp->message("Global Types: Annotation: %s, Expression: %s",
                            resolved_type->str().c_str(),
                            expr_type.type->str().c_str());
        }

        if(!resolved_type)
            resolved_type = expr_type.type;

        if(!resolved_type)
            return nullptr;


        interp->message("Global resoved type: %s", resolved_type->str().c_str());

        // if the value is mutable the valarable should be converted to
        // a constant
        if(expr_type.val.is_constant and !global->is_mutable()) {
            interp->message("Global is a constant, is tranformed into a constant entity");
            // convert this entity to a constant.
            // interp->remove_entity(global);

            auto constant = interp->new_entity<Constant>(name, resolved_type, expr_type.val,
                                                         active_scope(), global->decl_ptr());

            constant->resolve_to(resolved_type);
            
            active_scope()->replace_name(constant->get_name(), constant);
            return constant;
        }
        else {
            if(init) {
                global->set_initialized();
            }

            global->set_addressing(get_addressing_by_type(resolved_type));
            global->resolve_to(resolved_type);
            return global;
        }
    }

    Entity *Typer::resolve(Local *local) {
        return local;
    }

    Entity *Typer::resolve(Type *type) {
        interp->message("Resolving %s", type->str().c_str());

        switch(type->get_decl()->kind) {
            case ast::ast_structure:
                return resolve_struct(type, type->get_decl());
            case ast::ast_type:
                return resolve_sumtype(type, type->get_decl());
            case ast::ast_type_class:
                return resolve_trait(type, type->get_decl());
            default:
                return nullptr;
        }
    }

    // the generics will be resolved before. There this would be a child scope of the const block containing the generic names
    // module_scope/function_scope -> const_block -> param_scope -- this is for polymorphic functions.
    // module_scope/function_scope -> param_scope -- this is for concrete functions.
    std::tuple<std::vector<Local *>, bool> Typer::resolve_function_members(ast::ProcedureSigniture *sig) {
        std::vector<Local*> params;

        assert(active_scope()->kind() == Parameter_Scope);

        bool is_first = true;
        bool is_error = false;
        for(auto param : sig->parameters) {
            switch(param->kind) {
                case ast::ast_procedure_parameter: {
                    auto p = param->as<ast::ProcedureParameter>();
                    if(p->pattern->kind == ast::ast_ident_pattern) {
                        auto name = p->pattern->as<ast::IdentPattern>();
                        types::Type* type = nullptr;
                        Operand operand(p->init.get());

                        if(is_redeclaration(name->name)) {
                            report(name->pos(), "parameter '%s' is redeclared in this scope", name->name->value().c_str());
                            is_error = true;
                        }

                        if(p->type)
                            type = resolve_spec(p->type.get());

                        if(p->init) {
                            operand = resolve_expr(p->init.get(), type);
                        }

                        // this is an actual error.
                        if(operand.type and operand.error) {
                            return std::make_tuple(params, false);
                        }

                        if(p->type->kind == ast::ast_infer_type and !operand.type) {
                            report_str(p->pos(), "infer types are not implemented at this time");
                            return std::make_tuple(params, false);
                        }

                        if(p->type->kind == ast::ast_infer_type) {
                            type = operand.type;
                        }

                        auto local = interp->new_entity<Local>(name->name, type, get_addressing_by_type(type), active_scope(), param);

                        add_entity(local);

                        // if there is an initialize then there is a default value
                        if(!operand.error) {
                            local->set_initialized();
                        }

                        if(type->is_mutable())
                            local->set_mutable();

                        // this needed but it is resetting the type.
                        local->set_parameter();
                        local->resolve_to(type);


                        params.push_back(local);
                    }
                    else {
                        report_str(p->pos(), "invalid pattern as function parameter, only identifiers are allowed right now");
                        return std::make_tuple(params, false);
                    }
                } break;
                case ast::ast_self_parameter: {
                    // I havent thought about mutability in method calls
                    // possibly add mut self and self as possible parameters
                    auto p = param->as<ast::SelfParameter>();
                    if(!is_first) {
                        report_str(param->pos(), "self must be the first parameter");
                        return std::make_tuple(params, false);
                    }
                    if(!context.impl_block_entity) {
                        report_str(param->pos(), "unable to resolve the self parameter");
                        interp->message("This is primarily due function not being in an impl block");
                        return std::make_tuple(params, false);
                    }
                    types::Type* mut = nullptr;
                    if(p->mut)
                        mut = interp->checked_new_type<types::Mutable>(context.impl_block_entity->get_type());
                    else
                        mut = context.impl_block_entity->get_type();

                    auto type = interp->checked_new_type<types::Reference>(mut);
                    auto ident = new ast::Ident(interp->find_name("self"), p->pos());
                    auto local = interp->new_entity<Local>(ident, nullptr, Reference, active_scope(), param);
                    local->resolve_to(type);
                    local->set_parameter();
                    local->set_initialized();
                    local->set_self();
                    add_entity(local);
                    params.push_back(local);
                } break;
                // because of how the parser works for the variadic parameters
                // the name pattern must be an identifier.
                case ast::ast_c_variadic: {
                    auto cvar = param->as<ast::CVariadicParameter>();
                    auto name =  cvar->pattern->as<ast::IdentPattern>();

                    // it is set to unit type so there isnt a seg fault later on.
                    auto local = interp->new_entity<Local>(name->name, type_unit, AddressType::Value, active_scope(), param);
                    local->resolve_to(type_unit);

                    add_entity(local);
                    // by setting it to c-variadic we know to ignore the unit type
                    local->set_cvariadic();
                    local->set_parameter();

                    params.push_back(local);
                } break;
                case ast::ast_variadic: {
                    auto var = param->as<ast::VariadicParameter>();
                    auto name =  var->pattern->as<ast::IdentPattern>();

                    auto type = resolve_spec(var->type.get());

                    if(type == nullptr and var->type->kind == ast::ast_infer_type) {
                        report_str(var->type->pos(), "unable to infer typed variadic parameters (at this time)");
                        is_error = true;
                        break;
                    }
                    else if(!type) {
                        is_error = true;
                        break;
                    }

                    auto local = interp->new_entity<Local>(name->name, type, AddressType::Value, active_scope(), param);
                    local->resolve_to(type);

                    add_entity(local);
                    local->set_typevariadic();
                    local->set_parameter();

                    params.push_back(local);
                } break;
                default:
                    interp->fatal("Invalid ast");
                    break;

            }
            is_first = false;
        }

        return std::make_tuple(params, !is_error);
    }

    Entity *Typer::resolve(Function *funct) {
        if(context.impl_block_entity) {
          interp->out_stream() << "Resolving method for: " << context.impl_block_entity->get_name()->value() << std::endl;
          interp->out_stream() << "\tMethod Name: " << funct->get_name()->value() << std::endl;
        }

        auto function_decl = funct->get_decl_as<ast::Procedure>();
        ast::Ident *foreign_name{nullptr};

        for (auto &attr : function_decl->attributeList.attributes) {
            if (attr.attr->val == interp->find_name("foreign")) {
                funct->set_foreign(attr.value);
                foreign_name = attr.attr;
            }
        }

        auto params_scope = make_scope<ParameterScope>(function_decl, active_scope());
        push_scope(params_scope);

        auto [params, valid] = resolve_function_members(function_decl->signiture.get());
        std::vector<types::Type*> param_types;
        if(!valid) {
            // the error has already been reported.
            interp->message("There was an error processing members");
            for(auto p : params)
              p->debug_print(interp->out_stream());
            return nullptr;
        }


        for(auto e : params) {
            if(e->is_cvariadic() || e->is_typevariadic() || e->is_polyvariadic())
              funct->set_variadic();

            param_types.emplace_back(e->get_type());
        }



        funct->set_param_info(params, params_scope);

        types::Type* ret_type{nullptr};
        Operand expr_ret(function_decl->body.get());

        if(function_decl->signiture->ret) {
            auto ret = function_decl->signiture->ret;
            if(ret->kind == ast::ast_infer_type and context.resolving_trait_body) {
                report_str(ret->pos(), "unable to infer the return type of a trait function");
                return nullptr;
            }
            else if(ret->kind == ast::ast_infer_type and funct->is_foreign()) {
                report_str(ret->pos(), "unable to infer the return type of a foreign function");
                return nullptr;
            }
            else
                ret_type = resolve_spec(ret.get());
        }

        if(function_decl->body) {
            if(funct->is_foreign()) {
                report_str(foreign_name->pos, "function is foreign and has a body");
                return nullptr;
            }
            else {
                push_context_state(function_body, true)
                expr_ret = resolve_expr(function_decl->body.get(), ret_type);
                pop_context_state(function_body)

                if(expr_ret.error) {
                    return nullptr;
                }
            }
        }
        else {
            funct->set_no_body();
            if(!funct->is_foreign() and !context.resolving_trait_body) {
                report_str(function_decl->pos(), "function is missing a body. Not marked as foreign nor a trait function");
                return nullptr;
            }
            else {
                // are there any special cases in regards to foreign functions and trait functions?
                auto type = interp->checked_new_type<types::FunctionType>(param_types, ret_type);
                funct->resolve_to(type);
                return funct;
            }
        }


        pop_scope();

        if(!ret_type)
            ret_type = expr_ret.type;

        for(auto p : params) {
            if(!p->is_used()) {
              // maybe refactor this to a warning interp call
              interp->print_file_pos(p->get_decl()->pos());
              interp->message("parameter '%s' is not used", p->get_name()->value().c_str());
              interp->print_file_section(p->get_decl()->pos());
            }
        }
    
        // if we are resolving an impl block
        if(context.impl_block_entity) {
          if(params.size() >= 1)
            // since the first parameter is a self, then this is a method.
            if(params.front()->is_self())
              funct->set_method();
          // since this is a impl block function and the first parameter is not a self
          // this is a static function
          if(!funct->is_method())
            funct->set_static();
        }

        auto type = interp->checked_new_type<types::FunctionType>(param_types, ret_type);
        funct->resolve_to(type);
        return funct;
    }

    Entity *Typer::resolve(Alias *alias) {
        return alias;
    }

    Entity *Typer::resolve(Constant *constant) {
        return constant;
    }

    std::tuple<u64, u64, std::vector<ast::Ident*>> Typer::construct_member_order(std::vector<Entity*>& member) {
        u64 size = 0;
        if(member.empty()) {
            return std::make_tuple(0, 0, std::vector<ast::Ident*>({}));
        }
        std::vector<ast::Ident*> order;

        u32 num_paddings = 0;
        u64 largest_align = 0;

        std::vector<Entity*> entity_order;

        for(auto e : member) {
            // ignoring anything but locals.
            if(!e->is_local())
                continue;

            auto type = e->get_type();

            // this computes the needed padding
            auto align = type->alignment();

            if(align > largest_align)
                largest_align = align;

            auto padding = (align - (size % align)) % align;

            interp->out_stream() << "Padding: " << padding << " Align: " << align << std::endl;

            if(padding > 0) {
                // add padding
                std::string s = "__pad" + std::to_string(num_paddings++);
                auto pad = new_padding(s, padding);
//                entity_order.emplace_back(pad);
                size += pad->get_type()->size();

            } 

            e->as<Local>()->set_member(size);
            entity_order.emplace_back(e);

            size += type->size();
        }

        auto alignment = largest_align;


        // size is not a multple of the largest align. padding needs to be added.
        if(size % largest_align != 0) {
            auto new_size = next_multiple(size, largest_align);
            auto new_padding = new_size - size;

            auto pad = this->new_padding("__pad" + std::to_string(num_paddings), new_padding);
            pad->set_member(size);

//            entity_order.emplace_back(pad);

            size = new_size;
        }        

        member = entity_order; 

        interp->out_stream() << "Struct Size: " << size << " Alignment: " << alignment << std::endl;

        return std::make_tuple(alignment, size, order);
    }

    Entity* Typer::resolve_struct(Type* entity, ast::DeclPtr decl_ptr) {
        auto decl = decl_ptr->as<ast::Structure>();
        // I will duplicate code to reduce complexity.
        if(decl->generics)
            return resolve_poly_struct(entity, decl_ptr);

        // creates the member scope for the struct.
        auto member_scope = mu::make_scope<mu::MemberScope>(entity->get_name(), decl, active_scope());        

        push_scope(member_scope);

        // maybe the self type is added at the beginning or it could be added
        // at first use.
//        ast::Ident self_type = ast::Ident(interp->find_name("Self"), mu::Pos());
//        member_scope->insert(&self_type, entity);

        std::vector<Entity*> members;

        for(auto local : decl->members) {
            switch(local->kind) {
                case ast::ast_member_variable: {
                    auto [entities, valid] = resolve_member_variable(local);
                    // there was an error resolving the members
					if(!valid) {
						interp->message("There was an error with member");
						return nullptr;
					}
                    for(auto e : entities)
                        members.emplace_back(e);
                    break;
                }
                default:
                    break;
            }
        }

        // @TODO: Right now, struct padding is explicitly represented. This could be simplified
        // to having an implied padding by only setting the offset.
        auto [alignment, size, order] = construct_member_order(members);

        auto type = interp->checked_new_type<types::StructType>(entity->get_name(),
            members, member_scope, size, entity, alignment);


        interp->message("Structure Resolution");
        for(auto& entity : members) {
            auto local = entity->as<Local>();
            interp->message("%s -> %s | %lu", entity->get_name()->value().c_str(), entity->get_type()->str().c_str(), local->get_offset());
        }

        entity->resolve_to(type);
		interp->message("Resolving Impl Blocks");

        if(resolve_impl_blocks(entity)) {
			pop_scope();
			interp->message("Finished resolving %s", entity->get_name()->value().c_str());
			return entity;
		}
		else {
			pop_scope();
			return nullptr;
		}
    }

    Entity* Typer::resolve_poly_struct(Type* entity, ast::DeclPtr decl_ptr) {
        return entity;
    }

    bool Typer::resolve_impl_blocks(Type *entity) {
        push_context_state(impl_block_entity, entity)

        std::vector<Function*> function_entities;
		
		interp->message("Collecting methods");
        for(auto block : entity->get_impls()) {
            assert(block->kind == ast::ast_impl);

            auto impl = block->as<ast::Impl>();

            // add all functions from the impl blocks into scope of the type.
            for(auto decl : impl->methods) {
                switch(decl->kind) {
                    case ast::ast_procedure: {
                        auto proc = decl->as<ast::Procedure>();
                        auto function = interp->new_entity<Function>(proc->name, active_scope(), decl);
                        function_entities.push_back(function);
                        if(is_redeclaration(function->get_name())) {
                            report(decl->pos(), "redefinition of name '%s' in '%s' member scope",
                                    proc->name->value().c_str(),
                                    entity->get_name()->value().c_str());
                            return false;
                        }
                        else {
							interp->message("Adding %s", function->get_name()->value().c_str());
                            add_entity(function);
						}
                    } break;
                    case ast::ast_trait_element_type:
                        // i do not know how to implement this yet.
                        // break;
                    default:
                        interp->fatal("Compiler Error: Invalid decl in impl block");
                        break;
                }
            }
        }
			
		interp->message("Resolving method entities");
        for(auto fn : function_entities) {
            auto e = resolve_entity(fn);
            if(e) e->debug_print(interp->out_stream()); 
            else interp->out_stream() << "There was an error resolving function" << std::endl;
        }


        pop_context_state(impl_block_entity)
		return true;
    }

//    Entity* Typer::resolve_function(Type* entity, ast::DeclPtr decl_ptr) {
//        return entity;
//    }
//
//    Entity* Typer::resolve_poly_function(Type* entity, ast::DeclPtr decl_ptr) {
//
//        return entity;
//    }

    Entity* Typer::resolve_sumtype(Type* entity, ast::DeclPtr decl_ptr) {

        return entity;
    }

    Entity* Typer::resolve_poly_sumtype(Type* entity, ast::DeclPtr decl_ptr) {

        return entity;
    }

    Entity* Typer::resolve_trait(Type* entity, ast::DeclPtr decl_ptr) {
        return entity;
    }

    Entity* Typer::resolve_poly_trait(Type* entity, ast::DeclPtr decl_ptr) {
        return entity;
    }

	std::tuple<std::vector<Local*>, bool> Typer::resolve_member_variable(ast::DeclPtr decl_ptr) {
        auto member = decl_ptr->as<ast::MemberVariable>();
        interp->message("Resolving Memeber");
		decl_ptr->renderer(&renderer);

        std::vector<Local*> entities;

        u32 num_names = member->names.size();
        u32 num_exprs = member->init.size();

        types::Type* type{nullptr};

        if(member->type)
            type = resolve_spec(member->type.get());


        if(num_names == num_exprs) {
            std::vector<Operand> init_types;

            // resolve the type of the expression
            for(auto& i : member->init) {
                auto op = resolve_expr(i.get(), type);

                if(op.error) {
					interp->message("Error resolving expression");
                    return std::make_tuple(entities, false);
				}

                init_types.push_back(op);
            }

            // build the entity
            for(u32 i = 0; i < num_exprs; ++i) {
                auto e = interp->new_entity<Local>(member->names[i], init_types[i].type,
                        get_addressing_by_type(init_types[i].type),
                        active_scope(),
                        decl_ptr);
                        
                
                if(member->vis == ast::Visibility::Public)
                    e->set_visable();

                e->set_initialized();
                entities.push_back(e);
            }


        }
        else if(num_exprs == 1) {
            auto op = resolve_expr(member->init.front().get(), type);

            for(u32 i = 0; i < num_names; ++i) {
                auto e = interp->new_entity<Local>(member->names[i], op.type,
                        get_addressing_by_type(op.type),
                        active_scope(),
                        decl_ptr);

                if(member->vis == ast::Visibility::Public)
                    e->set_visable();

                e->set_initialized();
                entities.push_back(e);
            }
        }
        else if(num_exprs == 0) {
            if(!type) {
                report_str(member->pos(), "member variable must have type annotation or initialized");
					interp->message("Error resolving expression");
                return std::make_tuple(entities, false);
            }

            for(u32 i = 0; i < num_names; ++i) {
                auto e = interp->new_entity<Local>(member->names[i], type,
                        get_addressing_by_type(type),
                        active_scope(),
                        decl_ptr);
                        

                if(member->vis == ast::Visibility::Public)
                    e->set_visable();

                entities.push_back(e);
            }
        }
        else {
            report(member->pos(), "unexected number of initialization expression, expecting '%u' or '1' "
                                "found %u", num_names, num_exprs);
			return std::make_tuple(entities, false);
        }

        for(auto e : entities) { 
            if(is_redeclaration(e->get_name())) {
                // check for redeclaration
                report(e->get_name()->pos, "redeclaration of member variable '%s'",  e->get_name()->value().c_str());

                auto entity = search_scope(active_scope(), e->get_name());

                interp->message("'%s' previously declared here:", e->get_name()->value().c_str());
                interp->print_file_section(entity->get_name()->pos);

                return std::make_tuple(entities, false);
            }
            else {
                // add it otherwise
                add_entity(e);
            }
        }
        return std::make_tuple(entities, true);
    }

    void Typer::resolve_local_from_decl(ast::DeclPtr decl_ptr, bool mut) {
        ast::PatternPtr pattern;
        ast::SpecPtr spec;
        ast::ExprPtr init;
        push_context_state(resolving_local, true);

        if(mut) {
            auto local = decl_ptr->as<ast::Mutable>();
            pattern = local->names;
            spec = local->type;
            init = local->init;
        }
        else {
            auto local = decl_ptr->as<ast::Local>();
            pattern = local->names;
            spec = local->type;
            init = local->init;

            if(!init) {
                report_str(decl_ptr->pos(), "constant local must be initialized");
                pop_context_state(resolving_local);
                return;
            }
        }

        if(spec->kind != ast::ast_infer_type and !init) {
            report_str(decl_ptr->pos(), "local variable requires a type annotation or initialization expression")
            pop_context_state(resolving_local);
            return;
        }

        types::Type* type = nullptr;
        Operand result(nullptr, init.get(), RValue);

        if(spec)
            type = resolve_spec(spec.get());

        if(init)
            result = resolve_expr(init.get(), type);

        auto expected_type = (type ? type : result.type);
        Operand op = result;

        op.error = false;
        op.type= expected_type;

        resolve_pattern(pattern.get(), op, decl_ptr);
        pop_context_state(resolving_local);
    }

    /// @NOTE: If a specific expression resolve function is
    // called the operand of the expression must be set after the call.
    Operand Typer::resolve_expr(ast::Expr *expr, types::Type *expected_type) {
        Operand result(expr);
        switch(expr->kind) {
            case ast::ast_name:
            case ast::ast_name_generic:
                result = resolve_name(expr);
                break;
            case ast::ast_unit_expr:
                result = Operand(type_unit, expr, RValue);
                break;
            case ast::ast_integer:
            case ast::ast_fl:
            case ast::ast_bool:
            case ast::ast_ch:
            case ast::ast_str:
                result = resolve_literals(expr);
                break;
            case ast::ast_binary:
                result = resolve_binary(expr->as<ast::Binary>(), nullptr);
                break;
            case ast::ast_unary:
                result = resolve_unary(expr->as<ast::Unary>(), nullptr);
                break;
            case ast::ast_tuple_expr: {
                auto t = expr->as<ast::TupleExpr>();
                u64 sz = 0;
                std::vector<types::Type*> types;
                for(auto& element : t->elements) {
                    auto op = resolve_expr(element.get());
                    if(op.error)
                        return Operand(expr);

                    types.push_back(op.type);
                    sz += op.type->size();
                }
                auto res_type = interp->checked_new_type<types::Tuple>(types, sz, types.front()->alignment());

                // Rvalue because this is a tuple literal.
                result = Operand(res_type, expr, RValue);
                break;

            }
            case ast::ast_accessor:
                result = resolve_accessor(expr);
                break;
            case ast::ast_tuple_accessor:
                result = resolve_tuple_accessor(expr);
                break;
            case ast::ast_struct_expr:
                result = resolve_struct_expr(expr);
                break;
            case ast::ast_call:
                result = resolve_call_or_curry(expr->as<ast::Call>());
                break;
            case ast::ast_block:
                result = resolve_block(expr);
                break;
            case ast::ast_self_expr: {
				if(!context.impl_block_entity) {
					report_str(expr->pos(), "'self' used outside of impl block");
					return Operand(expr);
				}

				auto name = ast::Ident(interp->find_name("self"), expr->pos());
				auto entity = search_active_scope(&name);

				if(!entity) {
					return Operand(expr);
				}
				else {
					return Operand(entity->get_type(), expr, SelfAccess, entity);
				}
            } break;
			// this needs to be implemented in resolve_expr
			case ast::ast_method: {
				result = resolve_method_call(expr);
			} break;
            default:
                break;
        }

        if(expected_type) {
            // check for compatibility of thr resulting type and the expected type.
             if(!compatible_types(expected_type, result.type)) {
                 report(expr->pos(), "incompitable type: '%s' expected: '%s'",
                     result.type->str().c_str(),
                     expected_type->str().c_str());
                 return Operand(expr);
             }
        }

        expr->type = result.type;
        expr->operand = result;

//#if defined(MU_DEBUG)
        interp->out_stream() << "*****************DEBUG: Expression Resulting type" << std::endl;
        expr->renderer(&renderer);
        interp->out_stream() << "*****************DEBUG: TYPE: "
			<< (result.type ? result.type->str() : "Error") << std::endl;
        interp->out_stream() << "*****************DEBUG: Entity: "
			<< (result.entity ? result.entity->full_path().str(): "null") << std::endl;
//#endif

        return result;
    }

    Operand Typer::resolve_binary(ast::Binary *expr, types::Type *expected_type) {
        // resolve sub expression
        auto lhs = resolve_expr(expr->lhs.get());
        auto rhs = resolve_expr(expr->rhs.get());

        // check for errors.
        if(lhs.error)
            return lhs;
        else if(rhs.error)
            return rhs;

        if(lhs.type->is_arithmetic() and lhs.type->is_primative()) {
            return resolve_arithmetic_binary(expr->op, lhs, rhs, expr, expected_type);
        }
        else if(lhs.type->is_ptr()) {

        }
        else return resolve_binary_overload(Tkn_Mut, rhs, lhs, nullptr, nullptr);
    }

    Operand Typer::resolve_arithmetic_binary(TokenKind op, Operand lhs, Operand rhs, ast::Expr *expr,
                                             types::Type *expected_type) {
        auto lhs_type = lhs.type;
        auto rhs_type = rhs.type;

        assert(lhs_type->is_arithmetic());

        bool invalid_lhs = lhs_type->is_bool() || lhs_type->is_char();
        if(lhs_type->is_ptr()) {
            switch(op) {
                // standard arithmitic
                case mu::Tkn_Plus:
                case mu::Tkn_Minus: {
                    if(rhs_type->is_integer())  {
                        // this should be checked to make sure it is still valid.
                        // *T + 1 -> *T
                        // *T - 1 -> *T
                        return Operand(lhs_type, expr, LValue);
                    }
                }
                default:
                    break;
            }
        }
        else if(!invalid_lhs and rhs_type->is_arithmetic() and rhs_type->is_primative()) {
            switch(op) {
               // standard arithmitic
                case mu::Tkn_Plus:
                case mu::Tkn_Minus:
                case mu::Tkn_Astrick:
                case mu::Tkn_Slash:
                case mu::Tkn_AstrickAstrick:
                    // translate this to a call to powf
                case mu::Tkn_Percent:
                    // translate this to a call to modf
                // equality operations
                case mu::Tkn_EqualEqual:
                case mu::Tkn_BangEqual:
                case mu::Tkn_LessEqual:
                case mu::Tkn_GreaterEqual:
                    if(lhs_type->kind() == rhs_type->kind()) {
                        return eval_binary_op(this, op, lhs, rhs, expr, lhs_type);
                    }
                    break;

                // boolean logic
                case mu::Tkn_And:
                case mu::Tkn_Or:
                    if(lhs_type->is_bool() and rhs_type->is_bool()) {
                        return eval_binary_op(this, op, lhs, rhs, expr, lhs_type);
                    }
                    break;
                // bitwise operations
                case mu::Tkn_LessLess:
                case mu::Tkn_GreaterGreater:
                case mu::Tkn_Ampersand:
                case mu::Tkn_Pipe:
                    if(lhs_type->is_integer() and rhs_type->is_integer()) {
                        if(lhs_type->kind() == rhs_type->kind()) {
                            return eval_binary_op(this, op, lhs, rhs, nullptr, lhs_type);
                        }
                    }
                    else {
                        report(expr->pos(), "bitwise operation '%s' must be an integer type, found '%s' and '%s'",
                            Token::get_string(op).c_str(),
                            lhs_type->str().c_str(),
                            rhs_type->str().c_str())
                        return Operand(expr);
                    }
                    break;
                default:
                    break;
            }
        }
        report(expr->pos(), "invalid operands for binary operation '%s' with types: '%s' and '%s'",
               Token::get_string(op).c_str(),
               lhs_type->str().c_str(),
               rhs_type->str().c_str())
        return Operand(expr);
    }

    Operand Typer::resolve_binary_overload(TokenKind op, Operand rhs, Operand lhs, ast::Expr *expr,
                                           types::Type *expected_type) {
        return Operand(nullptr, nullptr, LValue);
    }

    Operand Typer::resolve_unary(ast::Unary *expr, types::Type *expected_type) {
        auto operand = resolve_expr(expr->expr.get(), nullptr);
        if(operand.type->is_ptr()) {
            switch(expr->op) {
                case mu::Tkn_Bang: {
                    return Operand(type_bool, expr, RValue);
                } break;
                case mu::Tkn_Ampersand: {
                    auto type = interp->checked_new_type<types::Pointer>(operand.type);
                    // I think
                    return Operand(type, expr, LValue);
                } break;
                case mu::Tkn_Astrick: {
                    return Operand(operand.type->base_type(), expr, LValue);
                } break;
                default:
                    break;
            }
            report(expr->pos(), "invalid type for unary operation '%s' and type '%s'",
                    Token::get_string(expr->op).c_str(),
                    operand.type->str().c_str());
            return Operand(expr);
        }
        else if(operand.type->is_arithmetic()) {
            return resolve_arithmetic_unary(expr->op, operand, expr, expected_type);
        }
        else {
            return Operand(expr);
        }
    }

    Operand Typer::resolve_arithmetic_unary(TokenKind op, Operand operand, ast::Expr *expr, types::Type *expectd_type) {

        auto type = operand.type;
        bool invalid_type = type->is_char();

        if(!invalid_type and type->is_arithmetic()) {
            switch(op) {
                case mu::Tkn_Minus:
                    if(type->is_bool()) break;
                    else if(type->is_unsigned()) {
                        report(expr->pos(), "attempting to negate an unsigned type '%s'",
                                type->str().c_str());
                        interp->message("cast '%s' to signed type", type->str().c_str());
                    }
                    return eval_unary_op(this, op, operand, expr, type);
                case mu::Tkn_Tilde:
                    if(type->is_integer())
                        return eval_unary_op(this, op, operand, expr, type);
                    break;
                case mu::Tkn_Bang:
                    if(type->is_bool() or type->is_integer() or type->is_float())
                        return eval_unary_op(this, op, operand, expr, type);
                    break;
                case mu::Tkn_Ampersand:
                    if(operand.val.is_constant) {
                        report_str(operand.expr->pos(), "unable to take the address of a literal");
                        return Operand(expr);
                    }
                case mu::Tkn_Astrick:
                    report(operand.expr->pos(), "attempting to dereference a non-pointer type '%s'",
                            type->str().c_str());
                    return Operand(expr);
                default:
                    break;
            }
        }
        else {
            report(expr->pos(), "invalid type for unary operation '%s' and type '%s'",
                   Token::get_string(op).c_str(),
                   operand.type->str().c_str());
            return Operand(expr);
        }
    }

    Operand Typer::resolve_unary_overload(TokenKind op, Operand operand, ast::Expr *expr, types::Type *expected_type) {
        return Operand(nullptr, nullptr, LValue);
    }

    Operand Typer::resolve_literals(ast::Expr *expr) {
        switch(expr->kind) {
            case ast::ast_integer: {
                // defaults to i32 unless larger
                auto integer = expr->as<ast::Integer>();
                if(integer->value < MAX_I32)
                    return Operand(type_i32, expr, Val((i32) integer->value));
                else
                    return Operand(type_i64, expr, Val((i64) integer->value));
            }
            case ast::ast_fl: {
                // defaults to f32
                auto fl = expr->as<ast::Float>();
                if(fl->value < MAX_F32)
                    return Operand(type_f32, expr, Val((f32) fl->value));
                else
                    return Operand(type_f64, expr, Val(fl->value));
            }
            case ast::ast_bool: {
                auto boolean = expr->as<ast::Bool>();
                return Operand(type_bool, expr, Val(boolean->value));
            }
            case ast::ast_ch: {
                auto ch = expr->as<ast::Char>();
                return Operand(type_char, expr, Val(ch->value));
            }
            case ast::ast_str: {
                report_str(expr->pos(), "string literals are not implemented at this time");
            }
            default:
                report_str(expr->pos(), "Compiler Error: attempting to resolve a non-literal expression as literal");
        }
    }

    Operand Typer::resolve_name(ast::Expr *expr) {
        switch(expr->kind) {
            case ast::ast_name: {
                auto name = expr->as<ast::Name>();
                auto entity = resolve_name_expr(name);
                if(!entity) {
                    report(expr->pos(), "use of undeclared name '%s'", name->name->value().c_str());
                    return Operand(expr);
                }
                entity->set_used();
                if(entity->is_variable()) {
                    bool is_initialized = false;
                    switch(entity->kind()) {
                        case GlobalEntity:
                            is_initialized = entity->as<Global>()->is_initialized();
                            break;
                        case LocalEntity:
                            is_initialized = entity->as<Local>()->is_initialized() || context.function_body;
                            break;
                        case ConstantEntity:
                            is_initialized = true;
                            break;
                        default:
                            return Operand(expr);
                    }
                    if(!is_initialized) {
                        report(expr->pos(), "use of an uninitialized variable '%s'", name->name->value().c_str());
                        interp->message("'%s' is declared here:", name->name->value().c_str());
                        interp->print_file_section(entity->get_decl()->pos());
                        return Operand(expr);
                    }
                    return Operand(entity->get_type(), expr, LValue, entity);
                }
                else {
                    switch(entity->kind()) {
                        case TypeEntity:
                        case ModuleEntity:
                        case AliasEntity:
                            return Operand(entity->get_type(), expr, TypeAccess, entity);
                        case FunctionEntity:
                            return Operand(entity->get_type(), expr, FunctionAccess, entity);
                        default:
                            return Operand(expr);
                    }
                }
            }
            case ast::ast_name_generic: {
                interp->message("Generics are not implemented at this time");
               auto [op, entity] = resolve_name_generic_expr(expr->as<ast::NameGeneric>());
               return op;
            }
            default:
                interp->fatal("Invalid name expression");
                return Operand(expr);
        }
    }

    Entity * Typer::resolve_name_expr(ast::Name *expr) {
        return search_active_scope(expr->name);
    }

    std::tuple<Operand, Entity *> Typer::resolve_name_generic_expr(ast::NameGeneric *expr) {
        return std::make_tuple(Operand(expr), (Entity*) nullptr);
    }


    Operand Typer::resolve_accessor(ast::Expr* expr) {
        auto accessor = expr->as<ast::Accessor>();

        if(!accessor) {
            interp->fatal("Compiler Error: invalid ast");
        }

        auto operand = resolve_expr(accessor->operand.get());
        if(operand.error)
            return operand;

        auto type = operand.type;

        interp->message("Accessor type: %s", type->str().c_str());
        // if type is a reference or pointer, then they must be dereferenced.
        if(type->is_ptr() || type->is_ref()) {
            type = type->base_type();
        }



        // I will allow trait and sum types to be included.
        bool allowed_types = type->is_struct(); // || type->is_trait() || type->is_stype();

        if(!allowed_types) {
            /// @TODO-Define: non-accessable
            report(operand.expr->pos(), "request for member '%s' in non-accessable type: '%s'", type->str().c_str(),
                    type->str().c_str())
            operand.error = true;
            return operand;
        }

        // @TODO-refactor to allow trait and sum types to be used.

        auto access = operand.access;

        switch(type->kind()) {
            case types::StructureType: {
                auto ctype = type->as<types::StructType>();
                auto scope = ctype->get_scope();

                // search for entity
                auto entity = search_scope(scope, accessor->name);
                if(!entity) {
                    report(accessor->name->pos, "'%s' is not a member of '%s'",
                        accessor->name->value().c_str(),
                        ctype->str().c_str());
                    return Operand(expr);
                }


                switch(access) {
                    case TypeAccess: {
                        // this meant the element is a local member of the struct
                        // and is accessable only via an instance of the type.
                        // Not statically as is in this case.
                        if(entity->is_local()) {
                            report(accessor->name->pos, "unable to access '%s' of '%s' in this context",
                                accessor->name->value().c_str(), type->str().c_str());
                            return Operand(expr);
                        }
                        else if(entity->is_function()) {
                            return Operand(entity->get_type(), expr, TypeAccess, entity);
                        }
                    }
                    default:
                        if(entity->is_local()) {
                            auto local = entity->as<Local>();
                            if(local->is_visable() || access == SelfAccess)
                                return Operand(entity->get_type(), expr, access, entity);
                            else {
                                report(expr->pos(), "unable to access private member '%s' of struct '%s'", entity->get_name()->value().c_str(),
                                        ctype->get_name()->value().c_str());
                                return Operand(expr);
                            }
                        }
                        else if(entity->is_module() || entity->is_type()) {
                            return Operand(entity->get_type(), expr, TypeAccess, entity);
                        }
                        else if(entity->is_function()) {
                            // If this method doesnt have any required parameters then
                            // this is a valid calling fo the function.
                            // To get a function pointer, use Type.funct_name.

                            // I think this should be TypeAccess maybe this could be change to be just a function Acces type.
                            return Operand(entity->get_type(), expr, TypeAccess, entity);
                            // report(accessor->name->pos, "unable to access method")
                        }
                        else {
                           report_str(expr->pos(), "invalid accessor expression: this is probably a compiler bug");
                            return Operand(expr);
                        }
                }
            }
            case types::SType:
            case types::TraitAttributeType:
                report(expr->pos(), "acessing of a '%s' is not imlementented", type->str().c_str());
                return Operand(expr);
            default:
                return Operand(expr);

        }
    }

    Operand Typer::resolve_tuple_accessor(ast::Expr* expr) {
        auto accessor = expr->as<ast::TupleAcessor>();

        if(!accessor) {
            interp->fatal("Compiler Error: invalid ast");
        }

        auto operand = resolve_expr(accessor->operand.get());

        if(operand.error) {
            return operand;
        }

        if(operand.type->is_tuple()) {
            auto tuple = operand.type->as<types::Tuple>();

            if(accessor->value >= tuple->num_elements()) {
                report(expr->pos(), "request for element '%lu' of tuple of '%lu' element",
                    accessor->value, tuple->num_elements());
                return Operand(expr);
            }
            auto out_type = tuple->get_element_type(accessor->value);
            if(!out_type) {
                interp->out_stream() << tuple->str() << std::endl;
                interp->fatal("tuple type was constructed incorrectly");
            }
            return Operand(out_type, expr, operand.access);
        }
        else {
            report(operand.expr->pos(), "unable to constant index non-tuple types, found type '%s'",
                operand.type->str().c_str());
            return operand;
        }
    }

    Operand Typer::resolve_struct_expr(ast::Expr* expr) {
        auto str = expr->as<ast::StructExpr>();

        if(!str) {
            interp->fatal("Compiler Error: invalid ast");
        }

        auto type = resolve_spec(str->spec.get()); 

        if(!type)
            return Operand(expr);

        if(!type->is_struct()) {
            report(str->spec->pos(), "expecting a structure type, found '%s'", type->str().c_str());
            return Operand(expr);
        }

        auto struct_type = type->as<types::StructType>();

        auto scope = struct_type->get_scope();
        auto num_members = struct_type->num_members();

        // default initialize the list with the number of expected elements.
        // if there are any that are not filled, they will be check to see if that field
        // has a default initializer.
        std::vector<Operand> resolved_members(num_members, Operand(expr));
        u64 member_count = 0;
       for(auto& member : str->members) {
            switch(member->kind) {
                case ast::ast_expr_binding: {
                    auto bind = member->as<ast::BindingExpr>();
                    auto name = bind->name;
                    auto binded_expr = bind->expr.get();

                    auto [member, valid] = scope->find(name);

                    if(valid) {
                        auto member_type = member->get_type();
                        auto expr_type = resolve_expr(binded_expr, member_type);
                        if(expr_type.error)
                            return expr_type;

                        auto index = struct_type->get_index_of_member(member);
                        resolved_members[index] = expr_type;
                    }
                    else {
                        report(name->pos, "struct '%s' does not have member '%s'",
                            struct_type->get_name()->value().c_str(),
                            name->value().c_str());
                        return Operand(expr);
                    }
                } break;
                default: {
                    // gets the expected member of the struct considering where it is
                    // in the expresion list.
                    auto member_entity = struct_type->get_member(member_count);
                    auto expr = resolve_expr(member.get(), member_entity->get_type());

                    // if there was an error resolving the expression, the return an error operand.
                    if(expr.error)
                        return expr;

                    resolved_members[member_count] = expr;
                }
            }
           member_count++;
       }

       // now the member fields have been filled with the expressions given by the structure expression.

       for(u64 i = 0; i < num_members; ++i) {

           // this field was not filled by the expression
           // checking for default.
           if(resolved_members[i].error) {
               auto i_member = struct_type->get_member(i);

               // if this is null there is a bug in the compiler.
               auto local_member = i_member->as<Local>();
               if(!local_member) {
                   interp->fatal("Non Local entity as member of structure");
               }

               if(local_member->is_initialized()) {
                   resolved_members[i] = Operand(local_member->get_type(), nullptr, RValue);
               }
               else {
                   report(expr->pos(), "field '%s' is not initialized", local_member->get_name()->value().c_str());
                   return Operand(expr);
               }
           }
       }

       return Operand(struct_type, expr, RValue);
    }

    std::tuple<std::vector<Operand>, bool>
    Typer::resolve_call_actuals(Function *fn, const std::vector<ast::ExprPtr> &actuals, const mu::Pos &call_pos) {
//        auto type = fn->get_type();
        auto scope_param = fn->get_param_scope();
        // if the function is variadic then ignore the last one for now.
        auto num_params = fn->num_params() - (fn->is_variadic() ? 1 : 0);
        std::vector<Operand> resolved_actuals(num_params, Operand(nullptr));

        if(!fn->is_variadic()) {
            if(actuals.size() != fn->num_params()) {
                report(call_pos, "unexpected number of parameters, %u given, %u expected",
                        actuals.size(), fn->num_params());
                return std::make_tuple(resolved_actuals, false);
            }
        }

        u64 actual_count = 0;
        for(u32 i = 0; i < num_params; ++i) {
            ast::Expr* actual = actuals[i].get();

            switch(actual->kind) {
                case ast::ast_expr_binding: {
                    auto bind = actual->as<ast::BindingExpr>();
                    auto name = bind->name;
                    auto binded_expr = bind->expr.get();

                    auto [param, valid] = scope_param->find(name);

                    if(valid) {
                        auto param_type = param->get_type();
                        auto expr_type = resolve_expr(binded_expr, param_type);

                        if(expr_type.error)
                            return std::make_tuple(resolved_actuals, false);

                        auto index = fn->get_index_of_param(param->as<Local>());
                        if(resolved_actuals[index].error)
                            resolved_actuals[index] = expr_type;
                        else {
                            // attempting to resolve the same parameter
                            report(name->pos, "attempting to rebind a parameter '%s'", name->value().c_str());
                            interp->message("originally bound here:");
                            interp->print_file_section(resolved_actuals[index].expr->pos());
                            return std::make_tuple(resolved_actuals, false);
//                            return false;
                        }
                    }
                    else {
                        report(name->pos, "function '%s' does not have parameter '%s'",
                               fn->get_name()->value().c_str(),
                               name->value().c_str());
                        return std::make_tuple(resolved_actuals, false);
//                        return false;
                    }
                } break;
                default: {
                    // gets the expected member of the struct considering where it is
                    // in the expresion list.
                    auto param_entity = fn->get_param(actual_count);
                    auto expr = resolve_expr(actual, param_entity->get_type());

                    // if there was an error resolving the expression, the return an error operand.
                    if(expr.error)
                        return std::make_tuple(resolved_actuals, false);
//                        return false;

                    if(resolved_actuals[actual_count].error) {
                        resolved_actuals[actual_count] = expr;
                    }
                    else {
                        // attempting to resolve the same parameter
                        report(actual->pos(), "attempting to positionally rebind a parameter '%s'",
                                fn->get_param(actual_count)->get_name()->value().c_str());
                        interp->message("originally bound here:");
                        interp->print_file_section(resolved_actuals[actual_count].expr->pos());
                        return std::make_tuple(resolved_actuals, false);
//                        return false;
                    }
                }
            }

            actual_count++;
        }

        // now the member fields have been filled with the expressions given by the structure expression.

        // all parameters have been filled
        for(u64 i = 0; i < num_params; ++i) {

            // this field was not filled by the expression
            // checking for default.
            if(resolved_actuals[i].error) {
                auto i_param = fn->get_param(i);

                // if this is null there is a bug in the compiler.
                auto local_param = i_param->as<Local>();

                if(local_param->is_initialized()) {
                    resolved_actuals[i] = Operand(local_param->get_type(), nullptr, RValue);
                }
                else {
                    report(call_pos, "field '%s' is not initialized", local_param->get_name()->value().c_str());
                    return std::make_tuple(resolved_actuals, false);
//                    return false;
                }
            }
        }
        // now check the variadic parameter
        // how could resolved_actuals be used later one?
        // right now it is a book keeping structure that is discarded when the function returns.
        // the values are stored in the ast though..
        if(fn->is_variadic()) {
            // the variadic param must be the last one.
            auto vparam =  fn->get_param(fn->num_params() - 1);
            assert(vparam and vparam->is_local());

            auto plocal = vparam->as<Local>();

            types::Type* expected_type = nullptr;
            if(plocal->is_typevariadic()) {
               expected_type = plocal->get_type();
            }

            for(u32 i = num_params; i < actuals.size(); ++i) {
                auto operand = resolve_expr(actuals[i].get(), expected_type);
                if(operand.error) {
                    return std::make_tuple(resolved_actuals, false);
                }
                resolved_actuals.push_back(operand);
            }
        }

        return std::make_tuple(resolved_actuals, true);
    }

    std::tuple<std::vector<Operand>, bool>
    Typer::resolve_actauls(types::FunctionType *fn, const std::vector<ast::ExprPtr> &actuals, const mu::Pos &call_pos) {
//        if(!fn->is_variadic()) {
//            if(actuals.size() > fn->num_params()) {
//                report(call_pos, "unexpected number of parameters, %u given, %u expected",
//                       actuals.size(), fn->num_params());
//                return std::make_tuple(resolved_actuals, false);
//            }
//        }
        std::vector<Operand> resolved_actuals;

        if(fn->num_params() != actuals.size()) {
            report(call_pos, "unexpected number of parameters, expected %u, recieved %u", fn->num_params(), actuals.size());
            return std::make_tuple(resolved_actuals, false);
        }
        u32 param_count = 0;
        for(auto& actual : actuals) {
            switch(actual->kind) {
                case ast::ast_expr_binding: {
                    report(call_pos, "unable to resolve parameter binding of a function pointer with type '%s'", fn->str().c_str());
                    return std::make_tuple(resolved_actuals, false);
                } break;
                default: {
                    // gets the expected member of the struct considering where it is
                    // in the expresion list.
                    auto param_type = fn->get_param(param_count);
                    auto expr = resolve_expr(actual.get(), param_type);

                    // if there was an error resolving the expression, the return an error operand.
                    if(expr.error)
                        return std::make_tuple(resolved_actuals, false);
//                        return false;
                    resolved_actuals.push_back(expr);
                }
            }

            param_count++;
        }

        // now the member fields have been filled with the expressions given by the structure expression.

        // all parameters have been filled

        return std::make_tuple(resolved_actuals, true);
    }

    Operand Typer::resolve_call_or_curry(ast::Call *expr) {
        auto res = resolve_expr(expr->name.get());
		auto function = res.entity;

        if(function->is_function()) {
            auto fn_entity = function->as<Function>();

            auto [actuals, valid] = resolve_call_actuals(fn_entity, expr->actuals, expr->pos());

            if(valid) {
                auto ret_type = fn_entity->get_ret_type();
                return Operand(ret_type, expr, RValue);
            }
            else {
                // TODO: check if curried functions is paractial in the scope of this language.
               return Operand(expr);
            }
        }
        else if(function->is_variable()) {
            auto type = function->get_type();
            if(type->is_function()) {
                auto fn_type = type->as<types::FunctionType>();
                auto [actuals, valid] = resolve_actauls(fn_type, expr->actuals, expr->pos());
                if(valid) {
                    auto ret_type = fn_type->get_ret();
                    return Operand(ret_type, expr, RValue);
                }
                else {
                    // TODO: check if curried functions is paractial in the scope of this language.
					// maybe not....
                    return Operand(expr);
                }
            }
            else if(type->is_array()) {
                // TODO: Refactor this to be idiomatic
                // The array type [f32; ..] will actually implement a trait
                // called Apply[Index < Unsigned] which allows for the type to treated as a function.
                // the trait will require the parameter to be an unsigned integer
                // it will be simulated here.

                // If there are zero actuals or there are more than 1
                if(expr->actuals.size() == 0 or expr->actuals.size() > 1) {
                    report(expr->pos(), "unexpected number of parameters, expected %u, recieved %u", (u64) 1, expr->actuals.size());
                    return Operand(expr);
                }

                auto result = resolve_expr(expr->actuals.front().get());
                if(result.error) {
                    return result;
                }
                auto type = result.type;
                if(type->is_integer() and type->is_unsigned()) {
                    auto array_type = type->as<types::Array>();
                    return Operand(array_type->base_type(), expr, LValue);
                }
                else {
                    report(expr->actuals.front()->pos(), "expecting an unsigned integer as array index, found '%s'", type->str().c_str());
                    return Operand(expr);
                }
            }
        }
        report(expr->pos(), "attempting to call a non-function type, type found: '%s'", function->get_type()->str().c_str());
        return Operand(expr);
    }

	Operand Typer::resolve_method_call(ast::Expr* expr) {
		Operand result(expr);
		auto method = expr->as<ast::Method>();

		// resolve to the entity because we need to know scope when resolving it.
		auto res = resolve_expr(method->expr.get());	

		// If there was an error resolving the reciever.
		if(res.error)
			return res;

		auto operand_entity = res.entity;
		auto entity_type = operand_entity->get_type();

		interp->message("Found Entity Name: %s", operand_entity->get_name()->value().c_str());
		interp->message("Found Entity Type: %s", entity_type->str().c_str());

		auto name = method->name->as<ast::Name>();

		if(!name) {
			report_str(method->name->pos(), "generics are not implemented at this time");
			return result;
		}
		
		ScopePtr member_scope;


		
		// check the type of the resolved entity
		switch(entity_type->kind()) {
			case types::StructureType: {
				auto type = entity_type->as<types::StructType>();
				member_scope = type->get_scope();
			} break;
			case types::SType: {
				// special case
				//auto type = entity_type->as<
				interp->message("Accessing sum type as a call, this is calling a constructor of a sum type element");
				return result;
			}
			case types::TraitAttributeType: {
				// directly access a method of a triat shouldn't be allowed. Unless it has a default implementation.
				interp->message("Accessing a triat type. to call its method. This shouldn't be allowed");
			} break;
			default:
				report(method->expr->pos(), "'%s' is a non-accessable type: '%s'",
					operand_entity->get_name()->value().c_str(),
					entity_type->str().c_str());
				break;
					
		}

		auto member = search_scope(member_scope, name->name);

		if(!member) {
			interp->message("Entity Not found for name %s", name->name->value().c_str());
		}
		else {
			interp->message("Entity Found for name %s for type %s", name->name->value().c_str(), 
					member->get_type()->str().c_str());
		}

		// auto fn = member->as<Function>();
		switch(res.access) {
			case TypeAccess:
				return resolve_static_method(operand_entity, member,
						method->actuals, res, name);
			case LValue:
				return resolve_received_method(operand_entity, member,
						method->actuals, res, name);
			default:
				break;
		}


		return Operand(expr);	 
	}

    Operand Typer::resolve_cast(ast::Cast *expr) {
        return Operand(nullptr, nullptr, LValue);
    }

    Operand Typer::resolve_block(ast::Expr *expr) {
        auto block = expr->as<ast::Block>();
        auto res = Operand(type_unit, expr, RValue);

        // build the scope for the block
        auto scope = make_scope<BlockScope>(expr, active_scope());
        push_scope(scope);

        for(auto stmt : block->elements) {
            if(stmt->kind == ast::ast_empty)
                continue;

            res = resolve_stmt(stmt.get());
            if(res.error)
                break;
        }

        pop_scope();

        return res;
    }

    // Some of my spec resolution can be reduced to resolving expression.
    // how to make sure multiple of the same type are not created
    types::Type *Typer::resolve_spec(ast::Spec *spec) {
        switch(spec->kind) {
            case ast::ast_expr_type: {
                auto e = spec->as<ast::ExprSpec>();

                auto entity = resolve_expr_spec(e->type.get());

                // this is for pointers and reference with in a struct to itself.
                if(context.allow_incomplete_types and !entity->is_resolved()) {
                }

                if(!entity->is_resolved())
                    entity->resolve(this);
                
                if(entity->is_type())
                    return entity->get_type();
                else {
                    report(e->pos(), "'%s' does not refer to a type", entity->str().c_str());
                    return nullptr;
                }
            } break;
            case ast::ast_tuple: {
                auto s = spec->as<ast::TupleSpec>();
                std::vector<types::Type*> types;
                u64 sz = 0;
                for(auto& t : s->elements) {
                    auto e = resolve_spec(t.get());
                    if(e) {
                        types.push_back(e);
                        e += e->size();
                    }
                    return nullptr;
                }
                return interp->checked_new_type<types::Tuple>(types, sz, types.front()->alignment());
            }
            case ast::ast_list_spec: {

            }
            case ast::ast_list_spec_dyn:
                break;
            case ast::ast_ptr: {
                auto s = spec->as<ast::PtrSpec>();

                push_context_state(allow_incomplete_types, true)
                
                auto base_type = resolve_spec(s->type.get());

                pop_context_state(allow_incomplete_types);

                return interp->checked_new_type<types::Pointer>(base_type);
            }
            case ast::ast_ref: {
                auto s = spec->as<ast::RefSpec>();
                push_context_state(allow_incomplete_types, true)

                auto base_type = resolve_spec(s->type.get());

                pop_context_state(allow_incomplete_types);

                return interp->checked_new_type<types::Reference>(base_type);
            }
            case ast::ast_mut: {
                auto s = spec->as<ast::MutSpec>();
                auto base_type = resolve_spec(s->type.get());
                return interp->checked_new_type<types::Mutable>(base_type);
            }
            case ast::ast_self_type: {
                // check if we are resolving an
                // impl block. If we are, then return the type
                // of the impl type being implemented.
            }
                //
            case ast::ast_procedure_spec: {
                auto p = spec->as<ast::ProcedureSpec>();
                std::vector<types::Type*> params;
                types::Type* ret_type = resolve_spec(p->ret.get());
                for(auto pa : p->params) {
                    auto t = resolve_spec(pa.get());
                    if(t) params.push_back(t);
                    else {
                        // the error should have been reported.
                        return nullptr;
                    }
                }
                return interp->checked_new_type<types::FunctionType>(params, ret_type);
            } break;
            case ast::ast_type_lit:
                return nullptr;
            case ast::ast_infer_type:
                return nullptr;
            case ast::ast_unit_type:
                return type_unit;
            default:
                // report the error
                break;
        }
    }

    Entity *Typer::resolve_expr_spec(ast::Expr *expr) {
        auto res = resolve_expr(expr);
		if(res.error)
			return nullptr;

		const auto entity = res.entity;
        if(entity and entity->is_type()) {
            return entity;
        }
        else {
            report_str(expr->pos(), "invalid type expression");
            return nullptr;
        }
    }

    Entity* Typer::resolve_accessor_spec(ast::Accessor* expr) {
        auto root_entity = resolve_expr_spec(expr->operand.get());

        // if there was an error above then propogate it through the rest.
        if(!root_entity) return nullptr;

        ScopePtr scope;
        switch(root_entity->kind()) {
            case mu::ModuleEntity: {
                auto mod = CAST_PTR(Module, root_entity);
                interp->message("Modules are not implemented");
                return nullptr;
                // return search_scope(mod->)
            }
            case mu::TypeEntity: {
                auto type = CAST_PTR(Type, root_entity);

                if(type->is_struct()) {
                    auto struct_type = CAST_PTR(types::StructType, type->get_type());
                    scope = struct_type->get_scope();
                }
                else if(type->is_sumtype()) {
                    auto sum_type = CAST_PTR(types::SumType, type->get_type());
                    scope = sum_type->get_scope();
                }
                else if(type->is_trait()) {
                    auto trait_type = CAST_PTR(types::TraitType, type->get_type());
                    scope = trait_type->get_scope();
                }
                else {
                    report_str(expr->operand->pos(), "type does doesn't have a scope");
                    return nullptr;
                }

            } break;
            default:
                report_str(expr->operand->pos(), "is not a module, structure, or Sum Type");
                return nullptr;
        }

        auto entity = search_scope(scope, expr->name);                    
        if(!entity) {
            report(expr->name->pos, "'%s' is not a member of '%s'",
                    expr->name->value().c_str(), entity->get_name()->value().c_str());
        }
        return entity;
    }

    Operand Typer::resolve_stmt(ast::Stmt *stmt) {
        switch(stmt->kind) {
            case ast::ast_expr: {
                auto expr = stmt->as<ast::ExprStmt>();
                return resolve_expr(expr->expr.get());
            }
            case ast::ast_decl: {
                auto decl = stmt->as<ast::DeclStmt>();
                resolve_decl_stmt(decl);
                return Operand(type_unit, nullptr, RValue);
            }
            case ast::ast_empty:
                return Operand(type_unit, nullptr, RValue);
			default:
				break;
        }
    }

    Operand Typer::resolve_decl_stmt(ast::DeclStmt *stmt) {
        auto decl = stmt->decl;
        Entity* entity;

        switch(decl->kind) {
            case ast::ast_local: {
                resolve_local_from_decl(decl, false);
                return Operand(type_unit, nullptr, RValue);
            }
            case ast::ast_mutable: {
                resolve_local_from_decl(decl, true);
                return Operand(type_unit, nullptr, RValue);
            }
            case ast::ast_procedure: {
                auto p = decl->as<ast::Procedure>();
                entity = interp->new_entity<Function>(p->name, active_scope(), decl);
            } break;
            case ast::ast_structure: {
                auto s = decl->as<ast::Structure>();
                entity = interp->new_entity<Type>(s->name, active_scope(), decl);
            } break;
            case ast::ast_type: {
                auto s = decl->as<ast::Type>();
                entity = interp->new_entity<Type>(s->name, active_scope(), decl);
            } break;
            case ast::ast_use: {
                auto s = decl->as<ast::Use>();
                ast::Ident* name = nullptr;
                switch(s->use_path->kind) {
                    case ast::ast_use_path:
                    case ast::ast_use_path_list:
                    case ast::ast_use_path_alias:
                    default:
                    report_str(s->use_path->pos(), "Compiler Error: parser bug in use declaration");
                }
                interp->fatal("Use are not implemented at this moment");
                entity = interp->new_entity<Module>(name, active_scope(), decl);
            } break;
            case ast::ast_alias: {
                auto a = decl->as<ast::Alias>();
                entity = interp->new_entity<Alias>(a->name, (types::Type*) nullptr, active_scope(), decl);
            } break;
            default:
                report_str(decl->pos(), "invalid declaration in function scope");
                return Operand(nullptr);
        }

        entity = resolve_entity(entity);
        if(entity)
            return Operand(type_unit, nullptr, RValue);
        else
            return Operand(nullptr);
    }

	Operand Typer::resolve_static_method(Entity* op, Entity* fn,
			const std::vector<ast::ExprPtr>& actuals, Operand operand, ast::Expr* name) {
		if(!fn->is_function()) {
			report(name->pos(), "'%s' is not a function of '%s'",
					fn->get_name()->value().c_str(),
					op->get_name()->value().c_str());
			return Operand(name);
		}
		else {
			auto fn_type = fn->get_type()->as<types::FunctionType>();
			auto fn_entity = fn->as<Function>();
			if(!fn_entity->is_static()) {
				report(name->pos(), "'%s' is not a static function of '%s'",
						fn->get_name()->value().c_str(),
						op->get_name()->value().c_str());
				return Operand(name);
			}
			auto [results, valid] = resolve_call_actuals(fn_entity, actuals, name->pos());
			if(valid) {
				auto ret_type = fn_type->get_ret();
				return Operand(ret_type, name, RValue);
			}
			else {
				return Operand(name);
			}
		}
		return Operand(name);
	}

	Operand Typer::resolve_received_method(Entity* op, Entity* fn,
			const std::vector<ast::ExprPtr>& actuals, Operand operand, ast::Expr* name) {
		return Operand(name);
	}


    void Typer::resolve_pattern(ast::Pattern *pattern, Operand expected_type, ast::DeclPtr decl) {
        auto type = expected_type.type;
        switch(pattern->kind) {
            case ast::ast_ident_pattern: {
                auto pat = pattern->as<ast::IdentPattern>();
                auto entity = interp->new_entity<Local>(pat->name, type, get_addressing_by_type(type),
                        active_scope(), decl);
                entity->resolve_to(type);
                add_entity(entity);
                entity->debug_print(interp->out_stream());
            } break;
            case ast::ast_tuple_desc: {
                auto pat = pattern->as<ast::TuplePattern>();
                if(type->is_tuple()) {
                    auto tuple_type = type->as<types::Tuple>();
                    u32 sub_pattern_index = 0;
                    for(auto sub_pattern : pat->patterns) {
                        resolve_pattern(sub_pattern.get(),
                                Operand(tuple_type->get_element_type(sub_pattern_index),
                                        expected_type.expr, expected_type.access),
                                        decl);
                        ++sub_pattern_index;
                    }
                }
                else {
                    report(pattern->pos(), "tuple pattern expects a tuple type, found '%s'", type->str().c_str())
                }
            } break;
            case ast::ast_struct_desc:
            case ast::ast_list_desc:
            case ast::ast_type_desc:

            case ast::ast_multi:
            case ast::ast_bind_pattern:
            case ast::ast_range_pattern:

            case ast::ast_int_pattern:
            case ast::ast_float_pattern:
            case ast::ast_char_pattern:
            case ast::ast_string_pattern:
            case ast::ast_bool_pattern:
                break;

            case ast::ast_ignore_pattern:
                break;
			default:
				break;
        }
    }
}
