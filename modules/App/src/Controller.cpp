#include "pch.h"
#include "Controller.h"
#include "ui_MainWindow.h"

#include <QWebChannel>
#include <QWebEngineProfile>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QFile>
#include <thread>

Controller::Controller(Ui::MainWindow& ui)
	: ui_(ui)
{
	InitStates();

	// Register web channel to communicate from JS to C++ side.
	QWebChannel* channel = new QWebChannel(ui_.webView_->page());
	ui_.webView_->page()->setWebChannel(channel);
	channel->registerObject(QStringLiteral("controller"), this);
}

void Controller::HandleInitialStateEnter()
{
	ui_.loginButton_->setDisabled(true);
	ui_.offerBox_->setDisabled(true);
	ui_.webView_->load(QStringLiteral("https://www.wooniezie.nl/mijn-wooniezie/inloggen/"));

	// Workaround to set input focus when event system is free.
	QTimer::singleShot(0, ui_.login_, SLOT(setFocus()));
}

void Controller::HandleLoggedInStateEnter()
{
	ui_.credentialsBox_->setDisabled(true);
	ui_.offerBox_->setDisabled(false);
}

void Controller::HandleLoggedOutStateEnter()
{
	WaitUntilAngularInit([&]() {
		ui_.login_->setDisabled(false);
		ui_.password_->setDisabled(false);
		ui_.loginButton_->setDisabled(false);

		InjectWebChannelJs(*ui_.webView_->page());
		ui_.webView_->page()->runJavaScript(QStringLiteral(
			"new QWebChannel(qt.webChannelTransport, function (channel) {"
			"  var controller = channel.objects.controller;"
			""
			"});"));
	});
}

void Controller::HandleLoggingInFinishedStateEnter()
{
	WaitUntilAngularInit([&]() {
		GetElementCountByClassName(QStringLiteral("logout-button"), [&](int count) {
			if (0 == count) {
				emit unauthorizedError();
				QMessageBox::critical(
					QApplication::activeWindow(),
					tr("Error"),
					tr("Login or password is invalid"),
					QMessageBox::Ok);
			}
			else {
				emit loginSuccess();
			}
		});
	});
}

void Controller::HandleLoggingInStateEnter()
{
	ui_.loginButton_->setDisabled(true);

	ui_.webView_->page()->runJavaScript(QStringLiteral(
		"document.getElementById(\"account-frontend-login\").scrollIntoView();"
		"document.getElementById(\"username\").value=\"%1\";"
		"document.getElementById(\"password\").value=\"%2\";"
		"document.getElementById(\"submit\").click();").arg(
			ui_.login_->text(),
			ui_.password_->text()));
}

void Controller::HandleOfferLoadState()
{
	ui_.offerBox_->setDisabled(true);
	ui_.webView_->load(QStringLiteral("https://www.wooniezie.nl/aanbod/te-huur/details/?dwellingID=%1").arg(
		ui_.offerId_->value()));
}

void Controller::HandleOfferLoadFinishedState()
{
	WaitUntilAngularInit([&]() {
		GetElementCountByClassName("reageer-button", [&](int count) {
			if (0 != count) {
				ui_.webView_->page()->runJavaScript(QStringLiteral(
					"document.getElementsByClassName(\"reageer-button\")[0].scrollIntoView();"
					"if (document.getElementsByClassName(\"reageer-button\")[0].innerText == \"Reageer\")"
					"document.getElementsByClassName(\"reageer-button\")[0].click();"));
			}
			emit offerProcessed();
		});
	});
}

void Controller::HandleScheduleOfferLoadState()
{
	QDateTime dt(ui_.date_->selectedDate(), ui_.time_->time());
	QDateTime dtCurr = QDateTime::currentDateTime();
	qint64 diff = dtCurr.msecsTo(dt);
	if (diff < 0) {
		emit offerProcessed();
		return;
	}

	ui_.offerBox_->setDisabled(true);
	QTimer::singleShot(diff, this, [=]() {
		ui_.webView_->load(QStringLiteral("https://www.wooniezie.nl/aanbod/te-huur/details/?dwellingID=%1").arg(
			ui_.offerId_->value()));
	});
}

