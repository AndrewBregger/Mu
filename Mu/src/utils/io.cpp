//
// Created by Andrew Bregger on 2019-01-08.
//

#include "io.hpp"
#include <iostream>

#if defined(MU_APPLE)
    #include <unistd.h>
    #include <cstdlib>
    #include <cstddef>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <dirent.h>
    #include <errno.h>
#else
    #include <windows.h>
#endif


io::Path::Path(const std::string &path) : path(path) {
    if(is_directory()) {
        if(this->path.back() == '/') {
            this->path.pop_back();
        }
    }
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
        char temp[MAX_PATH];
        auto size = GetFullPathNameA(path.c_str(), MAX_PATH, temp, nullptr);
		return Path(temp);
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
    //#error __FUNCTION__ is not implemented for this platform
    #endif
    return io::Path("");
}

io::Path io::Path::get_relative(const io::Path &to) {
    return const_cast<const Path&>(*this).get_relative(to);
}

io::Path io::Path::filename(bool with_extension) const {
    u64 index = path.find_last_of(DIR_SEP) + 1;
    u64 ext = path.find_last_of('.');

    if(index == std::string::npos)
        return *this;
    else
    if(with_extension)
        return Path(path.substr(index));
    else
        return Path(path.substr(index, ext - index));
}

io::Path io::Path::filename(bool with_extension) {
    return const_cast<const Path&>(*this).filename(with_extension);
}

io::Path io::Path::extension() const {
    if(is_file()) {
        u64 ext = path.find_last_of('.');

        // this file doenst have an extension
        if(ext == std::string::npos)
            return Path("");
        else
            return Path(path.substr(ext + 1));
    }
    else
        return Path("");

}

io::Path io::Path::extension() {
    return const_cast<const Path&>(*this).extension();
}

bool io::Path::is_directory() const {
    #if defined(MU_APPLE)
    struct stat buffer;
    if(stat (path.c_str(), &buffer) == 0) {
        return S_ISDIR(buffer.st_mode);
    }
    else return false;
    #else
	auto ret = GetFileAttributesA(path.c_str());
	if (ret == INVALID_FILE_ATTRIBUTES)
		return false;
	return ret & FILE_ATTRIBUTE_DIRECTORY;
    #endif
}

bool io::Path::is_directory() {
    return const_cast<const Path&>(*this).is_directory();
}

bool io::Path::is_file() const {
    #if defined(MU_APPLE)
    auto dir = opendir(path.c_str());
    if(dir == nullptr) {
        struct stat buffer;
        if(stat (path.c_str(), &buffer) == 0) {
            return S_ISREG(buffer.st_mode);
        }
        else return false;
    }
    else return false;
    #else
	auto ret = GetFileAttributesA(path.c_str());
	if (ret == INVALID_FILE_ATTRIBUTES)
		return false;
	return ret & FILE_ATTRIBUTE_NORMAL;
    #endif
}

bool io::Path::is_file() {
    return const_cast<const Path&>(*this).is_file();
}

void io::Path::remove_filename() {
    #if defined(MU_APPLE)
    #else
    //#error __FUNCTION__ is not implemented for this platform
    #endif

}

io::Path io::Path::parent_path() const {
    auto index = path.find_last_of(DIR_SEP);
    if(index == std::string::npos) {
        // maybe this is a bad idea.
        auto abs = get_absolute();
        return abs.parent_path();
    }
    else
        return Path(path.substr(0, index));
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

std::ostream &io::operator<<(std::ostream &out, const io::Path &path) {
    return out << path.path;
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

bool io::IO::is_directory() {
    return k == IODirectory;
}

bool io::IO::is_load() {
    return this->loaded;
}


u64 io::IO::hash_name(const Path &path) {
    return hash_name(path.get_absolute().string());
}


u64 io::IO::hash_name(const std::string &filename) {
    std::hash<std::string> hasher;
    return hasher(filename);
}
