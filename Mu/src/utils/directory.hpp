//
// Created by Andrew Bregger on 2019-01-07.
//

#pragma once

#include "common.hpp"
#include "file.hpp"
#include "io.hpp"
#include <tuple>
#include <dirent.h>
#include <unordered_map>

/// this is an index of the source directory
namespace io {
    class Directory : public IO {
    public:
        Directory(const Path& path, bool only_source = false);

        ~Directory();


        virtual bool load();

        bool contain(const Path& p);

        bool contain(u64 hash);

        std::tuple<IO*, bool> find(const Path& p);

        std::tuple<IO*, bool> find(u64 id);

        // searches for a name in this directory
        std::tuple<IO*, bool> search(const std::string& name, bool ignore_extension = false);

        // expecting the name to be relative to the this directory
        std::tuple<IO*, bool> search(const Path& name, bool ignore_extension = false);

    private:
        std::unordered_map<u64, IO*> content;

        bool only_source;
    };
}
