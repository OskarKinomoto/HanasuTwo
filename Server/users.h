#pragma once

#include "Common/Protocol/Header.pb.h"

#include <unordered_map>

class User
{
public:
    User();
};

// Singleton
class Users
{
public:
    static Users& get();

    Hanasu::ReqRes &reg(const Hanasu::RegReq &req);
private:
    Users();

    std::unordered_map<uint32_t, User> users;
};
