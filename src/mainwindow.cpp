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

#include "sequence.h"

MainWindow::MainWindow(QWidget *parent, QPointer<LogViewer> log) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    logWindow(log),
    connectControl(0),
    servoControl(0),
    port(0)
{
    ui->setupUi(this);
    SetupLayout();
    SetupStatusBar();

    Sequence s;
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

    this->ui->actionSave_Sequence->setVisible(false);
    this->ui->actionLoad_Sequence->setVisible(false);
    this->ui->actionSave_Sequence_As->setVisible(false);

    this->tabChanged(0);//call with the index of the starting tab to init controllers


}

MainWindow::~MainWindow()
{
    if( connectControl) this->cleanUpConnection();
    if (servoControl) this->cleanupServoBoard();

    // clean up any open serial connections before closing.
    delete networktab;
    networktab = 0; //combines with check in tabChanged,
    //the Tabwidget sends the tabChanged signal on destruction which can cause
    //intialization to be run during shut down, the network tab is checked to see
    //if destruction has started when the tabs are change.
    //Bit of a hacky workaround but can't think of a better one right now.
    delete serialconnecter;
    delete tabs;
    delete layout;
    delete ui;
}
void MainWindow::tabChanged(int index)
{
    if (!networktab) return; //See destructor for explaination of this line.
    switch(index)
    {
    case 0: //We are going to the connection controller.
        if (this->servoControl)
        {
            this->cleanupServoBoard();
        }
        if (connectControl == 0)
        {
            this->initConnection();
        }
        break;
    case 1://Network tab
        if (connectControl)
        {
            this->cleanUpConnection();
        }
        if(servoControl)
        {
            this->cleanupServoBoard();
        }
        qDebug() << "case 1";
        break;
    case 2: //Servoboard main
        if (connectControl)
        {
            this->cleanUpConnection();
        }
        this->initServoBoard();
        qDebug() << "case 2";
        break;
    default:
        qDebug() << "this is silly";
        break;
    }
}
void MainWindow::initConnection()
{
    connectControl = new ConnectionController(this,serialconnecter);
    serialconnecter->setConnectionController(connectControl);
}
void MainWindow::cleanUpConnection()
{
    this->port = this->connectControl->getSerialPort();
    delete connectControl;
    connectControl = 0;
}
void MainWindow::initServoBoard()
{
    this->ui->actionSave_Sequence->setVisible(true);
    this->ui->actionLoad_Sequence->setVisible(true);
    this->ui->actionSave_Sequence_As->setVisible(true);
    if (!this->servoControl)
    {
        servoControl = new ServoboardController(this->port,this->servotab,this);
    }
    connect(this->ui->actionLoad_Sequence,SIGNAL(triggered()),servoControl,SLOT(loadFile()));
    connect(this->ui->actionSave_Sequence,SIGNAL(triggered()),servoControl,SLOT(saveFile()));
    connect(this->ui->actionSave_Sequence_As,SIGNAL(triggered()),servoControl,SLOT(saveFileAs()));



}
void MainWindow::cleanupServoBoard()
{
    this->ui->actionSave_Sequence->setVisible(false);
    this->ui->actionLoad_Sequence->setVisible(false);
    this->ui->actionSave_Sequence_As->setVisible(false);
    delete this->servoControl;
    servoControl = 0;
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
