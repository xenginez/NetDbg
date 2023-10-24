#include "UdpServerWidget.h"

#include <QFile>
#include <QTimer>
#include <QUdpSocket>
#include <QFileDialog>
#include <QHostAddress>
#include <QNetworkInterface>

UdpSession::UdpSession( QUdpSocket * socket, const QHostAddress & addr, quint16 port, QObject * parent )
	: QObject( parent ), _socket( socket ), _addr( addr ), _port( port )
{
}

quint16 UdpSession::peerPort() const
{
	return _port;
}

const QHostAddress & UdpSession::peerAddress() const
{
	return _addr;
}

void UdpSession::write( const QByteArray & data )
{
	_socket->writeDatagram( data, _addr, _port );
}

void UdpSession::close()
{
	emit disconnected();
}

UdpServerWidget::UdpServerWidget( SocketConfig * config, QWidget * parent )
	: ServerWidget( config, parent )
{
	ui.setupUi( this );

	QList<QHostAddress> ips = QNetworkInterface::allAddresses();
	std::sort( ips.begin(), ips.end(), [](const auto & left, const auto & right ) { return left.toString() < right.toString(); } );
	for( auto it : ips )
	{
		if( it.protocol() == QAbstractSocket::NetworkLayerProtocol::IPv4Protocol )
			ui.comboBox_ip->addItem( it.toString() );
	}
	ui.lineEdit_port->setValidator( new QIntValidator( 1, 65535 ) );

	ui.lineEdit_name->setText( config->name );
	ui.comboBox_ip->setCurrentText( config->host );
	ui.lineEdit_port->setText( QString::number( config->port ) );
	ui.checkBox_auto_disconnect->setChecked( config->auto_disconnect );
	ui.lineEdit_disconnect_interval->setText( QString::number( config->interval ) );
}

UdpServerWidget::~UdpServerWidget()
{
}

void UdpServerWidget::listener()
{
	auto server = property( "server" ).value<QUdpSocket *>();
	if( server == nullptr )
	{
		on_pushButton_listener_clicked();
	}
}

void UdpServerWidget::disconnected()
{
	auto server = property( "server" ).value<QUdpSocket *>();
	if( server != nullptr )
	{
		on_pushButton_listener_clicked();
	}
}

void UdpServerWidget::on_pushButton_listener_clicked()
{
	auto server = property( "server" ).value<QUdpSocket *>();
	if( server != nullptr )
	{
		setProperty( "server", QVariant::fromValue( ( QUdpSocket * )nullptr ) );

		server->deleteLater();

		emit socketDisconnect();
	}

	if( ui.pushButton_listener->text() == QString::fromLocal8Bit( "开始监听" ) )
	{
		server = new QUdpSocket( this );

		if( server->bind( QHostAddress( ui.comboBox_ip->currentText() ), ui.lineEdit_port->text().toInt() ) )
		{
			setProperty( "server", QVariant::fromValue( server ) );

			connect( server, &QUdpSocket::readyRead, [this, server]()
			{
				if( server->hasPendingDatagrams() )
				{
					char buf[1024];
					QHostAddress addr;
					quint16 port;
					auto len = server->readDatagram( buf, 1024, &addr, &port );

					UdpSession * socket = nullptr;
					for( int i = 0; i < ui.listWidget_clients->count(); i++ )
					{
						auto item = ui.listWidget_clients->item( i );
						auto session = item->data( Qt::UserRole + 1 ).value<UdpSession *>();
						if( session != nullptr && session->peerAddress() == addr && session->peerPort() == port )
						{
							socket = session;
							break;
						}
					}

					if( socket == nullptr )
					{
						socket = new UdpSession( server, addr, port, this );

						socket->setProperty( "hex", false );
						socket->setProperty( "hide", false );
						socket->setProperty( "recv", false );
						socket->setProperty( "buffer", QByteArray() );

						auto item = new QListWidgetItem( QString( "%1:%2" ).arg( socket->peerAddress().toString() ).arg( socket->peerPort() ) );
						item->setData( Qt::UserRole + 1, QVariant::fromValue( socket ) );
						ui.listWidget_clients->addItem( item );

						if( ui.checkBox_auto_disconnect->isChecked() )
						{
							QTimer::singleShot( ui.lineEdit_disconnect_interval->text().toInt() * 1000, [this, item, socket]()
							{
								if( ui.checkBox_auto_disconnect->isChecked() )
								{
									socket->close();
								}
							} );
						}

						connect( socket, &UdpSession::disconnected, [this, item, socket]()
						{
							socket->deleteLater();

							delete ui.listWidget_clients->takeItem( ui.listWidget_clients->row( item ) );
						} );
					}
					else
					{
						auto data = QByteArray::fromRawData( buf, len );
						auto buffer = socket->property( "buffer" ).toByteArray();
						if( !buffer.isEmpty() ) buffer.append( '\n' );

						buffer.append( data );
						socket->setProperty( "buffer", buffer );

						auto item = ui.listWidget_clients->currentItem();
						if( item != nullptr && item->data( Qt::UserRole + 1 ).value<UdpSession *>() == socket )
						{
							if( !ui.checkBox_recv_hide->isChecked() )
							{
								ui.textEdit_recv->append( ui.checkBox_recv_16->isChecked() ? data.toHex() : data );
							}
						}

						if( ui.checkBox_send_recv->isChecked() )
						{
							socket->write( data );
						}
					}
				}
			} );

			ui.pushButton_listener->setText( QString::fromLocal8Bit( "停止监听" ) );
			ui.comboBox_ip->setEnabled( true );
			ui.lineEdit_port->setEnabled( true );
			ui.checkBox_auto_disconnect->setEnabled( true );
			ui.lineEdit_disconnect_interval->setEnabled( true );

			emit socketListener();
		}
		else
		{
			server->deleteLater();
		}
	}
	else
	{
		ui.pushButton_listener->setText( QString::fromLocal8Bit( "开始监听" ) );
		ui.comboBox_ip->setEnabled( true );
		ui.lineEdit_port->setEnabled( true );
		ui.checkBox_auto_disconnect->setEnabled( true );
		ui.lineEdit_disconnect_interval->setEnabled( true );
	}
}

