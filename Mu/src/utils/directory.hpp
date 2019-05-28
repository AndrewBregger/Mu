//
// Created by Andrew Bregger on 2019-01-07.
//

#pragma once

#include "common.hpp"
#include "file.hpp"
#include "io.hpp"
#include <tuple>
//#include <dirent.h>
#include <unordered_map>

/// this is an index of the source directory
namespace io {
    class Directory : public IO {
    public:
        Directory(const Path& path, bool only_source = false);

        virtual bool load();

        bool contain(const Path& p);

        bool contain(u64 hash);

        std::tuple<IO*, bool> find(const Path& p);

        std::tuple<IO*, bool> find(u64 id);

    private:
        std::unordered_map<u64, IO*> content;

        bool only_source;
    };
}
