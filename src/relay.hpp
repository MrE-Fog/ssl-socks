#ifndef _GROXY_RELAY_HPP
#define _GROXY_RELAY_HPP
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <queue>
#include <memory>
#include <cstdint>
#include "gfwlist.hpp"
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace logging = boost::log;
namespace keywords = boost::log::keywords;

namespace asio = boost::asio;
typedef asio::detail::socket_option::boolean<SOL_IP, IP_TRANSPARENT> _ip_transparent_t;

//namespace ip = boost::asio::ip;
using boost::asio::ip::tcp;
using boost::asio::ip::udp;

namespace ssl = boost::asio::ssl;
typedef ssl::stream<tcp::socket> ssl_socket;

const int READ_BUFFER_SIZE = 4096;
const int TIMEOUT = 10;
enum server_type {
	LOCAL_SERVER,
	REMOTE_SERVER,
	LOCAL_TRANSPARENT
};
class raw_tcp;
class ssl_relay;

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

ssl::context init_ssl(const relay_config &config);

class relay_data
{

public:
	enum command {
		STOP_RELAY,
		START_TCP,
		START_UDP,
		DATA_RELAY,
		KEEP_RELAY
	};
	enum stop_src {
		from_ssl,
		from_raw,
		ssl_err
	};

	struct _header_t {
		uint32_t _session;
		command _cmd;
		std::size_t _len;
		_header_t(uint32_t session, command cmd, std::size_t len) :_session(session), _cmd(cmd), _len(len) {
		}
	};
private:
	_header_t _header;
	uint8_t _data[READ_BUFFER_SIZE];

public:
	explicit relay_data(uint32_t session) :_header(session, DATA_RELAY, READ_BUFFER_SIZE), _data{0} {
	}

	relay_data(uint32_t session, command cmd) : _header(session, cmd, 0), _data{0} {

	}

	_header_t & head() {
		return _header;
	}
	auto session() {
//		auto hd = (_header_t*)&_data[0];
		return _header._session;
	}
	auto cmd() {
//		auto hd = (_header_t*)&_data[0];
		return _header._cmd;
	}

	auto header_buffer() {
		return asio::buffer(&_header, sizeof(_header_t));
	}
	auto data_buffer() {
		//return asio::buffer(&_data[sizeof(_header_t)], _header._len);
		return asio::buffer(_data, _header._len);
	}
	auto buffers() {
		//return asio::buffer(&_header, sizeof(_header_t)+_header._len);
		return std::array<asio::mutable_buffer, 2> { header_buffer(), data_buffer() };
//asio::buffer(&_header, sizeof(_header_t)), asio::buffer(_data)} ;
	}
	void resize(std::size_t data_len) {
		_header._len = data_len;
//		_data.resize(data_len);
	}
	auto header_size() {
		return sizeof(_header_t);
	}
	auto data_size() {
		return _header._len;
	}
	auto size() {
		return _header._len + sizeof(_header_t);
	}
private:


};

// raw relay , base class for raw_tcp and raw_udp
class raw_relay
    :public std::enable_shared_from_this<raw_relay>
{
public:
	raw_relay(asio::io_context *io, const std::shared_ptr<ssl_relay> &manager, uint32_t session = 0);
	virtual ~raw_relay();

	uint32_t session();
	void session(uint32_t id);
	asio::strand<asio::io_context::executor_type> & strand();
    std::shared_ptr<ssl_relay> & manager();

	void stop_raw_relay(relay_data::stop_src);
	void send_data_on_raw(std::shared_ptr<relay_data> buf);
	void send_next_data();
	virtual void stop_this_relay() = 0;
    virtual void start_raw_send(std::shared_ptr<relay_data> buf) = 0;
protected:
    struct raw_impl;
    std::unique_ptr<raw_impl> _impl;

};

// class base_relay
//	:public std::enable_shared_from_this<base_relay>
// {

// }

inline void throw_err_msg(const std::string &msg)
{
	throw(boost::system::system_error(boost::system::error_code(), msg));
}

#endif
