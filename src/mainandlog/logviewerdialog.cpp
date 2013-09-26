#include "logviewerdialog.h"
 
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\QPlainTextEdit>
#include <QtWidgets\QPushButton>
#include <QtWidgets\QFileDialog>
#include <QDir>
#include <QFile>
#include <QtWidgets\QMessageBox>
#include <QTextStream>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QtCore/QDateTime>


LogViewerDialog::LogViewerDialog(QWidget *parent)
    : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
    this->setWindowTitle("View Debug Log");

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);
 
    logger = new QPlainTextEdit(this);
    layout->addWidget(logger);
 
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(buttonLayout);
 
    buttonLayout->addStretch(10);
 
    clearButton = new QPushButton(this);
    clearButton->setText("Clear");
    buttonLayout->addWidget(clearButton);
    connect(clearButton, SIGNAL(clicked()), logger, SLOT(clear()));
 
    saveButton = new QPushButton(this);
    saveButton->setText("Save to File");
    buttonLayout->addWidget(saveButton);
    connect(saveButton, SIGNAL(clicked()), this, SLOT(slotSave()));
 
    hideButton = new QPushButton(this);
    hideButton->setText("Hide");
    hideButton->setDefault(true);
    hideButton->setShortcut(QKeySequence("ESC"));
    buttonLayout->addWidget(hideButton);
    connect(hideButton, SIGNAL(clicked()), this, SLOT(slotHide()));

    resize(600, 180);
}

LogViewerDialog::~LogViewerDialog()
{
    // Clean up
    delete logger;
    delete clearButton;
    delete saveButton;
    delete hideButton;
}

//----- public slots -----

void LogViewerDialog::slotHide()
{
    this->hide();
    emit logHidden();
}

void LogViewerDialog::outputMessage(QtMsgType type, const QString &msg)
{
    // Add the Current Time to the message string.
    QString timestamp = QDateTime::currentDateTime().time().toString();
    QString message = msg;
    timestamp.append(" > ");
    message.prepend(timestamp);

    // Output based on message type.
    switch (type) {
    case QtDebugMsg:
        logger->appendPlainText(message);
        break;

    case QtWarningMsg:
        logger->appendPlainText(tr("-- WARNING: %1").arg(message));
        break;

    case QtCriticalMsg:
        logger->appendPlainText(tr("-- CRITICAL: %1").arg(message));
        break;

    case QtFatalMsg:
        logger->appendPlainText(tr("-- FATAL: %1").arg(message));
        break;
    }
}
 
void LogViewerDialog::slotSave()
{
    QString saveFileName = QFileDialog::getSaveFileName(
                this,
                tr("Save Log Output"),
                tr("%1/logfile.txt").arg(QDir::homePath()),
                tr("Text Files (*.txt);;All Files (*)")
                );
 
    if(saveFileName.isEmpty())
        return;
 
    QFile file(saveFileName);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(
                    this,
                    tr("Error"),
                    QString(tr("<nobr>File '%1'<br/>cannot be opened for writing.<br/><br/>"
                               "The log output could <b>not</b> be saved!</nobr>"))
                    .arg(saveFileName));
        return;
    }
 
    QTextStream stream(&file);
    stream << logger->toPlainText();
    file.close();
}
//-----

 
void LogViewerDialog::closeEvent(QCloseEvent *e)
{
    // Don't actually close the window, hide it.
    e->ignore();
    this->hide();
    emit logHidden();

//    QMessageBox::StandardButton answer = QMessageBox::question(
//                this,
//                tr("Close Log Viewer?"),
//                tr("Do you really want to close the log viewer?"),
//                QMessageBox::Yes | QMessageBox::No
//                );
 
//    if (answer == QMessageBox::Yes)
//        e->accept();
//    else
//        e->ignore();
}
 
 
void LogViewerDialog::keyPressEvent(QKeyEvent *e)
{
    // ignore all keyboard events
    // protects against accidentally closing of the dialog
    // without asking the user
    //e->ignore();
}
