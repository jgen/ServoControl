/*! @file main.cpp
    \brief This is the main entry point for the application.

    The main function instantiates the Main Window object,
    to which all the various 'wigets' are attached. (ie: the serial_widget)
*/


#ifndef __PRETTY_FUNCTION__
 #ifndef __FUNCDNAME__
  #define __PRETTY_FUNCTION__ __FILE__
 #endif//#ifndef __FUNCDNAME__
 #ifdef __FUNCDNAME__
  #define __PRETTY_FUNCTION__ __FUNCDNAME__
 #endif //#ifdef __FUNCDNAME__
#endif //__PRETTY_FUNCTION__

/*!
 * Global variable that is used to set if verbose logging will be used in
 * the program.
 */
bool VERBOSE = true;

#include <QtWidgets\QApplication>
#include <QString>
#include <QtDebug>
#include <QPointer>

#include "serialtools/serial_widget.h"
#include "mainwindow.h"
#include "logviewer.h"

/*!
    Global variable that points to the Log Viewer object.
    (This object is a container for the Log Viewer Dialog.)
*/
QPointer<LogViewer> logViewer;


/*!
  Custom message handler that redirects messages to a log viewer (if available).
*/
void MyMessageOutput(QtMsgType Type, const QMessageLogContext& Context, const QString &Message)
{
    if(logViewer)
       logViewer->outputMessage( Type, Message );
}

int main(int argc, char *argv[])
{
    QString appTitle= "WE Bots Servo Board Controller";
    QString appAuthor = "WE Bots";
    QString appVersion = "1.0";

    QApplication a(argc, argv);
    a.setApplicationName(appTitle);
    a.setApplicationVersion(appVersion);
    a.setOrganizationName(appAuthor);

    // Instantiate a log viewer and redirect debug messages to it.
    logViewer = new LogViewer;
    qInstallMessageHandler(MyMessageOutput);

    // Instantiate the Main Window
    MainWindow w(0, logViewer);
    w.setAttribute(Qt::WA_AlwaysShowToolTips);
    w.setWindowTitle(appTitle);
    w.show();

    // Connect the log viewer so we can uncheck the menu item when hidden.
    QObject::connect(logViewer, SIGNAL(logDialogHidden()), &w, SLOT(logWindowHidden()));

    qDebug() << "Program started.";

    int result = a.exec();  // Fire up the GUI and hand over control.

    logViewer->closeLogViewer();
    delete logViewer;   // clean up the log viewer
    return result;
}

