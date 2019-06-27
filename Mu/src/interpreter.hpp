//
// Created by Andrew Bregger on 2019-05-18.
//

#ifndef MU_INTERPRETER_HPP
#define MU_INTERPRETER_HPP


#include "analysis/entity.hpp"
#include "analysis/types/type.hpp"
#include "analysis/typer.hpp"
#include "analysis/scope.hpp"

#include "parser/ast/ast_common.hpp"
#include "parser/ast/module.hpp"

#include "utils/io.hpp"
#include "utils/file.hpp"
#include "utils/directory.hpp"

#include <cstdio>
#include <ostream>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

enum InterpResult {
    Success,
    Error,
};

class Interpreter {
public:

    enum PrimaryCommand {
        BuildExe, // default, no command
        BuildLib,
        AstRender,
        LLVMRender,
        PrintUsage,
        Error,
    };

    struct Context {
        PrimaryCommand cmd;

        io::File* current_file{nullptr};
        io::Directory* dir{nullptr};
        // io::Directory* standard; // Directory containing standard library
        // command line argument arguments are stored here.
        std::vector<std::string> args;

        io::File* get_root();

        std::string render_file;
        std::string root_file;

        Context(const std::vector<std::string>& args);
        void process_args();
    };

    Interpreter(const std::vector<std::string>& args);

    void setup();

    void compile();

    InterpResult process(io::File *file);

    InterpResult render(io::File* file);

    void usage();

    Atom* find_name(const std::string& name);

    inline void set_stream(std::ostream* out) { this->out = out; }
    inline std::ostream& out_stream() { return *out; }

    inline io::File* current_file() { return context.current_file; }

    io::File* find_file_by_id(u64 id);

    template<typename... Args>
    void report_error(const mu::Pos& pos, const std::string& fmt, Args... args) {
        print_file_pos(pos);
        out_stream() << "Error: ";
        printf(fmt.c_str(), args...);
        out_stream() << std::endl;
        print_file_section(pos);
    }

    template <typename... Args>
    void message(const std::string& fmt, Args... args) {
        out_stream() << "\t";
        printf(fmt.c_str(), args...);
        out_stream() << std::endl;
    }

    void fatal(const std::string& msg);

    void quit();

    void print_file_pos(const mu::Pos& pos);

    void print_file_section(const mu::Pos& pos);

    static Interpreter* get() { return instance; }

    // this creates the TypePtr
    template<typename Ty, typename... Args>
    mu::types::TypePtr new_type(Args... args) {
        static_assert(std::is_base_of<mu::types::Type, Ty>::value, "attempting to constructing non-scope object");
        return std::make_shared<Ty>(args...);
    }

    // checks if the type already exists. Doesn't check compatability.
    template<typename Ty, typename... Args>
    Ty* checked_new_type(Args... args) {
        static_assert(std::is_base_of<mu::types::Type, Ty>::value, "attempting to constructing non-scope object");

        /// this type is not added to the set then it will be cleanup automatically.
        auto type = new_type<Ty>(args...);

        // I know this isn't the most efficent but it works for now.
        for(auto& ty : types) {
            if(equivalent_types(ty.get(), type.get())) {
                return ty.get()->template as<Ty>();;
            }
        }

        // an equivalent type is not found. Add it to the type set and return it casted.
        auto iter = types.emplace(std::move(type));
        return (*iter.first)->template as<Ty>();
    }


    template<typename Ty, typename... Args>
    Ty* new_entity(Args... args) {
        static_assert(std::is_base_of<mu::Entity, Ty>::value, "attempting to constructing non-entity object");
        auto iter = entities.emplace(std::move(std::make_unique<Ty>(args...)));
        return (*iter.first)->template as<Ty>();
    }

    // checks whether two given types are structurally the same. This method only works
    // for concrete types. It will return false if any input is polymorphic. The typer
    // should be use to determine if an instance of a polymporhic type satisfies a polymorhpic
    // entity.
    //
    // the name is not used to check equivalency because if they have the same name
    // but different paths, they they are not the same.
    bool equivalent_types(mu::types::Type* t1, mu::types::Type* t2);

    bool equivalent_structures(mu::types::StructType* st1, mu::types::StructType* st2);

    bool equivalent_sumtype(mu::types::SumType* st1, mu::types::SumType* st2);

    bool equivalent_trait(mu::types::TraitType* t1, mu::types::TraitType* t2);

    bool equivalent_function(mu::types::FunctionType* f1, mu::types::FunctionType* f2);

    mu::ScopePtr get_prelude();

    void remove_entity(mu::Entity* entity);

private:
    Context context;
    std::unordered_map<std::string, Atom*> names;
    std::ostream* out;

    std::unordered_set<mu::types::TypePtr> types;
    std::unordered_set<mu::EntityPtr> entities;
//    mu::Module* prelude{nullptr};
    mu::ScopePtr prelude;
    static Interpreter* instance;
};


#endif //MU_INTERPRETER_HPP
