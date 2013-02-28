#include "mainwindow.h"
#include "logviewer.h"
#include "ui_mainwindow.h"

#include "serialtools/serial_widget.h"
#include "network/network_widget.h"
#include "servoboard/servoboard_main.h"

#include <QLabel>
#include <QFont>
#include <QMessageBox>
#include <QGridLayout>
#include <QTabWidget>

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
}


void MainWindow::SetupLayout()
{
    layout = new QGridLayout(ui->centralwidget);
    tabs = new QTabWidget(this);

    // Instantiate the various tab objects...
    //  Note: the argument to the constructor is the parent, in this case, the tab container.
    serialconnecter = new SerialWidget(tabs);
    servotab = new servoboard_main(tabs);
    networktab = new NetworkWidget(tabs,0);

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

    //Get rid of the menu items that don't make sense, this is on the asumption that
    //it starts with the connectionwidget, if there is a change this will have to change.
    this->ui->actionLoad_Sequence->setVisible(false);
    this->ui->actionSave_Sequence->setVisible(false);
    this->ui->actionSave_Sequence_As->setVisible(false);
    this->ui->actionSet_Global_Values->setVisible(false);
    this->ui->actionKeep_Edits_Automatically->setVisible(false);


}

MainWindow::~MainWindow()
{
    //logWindow->hideLogViewer();
    if( connectControl) this->cleanUpConnection();
    if (servoControl) this->cleanupServoBoard();
    if (port)
    {
        if(port->isOpen())
        {
            port->close();
            qDebug() << "Closing serial port connections";
        }
        delete port;
    }

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
        this->initNetworking();
        qDebug() << "case 1";
        break;
    case 2: //Servoboard main
        if (connectControl)
        {
            this->cleanUpConnection();
        }
        this->initServoBoard();
        break;
    default:
        qDebug() << "this is silly";
        break;
    }
}
void MainWindow::initConnection()
{
    connectControl = new ConnectionController(this,serialconnecter,port);
    serialconnecter->setConnectionController(connectControl);
}
void MainWindow::cleanUpConnection()
{
    this->port = this->connectControl->getSerialPort();
    delete connectControl;
    connectControl = 0;
}
void MainWindow::closeEvent(QCloseEvent *r)
{
    this->logWindow->hideLogViewer();
}

void MainWindow::initServoBoard()
{
    this->ui->actionSave_Sequence->setVisible(true);
    this->ui->actionLoad_Sequence->setVisible(true);
    this->ui->actionSave_Sequence_As->setVisible(true);
    this->ui->actionSet_Global_Values->setVisible(true);
    this->ui->actionKeep_Edits_Automatically->setVisible(true);
    if (!this->servoControl)
    {
        servoControl = new ServoboardController(this->port,this->servotab,this);
    }
    connect(this->ui->actionLoad_Sequence,SIGNAL(triggered()),servoControl,SLOT(loadFile()));
    connect(this->ui->actionSave_Sequence,SIGNAL(triggered()),servoControl,SLOT(saveFile()));
    connect(this->ui->actionSave_Sequence_As,SIGNAL(triggered()),servoControl,SLOT(saveFileAs()));
    connect(this->servotab,SIGNAL(newPositionToSequence(Position*)),
            servoControl,SLOT(newPositionForSequence(Position*)));
    connect(this->ui->actionSet_Global_Values,SIGNAL(triggered()),
            servoControl,SLOT(globalVariableSetRequested()));
    connect(this->servotab,SIGNAL(playSequence()),servoControl,SLOT(playCurrentSequence()));
    connect(this->servotab,SIGNAL(playPosition(Position*)),this->servoControl,SLOT(playPosition(Position*)));
    connect(this->ui->actionKeep_Edits_Automatically,SIGNAL(toggled(bool)),
            servoControl,SLOT(suppressChangeNotifications(bool)));

    connect(this->servotab,SIGNAL(pauseSequence()),this->servoControl,SLOT(pauseSequence()));
    connect(this->servotab,SIGNAL(stopSequence()),this->servoControl,SLOT(stopSequence()));
    connect(this->servoControl,SIGNAL(newPositionSent(Position*)),this->servotab,SLOT(servoPositionChanged(Position*)));
    connect(this->servotab,SIGNAL(setStartPosition(Position*)),this->servoControl,SLOT(setStartPosition(Position*)));
    connect(this->ui->actionBurn_Start_Position_from_File,SIGNAL(triggered()),servoControl,SLOT(burnStartPosition()));
    connect(this->servotab,SIGNAL(hotModeToggle(Position*)),this->servoControl,SLOT(hotModeToggle(Position*)));


}
void MainWindow::cleanupServoBoard()
{
    this->ui->actionSave_Sequence->setVisible(false);
    this->ui->actionLoad_Sequence->setVisible(false);
    this->ui->actionSave_Sequence_As->setVisible(false);
    this->ui->actionSet_Global_Values->setVisible(false);
    this->ui->actionKeep_Edits_Automatically->setVisible(false);
    port = servoControl->returnSerialPort();
    delete this->servoControl;
    servoControl = 0;
}
void MainWindow::initNetworking()
{
    networktab->setPort(this->port);
}

void MainWindow::cleanUpNetworking()
{
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