void Controller::InitStates()
{
	QState* initialState = new QState();
	QState* loggedOutState = new QState();
	QState* loggingInState = new QState();
	QState* loggingInFinishedState = new QState();
	QState* loggedInState = new QState();
	QState* offerLoadState = new QState();
	QState* scheduleOfferLoadState = new QState();
	QState* offerLoadFinishedState = new QState();

	// Initial application state.
	connect(initialState, &QState::entered, [&]() {
		HandleInitialStateEnter();
	});
	initialState->addTransition(ui_.webView_, SIGNAL(loadFinished(bool)), loggedOutState);

	// Website home page loaded state.
	connect(loggedOutState, &QState::entered, [&]() {
		HandleLoggedOutStateEnter();
	});
	loggedOutState->addTransition(ui_.loginButton_, SIGNAL(pressed()), loggingInState);

	// User logging in.
	connect(loggingInState, &QState::entered, [&]() {
		HandleLoggingInStateEnter();
	});
	loggingInState->addTransition(ui_.webView_, SIGNAL(loadFinished(bool)), loggingInFinishedState);

	// User finished logging in.
	connect(loggingInFinishedState, &QState::entered, [&]() {
		HandleLoggingInFinishedStateEnter();
	});
	loggingInFinishedState->addTransition(this, SIGNAL(unauthorizedError()), initialState);
	loggingInFinishedState->addTransition(this, SIGNAL(loginSuccess()), loggedInState);

	// User logged in.
	connect(loggedInState, &QState::entered, [&]() {
		HandleLoggedInStateEnter();
	});
	loggedInState->addTransition(ui_.offerId_, SIGNAL(editingFinished()), offerLoadState);
/*
	loggedInState->addTransition(ui_.clickNowButton_, SIGNAL(pressed()), offerLoadState);
	loggedInState->addTransition(ui_.scheduleClickButton_, SIGNAL(pressed()), scheduleOfferLoadState);
*/

	// Offer load.
	connect(offerLoadState, &QState::entered, [&]() {
		HandleOfferLoadState();
	});
	offerLoadState->addTransition(ui_.webView_, SIGNAL(loadFinished(bool)), offerLoadFinishedState);

	// Schedule offer load.
	connect(scheduleOfferLoadState, &QState::entered, [&]() {
		HandleScheduleOfferLoadState();
	});
	scheduleOfferLoadState->addTransition(ui_.webView_, SIGNAL(loadFinished(bool)), offerLoadFinishedState);
	scheduleOfferLoadState->addTransition(this, SIGNAL(offerProcessed()), loggedInState);

	// Offer load finished.
	connect(offerLoadFinishedState, &QState::entered, [&]() {
		HandleOfferLoadFinishedState();
	});
	offerLoadFinishedState->addTransition(this, SIGNAL(offerProcessed()), loggedInState);

	stateMachine_.addState(initialState);
	stateMachine_.addState(loggedOutState);
	stateMachine_.addState(loggingInState);
	stateMachine_.addState(loggingInFinishedState);
	stateMachine_.addState(loggedInState);
	stateMachine_.addState(offerLoadState);
	stateMachine_.addState(scheduleOfferLoadState);
	stateMachine_.addState(offerLoadFinishedState);

	stateMachine_.setInitialState(initialState);
}

void Controller::InjectWebChannelJs(QWebEnginePage& page) const
{
	QFile file(":/qtwebchannel/qwebchannel.js");
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "Failed to inject qwebchannel.js: " << file.errorString();
		return;
	}

	QString script = QString::fromUtf8(file.readAll());
	page.runJavaScript(script);
}

void Controller::Start()
{
	stateMachine_.start();
}
