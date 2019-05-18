//
// Created by Andrew Bregger on 2019-01-08.
//

#pragma once

#include "common.hpp"

namespace io {
    enum FileKind {
        IOFile,
        IODirectory
    };

    class Path {
    public:     
        Path(const std::string& path);
        Path(const Path& p);

        Path get_absolute() const;
        Path get_absolute();

        Path get_relative(const Path& to) const;
        Path get_relative(const Path& to);

        Path filename() const;
        Path filename();

        bool is_directory() const;
        bool is_directory();

        bool is_file() const;
        bool is_file();

        void remove_filename();

        Path parent_path() const;
        Path parent_path();

        const std::string& string() const;
        const std::string& string();
    private:
        std::string path;
    };


    class IO {
    public:
        IO(FileKind k, const Path &p);

        virtual ~IO();

        inline FileKind kind() { return k; }

        const Path& path();
        std::string name();

        Path relative_path(const Path& dir);
        Path absolute_path();

        bool is_file();

        bool is_load();

        virtual bool load() = 0;

        inline u64 id() { return uid; }

        static u64 hash_name(const Path& path);

        static u64 hash_name(const std::string& filename);
    private:
        FileKind k;         /// the type of io this is.
        Path n;         /// the name of the entity
        Path p;         /// the path of the entity
        u64 uid{0};         /// unique id of the entity
        bool loaded{false}; /// whether it is loaded
    };
}
