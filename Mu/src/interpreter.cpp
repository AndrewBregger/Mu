//
// Created by Andrew Bregger on 2019-05-18.
//

#include "interpreter.hpp"
#include "parser/parser.hpp"
#include <iostream>

using namespace mu::types;

const u32 LINE_RANGE = 2; // +/-2 from the line in question

Type* type_u8;
Type* type_u16;
Type* type_u32;
Type* type_u64;
Type* type_i8;
Type* type_i16;
Type* type_i32;
Type* type_i64;
Type* type_f32;
Type* type_f64;
Type* type_char;
Type* type_bool;
Type* type_unit;

Type* type_string{nullptr};

Interpreter* Interpreter::instance = nullptr;

Interpreter::Context::Context(const std::vector<std::string> &args) : args(args) {
    // get the current working directory(maybe this could be passed in through the args.
    dir = new io::Directory(io::Path("."), true);
}

io::File* Interpreter::Context::get_root() {
    // the first file is assumed to be the root file containing main function.

    auto root_file = args.front();

    auto path = io::Path(root_file);

    auto [file, valid] = dir->search(path);
    if(valid) {
        if(file->is_file())
            return CAST_PTR(io::File, file);
        else
            return nullptr;    
    }
}

Interpreter::Interpreter(const std::vector<std::string> &args) : context(args) {
    if(instance) {
        std::cerr << "Multiple instances of interpreter" << std::endl;
        quit();
    }
    else {
        instance = this;
    }

    setup();
}

io::File *Interpreter::find_file_by_id(u64 id) {
    auto [file, valid] = context.dir->find(id);
    if(valid and file->is_file())
        return CAST_PTR(io::File, file);
    else
        return nullptr;
}

