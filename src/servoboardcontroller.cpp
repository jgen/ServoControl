#include "servoboardcontroller.h"

ServoboardController::ServoboardController(QObject *parent) :
    QObject(parent),
    port(0),
    view(0),
    displayedData(0),
    timer(0),
    globalDelay(1),
    suppressChangeNotification(false),
    playState(stop)
{
    this->init();
}
ServoboardController::ServoboardController(AbstractSerial *port,
                                           servoboard_main *form,
                                           QObject *parent):
    QObject(parent),
    port(port),
    view(form),
    displayedData(0),
    timer(0),
    globalDelay(1),
    globalReplay(0),
    currentReplays(0),
    suppressChangeNotification(false),
    playState(stop)
{
    this->init();
}
void ServoboardController::init()
{
    if (!view)
    {
        qDebug() << tr("There was no formed passed to ServoboardController");
        //I am going to fail fast, this should never happen. And nothing
        //get attention like a segfault.
        view->disableButtons();
    }
    if (!port || !port->isOpen())//shouldn't evaluate second if first is true.
    {
        view->disableButtons();
        qDebug() << tr("No port was passed in");
        view->displayConnectionWarning();
    }
    else
    {
        port->atEnd();//No reason to call this, if there is an invalid port passsed in
        //I want to fail fast.
        view->enableButtons();
    }

    if(this->view->hasSequenceInText()) //If there is data when a tab switches, this way it can be recovered.
    {
        displayedData = new Sequence(this);

        if (!displayedData->fromString(view->currentSequenceText()))
        {
            qDebug() << "Data in the text box is not a valid sequence";
        }
        else
        {
            bool ok = false;
            view->displayNewSequence(displayedData->toString(&ok));
            if (!ok)
            {
                qDebug() << "Failed converting data to a string";
                delete displayedData;
                displayedData = 0;
                view->displayNewSequence("");
            }
        }
    }

}

ServoboardController::~ServoboardController()
{
    if(displayedData)
        delete displayedData;
}

/*Public Methods*/
AbstractSerial* ServoboardController::returnSerialPort()
{
    return port;
}

