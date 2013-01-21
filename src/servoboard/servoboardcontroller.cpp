#include "servoboardcontroller.h"


/*!
 \todo Why is the file name not part of the controller and how can this
        be fixed?
 */
QString ServoboardController::fileName = "";

/*!
 * \brief The simplest constructor allowed by Qt, it will be included
 *      it is private to stop people from calling it and segfaulting.
 *
 * \param parent
 *      The parent of the object, responsible for cleaning the object up
 *      when it is destroyed.
 */
ServoboardController::ServoboardController(QObject *parent) :
    QObject(parent),
    port(0),
    view(0),
    displayedData(0),
    timer(0),
    globalDelay(1),
    globalReplay(0),
    currentReplays(0),
    suppressChangeNotification(false),
    playState(stop),
    isHotMode(false)
{
    this->init();
}
/*!
 * \brief Constructs a new controller with the given serial port and form.
 *
 * The default values are set here for many members, but some may be initalized
 * in the init method.
 *
 * \param port
 *      The serial port that the controller will communicate with. If the port is
 *      not valid there are no guarentees that the controller will not fault.
 *
 * \param form
 *      The form that the controller is responsible for managing.
 *
 * \param parent
 *      The parent of the object, responsible for cleaning the object up
 *      when it is destroyed.
 */
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
    playState(stop),
    isHotMode(false)
{
    this->init();
}
/*!
 *  \brief Initalizes the controller and will notify is something is wrong.
 *
 * In order to prevent subtle errors due to issues serial port or view issues,
 * this method forces a program halting fault if there is an issue detected.
 * As well this method recovers any data that may have been left in the text
 * edit during a tab switch so the user doesn't notice any change in state when
 * they are switching tabs.
 */
