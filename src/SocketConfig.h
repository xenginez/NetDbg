#pragma once

#include <QObject>

class SocketConfig : public QObject
{
	Q_OBJECT

public:
	SocketConfig( QObject * parent );
	SocketConfig( const SocketConfig & other );
	~SocketConfig();

public:
	QString name;
	QString host = "127.0.0.1";
	quint16 port = 48888;
	int interval = 0;
	bool auto_send = false;
	bool auto_connect = false;
	bool auto_disconnect = false;
};
