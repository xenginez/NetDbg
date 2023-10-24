#include "MainWindow.h"

#include <QMenu>
#include <QStyle>
#include <QSettings>
#include <QApplication>

#include "SocketConfig.h"

#define CLIENT_TAB_INDEX 0
#define SERVER_TAB_INDEX 1
#define TCP_ITEM( TREE ) TREE->topLevelItem( 0 )
#define UDP_ITEM( TREE ) TREE->topLevelItem( 1 )

TUG::TUG( QWidget * parent )
    : QMainWindow( parent )
{
    ui.setupUi( this );

    ui.splitter_client->setSizes( { 30000, 70000 } );
    ui.splitter_server->setSizes( { 30000, 70000 } );

    load();
}

TUG::~TUG()
{
}

void TUG::on_treeWidget_client_customContextMenuRequested( const QPoint & pos )
{
    auto item = ui.treeWidget_client->itemAt( pos );
    if( item != nullptr )
    {
        if( item->text( 0 ) == "TCP" )
        {
            QMenu menu( this );
            {
                menu.addAction( QString::fromLocal8Bit( "创建TCP" ), [this]()
                {
                    createTcpClient( new SocketConfig( this ), nullptr);
                } );
                menu.addAction( QString::fromLocal8Bit( "连接所有" ), [this]()
                {
                    connectAllTcpClient();
                } );
                menu.addAction( QString::fromLocal8Bit( "断开所有" ), [this]()
                {
                    disconnectAllTcpClient();
                } );
                menu.addAction( QString::fromLocal8Bit( "删除所有" ), [this]()
                {
                    auto tcp = TCP_ITEM( ui.treeWidget_client );
                    while( tcp->childCount() > 0 )
                    {
                        auto item = tcp->child( 0 );
                        auto widget = item->data( 0, Qt::UserRole + 1 ).value<TcpClientWidget *>();
                        if( widget != nullptr )
                        {
                            widget->disconnected();
                        }

                        for( int j = 0; j < ui.tabWidget_client->count(); j++ )
                        {
                            if( ui.tabWidget_client->widget( j ) == widget )
                            {
                                ui.tabWidget_client->removeTab( j );
                            }
                        }

                        delete tcp->takeChild( tcp->indexOfChild( item ) );
                    }
                } );
            }
            menu.exec( QCursor::pos() );
        }
        else if( item->text( 0 ) == "UDP" )
        {
            QMenu menu( this );
            {
                menu.addAction( QString::fromLocal8Bit( "创建UDP" ), [this]()
                {
                    createUdpClient( new SocketConfig( this ), nullptr );
                } );
                menu.addAction( QString::fromLocal8Bit( "连接所有" ), [this]()
                {
                    connectAllUdpClient();
                } );
                menu.addAction( QString::fromLocal8Bit( "断开所有" ), [this]()
                {
                    disconnectAllUdpClient();
                } );
                menu.addAction( QString::fromLocal8Bit( "删除所有" ), [this]()
                {
                    auto udp = UDP_ITEM( ui.treeWidget_client );
                    while( udp->childCount() > 0 )
                    {
                        auto item = udp->child( 0 );
                        auto widget = item->data( 0, Qt::UserRole + 1 ).value<UdpClientWidget *>();
                        if( widget != nullptr )
                        {
                            widget->disconnected();
                        }

                        for( int j = 0; j < ui.tabWidget_client->count(); j++ )
                        {
                            if( ui.tabWidget_client->widget( j ) == widget )
                            {
                                ui.tabWidget_client->removeTab( j );
                            }
                        }

                        delete udp->takeChild( udp->indexOfChild( item ) );
                    }
                } );
            }
            menu.exec( QCursor::pos() );
        }
        else if( item->parent()->text( 0 ) == "TCP" )
        {
            QMenu menu( this );
            {
                menu.addAction( QString::fromLocal8Bit( "连接" ), [this, item]()
                {
                    connectTcpClient( item );
                } );
                menu.addAction( QString::fromLocal8Bit( "断开" ), [this, item]()
                {
                    disconnectTcpClient( item );
                } );
                menu.addAction( QString::fromLocal8Bit( "删除" ), [this, item]()
                {
                    auto widget = item->data( 0, Qt::UserRole + 1 ).value<TcpClientWidget *>();
                    if( widget != nullptr )
                    {
                        widget->disconnected();
                    }

                    for( int j = 0; j < ui.tabWidget_client->count(); j++ )
                    {
                        if( ui.tabWidget_client->widget( j ) == widget )
                        {
                            ui.tabWidget_client->removeTab( j );
                            break;
                        }
                    }

                    delete item->parent()->takeChild( item->parent()->indexOfChild( item ) );
                } );
            }
            menu.exec( QCursor::pos() );
        }
        else if( item->parent()->text( 0 ) == "UDP" )
        {
            QMenu menu( this );
            {
                menu.addAction( QString::fromLocal8Bit( "连接" ), [this, item]()
                {
                    connectUdpClient( item );
                } );
                menu.addAction( QString::fromLocal8Bit( "断开" ), [this, item]()
                {
                    disconnectUdpClient( item );
                } );
                menu.addAction( QString::fromLocal8Bit( "删除" ), [this, item]()
                {
                    auto widget = item->data( 0, Qt::UserRole + 1 ).value<UdpClientWidget *>();
                    if( widget != nullptr )
                    {
                        widget->disconnected();
                    }

                    for( int j = 0; j < ui.tabWidget_client->count(); j++ )
                    {
                        if( ui.tabWidget_client->widget( j ) == widget )
                        {
                            ui.tabWidget_client->removeTab( j );
                            break;
                        }
                    }

                    delete item->parent()->takeChild( item->parent()->indexOfChild( item ) );
                } );
            }
            menu.exec( QCursor::pos() );
        }
    }
}

