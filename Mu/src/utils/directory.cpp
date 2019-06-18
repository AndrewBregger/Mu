//
// Created by Andrew Bregger on 2019-01-07.
//

#include "directory.hpp"
#include <iostream>


// If i needed this else where then I will make it more general
std::vector<std::string> split(const std::string& str, char delim) {
    u64 start = 0;
    std::vector<std::string> res;

    for(u64 curr = start; curr < str.size(); ++curr) {
        if(str[curr] == delim) {
            res.push_back(str.substr(start, curr - start));
            start = curr + 1;
        }
        else if(curr + 1 == str.size()) {
            res.push_back(str.substr(start, curr - start + 1));
            start = curr + 1;
        }
    }

    return res;
}

namespace io {


    Directory::Directory(const Path &path, bool only_source) : IO(io::IODirectory, path), only_source(only_source) {
    }

    Directory::~Directory() {
    }

    bool Directory::contain(const Path &p) {
        return contain(IO::hash_name(p));
    }

    bool Directory::contain(u64 hash) {
        return content.find(hash) != content.end();
    }

    bool Directory::load() {
        std::string s;
        if(path().is_file())
            s = path().parent_path().get_absolute().string();
        else
            s = path().get_absolute().string();

        auto dir = opendir(s.c_str());

        if(dir) {
            while(auto entry = readdir(dir)) {
                std::string name = entry->d_name;
                auto p = Path(path().string() + "/" + name);
                if(name == ".." || name == ".")
                    continue;

                if((entry->d_type & DT_DIR) == DT_DIR) {
                    // a directory
                    auto file = new io::Directory(p, only_source);
                    std::cout << "New Directory: " << p.string() << std::endl;
                    content.emplace(file->id(), file);
                }
                else if((entry->d_type & DT_REG) == DT_REG) {
                    // if we only want source files, skip non-source files.
                    if(only_source)
                        if(p.extension().string() != EXTENSION_NAME.string())
                            continue;

                    std::cout << "New File: " << p.string() << std::endl;
                    auto file = new io::File(p);
                    content.emplace(file->id(), file);
                }
                else if((entry->d_type & DT_LNK) == DT_LNK) {
                    std::cout << name << " is a symbolic link" << std::endl;
                    return false;
                }
            }
        }
        else {
            std::cout << "Path doesn't exist: " << s << std::endl;
            return false;
        }

        this->loaded = true;
        return true;
    }

    std::tuple<IO *, bool> Directory::find(const Path &p) {
        return search(p);
    }

    std::tuple<IO *, bool> Directory::find(u64 id) {
        if(!is_load())
            load();

        if(contain(id))
            return std::make_tuple(content[id], true);

        for(const auto& [key, value] : content) {
            if(!value->is_file()) {
                auto dir = dynamic_cast<io::Directory*>(value);
                auto [file, valid] = dir->find(id);
                if(valid)
                    return std::make_tuple(file, valid);
            }
        }

        return std::make_tuple(nullptr, false);
    }

    std::tuple<IO *, bool> Directory::search(const std::string &name, bool ignore_extension) {
        if(!is_load())
            load();

        for(auto& [id, io] : content) {
            auto p = io->path();
            if(name == p.filename(!ignore_extension).string())
                return std::make_tuple(io, true);
        }
        return std::make_tuple(nullptr, false);

    }

    std::tuple<IO *, bool> Directory::search(const Path &name, bool ignore_extension) {
        // if not loaded, load it.
        if(!is_load())
            load();

        auto curr = (IO*) this;
        auto p = name.string();

        // get the names of the path
        auto paths = split(p, DIR_SEP);

        // iteratively search the directory for the next name

        // I.E the current directory will be search for the first name in the path
        // and so on if it is a directory.
        for(auto n : paths) {
            if(curr->is_file()) {
                std::cout << "Invalid path given to Directory::search" << std::endl;
                return std::make_tuple(nullptr, false);
            }

            auto [io, valid] = CAST_PTR(Directory, curr)->search(n, ignore_extension);

            if(valid)
                curr = io;
            else
                return std::make_tuple(nullptr, false);
        }

        return std::make_tuple(curr, true);
    }
}
