#pragma once

#include "common.hpp"
#include "io.hpp"

// it is assumed this class is given the absolute path of the file.

namespace io {
	class File : public IO {
    public:
        File(const Path& p);

        virtual ~File();

        std::string extention();

        virtual bool load();

        const std::string& value();

        const std::string& get_line(u64 line);

        bool has_line(u64 line);

    private:

		struct LineInfo {
			std::string line;
			u64 start, end;
		};
        std::string content;

		bool cache_has_line(u64 line);

		void build_cache_to(u64 line);

        std::vector<LineInfo> line_cache;
	};
}
