#ifndef LOGVIEWER_H
#define LOGVIEWER_H
 
#include <QObject>
 
class LogViewerDialog;

/*! \class LogViewer
  This class acts as a logging facility for the application.
  It creates a LogViewerDialog to output all the messages to.
  \sa LogViewerDialog
*/
class LogViewer : public QObject
{
    Q_OBJECT
public:
    explicit LogViewer(QObject *parent = 0);
    ~LogViewer();
    void showLogViewer();
    void hideLogViewer();
    void closeLogViewer();  //!< This deletes the Log Viewer Dialog
 
public slots:
    void outputMessage( QtMsgType type, const QString &msg );
    void logWindowHidden();
 
signals:
    void sendMessage( QtMsgType type, const QString &msg );
    void logDialogHidden();
 
private:
    LogViewerDialog *viewerDialog;
 
};
 
#endif // LOGVIEWER_H
