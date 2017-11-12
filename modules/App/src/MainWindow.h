#pragma once

#include <memory>
#include <QMainWindow>

class Controller;

namespace Ui {
class MainWindow;
}

class MainWindow
	: public QMainWindow
{
	Q_OBJECT

	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	explicit MainWindow(QWidget* parent = nullptr);

	//
	// Private data members.
	//
private:
	//! UI controller.
	std::unique_ptr<Controller> controller_;
	//! Window UI.
	std::unique_ptr<Ui::MainWindow> ui_;
};
