#ifndef LOGVIEWER_H
#define LOGVIEWER_H
 
#include <QObject>
 
class LogViewerDialog;
 
class LogViewer : public QObject
{
    Q_OBJECT
public:
    explicit LogViewer(QObject *parent = 0);
    ~LogViewer();
    void showLogViewer();
    void hideLogViewer();
    void closeLogViewer();  // this delete the Log Viewer Dialog
 
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
