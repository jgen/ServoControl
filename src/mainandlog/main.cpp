/*
    The main function instantiates the Main Window object,
    to which all the various 'wigets' are attached. (ie: the serial_widget)
*/

#include <QtGui/QApplication>
#include <QString>
#include <QtDebug>
#include <QPointer>

#include "serialtools/serial_widget.h"
#include "mainwindow.h"
#include "logviewer.h"

/*
    Global variable that points to the Log Viewer object.
    (This object is a container for the Log Viewer Dialog.)
*/
QPointer<LogViewer> logViewer;


/*
  Custom message handler that redirects messages to a log viewer (if available).
*/
void myMessageOutput(QtMsgType type, const char *msg)
{
    if(logViewer)
       logViewer->outputMessage( type, msg );
}

int main(int argc, char *argv[])
{
    QString appTitle= "Serial Port Program";
    QString appAuthor = "WE Bots";
    QString appVersion = "0.1";

    QApplication a(argc, argv);
    a.setApplicationName(appTitle);
    a.setApplicationVersion(appVersion);
    a.setOrganizationName(appAuthor);

    // Instantiate a log viewer and redirect debug messages to it.
    logViewer = new LogViewer;
    qInstallMsgHandler(myMessageOutput);

    // Instantiate the Main Window
    MainWindow w(0, logViewer);
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

