#pragma once

#include <QWidget>

#include "SocketConfig.h"

class ClientWidget : public QWidget
{
	Q_OBJECT

public:
	ClientWidget( SocketConfig * config, QWidget * parent = nullptr );
	~ClientWidget();

public:
	virtual void connected() = 0;
	virtual void disconnected() = 0;

signals:
	void socketConnect();
	void socketDisconnect();
	void socketChanged( SocketConfig * config );

public:
	SocketConfig * config() const;

private:
	SocketConfig * _config;
};