void Interpreter::setup() {
    // load prelude
    //      find location of prelude module
    //      parse and type-check module content.
    //      set this->prelude to the scope of this module.
    //          this is the parent scope of all use defined modules.

    // until that is setup.
    // hand define the prelude scope.

    prelude = mu::make_scope<mu::ModuleScope>(new ast::Ident(find_name("_prelude"), mu::Pos()), nullptr, nullptr);

    // use the platform to determine the size of these types.
    type_u8  =  checked_new_type<PrimitiveInt>(Primitive_U8,  (u64) 1, (u64) 1);
    type_u16 =  checked_new_type<PrimitiveInt>(Primitive_U16, (u64) 2, (u64) 2);
    type_u32 =  checked_new_type<PrimitiveInt>(Primitive_U32, (u64) 4, (u64) 4);
    type_u64 =  checked_new_type<PrimitiveInt>(Primitive_U64, (u64) 8, (u64) 8);

    type_i8  =  checked_new_type<PrimitiveInt>(Primitive_I8,  (u64) 1, (u64) 1);
    type_i16 =  checked_new_type<PrimitiveInt>(Primitive_I16, (u64) 2, (u64) 2);
    type_i32 =  checked_new_type<PrimitiveInt>(Primitive_I32, (u64) 4, (u64) 4);
    type_i64 =  checked_new_type<PrimitiveInt>(Primitive_I64, (u64) 8, (u64) 8);

    type_f32 =  checked_new_type<PrimitiveFloat>(Primitive_Float32, (u64) 4, (u64) 4);
    type_f64 =  checked_new_type<PrimitiveFloat>(Primitive_Float64, (u64) 8, (u64) 8);

    type_char = checked_new_type<PrimitiveInt>(Primitive_Char, (u64) 1, (u64) 1);
    type_bool = checked_new_type<PrimitiveInt>(Primitive_Bool, (u64) 1, (u64) 1);

    type_unit = checked_new_type<UnitType>();

    auto type_u8_entity  =  new_entity<mu::Type>(new ast::Ident(find_name("u8"),   mu::Pos()), prelude, nullptr);
    auto type_u16_entity =  new_entity<mu::Type>(new ast::Ident(find_name("u16"),  mu::Pos()), prelude, nullptr);
    auto type_u32_entity =  new_entity<mu::Type>(new ast::Ident(find_name("u32"),  mu::Pos()), prelude, nullptr);
    auto type_u64_entity =  new_entity<mu::Type>(new ast::Ident(find_name("u64"),  mu::Pos()), prelude, nullptr);
    auto type_i8_entity  =  new_entity<mu::Type>(new ast::Ident(find_name("i8"),   mu::Pos()), prelude, nullptr);
    auto type_i16_entity =  new_entity<mu::Type>(new ast::Ident(find_name("i16"),  mu::Pos()), prelude, nullptr);
    auto type_i32_entity =  new_entity<mu::Type>(new ast::Ident(find_name("i32"),  mu::Pos()), prelude, nullptr);
    auto type_i64_entity =  new_entity<mu::Type>(new ast::Ident(find_name("i64"),  mu::Pos()), prelude, nullptr);
    auto type_f32_entity =  new_entity<mu::Type>(new ast::Ident(find_name("f32"),  mu::Pos()), prelude, nullptr);
    auto type_f64_entity =  new_entity<mu::Type>(new ast::Ident(find_name("f64"),  mu::Pos()), prelude, nullptr);
    auto type_char_entity = new_entity<mu::Type>(new ast::Ident(find_name("char"), mu::Pos()),  prelude, nullptr);
    auto type_bool_entity = new_entity<mu::Type>(new ast::Ident(find_name("bool"), mu::Pos()),  prelude, nullptr);
    auto type_unit_entity = new_entity<mu::Type>(new ast::Ident(find_name("Unit"), mu::Pos()),  prelude, nullptr);

    type_u8_entity->resolve_to(type_u8);
    type_u16_entity->resolve_to(type_u16);
    type_u32_entity->resolve_to(type_u32);
    type_u64_entity->resolve_to(type_u64);
    type_i8_entity->resolve_to(type_i8);
    type_i16_entity->resolve_to(type_i16);
    type_i32_entity->resolve_to(type_i32);
    type_i64_entity->resolve_to(type_i64);
    type_f32_entity->resolve_to(type_f32);
    type_f64_entity->resolve_to(type_f64);
    type_char_entity->resolve_to(type_char);
    type_bool_entity->resolve_to(type_bool);
    type_unit_entity->resolve_to(type_unit);

    prelude->insert(type_u8_entity->get_name(),   type_u8_entity);
    prelude->insert(type_u16_entity->get_name(),  type_u16_entity);
    prelude->insert(type_u32_entity->get_name(),  type_u32_entity);
    prelude->insert(type_u64_entity->get_name(),  type_u64_entity);
    prelude->insert(type_i8_entity->get_name(),   type_i8_entity);
    prelude->insert(type_i16_entity->get_name(),  type_i16_entity);
    prelude->insert(type_i32_entity->get_name(),  type_i32_entity);
    prelude->insert(type_i64_entity->get_name(),  type_i64_entity);
    prelude->insert(type_f32_entity->get_name(),  type_f32_entity);
    prelude->insert(type_f64_entity->get_name(),  type_f64_entity);
    prelude->insert(type_char_entity->get_name(), type_char_entity);
    prelude->insert(type_bool_entity->get_name(), type_bool_entity);
    prelude->insert(type_unit_entity->get_name(), type_unit_entity);

    std::cout << type_u8_entity->path().str() << std::endl;
}

void Interpreter::compile() {
    auto root = context.get_root();

    auto res = process(root);
}

InterpResult Interpreter::process(io::File *file) {
    mu::Parser parser(this);
    mu::Typer typer(this);
    context.current_file = file;

    auto module = parser.process(file);
//    auto expr = parser.parse_expr();
//    auto res = typer.resolve_expr(expr.get(), nullptr);
    if(!module)
        return Error;
    else {
        auto modulefile = typer.resolve_main_module(module);
    }
}

void Interpreter::print_file_pos(const mu::Pos &pos) {
    auto file = find_file_by_id(pos.fid);
    out_stream() << file->absolute_path() << ":" << pos << " ";
}

void Interpreter::print_file_section(const mu::Pos &pos) {
    auto file = find_file_by_id(pos.fid);

    auto line = pos.line;

    out_stream() << "\t" << file->get_line(line) << std::endl;
    out_stream() << "\t" << std::string(pos.column - 1, ' ') << '^' << std::endl;
}

void Interpreter::fatal(const std::string &msg) {
    out_stream() << "Fatal Error: " << msg << std::endl;
    quit();
}

Atom* Interpreter::find_name(const std::string &name) {
    auto iter = names.find(name);
    if(iter == names.end())
        return names.emplace(name, new Atom(name)).first->second;
    else
        return iter->second;
}

void Interpreter::quit() {
    exit(1);
}

