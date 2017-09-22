#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <thread>
#include <vector>

#include <boost/asio.hpp>

#define BOOST_LOG_DYN_LINK 1
#include <boost/log/trivial.hpp>

#include <boost/locale.hpp>

using namespace std;

using namespace boost::asio;
using namespace boost::asio::ip;

class Session;

typedef std::shared_ptr<Session> SessionPtr;

#include "Common/Protocol/Header.pb.h"
#include "config.h"
#include "sql.h"

class Session {
public:
    Session(boost::asio::io_service& service) : soc(service) {}

    void do_write() {
        BOOST_LOG_TRIVIAL(debug) << "Connection from " << soc.remote_endpoint().address() << ":"
                                 << soc.remote_endpoint().port();

        write();

    }

    void write() {


        Hanasu::Header header{};
        header.set_type(Hanasu::Header_MsgType_NONE);
        header.set_text("Kocham Słonko");

        std::string out;
        header.SerializeToString(&out);

        size_t size = out.size();

        soc.async_send(boost::asio::buffer(&size, sizeof(size)), [](auto, auto){});
        soc.async_send(boost::asio::buffer(out.c_str(), out.size()), [](auto, auto){});
    }

    boost::asio::ip::tcp::socket soc;

    static std::vector<SessionPtr> sessions;
};

std::vector<SessionPtr> Session::sessions{};

class ChatServer {
public:
    ChatServer(boost::asio::io_service& ios,
               const boost::asio::ip::tcp::endpoint endpoint) :
        acceptor(ios, endpoint) {
        acceptor.listen(10);
        accept();
    }

private:
    void accept() {
        auto session = std::make_shared<Session>(acceptor.get_io_service());
        acceptor.async_accept(session->soc, [=](auto &ec){
            if (!ec) {
                BOOST_LOG_TRIVIAL(debug) << "Connection from "
                                         << session->soc.remote_endpoint().address() << ":"
                                         << session->soc.remote_endpoint().port();

                Session::sessions.push_back(session);
                session->do_write();
            }
            accept();
        });
    }

private:
    boost::asio::ip::tcp::acceptor acceptor;
};

#define MAIN_CATCH 0

int main()
{

#if defined (MAIN_CATCH) && MAIN_CATCH == 1
    try {
#endif

        config::init("/home/oskar/hanasu.conf");
        sql::init();

        BOOST_LOG_TRIVIAL(info) << "Hanasu is starting...";

        boost::locale::generator gen;
        std::locale::global(gen(""));

        io_service ioservice;
        tcp::endpoint tcp_endpoint{tcp::v6(), 2017};
        ChatServer server(ioservice, tcp_endpoint);

        ioservice.run();
#if defined (MAIN_CATCH) && MAIN_CATCH == 1
    } catch (const std::exception & e) {
        BOOST_LOG_TRIVIAL(fatal)  << "Exception: " << e.what() << "\n";
        throw;
    }
#endif
    return 0;
}
