/*

  The idea and most of the code for the Log Viewer came from this page on
  the Qt website: http://developer.qt.nokia.com/wiki/Browser_for_QDebug_output

*/

#include "logviewer.h"
#include "logviewerdialog.h"
#include <QMetaType>
 
LogViewer::LogViewer(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<QtMsgType>("QtMsgType");
    viewerDialog = new LogViewerDialog;
    connect(this, SIGNAL(sendMessage(QtMsgType,QString)), viewerDialog, SLOT(outputMessage(QtMsgType,QString)), Qt::QueuedConnection);
    viewerDialog->hide();

    connect(viewerDialog, SIGNAL(logHidden()), this, SLOT(logWindowHidden()));
}
 
LogViewer::~LogViewer()
{
    delete viewerDialog;
}
 
void LogViewer::outputMessage(QtMsgType type, const QString &msg)
{
    emit sendMessage( type, msg );
}

void LogViewer::showLogViewer()
{
    this->viewerDialog->show();
}

void LogViewer::hideLogViewer()
{
    this->viewerDialog->hide();
}

void LogViewer::closeLogViewer()
{
    // this deletes the Log Viewer Dialog
    this->viewerDialog->close();
}

void LogViewer::logWindowHidden()
{
    /*
      When the Log dialog is hidden, it emits a logHidden() signal.
      This is fed into the slot logWindowHidden() which emits logDialogHidden() signal.
      This signal is then fed into the slot logWindowHidden() which ultimately un-checks the menu.

      Basically we are just pushing the signal up the chain of inheritance so that the MainWindow can see the signal.
      This is probably the wrong way to do it, but it works.
     */
    emit logDialogHidden();
}
