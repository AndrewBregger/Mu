//
// Created by Andrew Bregger on 2019-06-07.
//

#ifndef MU_SCHEMA_HPP
#define MU_SCHEMA_HPP

#include "common.hpp"
#include "type.hpp"

namespace mu {
    namespace type {

        enum SchemaKind {
            FunctionSch,
            StructSch,
            TypeSch,
        };

        class Schema {
        public:
            explicit Schema(SchemaKind k);

            inline SchemaKind kind() { return k; }

        private:
            SchemaKind k;
        };

        class FunctionSchema : public Schema {
        public:
        };

        class StructSchema : public Schema {
        public:

        };

        class TypeSchema : public Schema {
        public:

        };
    }
}


#endif //MU_SCHEMA_HPP
