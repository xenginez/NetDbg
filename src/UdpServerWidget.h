#pragma once

#include <QHostAddress>

#include "ServerWidget.h"
#include "ui_UdpServerWidget.h"

class QUdpSocket;

class UdpSession : public QObject
{
	Q_OBJECT

public:
	UdpSession( QUdpSocket * socket, const QHostAddress & addr, quint16 port, QObject * parent );

public:
	quint16 peerPort() const;
	const QHostAddress & peerAddress() const;

public:
	void write( const QByteArray & data );
	void close();

signals:
	void disconnected();

private:
	quint16 _port;
	QHostAddress _addr;
	QUdpSocket * _socket;
};

class UdpServerWidget : public ServerWidget
{
	Q_OBJECT

public:
	UdpServerWidget( SocketConfig * config, QWidget * parent = nullptr );
	~UdpServerWidget();

public:
	void listener() override;
	void disconnected() override;

public slots:
	void on_pushButton_listener_clicked();
	void on_listWidget_clients_currentItemChanged( QListWidgetItem * current, QListWidgetItem * previous );

	void on_pushButton_send_send_clicked();
	void on_pushButton_send_clear_clicked();

	void on_pushButton_recv_save_clicked();
	void on_pushButton_recv_clear_clicked();

	void on_checkBox_auto_disconnect_clicked( bool clicked );
	void on_lineEdit_port_textChanged( const QString & text );
	void on_lineEdit_name_textChanged( const QString & text );
	void on_comboBox_ip_currentTextChanged( const QString & text );
	void on_lineEdit_disconnect_interval_textChanged( const QString & text );

private:
	Ui::UdpServerWidget ui;
};