void ServoboardController::init()
{
    if (!view)
    {
        qDebug() << tr("There was no form passed to ServoboardController");
        //I am going to fail fast, this should never happen. And nothing
        //gets attention like a segfault.
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
/*!
 * \brief Clean up any memory that the object is responsible for.
 *
 * Note that it will not never be responisble for the serial port or the view
 * as the main window will clean those up on its way out.
 */
ServoboardController::~ServoboardController()
{
    if(displayedData)
        delete displayedData;
}

/*Public Methods*/
/*!
 * \brief Give up control of the serial port.
 *
 * This is used by the main window to get control of the serial port back
 * before destroying the object.
 *
 * \return A pointer to the serial port that the controller was using.
 */
AbstractSerial* ServoboardController::returnSerialPort()
{
    return port;
}

/*Public Slot*/

/*!
 * \brief This gets the file that the user wants to load and loads it if it is
 *      valid.
 *
 * This method gets the file name using the built in classes from Qt, and opens
 * the file. If there is an issue with the loading the file it will stop,
 * otherwise the sequence from the file is loaded. If there is a start position
 * in the file, it will ask it the user wishes to have it burnt in.
 */
void ServoboardController::loadFile()
{
    QString fileName = QFileDialog::getOpenFileName(view,
                                                    tr("Open Sequence"), "./", tr("Servo Sequence Files (*.SER *.SERVO)"));
    ServoboardController::fileName = fileName;
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
    //ask burn start position
    if (displayedData->hasStartPosition())
    {
        if (view->displayBurnQuery())
        {
            port->write(displayedData->getStartPositionCommand());
            view->displayBurnSuccess();
            emit this->newPositionSent(displayedData->getStartPosition());
        }
    }

}
/*!
 * \brief This saves the file if there is currently one known to the controller
 *      otherwise it will ask the user where they wish to save the file.
 *
 * When a file is opened or saved, the program saves that location and will
 * continue to save to that location when the save function is called. If there
 * is no locations known to the program, this will function the same as a call
 * to save as.
 */
void ServoboardController::saveFile()
{
    if (!this->checkForChangesToTextSequence())
    {
        qDebug() << tr("Save function cancelled by user");
        return;
    }
    if (ServoboardController::fileName.isEmpty())
    {
        this->saveFileAs();
    }
    if (fileName.endsWith(".SER") && !view->displaySaveFormatWaring())
    {
        qDebug() << tr("Save aborted");
    }
    this->displayedData->toFile(ServoboardController::fileName);

}
/*!
 * \brief This getes a file location from the user where they wish to save the
 *      file and saves the file there.
 *
 * This opens the dialog to get a file name from the user. Uses the name to as
 * the location to store the data. Before saving it chacks for changes and
 * confirms that the changes are okay. Finnally is re displays the file that
 * was saved to clear the changes from the view.
 */
void ServoboardController::saveFileAs()
{
    if (!this->checkForChangesToTextSequence())
    {
        qDebug() << tr("Save as operation was cancelled by the user");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(view,tr("Save Sequence As"),"./",
                                                    tr("Servo Sequnce file *.SERVO;;Eugen Servo File *.SER"));
    ServoboardController::fileName = fileName;
    if (fileName.endsWith(".SER") && !view->displaySaveFormatWaring()) //Lazy evaluation is fun!!
    {
        qDebug() << tr("Save as aborted");
    }
    displayedData->toFile(fileName);
}
/*!
 * \brief This takes a position and adds it to the current sequence. It then
 *      triggers the updated sequence to be rediplayed.
 *
 * It does not delete the pointer to the position.
 *
 * \param p
 *      A pointer to the position that is being added to the sequence.
 */
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
/*!
 * \brief This sets up for a sequnce to be played or resume playback
 *      if one has been paused.
 *
 * This method does not actually send the data to the serial port, this is
 * handled by the timerTimeout method.
 */
void ServoboardController::playCurrentSequence()
{
    if (this->displayedData == 0)
    {
        displayedData = new Sequence(this);
    }
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
    /// \todo There is a bug where global repeays may be ignored by sequence repeats.
    this->globalReplay = displayedData->getRepeats();
    this->playState = play;
    timer->singleShot(0,this,SLOT(timerTimeout()));


}
/*!
 * \brief Sends a positions out the serial port
 *
 * This will write any position that it is given a pointer to out the serial
 * port as long as it is non-empty. Note that it ignores the PWM repeat values
 * that may be attached to the position.
 *
 * \param p
 *      A pointer to the position that will be output. This pointer will be
 *      invalid after this call as the method calls delete on it.
 */
void ServoboardController::playPosition(Position *p)
{
    if (!p || p->isEmpty())
    {
        qDebug() << "Servo controller was passed an invalid position";
    }
    port->write(p->toServoSerialData());
    emit this->newPositionSent(p);
    delete p;
}
/*!
 * \brief Called when there needs to be a new position sent to the serial port
 *      during sequence playback.
 *
 * This method also handles when the playback state changes from play to pause
 * or stop.
 */
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
        this->currentReplays++;
        if (this->currentReplays >= this->globalReplay)
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
    Position *p;
    QByteArray data = displayedData->getNextData(p);
    this->port->write(data);
    emit this->newPositionSent(p);
    this->view->highlightNextLine();
    timer->singleShot(delay*1000,this,SLOT(timerTimeout()));
}
/*!
 * \brief Allows the user to set the global values using the advanced line
 *      options dialog.
 */
void ServoboardController::globalVariableSetRequested()
{
    int seqDelay,seqRepeat, PWMSweep, PWMRepeat;
    bool isFreeze;
    advancedLineOptionsDialog* dialog = new advancedLineOptionsDialog();
    dialog->showSequenceRepeat();
    dialog->setModal(true);
    dialog->exec(); //Modal
    if (dialog->getGlobalValues(isFreeze,seqDelay,seqRepeat,PWMSweep, PWMRepeat))
    {
        this->setGlobalDelay(seqDelay);
        this->setGlobalReplay(seqRepeat);
        this->displayedData->setPWMValues(PWMRepeat,PWMSweep);
    }
}
/*!
 * \brief Sets the sequence level delay for the current sequence.
 *
 * \param delay
 *      The delay value in seconds, this is not checked to see if it is valid.
 */
void ServoboardController::setGlobalDelay(int delay)
{
    if (!displayedData)
    {
        displayedData = new Sequence();
    }
    displayedData->setDelay(delay);
}
/*!
 * \brief Sets the sequence level replay value for the current sequence.
 *
 * \param replay
 *      The user facing value for the sequence replay. This is not checked to
 *      see if it is valid.
 */
void ServoboardController::setGlobalReplay(int replay)
{
    if (!displayedData)
    {
        displayedData = new Sequence();
    }
    displayedData->setReplay(replay);
}
/*!
 * \brief This is used to suppress if the warning about changes to the sequence
 *      text are displayed to the user.
 *
 * \param isChecked
 *      Whether the warnings about changing the text will be suppressed.
 */
void ServoboardController::suppressChangeNotifications(bool isChecked)
{
    qDebug() << this->suppressChangeNotification;
    this->suppressChangeNotification = isChecked;
    qDebug() << isChecked;
}
/*!
 * \brief Records that the user wishes to stop the sequence playback
 */
void ServoboardController::stopSequence()
{
    if (this->playState == pause)
    {
        this->resetAfterPlayback();
    }
    this->playState = stop;
}
/*!
 * \brief Records that the user wishes to pause the sequence playback.
 */
void ServoboardController::pauseSequence()
{
    this->playState = pause;
}
/*!
 * \brief Sets the sequence start position and burns it into the micro
 *
 * \param p
 *      The new starting position. This will be checked to ensure that it is
 *      a valid starting position.
 */
void ServoboardController::setStartPosition(Position *p)
{
    if (!displayedData)
    {
        displayedData = new Sequence();
    }
    if (!displayedData->setStartPosition(p))
    {
        //Display start state set failure.
        view->displaySetStartFailure();
        return;
    }
    QByteArray toWrite = displayedData->getStartPositionCommand();
    qint64 bytesWritten = this->port->write(toWrite);
    qDebug() << "Bytes written: " << bytesWritten;
    if (bytesWritten != toWrite.length())
    {
        qDebug() << "Not all bytes written to get the serial port in "
                 << "ServoboardController::setStartPosition(Position* p): "
                 << bytesWritten << " bytes written, " << toWrite.length()
                 << "expected.";
        return;
    }
    //Confirm start state burnt in memory.
    view->displayBurnSuccess();
    emit this->newPositionSent(p);
    return;
}
/*!
 * \brief Burns the start position from the micro using the value from a file.
 */
void ServoboardController::burnStartPosition()
{
    if (!this->displayedData)
    {
        view->displaySetStartFailure();
        displayedData = new Sequence();
        return;
    }
    if (!this->displayedData->hasStartPosition())
    {
        //show error
        view->displaySetStartFailure();
        return;
    }
    QByteArray toWrite = displayedData->getStartPositionCommand();
    qint64 bytesWritten = this->port->write(toWrite);
    qDebug() << "Bytes written: " << bytesWritten;
    if (bytesWritten != toWrite.length())
    {
        qDebug() << "Not all bytes written to get the serial port in "
                 << "ServoboardController::setStartPosition(Position* p): "
                 << bytesWritten << " bytes written, " << toWrite.length()
                 << "expected.";
        return;
    }
    //Confirm start state burnt in memory.
    view->displayBurnSuccess();
    emit this->newPositionSent(displayedData->getStartPosition());
    return;

}
void ServoboardController::hotModeToggle(Position *p)
{
    this->isHotMode = !this->isHotMode;
    if (isHotMode)
    {
        port->write(p->toServoSerialData());
        delete p;
        QTimer::singleShot(10,this,SLOT(hotModeUpdate()));
    }
}
void ServoboardController::hotModeUpdate()
{
    if (isHotMode)
    {
        Position *p = view->getHotModePosition();
        port->write(p->toServoSerialData());
        delete p;
        QTimer::singleShot(10,this,SLOT(hotModeUpdate()));
    }
}

/*Private Methods*/

/*!
 * \brief This resets the state of the program after a sequence has finished
 *      playing back.
 */
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
/*!
 * \brief This checks to see if the user has made a change to the sequence
 *      to see if they have made any changes.
 *
 * It checks for changes, if the user wants the changes and they are valid the
 * current sequence is updated. If they are not valid, then it asks whether the
 * changes are to be discarded or the operation stopped.
 *
 * \return True if the user wishes to continue with the operation, false
 *      false otherwise.
 */
bool ServoboardController::checkForChangesToTextSequence()
{
    if (!view->hasSequenceChanged()||
            (!view->hasSequenceInText() && displayedData->isEmpty()))//see if there are changes
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
        else if (view->currentSequenceText().isEmpty())
        {
            displayedData->clearStoredPositions();
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
