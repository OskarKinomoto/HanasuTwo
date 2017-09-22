#include "users.h"

Users &Users::get()
{
    static Users users{};
    return users;
}

Hanasu::ReqRes &Users::reg(const Hanasu::RegReq &req)
{
    static Hanasu::ReqRes res{};

    return res;
}

Users::Users()
{

}
