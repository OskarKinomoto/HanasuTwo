#include "config.h"

#include <fstream>

#include <boost/exception/all.hpp>

#include <experimental/filesystem>

void config::init(StringRef path)
{
    static bool one = false;

    //if(one)
    //    BOOST_THROW_EXCEPTION("Config buu");

    auto& c = get();
    c.SetPath(path);

    if (!std::experimental::filesystem::exists(path)) {
        c.save();
    }


    c.read();

    one = true;
}

config &config::get()
{
    static config c;
    return c;
}

String config::db_path()
{
    return m_db_path;
}

config::~config()
{
    save();
}

config::config() : desc("Settings"), m_db_path("/home/oskar/hanasu.db")
{
    populate_desc();
}

void config::read()
{
    std::ifstream file(path);
    vm = boost::program_options::variables_map();
    boost::program_options::store(boost::program_options::parse_config_file(file , desc, true), vm);
    boost::program_options::notify(vm);
}

void config::save()
{
    std::ofstream file(path);
    file << "db_path=" << m_db_path << std::endl;
}

void config::SetPath(StringRef path)
{
    this->path = path;
}

void config::populate_desc()
{
    desc.add_options()
        ("db_path", boost::program_options::value<std::string>(&m_db_path), "dp_path");
}