void TUG::on_treeWidget_server_customContextMenuRequested( const QPoint & pos )
{
    auto item = ui.treeWidget_server->itemAt( pos );
    if( item != nullptr )
    {
        if( item->text( 0 ) == "TCP" )
        {
            QMenu menu( this );
            {
                menu.addAction( QString::fromLocal8Bit( "创建TCP" ), [this]()
                {
                    createTcpServer( new SocketConfig( this ), nullptr );
                } );
                menu.addAction( QString::fromLocal8Bit( "监听所有" ), [this]()
                {
                    connectAllTcpServer();
                } );
                menu.addAction( QString::fromLocal8Bit( "断开所有" ), [this]()
                {
                    disconnectAllTcpServer();
                } );
                menu.addAction( QString::fromLocal8Bit( "删除所有" ), [this]()
                {
                    auto tcp = TCP_ITEM( ui.treeWidget_server );
                    while( tcp->childCount() > 0 )
                    {
                        auto item = tcp->child( 0 );
                        auto widget = item->data( 0, Qt::UserRole + 1 ).value<TcpServerWidget *>();
                        if( widget != nullptr )
                        {
                            widget->disconnected();
                        }

                        for( int j = 0; j < ui.tabWidget_server->count(); j++ )
                        {
                            if( ui.tabWidget_server->widget( j ) == widget )
                            {
                                ui.tabWidget_server->removeTab( j );
                            }
                        }

                        delete tcp->takeChild( tcp->indexOfChild( item ) );
                    }
                } );
            }
            menu.exec( QCursor::pos() );
        }
        else if( item->text( 0 ) == "UDP" )
        {
            QMenu menu( this );
            {
                menu.addAction( QString::fromLocal8Bit( "创建UDP" ), [this]()
                {
                    createUdpServer( new SocketConfig( this ), nullptr );
                } );
                menu.addAction( QString::fromLocal8Bit( "监听所有" ), [this]()
                {
                    connectAllUdpServer();
                } );
                menu.addAction( QString::fromLocal8Bit( "断开所有" ), [this]()
                {
                    disconnectAllUdpServer();
                } );
                menu.addAction( QString::fromLocal8Bit( "删除所有" ), [this]()
                {
                    auto udp = UDP_ITEM( ui.treeWidget_server );
                    while( udp->childCount() > 0 )
                    {
                        auto item = udp->child( 0 );
                        auto widget = item->data( 0, Qt::UserRole + 1 ).value<UdpServerWidget *>();
                        if( widget != nullptr )
                        {
                            widget->disconnected();
                        }

                        for( int j = 0; j < ui.tabWidget_server->count(); j++ )
                        {
                            if( ui.tabWidget_server->widget( j ) == widget )
                            {
                                ui.tabWidget_server->removeTab( j );
                            }
                        }

                        delete udp->takeChild( udp->indexOfChild( item ) );
                    }
                } );
            }
            menu.exec( QCursor::pos() );
        }
        else if( item->parent()->text( 0 ) == "TCP" )
        {
            QMenu menu( this );
            {
                menu.addAction( QString::fromLocal8Bit( "监听" ), [this, item]()
                {
                    connectTcpServer( item );
                } );
                menu.addAction( QString::fromLocal8Bit( "断开" ), [this, item]()
                {
                    disconnectTcpServer( item );
                } );
                menu.addAction( QString::fromLocal8Bit( "删除" ), [this, item]()
                {
                    auto widget = item->data( 0, Qt::UserRole + 1 ).value<TcpServerWidget *>();
                    if( widget != nullptr )
                    {
                        widget->disconnected();
                    }

                    for( int j = 0; j < ui.tabWidget_server->count(); j++ )
                    {
                        if( ui.tabWidget_server->widget( j ) == widget )
                        {
                            ui.tabWidget_server->removeTab( j );
                            break;
                        }
                    }

                    delete item->parent()->takeChild( item->parent()->indexOfChild( item ) );
                } );
            }
            menu.exec( QCursor::pos() );
        }
        else if( item->parent()->text( 0 ) == "UDP" )
        {
            QMenu menu( this );
            {
                menu.addAction( QString::fromLocal8Bit( "监听" ), [this, item]()
                {
                    connectUdpServer( item );
                } );
                menu.addAction( QString::fromLocal8Bit( "断开" ), [this, item]()
                {
                    disconnectUdpServer( item );
                } );
                menu.addAction( QString::fromLocal8Bit( "删除" ), [this, item]()
                {
                    auto widget = item->data( 0, Qt::UserRole + 1 ).value<UdpServerWidget *>();
                    if( widget != nullptr )
                    {
                        widget->disconnected();
                    }

                    for( int j = 0; j < ui.tabWidget_server->count(); j++ )
                    {
                        if( ui.tabWidget_server->widget( j ) == widget )
                        {
                            ui.tabWidget_server->removeTab( j );
                            break;
                        }
                    }

                    delete item->parent()->takeChild( item->parent()->indexOfChild( item ) );
                } );
            }
            menu.exec( QCursor::pos() );
        }
    }
}

