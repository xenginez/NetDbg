#include "ClientWidget.h"

ClientWidget::ClientWidget( SocketConfig * config, QWidget * parent )
	: QWidget( parent ), _config( config )
{
}

ClientWidget::~ClientWidget()
{
}

SocketConfig * ClientWidget::config() const
{
	return _config;
}
