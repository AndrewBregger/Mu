//
// Created by andy on 6/22/19.
//

#include "operand.hpp"

namespace mu {
    Operand::Operand(mu::types::Type *type, ast::Expr *expr,
			AccessType access_type)
		: Operand(type, expr, access_type, nullptr) {
                                                                                       
    }

    Operand::Operand(mu::types::Type *type, ast::Expr *expr,
			AccessType access_type, Entity* entity) :
		type(type), expr(expr), access(access_type), entity(entity) {
    }

    Operand::Operand(mu::types::Type *type, ast::Expr *expr, const Val &val) : type(type), expr(expr),
                                                                               val(val) {
    }

    Operand::Operand(ast::Expr* expr) : type(nullptr), expr(expr), error(true) {}
}
