#pragma once

#include "ServerWidget.h"
#include "ui_TcpServerWidget.h"

class TcpServerWidget : public ServerWidget
{
	Q_OBJECT

public:
	TcpServerWidget( SocketConfig * config, QWidget * parent = nullptr );
	~TcpServerWidget();

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
	Ui::TcpServerWidget ui;
};
