#pragma once

#include <QMainWindow>
#include "ui_MainWindow.h"

#include "TcpServerWidget.h"
#include "TcpClientWidget.h"
#include "UdpServerWidget.h"
#include "UdpClientWidget.h"

class TUG : public QMainWindow
{
    Q_OBJECT

public:
    TUG(QWidget *parent = nullptr);
    ~TUG();

public slots:
    void on_treeWidget_client_customContextMenuRequested( const QPoint & pos );
    void on_treeWidget_server_customContextMenuRequested( const QPoint & pos );
    void on_treeWidget_client_itemDoubleClicked( QTreeWidgetItem * item, int column );
    void on_treeWidget_server_itemDoubleClicked( QTreeWidgetItem * item, int column );

private:
    void connectTcpClient( QTreeWidgetItem * item );
    void connectTcpServer( QTreeWidgetItem * item );
    void connectUdpClient( QTreeWidgetItem * item );
    void connectUdpServer( QTreeWidgetItem * item );
    void disconnectTcpClient( QTreeWidgetItem * item );
    void disconnectTcpServer( QTreeWidgetItem * item );
    void disconnectUdpClient( QTreeWidgetItem * item );
    void disconnectUdpServer( QTreeWidgetItem * item );
    void connectAllTcpClient();
    void connectAllTcpServer();
    void connectAllUdpClient();
    void connectAllUdpServer();
    void disconnectAllTcpClient();
    void disconnectAllTcpServer();
    void disconnectAllUdpClient();
    void disconnectAllUdpServer();
    TcpClientWidget * createTcpClient( SocketConfig * config, QTreeWidgetItem * item );
    UdpClientWidget * createUdpClient( SocketConfig * config, QTreeWidgetItem * item );
    TcpServerWidget * createTcpServer( SocketConfig * config, QTreeWidgetItem * item );
    UdpServerWidget * createUdpServer( SocketConfig * config, QTreeWidgetItem * item );

private:
    void load();
    void save();

protected:
    void closeEvent( QCloseEvent * e ) override;

private:
    Ui::TUGClass ui;
};
