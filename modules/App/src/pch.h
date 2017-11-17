#pragma once

#ifndef STRICT
#define STRICT
#endif

#ifdef WIN32

#include "targetver.h"

#ifdef NDEBUG

// Turn on intrinsics.
#include <intrin.h>

#endif	// #ifdef NDEBUG

#endif	// #ifdef WIN32

//////////////////////////////////////////////////////////////////////////
// STL includes and definitions.
//

#include <memory>

//////////////////////////////////////////////////////////////////////////
// boost includes and definitions.
//

//////////////////////////////////////////////////////////////////////////
// Qt includes and definitions.
//

#include <QApplication>
#include <QCoreApplication>
#include <QMainWindow>
#include <QMessageBox>
#include <QStateMachine>
#include <QTimer>
#include <QWebEnginePage>
#include <QWebEngineView>
