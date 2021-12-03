#ifndef _SSL_SOCKS_RAW_UDP_HPP
#define _SSL_SOCKS_RAW_UDP_HPP
#include <memory>
#include "relay.hpp"
#include "raw_relay.hpp"

// raw udp, for client to local server and remote server to dest
class raw_udp
    :public raw_relay
{
public:
    raw_udp(asio::io_context &io, server_type type, const std::string &host="", const std::string &service="");
    ~raw_udp();
    void start_relay();

    void stop_raw_relay();
private:
    struct udp_impl;
    std::unique_ptr<udp_impl> _impl ;

    std::size_t internal_send_data(const std::shared_ptr<relay_data> &buf, asio::yield_context &yield);
    void internal_stop_relay();
    // void local_relay(bool dir);
    void internal_log(boost::system::system_error&error, const std::string &desc);
};
#endif
