#pragma once

#include <QWidget>

#include "SocketConfig.h"

class ServerWidget : public QWidget
{
	Q_OBJECT

public:
	ServerWidget( SocketConfig * config, QWidget * parent = nullptr );
	~ServerWidget();

public:
	virtual void listener() = 0;
	virtual void disconnected() = 0;

signals:
	void socketListener();
	void socketDisconnect();
	void socketChanged( SocketConfig * config );

public:
	SocketConfig * config() const;

private:
	SocketConfig * _config;
};
