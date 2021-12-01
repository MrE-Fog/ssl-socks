#ifndef _GROXY_RELAY_HPP
#define _GROXY_RELAY_HPP
#include <cstdint>
#include <boost/asio/detail/socket_option.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

using boost::asio::ip::tcp;
using boost::asio::ip::udp;
namespace logging = boost::log;
namespace keywords = boost::log::keywords;

typedef boost::asio::detail::socket_option::boolean<SOL_IP, IP_TRANSPARENT> _ip_transparent_t;


const int RELAY_TICK = 10;
const int TIMEOUT_COUNT = 12;
const int TIMEOUT = RELAY_TICK*TIMEOUT_COUNT;

class base_relay;
class raw_relay;
class raw_tcp;
class raw_udp;

class ssl_relay;

enum server_type {
	LOCAL_SERVER,
	REMOTE_SERVER,
	LOCAL_TRANSPARENT
};

struct relay_config
{
	int local_port = 10230;
	int remote_port = 10230;
	std::string remote_ip = "";
	int thread_num = 1;
//	bool local = true;
	server_type type = LOCAL_SERVER;
	std::string cert = "/etc/groxy_ssl/groxy_ssl.crt";
	std::string key = "/etc/groxy_ssl/groxy_ssl.pem";
	std::string logfile = "/dev/null";
	std::string gfw_file = "/etc/groxy_ssl/gfwlist";

};

inline void throw_err_msg(const std::string &msg)
{
	throw(boost::system::system_error(boost::system::error_code(), msg));
}

#endif
