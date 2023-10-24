#include "TcpClientWidget.h"

#include <QFile>
#include <QTimer>
#include <QTcpSocket>
#include <QFileDialog>
#include <QHostAddress>
#include <QIntValidator>
#include <QRegExpValidator>

TcpClientWidget::TcpClientWidget( SocketConfig * config, QWidget * parent )
	: ClientWidget( config, parent )
{
	ui.setupUi( this );

	ui.lineEdit_target_ip->setValidator( new QRegExpValidator( QRegExp( "\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b" ) ) );
	ui.lineEdit_target_port->setValidator( new QIntValidator( 1, 65535 ) );
	ui.lineEdit_send_interval->setValidator( new QIntValidator( 0, 99999 ) );

	ui.lineEdit_name->setText( config->name );
	ui.lineEdit_target_ip->setText( config->host );
	ui.lineEdit_target_port->setText( QString::number( config->port ) );
	ui.checkBox_auto_send->setChecked( config->auto_send );
	ui.checkBox_auto_connect->setChecked( config->auto_connect );
	ui.lineEdit_send_interval->setText( QString::number( config->interval ) );
}

TcpClientWidget::~TcpClientWidget()
{
}

void TcpClientWidget::connected()
{
	QTcpSocket * socket = property( "socket" ).value<QTcpSocket *>();
	if( socket == nullptr )
	{
		on_pushButton_connect_clicked();
	}
}

void TcpClientWidget::disconnected()
{
	QTcpSocket * socket = property( "socket" ).value<QTcpSocket *>();
	if( socket != nullptr )
	{
		on_pushButton_connect_clicked();
	}
}

void TcpClientWidget::on_pushButton_connect_clicked()
{
	QTcpSocket * socket = property( "socket" ).value<QTcpSocket *>();
	if( socket != nullptr )
	{
		setProperty( "socket", QVariant::fromValue( ( QTcpSocket *)nullptr ) );

		socket->deleteLater();

		emit socketDisconnect();
	}
	QTimer * timer = property( "timer" ).value<QTimer *>();
	if( timer != nullptr )
	{
		setProperty( "timer", QVariant::fromValue( ( QTimer * )nullptr ) );

		timer->deleteLater();
	}

	if( ui.pushButton_connect->text() == QString::fromLocal8Bit( "连接" ) )
	{
		socket = new QTcpSocket( this );

		connect( socket, &QTcpSocket::readyRead, [this, socket]()
		{
			auto bytes = socket->readAll();

			ui.label_recv_count->setText( QString::number( ui.label_recv_count->text().toInt() + bytes.size() ) );

			if( !ui.checkBox_recv_hide->isChecked() )
			{
				if( ui.checkBox_recv_16->isChecked() )
				{
					bytes = bytes.toHex();
				}

				ui.textEdit_recv->append( bytes );
			}

			if( ui.checkBox_send_recv->isChecked() )
			{
				ui.textEdit_send->setPlainText( bytes );
			}
		} );
		connect( socket, &QTcpSocket::connected, [this, socket]()
		{
			setProperty( "socket", QVariant::fromValue( socket ) );

			ui.label_recv_count->setText( "0" );
			ui.label_send_count->setText( "0" );
			ui.pushButton_connect->setEnabled( true );
			ui.pushButton_connect->setText( QString::fromLocal8Bit( "断开" ) );

			if( ui.checkBox_auto_send->isChecked() )
			{
				if( ui.lineEdit_send_interval->text().toInt() == 0 )
				{
					auto bytes = ui.textEdit_send->toPlainText().toLatin1();
					if( ui.checkBox_send_16->isChecked() )
					{
						bytes = QByteArray::fromHex( bytes );
					}
					socket->write( bytes );
					ui.label_send_count->setText( QString::number( ui.label_send_count->text().toInt() + bytes.size() ) );
				}
				else
				{
					QTimer * timer = new QTimer( this );
					setProperty( "timer", QVariant::fromValue( timer ) );
					timer->start( ui.lineEdit_send_interval->text().toInt() * 1000 );
					connect( timer, &QTimer::timeout, [this, socket]()
					{
						auto bytes = ui.textEdit_send->toPlainText().toLatin1();
						if( ui.checkBox_send_16->isChecked() )
						{
							bytes = QByteArray::fromHex( bytes );
						}
						socket->write( bytes );
						ui.label_send_count->setText( QString::number( ui.label_send_count->text().toInt() + bytes.size() ) );
					} );
				}
			}

			emit socketConnect();
		} );
		connect( socket, &QTcpSocket::disconnected, [this]()
		{
			disconnected();
		} );
		connect( socket, ( void( QTcpSocket:: * )( QAbstractSocket::SocketError ) ) & QTcpSocket::error, [this, socket]()
		{
			switch( socket->state() )
			{
			case QAbstractSocket::UnconnectedState:
			case QAbstractSocket::HostLookupState:
			case QAbstractSocket::ConnectingState:
				socket->deleteLater();
				on_pushButton_connect_clicked();
				break;
			case QAbstractSocket::ConnectedState:
			case QAbstractSocket::BoundState:
			case QAbstractSocket::ListeningState:
			case QAbstractSocket::ClosingState:
				break;
			}
		} );

		ui.pushButton_connect->setText( QString::fromLocal8Bit( "连接中" ) );
		ui.pushButton_connect->setEnabled( false );
		ui.lineEdit_target_ip->setEnabled( false );
		ui.lineEdit_target_port->setEnabled( false );
		ui.checkBox_auto_connect->setEnabled( false );
		ui.checkBox_auto_send->setEnabled( false );
		ui.lineEdit_send_interval->setEnabled( false );
		
		socket->connectToHost( QHostAddress( ui.lineEdit_target_ip->text() ), ui.lineEdit_target_port->text().toInt() );
	}
	else
	{
		ui.pushButton_connect->setText( QString::fromLocal8Bit( "连接" ) );
		ui.pushButton_connect->setEnabled( true );
		ui.lineEdit_target_ip->setEnabled( true );
		ui.lineEdit_target_port->setEnabled( true );
		ui.checkBox_auto_connect->setEnabled( true );
		ui.checkBox_auto_send->setEnabled( true );
		ui.checkBox_auto_send->setEnabled( true );
		ui.lineEdit_send_interval->setEnabled( true );
	}
}

