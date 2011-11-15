#ifndef LOGVIEWERDIALOG_H
#define LOGVIEWERDIALOG_H
 
#include <QDialog>

QT_BEGIN_NAMESPACE
class QTextBrowser;
class QPlainTextEdit;
class QPushButton;
QT_END_NAMESPACE

class LogViewerDialog : public QDialog
{
    Q_OBJECT
 
public:
    LogViewerDialog(QWidget *parent = 0);
    ~LogViewerDialog();

public slots:
    void outputMessage( QtMsgType type, const QString &msg );
    void slotHide();
 
protected slots:
    void slotSave();

signals:
    void logHidden();

protected:
    virtual void keyPressEvent( QKeyEvent *e );
    virtual void closeEvent( QCloseEvent *e );
 
    QPlainTextEdit *logger;
    QPushButton *clearButton;
    QPushButton *saveButton;
    QPushButton *hideButton;
};
 
#endif // LOGVIEWERDIALOG_H
