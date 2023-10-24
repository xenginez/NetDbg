#pragma once

#include "ClientWidget.h"
#include "ui_TcpClientWidget.h"

class TcpClientWidget : public ClientWidget
{
	Q_OBJECT

public:
	TcpClientWidget( SocketConfig * config, QWidget * parent = nullptr );
	~TcpClientWidget();

public:
	void connected() override;
	void disconnected() override;

public slots:
	void on_pushButton_connect_clicked();
	void on_pushButton_clear_count_clicked();

	void on_pushButton_send_send_clicked();
	void on_pushButton_send_clear_clicked();

	void on_pushButton_recv_save_clicked();
	void on_pushButton_recv_clear_clicked();

	void on_checkBox_auto_send_clicked( bool clicked );
	void on_checkBox_auto_connect_clicked( bool clicked );
	void on_lineEdit_name_textChanged( const QString & text );
	void on_lineEdit_target_ip_textChanged( const QString & text );
	void on_lineEdit_target_port_textChanged( const QString & text );
	void on_lineEdit_send_interval_interval_textChanged( const QString & text );

private:
	Ui::TcpClientWidget ui;
};
