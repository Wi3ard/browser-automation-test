#pragma once

#include <QObject>
#include <QStateMachine>
#include <QWebEnginePage>

namespace Ui {
class MainWindow;
}

class Controller
	: public QObject
{
	Q_OBJECT

	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	explicit Controller(Ui::MainWindow& ui);

	// Delete implicit copy constructor and assignment operator.
	Controller(const Controller&) = delete;
	Controller& operator= (const Controller&) = delete;

	//
	// Public interface.
	//
public:
	//! Starts controller.
	void Start();

	//
	// Signals/slots.
	//
signals:
	void loginSuccess();
	void offerProcessed();
	void unauthorizedError();

	//
	// Private methods.
	//
private:
	//! Returns the number of elements with the given class name in the DOM.
	template <typename Pred>
	void GetElementCountByClassName(const QString className, Pred pred) const;
	//! State events handlers.
	void HandleInitialStateEnter();
	void HandleLoggedInStateEnter();
	void HandleLoggedOutStateEnter();
	void HandleLoggingInFinishedStateEnter();
	void HandleLoggingInStateEnter();
	void HandleOfferLoadState();
	void HandleOfferLoadFinishedState();
	void HandleScheduleOfferLoadState();
	//! Initializes application states.
	void InitStates();
	//! Injects qwebchannel.js into the given page.
	void InjectWebChannelJs(QWebEnginePage& page) const;
	//! Waits until Angular application finishes initialization.
	template <typename Pred>
	void WaitUntilAngularInit(Pred pred, int periodMs = 1000);

	//
	// Private data members.
	//
private:
	//! State machine to control transitions between application states.
	QStateMachine stateMachine_;
	//! Main window UI.
	Ui::MainWindow& ui_;
};

template <typename Pred>
void Controller::GetElementCountByClassName(const QString className, Pred pred) const
{
	ui_.webView_->page()->runJavaScript(
		QStringLiteral("document.getElementsByClassName(\"%1\").length").arg(className),
		[=](const QVariant& v)
	{
		if (!v.isValid()) {
			pred(0);
			return;
		}
		pred(v.toInt());
	});
}

template <typename Pred>
void Controller::WaitUntilAngularInit(Pred pred, int periodMs /*= 1000*/)
{
	QTimer::singleShot(periodMs, this, [=]() {
		ui_.webView_->page()->runJavaScript(
			QStringLiteral("angular.element('body').find('.ajaxContent.loading').length"),
			[=](const QVariant& v)
		{
			if (v.isValid() && (0 != v.toInt())) {
				WaitUntilAngularInit(pred, periodMs);
			}
			else {
				pred();
			}
		});
	});
}