void TUG::on_treeWidget_client_itemDoubleClicked( QTreeWidgetItem * item, int column )
{
    if( item->parent() != nullptr )
    {
        auto widget = item->data( 0, Qt::UserRole + 1 ).value<QWidget *>();
        if( widget != nullptr )
        {
            for( int i = 0; i < ui.tabWidget_client->count(); i++ )
            {
                if( ui.tabWidget_client->widget( i ) == widget )
                {
                    ui.tabWidget_client->setCurrentIndex( i );
                    break;
                }
            }
        }
        else if( item->parent()->text( 0 ) == "TCP" )
        {
            createTcpClient( item->data( 0, Qt::UserRole + 2 ).value<SocketConfig *>(), item );
        }
        else if( item->parent()->text( 0 ) == "UDP" )
        {
            createUdpClient( item->data( 0, Qt::UserRole + 2 ).value<SocketConfig *>(), item );
        }
    }
}

void TUG::on_treeWidget_server_itemDoubleClicked( QTreeWidgetItem * item, int column )
{
    if( item->parent() != nullptr )
    {
        auto widget = item->data( 0, Qt::UserRole + 1 ).value<QWidget *>();
        if( widget != nullptr )
        {
            for( int i = 0; i < ui.tabWidget_server->count(); i++ )
            {
                if( ui.tabWidget_server->widget( i ) == widget )
                {
                    ui.tabWidget_server->setCurrentIndex( i );
                    break;
                }
            }
        }
        else if( item->parent()->text( 0 ) == "TCP" )
        {
            createTcpServer( item->data( 0, Qt::UserRole + 2 ).value<SocketConfig *>(), item );
        }
        else if( item->parent()->text( 0 ) == "UDP" )
        {
            createUdpServer( item->data( 0, Qt::UserRole + 2 ).value<SocketConfig *>(), item );
        }
    }
}

void TUG::connectTcpClient( QTreeWidgetItem * item )
{
    auto widget = item->data( 0, Qt::UserRole + 1 ).value<TcpClientWidget *>();
    if( widget == nullptr )
    {
        widget = createTcpClient( item->data( 0, Qt::UserRole + 2 ).value<SocketConfig *>(), item );
    }
    widget->connected();
}

void TUG::connectTcpServer( QTreeWidgetItem * item )
{
    auto widget = item->data( 0, Qt::UserRole + 1 ).value<TcpServerWidget *>();
    if( widget == nullptr )
    {
        widget = createTcpServer( item->data( 0, Qt::UserRole + 2 ).value<SocketConfig *>(), item );
    }
    widget->listener();
}

