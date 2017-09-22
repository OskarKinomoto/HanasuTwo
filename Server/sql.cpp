#include "sql.h"

#include "config.h"

#include <boost/exception/all.hpp>

#define BOOST_LOG_DYN_LINK 1
#include <boost/log/trivial.hpp>

inline auto sqlite3_bind_str(sqlite3_stmt* stmt, int num, StringRef s) {
    return sqlite3_bind_text(stmt, num, s.c_str(), s.length(), nullptr);
}

void sql::init()
{
    get();
}

sql &sql::get()
{
    static sql s;
    return s;
}

void sql::createTable(StringRef tableName, StringRef cells)
{
    char *errMsg = nullptr;
    std::stringstream s;
    s << "CREATE TABLE IF NOT EXISTS " << tableName << "(";
    s << cells;
    s << ")";

    int rc = sqlite3_exec(db, s.str().c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        // TODO throw
        BOOST_LOG_TRIVIAL(error)  << "Faild to create table '" << tableName << "'";
    } else {
        BOOST_LOG_TRIVIAL(debug)  << "Table '" << tableName << "' created";
    }
}

sql::StmtWrap &&sql::existsPrepare(StringRef tableName, StringRef cellName)
{
    std::stringstream s;
    s << "SELECT " << cellName << " FROM " << tableName << " WHERE "
      << cellName << "=?001";

    return std::move(StmtWrap(s.str()));
}

sql::StmtWrap &&sql::insertPrepare(StringRef tableName, StringVecRef cells)
{
    std::stringstream s;
    s << "INSERT INTO " << tableName << " (";

    char del[2] = {0,0};
    for (const auto& cell : cells) {
        s << del << cell;
        del[0] = ',';
    }

    s << ") VALUES(";

    del[0] = 0;
    for (size_t i = 0; i < cells.size(); ++i) {
        s << del << "?";
        del[0] = ',';
    }

    s << ")";

    return std::move(StmtWrap(s.str()));
}

bool sql::userExists(StringRef name)
{
    auto stmt = existsPrepare("users", "name");
    stmt.bind(name, 1);
    return stmt.step() == SQLITE_ROW;
}

bool sql::userExists(UserID id)
{
    auto stmt = existsPrepare("users", "userID");
    stmt.bind(id, 1);
    return stmt.step() == SQLITE_ROW;
}

sql::sql()
{
    int ret = sqlite3_open(config::get().db_path().c_str(), &db);

    if (ret) {
        //       BOOST_THROW_EXCEPTION("Can not open database: ");
    }

    sqlite3_exec(db, "PRAGMA foreign_keys = ON", nullptr, nullptr, nullptr);

    createTable("users",
                "userID INTEGER PRIMARY KEY NOT NULL,"
                "login VARCHAR(300),"
                "nick VARCHAR(300),"
                "pass VARCHAR(40)");

    createTable("friends",
                "user_a INTEGER, "
                "user_b INTEGER, "
                "accepted INT(1) DEFAULT 0, "
                "convID INTEGER, "
                "FOREIGN KEY(user_a) REFERENCES users(userID), "
                "FOREIGN KEY(user_b) REFERENCES users(userID) "
                "FOREIGN KEY(convID) REFERENCES conv(convID)");

    createTable("conv",
                "convID INTEGER PRIMARY KEY NOT NULL, "
                "name VARCHAR(100) DEFAULT '', "
                "time TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
                "guid INTEGER, "
                "public INT(1) DEFAULT 0");

    createTable("messages",
                "msgID INTEGER PRIMARY KEY NOT NULL, "
                "convID INTEGER, "
                "userID INTEGER, "
                "time TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
                "message TEXT, "
                "FOREIGN KEY(convID) REFERENCES conv(convID), "
                "FOREIGN KEY(userID) REFERENCES users(userID)");

    createTable("participants",
                "userID INTEGER, "
                "convID INTEGER, "
                "FOREIGN KEY(convID) REFERENCES conv(convID), "
                "FOREIGN KEY(userID) REFERENCES users(userID)");
}

sql::~sql()
{
    sqlite3_close(db);
}

sql::StmtWrap::StmtWrap(StringRef query)
{
    sqlite3_prepare_v2(sql::get().db, query.c_str(), -1, &stmt, &tail);
}

sql::StmtWrap::StmtWrap(sql::StmtWrap &&s)
{
    std::swap(stmt, s.stmt);
    std::swap(tail, s.tail);
}

sql::StmtWrap::~StmtWrap()
{
    sqlite3_finalize(stmt);
}

void sql::StmtWrap::bind(StringRef s, int n)
{
    sqlite3_bind_str(stmt, n, s);
}

void sql::StmtWrap::bind(uint32 u, int n)
{
    sqlite3_bind_int(stmt, n, u);
}

void sql::StmtWrap::bind(int32 i, int n)
{
    sqlite3_bind_int(stmt, n, i);
}

int sql::StmtWrap::step()
{
    return sqlite3_step(stmt);
}
