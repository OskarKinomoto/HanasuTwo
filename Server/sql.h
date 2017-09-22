#pragma once

#include "types.hpp"

#include <sqlite3.h>

class sql
{
private:
    class StmtWrap {
    public:
        StmtWrap(StringRef query);
        StmtWrap(StmtWrap &&s);
        ~StmtWrap();

        void bind(StringRef s, int n);
        void bind(uint32 u, int n);
        void bind(int32 i, int n);

        int step();

        sqlite3_stmt *stmt;

    private:
        const char *tail = nullptr;
    };
    friend StmtWrap;

public:
    static void init();
    static sql& get();

    void createTable(StringRef tableName, StringRef cells);
    StmtWrap &&existsPrepare(StringRef tableName, StringRef cellName);
    StmtWrap &&insertPrepare(StringRef tableName, StringVecRef cells);

    bool userExists(StringRef name);
    bool userExists(UserID id);


private:
    sql();
    ~sql();
    sqlite3 *db;
};
