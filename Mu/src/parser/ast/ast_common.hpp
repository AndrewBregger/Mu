#ifndef AST_COMMON_HPP_
#define AST_COMMON_HPP_

#include <fstream>
#include <type_traits>

#include "common.hpp"

namespace mu {
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
        std::string value;
        mu::Pos pos;

        inline Ident(const std::string& value, const mu::Pos& pos) : value(value), pos(pos) {
        }

        friend inline std::ostream& operator<< (std::ostream& out, const Ident& id) {
            out << id.value;
            return out;
        }
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
        ast_lambda,     // anonymous function, (x, y) -> x + y

        ast_name,       // name reference, x
        ast_unary,      // 1-arity operator, -x
        ast_binary,     // 2-arity operator, x + y

        ast_accessor,   // accessing attribute of an object, x.y
        ast_tuple_accessor, // accessing the field of a tuple, x.0

        ast_method,     // executing an operation of an object, x.y()
        ast_call,       // executing a function, x()

        // control flow
        ast_if_expr, //
        ast_while_expr, //
        ast_match_expr, //
        ast_for_expr, //
        ast_defer_expr, //

        ast_block,

        ast_expr_binding,

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

        // patterns
        ast_ident,
        ast_multi,
        ast_tuple_desc,
        ast_struct_desc,
        ast_list_desc,
        ast_type_desc,


        // type spec
        ast_named,
        ast_named_generic,
        ast_tuple,
        ast_list_spec,      // [f32; 5]
        ast_list_spec_dyn,  // [f32]
        ast_ptr,
        ast_ref,
        ast_mut,
    };

    extern std::vector<std::string> node_names;

    struct AstNode {
       AstKind kind;
       mu::Pos position;

       AstNode(AstKind kind, const mu::Pos& pos) : kind(kind), position(pos) {}

       inline const mu::Pos& pos() { return position; }

       virtual std::ostream& operator<< (std::ostream& out) {
           out << node_names[kind] << "|";
           out << position;
           return out;
       }
    };

    struct Expr;
    struct Decl;
    struct Stmt;
    struct Pattern;
    struct Spec;

    typedef std::shared_ptr<Expr> ExprPtr;
    typedef std::shared_ptr<Decl> DeclPtr;
    typedef std::shared_ptr<Stmt> StmtPtr;
    typedef std::shared_ptr<Pattern> PatternPtr;
    typedef std::shared_ptr<Spec> SpecPtr;

    template <typename Type, typename... Args>
    ExprPtr make_expr(Args... args) {
//        static_assert(std::is_base_of<Type, Expr>::value, "Type must be a base type of Expr");
        return ExprPtr(new Type(args...));
    }

    template <typename Type, typename... Args>
    StmtPtr make_stmt(Args... args) {
//        static_assert(std::is_base_of<Type, Stmt>::value, "Type must be a base type of Stmt");
        return StmtPtr(new Type(args...));
    }

    template <typename Type, typename... Args>
    SpecPtr make_spec(Args... args) {
//        static_assert(std::is_base_of<Type, Spec>::value, "Type must be a base type of Spec");
        return SpecPtr(new Type(args...));
    }

    template <typename Type, typename... Args>
    DeclPtr make_decl(Args... args) {
//        static_assert(std::is_base_of<Type, Decl>::value, "Type must be a base type of Decl");
        return DeclPtr(new Type(args...));
    }


}

#endif