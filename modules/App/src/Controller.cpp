#include "pch.h"
#include "Controller.h"
#include "ui_MainWindow.h"

Controller::Controller(Ui::MainWindow& ui)
	: ui_(ui)
{
	InitStates();
}

void Controller::ClickOfferButton()
{
	ui_.webView_->page()->runJavaScript(QStringLiteral(
		"document.getElementsByClassName(\"reageer-button\")[0].click();"));
	QMessageBox::information(
		QApplication::activeWindow(), tr("Information"), tr("The button has been clicked successfully"), QMessageBox::Ok);
	emit offerProcessed();
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
	ui_.clickNowButton_->setDisabled(true);
	ui_.scheduleClickButton_->setDisabled(true);
}

void Controller::HandleLoggedOutStateEnter()
{
	WaitUntilAngularInit([&]() {
		ui_.login_->setDisabled(false);
		ui_.password_->setDisabled(false);
		ui_.loginButton_->setDisabled(false);
	});
}

void Controller::HandleLoggingInFinishedStateEnter()
{
	WaitUntilAngularInit([&]() {
		GetElementCountByClassName(QStringLiteral("logout-button"), [&](int count) {
			if (0 == count) {
				emit unauthorizedError();
				QMessageBox::critical(
					QApplication::activeWindow(), tr("Error"), tr("Login or password is invalid"), QMessageBox::Ok);
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

void Controller::HandleOfferClickState()
{
	ui_.offerBox_->setDisabled(true);
	ClickOfferButton();
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
			if (0 == count) {
				QMessageBox::warning(
					QApplication::activeWindow(), tr("Warning"), tr("The order is invalid or expired"), QMessageBox::Ok);
				emit offerProcessed();
				return;
			}

			ui_.webView_->page()->runJavaScript(QStringLiteral(
				"document.getElementsByClassName(\"reageer-button\")[0].scrollIntoView();"));

			GetInnerTextByClassName("reageer-button", [&](const QString& innerText) {
				if (innerText == QStringLiteral("Reageer")) {
					ui_.offerBox_->setDisabled(false);
					ui_.clickNowButton_->setDisabled(false);
					ui_.scheduleClickButton_->setDisabled(false);
					return;
				}
				else if (innerText == QStringLiteral("Verwijder reactie")) {
					QMessageBox::information(
						QApplication::activeWindow(), tr("Information"), tr("This order has already been clicked"), QMessageBox::Ok);
				}
				else {
					QMessageBox::warning(
						QApplication::activeWindow(), tr("Warning"), tr("Invalid order"), QMessageBox::Ok);
				}
				emit offerProcessed();
			});
		});
	});
}

void Controller::HandleScheduleOfferClickState()
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
		ClickOfferButton();
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
	QState* offerLoadFinishedState = new QState();
	QState* offerClickState = new QState();
	QState* scheduleOfferClickState = new QState();

	// Initial application state.
	connect(initialState, &QState::entered, [&]() {
		qDebug() << "[*] initialState entered";
		HandleInitialStateEnter();
	});
	initialState->addTransition(ui_.webView_, SIGNAL(loadFinished(bool)), loggedOutState);

	// Website home page loaded state.
	connect(loggedOutState, &QState::entered, [&]() {
		qDebug() << "[*] loggedOutState entered";
		HandleLoggedOutStateEnter();
	});
	loggedOutState->addTransition(ui_.loginButton_, SIGNAL(pressed()), loggingInState);

	// User logging in.
	connect(loggingInState, &QState::entered, [&]() {
		qDebug() << "[*] loggingInState entered";
		HandleLoggingInStateEnter();
	});
	loggingInState->addTransition(ui_.webView_, SIGNAL(loadFinished(bool)), loggingInFinishedState);

	// User finished logging in.
	connect(loggingInFinishedState, &QState::entered, [&]() {
		qDebug() << "[*] loggingInFinishedState entered";
		HandleLoggingInFinishedStateEnter();
	});
	loggingInFinishedState->addTransition(this, SIGNAL(unauthorizedError()), initialState);
	loggingInFinishedState->addTransition(this, SIGNAL(loginSuccess()), loggedInState);

	// User logged in.
	connect(loggedInState, &QState::entered, [&]() {
		qDebug() << "[*] loggedInState entered";
		HandleLoggedInStateEnter();
	});
	loggedInState->addTransition(ui_.offerId_, SIGNAL(editingFinished()), offerLoadState);

	// Offer load.
	connect(offerLoadState, &QState::entered, [&]() {
		qDebug() << "[*] offerLoadState entered";
		HandleOfferLoadState();
	});
	offerLoadState->addTransition(ui_.webView_, SIGNAL(loadFinished(bool)), offerLoadFinishedState);

	// Offer load finished.
	connect(offerLoadFinishedState, &QState::entered, [&]() {
		qDebug() << "[*] offerLoadFinishedState entered";
		HandleOfferLoadFinishedState();
	});
	offerLoadFinishedState->addTransition(ui_.clickNowButton_, SIGNAL(pressed()), offerClickState);
	offerLoadFinishedState->addTransition(ui_.scheduleClickButton_, SIGNAL(pressed()), scheduleOfferClickState);
	offerLoadFinishedState->addTransition(this, SIGNAL(offerProcessed()), loggedInState);

	// Offer click.
	connect(offerClickState, &QState::entered, [&]() {
		qDebug() << "[*] offerClickState entered";
		HandleOfferClickState();
	});
	offerClickState->addTransition(this, SIGNAL(offerProcessed()), loggedInState);

	// Schedule offer click.
	connect(scheduleOfferClickState, &QState::entered, [&]() {
		qDebug() << "[*] scheduleOfferClickState entered";
		HandleScheduleOfferClickState();
	});
	scheduleOfferClickState->addTransition(this, SIGNAL(offerProcessed()), loggedInState);

	stateMachine_.addState(initialState);
	stateMachine_.addState(loggedOutState);
	stateMachine_.addState(loggingInState);
	stateMachine_.addState(loggingInFinishedState);
	stateMachine_.addState(loggedInState);
	stateMachine_.addState(offerLoadState);
	stateMachine_.addState(offerLoadFinishedState);
	stateMachine_.addState(offerClickState);
	stateMachine_.addState(scheduleOfferClickState);

	stateMachine_.setInitialState(initialState);
}

void Controller::Start()
{
	stateMachine_.start();
}
