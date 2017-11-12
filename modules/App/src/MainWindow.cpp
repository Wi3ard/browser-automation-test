#include "pch.h"
#include "Controller.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent /*= nullptr*/)
	: QMainWindow(parent)
	, ui_(new Ui::MainWindow())
{
	ui_->setupUi(this);

	controller_.reset(new Controller(*ui_));
	controller_->Start();
}