/*Public Slot*/
void ServoboardController::loadFile()
{
    QString fileName = QFileDialog::getOpenFileName(view,
                                                    tr("Open Sequence"), "./", tr("Servo Sequence Files (*.SER *.SERVO)"));
    displayedData = new Sequence(this);
    if (!displayedData->fromFile(fileName))
    {
        qDebug() << tr("The file %1 is invalid").arg(fileName);
    }
    bool ok = false;
    view->displayNewSequence(displayedData->toString(&ok));
    if (!ok)
    {
        qDebug() << tr("Failed to convert the sequence to a string");
    }

}
void ServoboardController::saveFile()
{
    this->saveFileAs(); //need to properly write later

}
void ServoboardController::saveFileAs()
{
    if (!this->checkForChangesToTextSequence())
    {
        qDebug() << tr("Save operation was cancelled by the user");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(view,tr("Save Sequence As"),"./",
                                                    tr("Servo Sequnce file *.SERVO;;Eugen Servo File *.SER"));
    if (fileName.endsWith(".SER") && !view->displaySaveFormatWaring())
    {
        qDebug() << tr("Saving aborted");
    }
    displayedData->toFile(fileName);
}
void ServoboardController::newPositionForSequence(Position* p)
{
    if (!displayedData)
    {
        displayedData = new Sequence();
    }
    if (!this->checkForChangesToTextSequence())
    {
        qDebug() << tr("Add operation was cancelled");
        return;
    }
    if (p->isEmpty())
    {
        return;
    }
    this->displayedData->addPosition(p);
    bool ok = false;
    view->displayNewSequence(displayedData->toString(&ok));
    if (!ok)
    {
        qDebug() << tr("Error displaying sequence");
    }
}

void ServoboardController::playCurrentSequence()
{
    if (this->playState == pause)
    {
        this->playState = play;
        timer->singleShot(displayedData->getNextDelay(),this,SLOT(timerTimeout()));
        return;
    }
    if (this->checkForChangesToTextSequence())
    {
        if (this->playState == stop)
        {
            view->resetHighlighting();
            timer = new QTimer(this);
            displayedData->resetIterator();
        }
    }
    else
    {
        qDebug() << tr("Play operation cancelled at the users request");
        return;
    }
    this->currentReplays = 0;
    this->globalReplay = displayedData->getRepeats();
    this->playState = play;
    timer->singleShot(0,this,SLOT(timerTimeout()));


}
void ServoboardController::playPosition(Position *p)
{
    if (!p || p->isEmpty())
    {
        qDebug() << "Servo controller was passed an invalid position";
    }
    port->write(p->toServoSerialData());
    delete p;
}

void ServoboardController::timerTimeout()
{
    if (this->playState == stop)
    {
        this->resetAfterPlayback();
        return;
    }
    if (this->playState == pause)
    {
        if(this->timer)
        {
            //timer->stop();
            //delete timer;
            timer = 0;
        }
        return;
    }
    if (!this->displayedData->hasNext())
    {
        if (++this->currentReplays > this->globalReplay)
        {
            this->resetAfterPlayback();
            return;
        }
        else
        {
            view->resetHighlighting();
            this->displayedData->resetIterator();
        }
    }

    int delay = displayedData->getNextDelay();
    if (delay == 0) //Will deal with this later for global values, errors.
    {
        delay = this->globalDelay;
    }
    this->port->write(displayedData->getNextData());
    this->view->highlightNextLine();
    timer->singleShot(delay*1000,this,SLOT(timerTimeout()));
}
void ServoboardController::globalVariableSetRequested()
{
    int seqDelay,seqRepeat, PWMSweep, PWMRepeat;
    bool isFreeze;
    advancedLineOptionsDialog* dialog = new advancedLineOptionsDialog();
    dialog->showSequenceRepeat();
    dialog->setModal(true);
    dialog->exec();
    if (dialog->getGlobalValues(isFreeze,seqDelay,seqRepeat,PWMSweep, PWMRepeat))
    {
        this->setGlobalDelay(seqDelay);
        this->setGlobalReplay(seqRepeat);
    }
}
void ServoboardController::setGlobalDelay(int delay)
{
    if (!displayedData)
    {
        displayedData = new Sequence();
    }
    displayedData->setDelay(delay);
}
void ServoboardController::setGlobalReplay(int replay)
{
    if (!displayedData)
    {
        displayedData = new Sequence();
    }
    displayedData->setReplay(replay);
}

void ServoboardController::suppressChangeNotifications(bool isChecked)
{
    qDebug() << this->suppressChangeNotification;
    this->suppressChangeNotification = isChecked;
    qDebug() << isChecked;
}

void ServoboardController::stopSequence()
{
    if (this->playState == pause)
    {
        this->resetAfterPlayback();
    }
    this->playState = stop;
}
void ServoboardController::pauseSequence()
{
    this->playState = pause;
}

/*Private Methods*/

void ServoboardController::resetAfterPlayback()
{
    if (this->timer)
    {
        //timer->stop();
        //delete timer;
        timer = 0;
    }
    view->resetHighlighting();
    view->setStoppedState();
    this->view->displayNewSequence(this->displayedData->toString());
    this->displayedData->resetIterator();
    return;//Have to notify the GUI later
}

bool ServoboardController::checkForChangesToTextSequence()
{
    if (!view->hasSequenceChanged()||
        !view->hasSequenceInText())//see if there are changes
    {
        return true;
    }
    QMessageBox::StandardButton response = QMessageBox::Cancel;
    if (!this->suppressChangeNotification)
    {
        response = view->displayKeepChangesWarning();
    }
    if (this->suppressChangeNotification ||
            response == QMessageBox::Ok) //see if they want the changes
    {
        if (displayedData->isVaild(view->currentSequenceText()))//See if what they have is valid
        {//They are valid, store it and move on
            bool ok;
            bool okay = displayedData->fromString(view->currentSequenceText());
            view->displayNewSequence(displayedData->toString(&ok));
            if(!ok || !okay)
            {
                qDebug() << tr("Failed to diplay sequence");
            }
            return true;
        }
        else
        {
            if (view->displayInvalidEditsWarning())
            {//discard the edits
                bool ok;
                view->displayNewSequence(displayedData->toString(&ok));
                if(!ok)
                {
                    qDebug() << tr("Failed to display the sequence");
                }
                return true;
            }
            else
            {//cancel the operation
                return false;
            }
        }

    }
    else if (response == QMessageBox::No)
    {
        bool ok = false;
        view->displayNewSequence(displayedData->toString(&ok));
        if (!ok)
        {
            qDebug() << "An error occured displaying the sequence";
            return true;
        }
        return true; //we can move on
    }
    else
    {
        return false; //cancel
    }
}