void TcpClientWidget::on_pushButton_clear_count_clicked()
{
	ui.label_send_count->setText( "0" );
	ui.label_recv_count->setText( "0" );
}

void TcpClientWidget::on_pushButton_send_send_clicked()
{
	auto socket = property( "socket" ).value<QTcpSocket *>();
	if( socket != nullptr )
	{
		auto bytes = ui.textEdit_send->toPlainText().toLatin1();
		if( ui.checkBox_send_16->isChecked() )
		{
			bytes = QByteArray::fromHex( bytes );
		}
		socket->write( bytes );
		ui.label_send_count->setText( QString::number( ui.label_send_count->text().toInt() + bytes.size() ) );
	}
}

void TcpClientWidget::on_pushButton_send_clear_clicked()
{
	ui.textEdit_send->clear();
}

void TcpClientWidget::on_pushButton_recv_save_clicked()
{
	QString path = QFileDialog::getSaveFileName( this, QString::fromLocal8Bit( "保存到文件" ) );
	if( !path.isEmpty() )
	{
		QFile file( path );
		file.open( QIODevice::WriteOnly );
		file.write( ui.textEdit_recv->toPlainText().toLatin1() );
	}
}

void TcpClientWidget::on_pushButton_recv_clear_clicked()
{
	ui.textEdit_recv->clear();
}

void TcpClientWidget::on_checkBox_auto_send_clicked( bool clicked )
{
	config()->auto_send = clicked;
	emit socketChanged( config() );
}

void TcpClientWidget::on_checkBox_auto_connect_clicked( bool clicked )
{
	config()->auto_connect = clicked;
	emit socketChanged( config() );
}

void TcpClientWidget::on_lineEdit_name_textChanged( const QString & text )
{
	config()->name = text;
	emit socketChanged( config() );
}

void TcpClientWidget::on_lineEdit_target_ip_textChanged( const QString & text )
{
	config()->host = text;
	emit socketChanged( config() );
}

void TcpClientWidget::on_lineEdit_target_port_textChanged( const QString & text )
{
	config()->port = text.toUShort();
	emit socketChanged( config() );
}

void TcpClientWidget::on_lineEdit_send_interval_interval_textChanged( const QString & text )
{
	config()->interval = text.toInt();
	emit socketChanged( config() );
}
