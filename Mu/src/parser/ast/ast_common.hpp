#ifndef AST_COMMON_HPP_
#define AST_COMMON_HPP_

#include <fstream>
#include <type_traits>

#include "analysis/operand.hpp"
#include "common.hpp"
struct Atom;

namespace mu {

    namespace types {
        class Type;
    }

    struct Pos {
        u64 line;
        u64 column;
        u64 span;
        u64 fid;


        inline Pos() = default;

        inline Pos(u64 line, u64 column, u64 span, u64 fid) : line(line), column(column), span(span), fid(fid) {
        }

        inline Pos extend(const Pos& pos) {
            span += pos.span;
            return *this;
        }

        bool operator== (const mu::Pos& pos) {
            return line == pos.line && column == pos.column &&
                   span == pos.span && fid == pos.fid;
        }

        friend inline std::ostream& operator<< (std::ostream& out, const mu::Pos& pos) {
            out << pos.line << ":" << pos.column;
            return out;
        }
    };
}

namespace ast {
    struct Ident {
        Atom* val;
        mu::Pos pos;

        inline Ident(Atom* val, const mu::Pos& pos) : val(val), pos(pos) {
        }

        inline const std::string& value() { return val->value; }

//        friend inline std::ostream& operator<< (std::ostream& out, const Ident& id) {
//            return out;
//        }
   };


    enum AstKind {
        // Expr

        // primative literals
        ast_integer,    // integer constant
        ast_fl,         // float constant
        ast_ch,         // character constant
        ast_str,        // string constant
        ast_bool,       // boolean constant
        ast_nil,        // nil constant

        // compound literals
        ast_list,       // list literal, [1, 2, 3]
        ast_map,        // map literal, [1:"h", 2:"e", 3:"l", 4:"l,]
        ast_lambda,     // anonymous procedure, \x, y -> x + y

        ast_name,       // name reference, x
        ast_name_generic, // name with type parameters, x[f32, i32]
        ast_unary,      // 1-arity operator, -x
        ast_binary,     // 2-arity operator, x + y

        ast_accessor,   // accessing attribute of an object, x.y
        ast_tuple_accessor, // accessing the field of a tuple, x.0

        ast_method,     // executing an operation of an object, x.y()
        ast_call,       // executing a procedure, x()
        ast_self_expr,  // self
        ast_cast_expr,  // type casting of an expression x as f32

        // control flow
        ast_if_expr, //
        ast_while_expr, //
        ast_match_expr, //
        ast_for_expr, //
        ast_defer_expr, //
        ast_match_arm,
        ast_return,

        ast_struct_expr,
        ast_tuple_expr,

        ast_block,

        ast_expr_binding,
        ast_range,

        ast_unit_expr,
        ast_assign,

        // Stmt
        ast_expr,
        ast_decl,
        ast_empty,

        // Decl
        ast_local,
        ast_mutable,
        ast_global,
        ast_global_mut,
        ast_procedure,
        ast_structure,
        ast_type,
        ast_type_class,
        ast_use,
        ast_alias,
        ast_generic,
        ast_bounded_generic,
        ast_generics_group,

        ast_member_variable,
        ast_procedure_parameter,
        ast_self_parameter,
        ast_c_variadic,
        ast_variadic,


        ast_trait_element_type,

        ast_impl,

        ast_type_member,

        ast_use_path,
        ast_use_path_list,
        ast_use_path_alias,

        // patterns
        ast_ident,
        ast_multi,
        ast_tuple_desc,
        ast_struct_desc,
        ast_list_desc,
        ast_type_desc,

        ast_ident_pattern,
        ast_ignore_pattern,
        ast_bind_pattern,
        ast_int_pattern,
        ast_float_pattern,
        ast_char_pattern,
        ast_string_pattern,
        ast_bool_pattern,
        ast_range_pattern,



        // type spec
        ast_expr_type,
        ast_tuple,
        ast_list_spec,      // [f32; 5]
        ast_list_spec_dyn,  // [f32]
        ast_ptr,
        ast_ref,
        ast_mut,
        ast_self_type,
        ast_procedure_spec,
        ast_type_lit,

        ast_infer_type,     // this is more for type infrencing
        ast_unit_type,

        ast_module_file,
        ast_module_dir,
    };

    extern std::vector<std::string> node_names;

    struct AstNode {
       AstKind kind;
       mu::Pos position;
       mu::types::Type* type{nullptr};

       AstNode(AstKind kind, const mu::Pos& pos) : kind(kind), position(pos) {}

       inline const mu::Pos& pos() { return position; }

       template <typename T>
       const T* as() const {
           return CAST_PTR(const T, this);
        }

        template <typename T>
        T* as() {
            return (T*) const_cast<const AstNode&>(*this).as<T>();
        }
    };

    struct Decl : public AstNode {
        Decl(AstKind kind, const mu::Pos& pos) : AstNode(kind, pos) {}
        virtual ~Decl() = default;
    };

    struct Expr : public AstNode {
        mu::Operand operand;
        Expr(AstKind k, const mu::Pos& pos) : AstNode(k, pos), operand(this) {}

        virtual ~Expr() = default;

//        virtual std::ostream& operator<< (std::ostream& out) override {
//            AstNode::operator<<(out);
//            return out;
//        }
    };

    // these are declared here instead of their files because there were some
    // instantiation issues with them being forward declared.
    struct Stmt : public AstNode {
        Stmt(AstKind kind, const mu::Pos& pos) : AstNode(kind, pos) {}

//        virtual std::ostream& operator<< (std::ostream& out) = 0;
    };

    struct Spec : public AstNode {
        Spec(AstKind kind, const mu::Pos& pos) : AstNode(kind, pos) {}
    };

    struct Pattern : public AstNode {
        Pattern(AstKind kind, const mu::Pos& pos) : AstNode(kind, pos) {}
    };

    typedef std::shared_ptr<Expr> ExprPtr;
    typedef std::shared_ptr<Decl> DeclPtr;
    typedef std::shared_ptr<Stmt> StmtPtr;
    typedef std::shared_ptr<Pattern> PatternPtr;
    typedef std::shared_ptr<Spec> SpecPtr;

    template <typename Type, typename... Args>
    ExprPtr make_expr(Args... args) {
//        static_assert(std::is_base_of<Type, Expr>::value, "Type must be a base type of Expr");
        return std::make_shared<Type>(args...);
    }

    template <typename Type, typename... Args>
    StmtPtr make_stmt(Args... args) {
//        static_assert(std::is_base_of<Type, Stmt>::value, "Type must be a base type of Stmt");
        return std::make_shared<Type>(args...);
    }

    template <typename Type, typename... Args>
    SpecPtr make_spec(Args... args) {
//        static_assert(std::is_base_of<Type, Spec>::value, "Type must be a base type of Spec");
        return std::make_shared<Type>(args...);
    }

    template <typename Type, typename... Args>
    DeclPtr make_decl(Args... args) {
//        static_assert(std::is_base_of<Type, Decl>::value, "Type must be a base type of Decl");
        return std::make_shared<Type>(args...);
    }

    template <typename Type, typename... Args>
    PatternPtr make_pattern(Args... args) {
//        static_assert(std::is_base_of<Type, Decl>::value, "Type must be a base type of Decl");
        return std::make_shared<Type>(args...);
    }

    typedef std::vector<Ident*> SPath;
}

#endif