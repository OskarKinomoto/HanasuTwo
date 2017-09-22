#pragma once

#include <boost/program_options.hpp>

#include "types.hpp"

class config
{
public:
    static void init(StringRef path);
    static config& get();

    String db_path();

    ~config();

private:
    config();

    void read();
    void save();

    void SetPath(StringRef path);

    // boost program_options
    void populate_desc();
    boost::program_options::options_description desc;
    boost::program_options::variables_map vm;

    String path;

    String m_db_path;
};
