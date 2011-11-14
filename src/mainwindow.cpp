#include "mainwindow.h"
#include "logviewer.h"
#include "ui_mainwindow.h"

#include "serial_widget.h"

#include <QLabel>
#include <QFont>
#include <QMessageBox>
#include <QGridLayout>
#include <QTabWidget>

MainWindow::MainWindow(QWidget *parent, QPointer<LogViewer> log) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    logWindow(log)
{
    ui->setupUi(this);
    SetupLayout();
    SetupStatusBar();

}
void MainWindow::SetupLayout()
{
    layout = new QGridLayout(ui->centralwidget);
    tabs = new QTabWidget(this);
    serialconnecter = new SerialWidget(tabs);

    tabs->addTab(serialconnecter,tr("Serial Port"));

    layout->setSpacing(4);
    layout->setMargin(5);
    layout->addWidget(tabs,0,0,1,1);

    serialconnecter->show();
}

MainWindow::~MainWindow()
{
    // clean up any open serial connections before closing.
    delete serialconnecter;
    delete tabs;
    delete layout;
    delete ui;
}

void MainWindow::SetupStatusBar()
{
    statusBar();
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::on_actionExit_triggered()
{
    // clean up the log viewer and close the program.
    logWindow->closeLogViewer();
    this->close();
}

void MainWindow::on_actionView_Log_triggered()
{
    // View Log was clicked
    if (logWindow) {
        logWindow->showLogViewer();
        this->ui->actionView_Log->setChecked(true);
    }
}

void MainWindow::logWindowHidden()
{
    // uncheck
    this->ui->actionView_Log->setChecked(false);
}

/* About Menu Items */
void MainWindow::on_actionAbout_triggered()
{
    QString title("About ");
    title.append(QApplication::applicationName());

    QString text("This program allows you to use your serial port  \n with the Servo Control Board. \n\n");
    text.append(QApplication::organizationName());
    text.append("\nVersion: ");
    text.append(QApplication::applicationVersion());

    QMessageBox::about(this, title, text);
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    qApp->aboutQt();
}
/* End About Menu Items */