void TUG::connectUdpClient( QTreeWidgetItem * item )
{
    auto widget = item->data( 0, Qt::UserRole + 1 ).value<UdpClientWidget *>();
    if( widget == nullptr )
    {
        widget = createUdpClient( item->data( 0, Qt::UserRole + 2 ).value<SocketConfig *>(), item );
    }
    widget->connected();
}

void TUG::connectUdpServer( QTreeWidgetItem * item )
{
    auto widget = item->data( 0, Qt::UserRole + 1 ).value<UdpServerWidget *>();
    if( widget == nullptr )
    {
        widget = createUdpServer( item->data( 0, Qt::UserRole + 2 ).value<SocketConfig *>(), item );
    }
    widget->listener();
}

void TUG::disconnectTcpClient( QTreeWidgetItem * item )
{
    auto widget = item->data( 0, Qt::UserRole + 1 ).value<TcpClientWidget *>();
    if( widget != nullptr )
    {
        widget->disconnected();
    }
}

void TUG::disconnectTcpServer( QTreeWidgetItem * item )
{
    auto widget = item->data( 0, Qt::UserRole + 1 ).value<TcpServerWidget *>();
    if( widget != nullptr )
    {
        widget->disconnected();
    }
}

void TUG::disconnectUdpClient( QTreeWidgetItem * item )
{
    auto widget = item->data( 0, Qt::UserRole + 1 ).value<UdpClientWidget *>();
    if( widget != nullptr )
    {
        widget->disconnected();
    }
}

void TUG::disconnectUdpServer( QTreeWidgetItem * item )
{
    auto widget = item->data( 0, Qt::UserRole + 1 ).value<UdpServerWidget *>();
    if( widget != nullptr )
    {
        widget->disconnected();
    }
}

void TUG::connectAllTcpClient()
{
    auto tcp = TCP_ITEM( ui.treeWidget_client );
    for( int i = 0; i < tcp->childCount(); i++ )
    {
        auto item = tcp->child( i );
        auto widget = item->data( 0, Qt::UserRole + 1 ).value<TcpClientWidget *>();
        if( widget == nullptr )
        {
            widget = createTcpClient( item->data( 0, Qt::UserRole + 2 ).value<SocketConfig *>(), item );
        }
        widget->connected();
    }
}

void TUG::connectAllTcpServer()
{
    auto tcp = TCP_ITEM( ui.treeWidget_server );
    for( int i = 0; i < tcp->childCount(); i++ )
    {
        auto item = tcp->child( i );
        auto widget = item->data( 0, Qt::UserRole + 1 ).value<TcpServerWidget *>();
        if( widget == nullptr )
        {
            widget = createTcpServer( item->data( 0, Qt::UserRole + 2 ).value<SocketConfig *>(), item );
        }
        widget->listener();
    }
}

void TUG::connectAllUdpClient()
{
    auto udp = UDP_ITEM( ui.treeWidget_client );
    for( int i = 0; i < udp->childCount(); i++ )
    {
        auto item = udp->child( i );
        auto widget = item->data( 0, Qt::UserRole + 1 ).value<UdpClientWidget *>();
        if( widget == nullptr )
        {
            widget = createUdpClient( item->data( 0, Qt::UserRole + 2 ).value<SocketConfig *>(), item );
        }
        widget->connected();
    }
}

void TUG::connectAllUdpServer()
{
    auto udp = UDP_ITEM( ui.treeWidget_server );
    for( int i = 0; i < udp->childCount(); i++ )
    {
        auto item = udp->child( i );
        auto widget = item->data( 0, Qt::UserRole + 1 ).value<UdpServerWidget *>();
        if( widget == nullptr )
        {
            widget = createUdpServer( item->data( 0, Qt::UserRole + 2 ).value<SocketConfig *>(), item );
        }
        widget->listener();
    }
}

void TUG::disconnectAllTcpClient()
{
    auto tcp = TCP_ITEM( ui.treeWidget_client );
    for( int i = 0; i < tcp->childCount(); i++ )
    {
        auto item = tcp->child( i );
        auto widget = item->data( 0, Qt::UserRole + 1 ).value<TcpClientWidget *>();
        if( widget != nullptr )
        {
            widget->disconnected();
        }
    }
}

void TUG::disconnectAllTcpServer()
{
    auto tcp = TCP_ITEM( ui.treeWidget_server );
    for( int i = 0; i < tcp->childCount(); i++ )
    {
        auto item = tcp->child( i );
        auto widget = item->data( 0, Qt::UserRole + 1 ).value<TcpServerWidget *>();
        if( widget != nullptr )
        {
            widget->disconnected();
        }
    }
}

