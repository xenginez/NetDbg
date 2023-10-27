#include "SocketConfig.h"

SocketConfig::SocketConfig(QObject *parent)
	: QObject(parent)
{
}

SocketConfig::SocketConfig( const SocketConfig & other )
	: QObject( other.parent() )
{
	name = other.name;
	host = other.host;
	port = other.port;
	interval = other.interval;
	auto_send = other.auto_send;
	auto_connect = other.auto_connect;
	auto_disconnect = other.auto_disconnect;
}

SocketConfig::~SocketConfig()
{
}
