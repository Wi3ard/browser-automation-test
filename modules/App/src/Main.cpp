#include "pch.h"
#include "Controller.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

int main(int argc, char** argv)
{
	QApplication app{ argc, argv };
	QCoreApplication::setApplicationName("Wooniezie Robot");
	QCoreApplication::setApplicationVersion("1.0.0");
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	MainWindow mainWindow;
	mainWindow.show();

	return app.exec();
}
