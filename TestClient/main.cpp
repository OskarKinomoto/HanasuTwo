#include <iostream>

#include <boost/asio.hpp>

#define BOOST_LOG_DYN_LINK 1
#include <boost/log/trivial.hpp>


using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::asio::ip;

#include "Common/Protocol/Header.pb.h"


int main()
{
    io_service io_service;
    tcp::resolver resolver(io_service);
    auto res_it = resolver.resolve({"localhost", "2017"});
    tcp::socket socket(io_service);


    Hanasu::Header header{};

    boost::asio::async_connect(socket, res_it,
                               [&](boost::system::error_code ec, tcp::resolver::iterator) {
        if (!ec) {
            size_t size = 0;

            socket.read_some(boost::asio::buffer(&size, sizeof(size)));

            std::vector<char> buf(size);

            socket.read_some(boost::asio::buffer(buf));

            Hanasu::Header header{};
            header.ParseFromArray(&buf[0], buf.size());
            cout << header.text() << endl;

            /* header.ByteSize */
        }
    });

    io_service.run();
}