void UdpServerWidget::on_listWidget_clients_currentItemChanged( QListWidgetItem * current, QListWidgetItem * previous )
{
	if( previous != nullptr )
	{
		auto socket = previous->data( Qt::UserRole + 1 ).value<UdpSession *>();
		if( socket != nullptr )
		{
			socket->setProperty( "hex", ui.checkBox_recv_16->isChecked() );
			socket->setProperty( "hide", ui.checkBox_recv_hide->isChecked() );
			socket->setProperty( "recv", ui.checkBox_send_recv->isChecked() );
		}
	}
	if( current != nullptr )
	{
		auto socket = current->data( Qt::UserRole + 1 ).value<UdpSession *>();

		ui.checkBox_recv_16->setChecked( socket->property( "hex" ).toBool() );
		ui.checkBox_recv_hide->setChecked( socket->property( "hide" ).toBool() );
		ui.checkBox_send_recv->setChecked( socket->property( "recv" ).toBool() );

		auto data = socket->property( "buffer" ).toByteArray();
		ui.textEdit_recv->append( socket->property( "hex" ).toBool() ? data.toHex() : data );

		ui.groupBox_send->setTitle( QString::fromLocal8Bit( "发送区（%1:%2）" ).arg( socket->peerAddress().toString() ).arg( socket->peerPort() ) );
		ui.groupBox_recv->setTitle( QString::fromLocal8Bit( "接收区（%1:%2）" ).arg( socket->peerAddress().toString() ).arg( socket->peerPort() ) );
	}
}

void UdpServerWidget::on_pushButton_send_send_clicked()
{
	auto item = ui.listWidget_clients->currentItem();
	if( item != nullptr )
	{
		auto socket = item->data( Qt::UserRole + 1 ).value<UdpSession *>();

		auto data = ui.textEdit_send->toPlainText().toLatin1();
		if( ui.checkBox_send_16->isChecked() )
		{
			data = QByteArray::fromHex( data );
		}

		socket->write( data );
	}
}

void UdpServerWidget::on_pushButton_send_clear_clicked()
{
	ui.textEdit_send->clear();
}

void UdpServerWidget::on_pushButton_recv_save_clicked()
{
	auto item = ui.listWidget_clients->currentItem();
	if( item != nullptr )
	{
		auto socket = item->data( Qt::UserRole + 1 ).value<UdpSession *>();

		auto path = QFileDialog::getSaveFileName( this, QString::fromLocal8Bit( "保存到文件" ) );
		if( !path.isEmpty() )
		{
			QFile file( path );
			file.open( QIODevice::WriteOnly );
			file.write( socket->property( "buffer" ).toByteArray() );
		}
	}
}

void UdpServerWidget::on_pushButton_recv_clear_clicked()
{
	auto item = ui.listWidget_clients->currentItem();
	if( item != nullptr )
	{
		auto socket = item->data( Qt::UserRole + 1 ).value<UdpSession *>();

		socket->setProperty( "buffer", QByteArray() );
	}

	ui.textEdit_recv->clear();
}

void UdpServerWidget::on_checkBox_auto_disconnect_clicked( bool clicked )
{
	config()->auto_disconnect = clicked;
	emit socketChanged( config() );
}

void UdpServerWidget::on_lineEdit_name_textChanged( const QString & text )
{
	config()->name = text;
	emit socketChanged( config() );
}

void UdpServerWidget::on_lineEdit_port_textChanged( const QString & text )
{
	config()->port = text.toUShort();
	emit socketChanged( config() );
}

void UdpServerWidget::on_comboBox_ip_currentTextChanged( const QString & text )
{
	config()->host = text;
	emit socketChanged( config() );
}

void UdpServerWidget::on_lineEdit_disconnect_interval_textChanged( const QString & text )
{
	config()->interval = text.toInt();
	emit socketChanged( config() );
}
