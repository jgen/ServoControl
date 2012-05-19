#include "mainwindow.h"
#include "logviewer.h"
#include "ui_mainwindow.h"

#include "serial_widget.h"
#include "network_widget.h"
#include "servoboard_main.h"

#include <QLabel>
#include <QFont>
#include <QMessageBox>
#include <QGridLayout>
#include <QTabWidget>


//For debugging only
#include "position.h"
MainWindow::MainWindow(QWidget *parent, QPointer<LogViewer> log) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    logWindow(log),
    connectControl(0)
{
    ui->setupUi(this);
    SetupLayout();
    SetupStatusBar();
    Position p;
}


void MainWindow::SetupLayout()
{
    layout = new QGridLayout(ui->centralwidget);
    tabs = new QTabWidget(this);

    // Instantiate the various tab objects...
    //  Note: the argument to the constructor is the parent, in this case, the tab container.
    serialconnecter = new SerialWidget(tabs);
    servotab = new servoboard_main(tabs);
    networktab = new NetworkWidget(tabs, serialconnecter->getSerialPointer());

    tabs->addTab(serialconnecter,tr("Serial Port"));
    tabs->addTab(networktab,tr("Network"));
    tabs->addTab(servotab,tr("Servo Board"));

    layout->setSpacing(4);
    layout->setMargin(5);
    layout->addWidget(tabs,0,0,1,1);


    connect(tabs,SIGNAL(currentChanged(int)),SLOT(tabChanged(int)));
    serialconnecter->show();
    this->tabChanged(0);//call with the index of the starting tab to init controllers
}

MainWindow::~MainWindow()
{
    // clean up any open serial connections before closing.
    delete networktab;
    delete serialconnecter;
    delete tabs;
    delete layout;
    delete ui;
}
void MainWindow::tabChanged(int index)
{
    switch(index)
    {
    case 0:
        if (connectControl == 0)
        {
            connectControl = new ConnectionController(this,serialconnecter);
            serialconnecter->setConnectionController(connectControl);
        }
        break;
    case 1:
        if (connectControl != 0)
        {
            delete connectControl;
            connectControl = 0;
        }
        qDebug() << "case 1";
        break;
    case 2:
        if (connectControl != 0)
        {
            delete connectControl;
            connectControl = 0;
        }
        qDebug() << "case 2";
        break;
    default:
        qDebug() << "this is silly";
        break;
    }
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