mu::ScopePtr Interpreter::get_prelude() {
    return prelude;
}

void Interpreter::remove_entity(mu::Entity* entity) {
    auto iter = entities.find(mu::EntityPtr(entity));
    if(iter == entities.end()) {
        for(auto& e : entities) {
            if(e.get() == entity) {
                entities.erase(e);
            }
        }
    }
    else {
        entities.erase(iter);
    }
}

bool Interpreter::equivalent_types(mu::types::Type *t1, mu::types::Type *t2) {

    if(t1 == t2)
        return true;

    // only valid for concrete types.
    if(t1->is_polymophic() || t2->is_polymophic())
        return false;

    // if they are note the base type then early return.
    if(t1->kind() != t2->kind())
        return false;

    // this must be integers and floats
    if(t1->is_primative() and t2->is_primative())
        return true;

    using namespace mu::types;

    switch(t1->kind()) {
        case mu::types::StructureType:
            return equivalent_structures(CAST_PTR(mu::types::StructType, t1),
                    CAST_PTR(mu::types::StructType, t2));
        case mu::types::SType:
            return equivalent_sumtype(CAST_PTR(mu::types::SumType, t1),
                    CAST_PTR(mu::types::SumType, t2));
        case mu::types::FunctType:
            return equivalent_function(CAST_PTR(mu::types::FunctionType, t1),
                    CAST_PTR(mu::types::FunctionType, t2));
        case mu::types::TraitAttributeType:
            return equivalent_trait(CAST_PTR(mu::types::TraitType, t1),
                    CAST_PTR(mu::types::TraitType, t2));
        case mu::types::ModType:
            // this should be check path
        case mu::types::PolyFunctionType:
        case mu::types::PolyStructureType:
        case mu::types::PolySType:
        case mu::types::PolyTraitAttributeType:
        case mu::types::PolyType:
        default:
            return false;
    }
}

// duplicate code... @TODO: refactor.
bool Interpreter::equivalent_structures(mu::types::StructType *st1, mu::types::StructType *st2) {

    auto st1_name = st1->get_name();
    auto st2_name = st2->get_name();

    // these types are declared somewhere. if they have the same name we need to check
    // if they have the same path as well.
    if(st1_name->val != st2_name->val)
        return false;

    const auto st1_entity = st1->get_entity();
    const auto st2_entity = st2->get_entity();

    auto st1_path = st1_entity->path().path();
    auto st2_path = st2_entity->path().path();

    if(st1_path.size() != st1_path.size())
        return false;

    // checks the names of the path.
    // It is assumed that if the names are equal
    // and all previous names are equal then the names
    // must be referring to the same entity
    for(u32 i = 0; i < st1_path.size(); ++i) {
        if(st1_path[i] != st2_path[i])
            return false;
    }

    return true;
}

bool Interpreter::equivalent_sumtype(mu::types::SumType *st1, mu::types::SumType *st2) {
    auto st1_entity = st1->get_entity();
    auto st2_entity = st2->get_entity();
    return false;
}

bool Interpreter::equivalent_trait(mu::types::TraitType *t1, mu::types::TraitType *t2) {
    auto t1_name = t1->get_name();
    auto t2_name = t2->get_name();

    // these types are declared somewhere. if they have the same name we need to check
    // if they have the same path as well.
    if(t1_name->val != t2_name->val)
        return false;

    const auto t1_entity = t1->get_entity();
    const auto t2_entity = t2->get_entity();

    auto t1_path = t1_entity->path().path();
    auto t2_path = t2_entity->path().path();

    if(t1_path.size() != t1_path.size())
        return false;

    // checks the names of the path.
    // It is assumed that if the names are equal
    // and all previous names are equal then the names
    // must be referring to the same entity
    for(u32 i = 0; i < t1_path.size(); ++i) {
        if(t1_path[i] != t2_path[i])
            return false;
    }

    return true;
}

bool Interpreter::equivalent_function(mu::types::FunctionType *f1, mu::types::FunctionType *f2) {

    if(f1->num_params() != f2->num_params())
        return false;

    for (u64 i = 0; i < f1->num_params(); ++i) {
        if(!equivalent_types(f1->get_param(i), f2->get_param(i)))
            return false;
    }

    // to this point, all is the same. The only difference would be the return type.
    return equivalent_types(f1->get_ret(), f2->get_ret());
}