void TUG::disconnectAllUdpClient()
{
    auto udp = UDP_ITEM( ui.treeWidget_client );
    for( int i = 0; i < udp->childCount(); i++ )
    {
        auto item = udp->child( i );
        auto widget = item->data( 0, Qt::UserRole + 1 ).value<UdpClientWidget *>();
        if( widget != nullptr )
        {
            widget->disconnected();
        }
    }
}

void TUG::disconnectAllUdpServer()
{
    auto udp = UDP_ITEM( ui.treeWidget_server );
    for( int i = 0; i < udp->childCount(); i++ )
    {
        auto item = udp->child( i );
        auto widget = item->data( 0, Qt::UserRole + 1 ).value<UdpServerWidget *>();
        if( widget != nullptr )
        {
            widget->disconnected();
        }
    }
}

TcpClientWidget * TUG::createTcpClient( SocketConfig * config, QTreeWidgetItem * item )
{
    auto widget = new TcpClientWidget( config, ui.tabWidget_client );
    ui.tabWidget_client->addTab( widget, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ), QString( "TCP %1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );

    if( item == nullptr )
    {
        item = new QTreeWidgetItem( TCP_ITEM( ui.treeWidget_client ) );
    }

    item->setData( 0, Qt::UserRole + 1, QVariant::fromValue( widget ) );
    item->setData( 0, Qt::UserRole + 2, QVariant::fromValue( config ) );
    item->setIcon( 0, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ) );
    item->setText( 0, QString( "%1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );

    ui.treeWidget_client->expandAll();

    connect( widget, &TcpClientWidget::socketChanged, [this, item, widget]( SocketConfig * config )
    {
        item->setText( 0, QString( "%1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );

        for( int i = 0; i < ui.tabWidget_client->count(); i++ )
        {
            if( ui.tabWidget_client->widget( i ) == widget )
            {
                ui.tabWidget_client->setTabText( i, QString( "TCP %1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );
                break;
            }
        }
    } );
    connect( widget, &TcpClientWidget::socketConnect, [this, item, widget]()
    {
        item->setIcon( 0, QApplication::style()->standardIcon( QStyle::SP_BrowserReload ) );

        for( int i = 0; i < ui.tabWidget_client->count(); i++ )
        {
            if( ui.tabWidget_client->widget( i ) == widget )
            {
                ui.tabWidget_client->setTabIcon( i, QApplication::style()->standardIcon( QStyle::SP_BrowserReload ) );
                break;
            }
        }
    } );
    connect( widget, &TcpClientWidget::socketDisconnect, [this, item, widget]()
    {
        item->setIcon( 0, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ) );

        for( int i = 0; i < ui.tabWidget_client->count(); i++ )
        {
            if( ui.tabWidget_client->widget( i ) == widget )
            {
                ui.tabWidget_client->setTabIcon( i, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ) );
                break;
            }
        }
    } );

    return widget;
}

UdpClientWidget * TUG::createUdpClient( SocketConfig * config, QTreeWidgetItem * item )
{
    auto widget = new UdpClientWidget( config, ui.tabWidget_client );
    ui.tabWidget_client->addTab( widget, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ), QString( "UDP %1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );

    if( item == nullptr )
        item = new QTreeWidgetItem( UDP_ITEM( ui.treeWidget_client ) );

    item->setData( 0, Qt::UserRole + 1, QVariant::fromValue( widget ) );
    item->setData( 0, Qt::UserRole + 2, QVariant::fromValue( config ) );
    item->setIcon( 0, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ) );
    item->setText( 0, QString( "%1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );

    ui.treeWidget_client->expandAll();

    connect( widget, &ClientWidget::socketChanged, [this, item, widget]( SocketConfig * config )
    {
        item->setText( 0, QString( "%1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );

        for( int i = 0; i < ui.tabWidget_client->count(); i++ )
        {
            if( ui.tabWidget_client->widget( i ) == widget )
            {
                ui.tabWidget_client->setTabText( i, QString( "UDP %1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );
                break;
            }
        }
    } );
    connect( widget, &ClientWidget::socketConnect, [this, item, widget]()
    {
        item->setIcon( 0, QApplication::style()->standardIcon( QStyle::SP_BrowserReload ) );

        for( int i = 0; i < ui.tabWidget_client->count(); i++ )
        {
            if( ui.tabWidget_client->widget( i ) == widget )
            {
                ui.tabWidget_client->setTabIcon( i, QApplication::style()->standardIcon( QStyle::SP_BrowserReload ) );
                break;
            }
        }
    } );
    connect( widget, &ClientWidget::socketDisconnect, [this, item, widget]()
    {
        item->setIcon( 0, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ) );

        for( int i = 0; i < ui.tabWidget_client->count(); i++ )
        {
            if( ui.tabWidget_client->widget( i ) == widget )
            {
                ui.tabWidget_client->setTabIcon( i, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ) );
                break;
            }
        }
    } );

    return widget;
}

TcpServerWidget * TUG::createTcpServer( SocketConfig * config, QTreeWidgetItem * item )
{
    auto widget = new TcpServerWidget( config, ui.tabWidget_server );
    ui.tabWidget_server->addTab( widget, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ), QString( "TCP %1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );

    if( item == nullptr )
        item = new QTreeWidgetItem( TCP_ITEM( ui.treeWidget_server ) );

    item->setData( 0, Qt::UserRole + 1, QVariant::fromValue( widget ) );
    item->setData( 0, Qt::UserRole + 2, QVariant::fromValue( config ) );
    item->setIcon( 0, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ) );
    item->setText( 0, QString( "%1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );

    ui.treeWidget_server->expandAll();

    connect( widget, &ServerWidget::socketChanged, [this, item, widget]( SocketConfig * config )
    {
        item->setText( 0, QString( "%1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );

        for( int i = 0; i < ui.tabWidget_server->count(); i++ )
        {
            if( ui.tabWidget_server->widget( i ) == widget )
            {
                ui.tabWidget_server->setTabText( i, QString( "UDP %1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );
                break;
            }
        }
    } );
    connect( widget, &ServerWidget::socketListener, [this, item, widget]()
    {
        item->setIcon( 0, QApplication::style()->standardIcon( QStyle::SP_BrowserReload ) );

        for( int i = 0; i < ui.tabWidget_server->count(); i++ )
        {
            if( ui.tabWidget_server->widget( i ) == widget )
            {
                ui.tabWidget_server->setTabIcon( i, QApplication::style()->standardIcon( QStyle::SP_BrowserReload ) );
                break;
            }
        }
    } );
    connect( widget, &ServerWidget::socketDisconnect, [this, item, widget]()
    {
        item->setIcon( 0, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ) );

        for( int i = 0; i < ui.tabWidget_server->count(); i++ )
        {
            if( ui.tabWidget_server->widget( i ) == widget )
            {
                ui.tabWidget_server->setTabIcon( i, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ) );
                break;
            }
        }
    } );

    return widget;
}

UdpServerWidget * TUG::createUdpServer( SocketConfig * config, QTreeWidgetItem * item )
{
    auto widget = new UdpServerWidget( config, ui.tabWidget_server );
    ui.tabWidget_server->addTab( widget, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ), QString( "UDP %1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );

    if( item == nullptr )
        item = new QTreeWidgetItem( UDP_ITEM( ui.treeWidget_server ) );

    item->setData( 0, Qt::UserRole + 1, QVariant::fromValue( widget ) );
    item->setData( 0, Qt::UserRole + 2, QVariant::fromValue( config ) );
    item->setIcon( 0, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ) );
    item->setText( 0, QString( "%1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );

    ui.treeWidget_server->expandAll();

    connect( widget, &ServerWidget::socketChanged, [this, item, widget]( SocketConfig * config )
    {
        item->setText( 0, QString( "%1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );

        for( int i = 0; i < ui.tabWidget_server->count(); i++ )
        {
            if( ui.tabWidget_server->widget( i ) == widget )
            {
                ui.tabWidget_server->setTabText( i, QString( "UDP %1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );
                break;
            }
        }
    } );
    connect( widget, &UdpServerWidget::socketListener, [this, item, widget]()
    {
        item->setIcon( 0, QApplication::style()->standardIcon( QStyle::SP_BrowserReload ) );

        for( int i = 0; i < ui.tabWidget_server->count(); i++ )
        {
            if( ui.tabWidget_server->widget( i ) == widget )
            {
                ui.tabWidget_server->setTabIcon( i, QApplication::style()->standardIcon( QStyle::SP_BrowserReload ) );
                break;
            }
        }
    } );
    connect( widget, &UdpServerWidget::socketDisconnect, [this, item, widget]()
    {
        item->setIcon( 0, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ) );

        for( int i = 0; i < ui.tabWidget_server->count(); i++ )
        {
            if( ui.tabWidget_server->widget( i ) == widget )
            {
                ui.tabWidget_server->setTabIcon( i, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ) );
                break;
            }
        }
    } );

    return widget;
}

void TUG::load()
{
    QSettings settings( "config.ini", QSettings::IniFormat );
    settings.beginGroup( "client" );
    {
        int count = settings.beginReadArray( "tcp" );
        for( int i = 0; i < count; i++ )
        {
            settings.setArrayIndex( i );

            auto config = new SocketConfig( this );
            {
                config->name = settings.value( "name", "" ).toString();
                config->host = settings.value( "host", "127.0.0.1" ).toString();
                config->port = settings.value( "port", 48888 ).toInt();
                config->interval = settings.value( "interval", 0 ).toInt();
                config->auto_send = settings.value( "auto_send", false ).toBool();
                config->auto_connect = settings.value( "auto_connect", false ).toBool();
                config->auto_disconnect = settings.value( "auto_disconnect", false ).toBool();
            }

            auto item = new QTreeWidgetItem( TCP_ITEM( ui.treeWidget_client ) );
            item->setText( 0, QString( "%1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );
            item->setData( 0, Qt::UserRole + 1, QVariant::fromValue( ( TcpClientWidget * )nullptr ) );
            item->setData( 0, Qt::UserRole + 2, QVariant::fromValue( config ) );
            item->setIcon( 0, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ) );
            ui.treeWidget_client->expandAll();
        }
        settings.endArray();

        count = settings.beginReadArray( "udp" );
        for( int i = 0; i < count; i++ )
        {
            settings.setArrayIndex( i );

            auto config = new SocketConfig( this );
            {
                config->name = settings.value( "name", "" ).toString();
                config->host = settings.value( "host", "127.0.0.1" ).toString();
                config->port = settings.value( "port", 48888 ).toInt();
                config->interval = settings.value( "interval", 0 ).toInt();
                config->auto_send = settings.value( "auto_send", false ).toBool();
                config->auto_connect = settings.value( "auto_connect", false ).toBool();
                config->auto_disconnect = settings.value( "auto_disconnect", false ).toBool();
            }

            auto item = new QTreeWidgetItem( UDP_ITEM( ui.treeWidget_client ) );
            item->setText( 0, QString( "%1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );
            item->setData( 0, Qt::UserRole + 1, QVariant::fromValue( ( UdpClientWidget * )nullptr ) );
            item->setData( 0, Qt::UserRole + 2, QVariant::fromValue( config ) );
            item->setIcon( 0, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ) );
            ui.treeWidget_client->expandAll();
        }
        settings.endArray();
    }
    settings.endGroup();

    settings.beginGroup( "server" );
    {
        int count = settings.beginReadArray( "tcp" );
        for( int i = 0; i < count; i++ )
        {
            settings.setArrayIndex( i );

            auto config = new SocketConfig( this );
            {
                config->name = settings.value( "name", "" ).toString();
                config->host = settings.value( "host", "127.0.0.1" ).toString();
                config->port = settings.value( "port", 48888 ).toInt();
                config->interval = settings.value( "interval", 0 ).toInt();
                config->auto_send = settings.value( "auto_send", false ).toBool();
                config->auto_connect = settings.value( "auto_connect", false ).toBool();
                config->auto_disconnect = settings.value( "auto_disconnect", false ).toBool();
            }

            auto item = new QTreeWidgetItem( TCP_ITEM( ui.treeWidget_server ) );
            item->setText( 0, QString( "%1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );
            item->setData( 0, Qt::UserRole + 1, QVariant::fromValue( ( TcpServerWidget * )nullptr ) );
            item->setData( 0, Qt::UserRole + 2, QVariant::fromValue( config ) );
            item->setIcon( 0, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ) );
            ui.treeWidget_server->expandAll();
        }
        settings.endArray();

        count = settings.beginReadArray( "udp" );
        for( int i = 0; i < count; i++ )
        {
            settings.setArrayIndex( i );

            auto config = new SocketConfig( this );
            {
                config->name = settings.value( "name", "" ).toString();
                config->host = settings.value( "host", "127.0.0.1" ).toString();
                config->port = settings.value( "port", 48888 ).toInt();
                config->interval = settings.value( "interval", 0 ).toInt();
                config->auto_send = settings.value( "auto_send", false ).toBool();
                config->auto_connect = settings.value( "auto_connect", false ).toBool();
                config->auto_disconnect = settings.value( "auto_disconnect", false ).toBool();
            }

            auto item = new QTreeWidgetItem( UDP_ITEM( ui.treeWidget_server ) );
            item->setText( 0, QString( "%1(%2:%3)" ).arg( config->name ).arg( config->host ).arg( config->port ) );
            item->setData( 0, Qt::UserRole + 1, QVariant::fromValue( ( UdpServerWidget * )nullptr ) );
            item->setData( 0, Qt::UserRole + 2, QVariant::fromValue( config ) );
            item->setIcon( 0, QApplication::style()->standardIcon( QStyle::SP_BrowserStop ) );
            ui.treeWidget_server->expandAll();
        }
        settings.endArray();
    }
    settings.endGroup();

    settings.beginGroup( "layout" );
    {
        this->restoreState( settings.value( "state" ).toByteArray() );
        this->restoreGeometry( settings.value( "geometry" ).toByteArray() );
    }
    settings.endGroup();
}

void TUG::save()
{
    QSettings settings( "config.ini", QSettings::IniFormat );
    settings.beginGroup( "client" );
    {
        settings.beginWriteArray( "tcp" );
        {
            auto item = TCP_ITEM( ui.treeWidget_client );
            for( int i = 0; i < item->childCount(); i++ )
            {
                settings.setArrayIndex( i );

                auto config = item->child( i )->data( 0, Qt::UserRole + 2 ).value<SocketConfig *>();

                settings.setValue( "name", config->name );
                settings.setValue( "host", config->host );
                settings.setValue( "port", config->port );
                settings.setValue( "interval", config->interval );
                settings.setValue( "auto_send", config->auto_send );
                settings.setValue( "auto_connect", config->auto_connect );
                settings.setValue( "auto_disconnect", config->auto_disconnect );
            }
        }
        settings.endArray();

        settings.beginWriteArray( "udp" );
        {
            auto item = UDP_ITEM( ui.treeWidget_client );
            for( int i = 0; i < item->childCount(); i++ )
            {
                settings.setArrayIndex( i );

                auto config = item->child( i )->data( 0, Qt::UserRole + 2 ).value<SocketConfig *>();

                settings.setValue( "name", config->name );
                settings.setValue( "host", config->host );
                settings.setValue( "port", config->port );
                settings.setValue( "interval", config->interval );
                settings.setValue( "auto_send", config->auto_send );
                settings.setValue( "auto_connect", config->auto_connect );
                settings.setValue( "auto_disconnect", config->auto_disconnect );
            }
        }
        settings.endArray();
    }
    settings.endGroup();

    settings.beginGroup( "server" );
    {
        settings.beginWriteArray( "tcp" );
        {
            auto item = TCP_ITEM( ui.treeWidget_server );
            for( int i = 0; i < item->childCount(); i++ )
            {
                settings.setArrayIndex( i );

                auto config = item->child( i )->data( 0, Qt::UserRole + 2 ).value<SocketConfig *>();

                settings.setValue( "name", config->name );
                settings.setValue( "host", config->host );
                settings.setValue( "port", config->port );
                settings.setValue( "interval", config->interval );
                settings.setValue( "auto_send", config->auto_send );
                settings.setValue( "auto_connect", config->auto_connect );
                settings.setValue( "auto_disconnect", config->auto_disconnect );
            }
        }
        settings.endArray();

        settings.beginWriteArray( "udp" );
        {
            auto item = UDP_ITEM( ui.treeWidget_server );
            for( int i = 0; i < item->childCount(); i++ )
            {
                settings.setArrayIndex( i );

                auto config = item->child( i )->data( 0, Qt::UserRole + 2 ).value<SocketConfig *>();

                settings.setValue( "name", config->name );
                settings.setValue( "host", config->host );
                settings.setValue( "port", config->port );
                settings.setValue( "interval", config->interval );
                settings.setValue( "auto_send", config->auto_send );
                settings.setValue( "auto_connect", config->auto_connect );
                settings.setValue( "auto_disconnect", config->auto_disconnect );
            }
        }
        settings.endArray();
    }
    settings.endGroup();

    settings.beginGroup( "layout" );
    {
        settings.setValue( "state", this->saveState() );
        settings.setValue( "geometry", this->saveGeometry() );
    }
    settings.endGroup();
}

void TUG::closeEvent( QCloseEvent * e )
{
    save();

    disconnectAllTcpClient();
    disconnectAllUdpClient();
    disconnectAllTcpServer();
    disconnectAllUdpServer();

    QMainWindow::closeEvent( e );
}
