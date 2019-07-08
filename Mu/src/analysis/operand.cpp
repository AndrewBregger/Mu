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
			AccessType access_type, Entity* ent) :
		type(type), expr(expr), access(access_type), entity(ent) {
    }

    Operand::Operand(mu::types::Type *type, ast::Expr *expr,
			const Val &val) :
			type(type), expr(expr),
            val(val), entity(nullptr) {
    }

    Operand::Operand(ast::Expr* expr) :
		type(nullptr), expr(expr), error(true), entity(nullptr) {}
}
