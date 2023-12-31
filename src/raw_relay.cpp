#include <sstream>
#include <iomanip>
#include <boost/format.hpp>
#include <boost/asio/spawn.hpp>
#include "raw_relay.hpp"
#include "ssl_relay.hpp"

struct raw_relay::raw_impl
{
	uint32_t _session;
	std::weak_ptr<ssl_relay> _manager;
};

raw_relay::raw_relay(asio::io_context &io, server_type type, const std::string &host, const std::string &service) :
    base_relay(io, type, host, service), _impl(std::make_unique<raw_relay::raw_impl> ())
{}

raw_relay::~raw_relay() = default;

uint32_t raw_relay::session()
{
    return _impl->_session;
}
void raw_relay::session(uint32_t id)
{
    _impl->_session = id;
}

std::shared_ptr<ssl_relay> raw_relay::manager()
{
    auto val = _impl->_manager.lock();
    if (!val) {
        throw_err_msg("manager invalid");
    }
    return val;
}
void raw_relay::manager(const std::shared_ptr<ssl_relay> mngr)
{
    _impl->_manager = mngr;
}
