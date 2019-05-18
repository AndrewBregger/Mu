//
// Created by Andrew Bregger on 2019-01-08.
//

#include "io.hpp"

#if defined(MU_APPLE)
    #include <unistd.h>
    #include <cstdlib>
#else
    #include <windows.h>
#endif


io::Path::Path(const std::string &path) : path(path) {
}

io::Path::Path(const io::Path &p) : path(p.path) {
}

io::Path io::Path::get_absolute() const {
    if(path.front() == '/')
        return *this;

    #if defined(MU_APPLE)
        char temp[PATH_MAX];
        char* ret = realpath(path.c_str(), temp);
        if(ret) {
            return Path(temp);
        }
        else {
            return Path("");
        }
    #else
    #error __FUNCTION__ is not implemented for this platform
    #endif
}

io::Path io::Path::get_absolute() {
    return const_cast<const Path&>(*this).get_absolute();
}

io::Path io::Path::get_relative(const io::Path &to) const {
    #if defined(MU_APPLE)
//        auto abs_this = get_absolute();
        if(to.is_file()) {

        }
    #else
    #error __FUNCTION__ is not implemented for this platform
    #endif
    return io::Path("");
}

io::Path io::Path::get_relative(const io::Path &to) {
    return const_cast<const Path&>(*this).get_relative(to);
}

io::Path io::Path::filename() const {
    u64 index = 0;
    #if defined(MU_APPLE)
    index = path.find_last_of('/');
    #else
    // #error __FUNCTION__ is not implemented for this platform
    index = path.find_last_of('\\');
    #endif
    // auto filename = 
    return io::Path("");
}

io::Path io::Path::filename() {
    return const_cast<const Path&>(*this).filename();
}

bool io::Path::is_directory() const {
    #if defined(MU_APPLE)
    #else
    #error __FUNCTION__ is not implemented for this platform
    #endif
    return false;
}

bool io::Path::is_directory() {
    return const_cast<const Path&>(*this).is_directory();
}

bool io::Path::is_file() const {
    #if defined(MU_APPLE)
    #else
    #error __FUNCTION__ is not implemented for this platform
    #endif
    return false;
}

bool io::Path::is_file() {
    return const_cast<const Path&>(*this).is_file();
}

void io::Path::remove_filename() {
    #if defined(MU_APPLE)
    #else
    #error __FUNCTION__ is not implemented for this platform
    #endif

}

io::Path io::Path::parent_path() const {
    #if defined(MU_APPLE)
    #else
    #error __FUNCTION__ is not implemented for this platform
    #endif
    return io::Path("");
}

io::Path io::Path::parent_path() {
    return const_cast<const Path&>(*this).parent_path();
}

const std::string &io::Path::string() const {
    return path;
}

const std::string &io::Path::string() {
    return const_cast<const Path&>(*this).string();
}


io::IO::IO(FileKind k, const Path &p) : k(k),
n(p.filename()), p(p), uid(IO::hash_name(p.string())) {
}

io::IO::~IO() = default;

const io::Path &io::IO::path() {
    return p;
}

std::string io::IO::name() {
    return n.string();
}

// stupid way of doing this.
// but oh well.
io::Path io::IO::relative_path(const Path &dir) {
    return n.get_relative(dir);
/*
    if (!fs::is_directory(dir)) {
        if (dir.has_parent_path())
            local_dir = dir.parent_path();
        else {
            local_dir = dir;
            local_dir.remove_filename();
        }
    }
    else
        local_dir = dir;

    auto dir_abs = fs::absolute(local_dir);
    auto abs = absolute_path();

    auto abs_iter = dir_abs.begin(), l_iter = abs.begin();
    for(;
        abs_iter != dir_abs.end() && l_iter != abs.end();
        ++abs_iter, ++l_iter) {
        if(*abs_iter != *l_iter)
            break;
    }

    if(abs_iter != dir_abs.end())
        return Path("");

    Path n_path;
    while(l_iter != abs.end()) {
        n_path /= *l_iter;
        ++l_iter;
    }
    */
}

io::Path io::IO::absolute_path() {
    return p.get_absolute();
}

bool io::IO::is_file() {
    return k == IOFile;
}

bool io::IO::is_load() {
    return this->loaded;
}


u64 io::IO::hash_name(const Path &path) {
    return hash_name(path.string());
}


u64 io::IO::hash_name(const std::string &filename) {
    std::hash<std::string> hasher;
    return hasher(filename);
}
