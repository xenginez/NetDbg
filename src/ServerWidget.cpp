#include "ServerWidget.h"

ServerWidget::ServerWidget( SocketConfig * config, QWidget * parent )
	: QWidget( parent ), _config( config )
{

}

ServerWidget::~ServerWidget()
{
}

SocketConfig * ServerWidget::config() const
{
	return _config;
}
