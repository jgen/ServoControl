#include "servoboard_main.h"
#include "ui_servoboard_main.h"
#include <QDebug>
/*!
 * This sets up the parent of the widget, make sure that there is a form to
 * display it on as this cannot be a window by itself.
 */
servoboard_main::servoboard_main(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::servoboard_main),
    hasAdvancedLineOptions(false),
    isFreeze(false),
    PWMSweep(0),
    PWMRepeatIndex(0),
    sequenceDelay(1), //This has to be more than 0 at all times.
    lineOptions(0),
    hasTextChanged(false)
{
    ui->setupUi(this);//Get the ui up on the screen, without the servobundles.
    this->initBundles();//Get the bundles on the screen
    //Start with the full syntax highligher.
    this->highlighter = new SequenceCompleteSyntaxHighlighter(ui->txtSequence->document());
}
/*!
 * All pointers that are passed in are memory managed. They will be deleted
 * here making references invalid, watchout.
 */
servoboard_main::~servoboard_main()
{
    delete ui;
    for (int i(0); i < this->servoBundles.size();i++)
    {
        delete servoBundles[i];
    }

}
/*!
 * Sets up the servo bundles into the grid view that is already on the form.
 * It is done this way since it is not possible to add the user created
 * widgets on the widget using the designer program. As well it connects the
 * signals from the bundles to the the recieving slot.
 */
void servoboard_main::initBundles()
{

    ServoControlBundle* t;
    for (int i(1); i <= 12; ++i)//They are numbered with a 1 base, not a 0 base.
    {
        t = new ServoControlBundle(this);
        t->setServoNumber(i);
        ui->gridMainLayout->addWidget(t,(i/7)+1,(i-1)%6 ,1,1);//The columns start at 1 and the rows at 0
        //There is one handler for all the different buttons, so they notify which one.
        connect(t,SIGNAL(playClicked(quint8,quint8)),SLOT(servoPlayButtonClicked(quint8,quint8)));
        this->servoBundles.append(t);
    }
    this->servoBundles.squeeze();//Make sure we aren't wasting memory, this won't grow again.
}
/*!
 * This is used to disable the ui when there is no serial port connected.
 * It should disable all parts of the ui that the user can interact with.
 */
void servoboard_main::disableButtons()
{
    //Make sure there is no way to interact with the ui.
    this->ui->btnAdvancedLineOptions->setEnabled(false);
    this->ui->btnClearAll->setEnabled(false);
    this->ui->btnPlaySelected->setEnabled(false);
    this->ui->btnSelectAll->setEnabled(false);

    this->ui->btnStore->setEnabled(false);
    this->ui->btnPlaySequence->setEnabled(false);
    this->ui->chkUseAdvanced->setEnabled(false);
    this->ui->btnPause->setEnabled(false);
    this->ui->btnStopSequence->setEnabled(false);
    this->ui->btnHotModeStartStop->setEnabled(false);
    this->ui->btnSetStartPosition->setEnabled(false);

    for (int i = 0; i < this->servoBundles.size(); ++i)
    {
        servoBundles.at(i)->setInactive();
    }

}
/*!
 * This should re-enable the ui for interactions after being disabled.
 */
void servoboard_main::enableButtons()
{
    //Get the ui up and ready to interact.
    this->ui->btnAdvancedLineOptions->setEnabled(true);
    this->ui->btnClearAll->setEnabled(true);
    this->ui->btnPlaySelected->setEnabled(true);
    this->ui->btnSelectAll->setEnabled(true);
    for (int i = 0; i < this->servoBundles.size(); ++i)
    {
        servoBundles.at(i)->setActive();
    }

    this->ui->btnStore->setEnabled(true);
    this->ui->btnPlaySequence->setEnabled(true);
    this->ui->chkUseAdvanced->setEnabled(true);
    this->ui->btnPause->setEnabled(true);
    this->ui->btnStopSequence->setEnabled(true);
    this->ui->btnHotModeStartStop->setEnabled(true);
    this->ui->btnSetStartPosition->setEnabled(true);

}
/*!
 * \brief This displays a message box to the user that they are not connected
 *      to a serial port
 */
void servoboard_main::displayConnectionWarning()
{
    QMessageBox warn(this);
    warn.setText(tr("You must connect to the serial port first"));
    warn.setIcon(QMessageBox::Critical);
    warn.exec();
}
/*!
 * \brief Displays a warning when saving a file in the older file format.
 *
 * This method does not check to see if there is any information that cannont
 * be saved.
 *
 * \return True if the user wishes to continue with the save operation, false
 *      false otherwise.
 */
bool servoboard_main::displaySaveFormatWaring()
{
    //Warns that features can't be saved in the current format.
    QMessageBox warn(this);
    warn.setText(tr("Some advanced features cannot be saved in this format"));
    warn.setInformativeText(tr("Do you wish to continue saving?"));
    warn.setIcon(QMessageBox::Warning);
    warn.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    warn.setDefaultButton(QMessageBox::Cancel);
    warn.setWindowTitle("Older Format");
    int retval = warn.exec();//Not show, this is a synchonous call.
    switch(retval)
    {
    case QMessageBox::Ok:
        return true;
        break;
    case QMessageBox::Cancel:
        return false;
        break;
    default:
        qDebug() << tr("Failure to get return value from warning");
        return false;
        break;
    }
    return false;//Should never get here, this is to stop the warnings.

}
/*!
 * \brief Asks the user if they wish to keep changes they have made to the text
 *      edit by hand or if they were accidental.
 *
 * This is used to confirm if the user made changes to the sequence text edit
 * that they wish to keep. This doesn't check if the user changes are valid.
 * They are able to choose to keep or discard the changes or cancel the
 * operation if they wish to make further changes to the sequence box.
 *
 * \return QMessageBox::Yes if they wish to keep the changes,
 *      QMessageBox::No if they don't want the changes and QMessageBox::Cancel
 *      if they wish to cancel the operation that brought up the dialog.
 */
QMessageBox::StandardButton servoboard_main::displayKeepChangesWarning()
{
    //Ask if user changes are still wanted.
    QMessageBox warn(this);
    warn.setText(tr("The sequence apears to have been edited by hand"));
    warn.setInformativeText(tr("Do you wish to keep the changes?"));
    warn.setIcon(QMessageBox::Warning);
    warn.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    warn.setDefaultButton(QMessageBox::Yes);
    warn.setWindowTitle("Changes Made to Squence Text");
    int retval = warn.exec();
    switch(retval)//Don't really need this but I want to make sure something is always returned.
    {
    case QMessageBox::Yes:
        return QMessageBox::Ok;
        break;
    case QMessageBox::No:
        return QMessageBox::No;
        break;
    case QMessageBox::Cancel:
        return QMessageBox::Cancel;
        break;
    default:
        qDebug() << "Something went wrong getting your response.";
        return QMessageBox::Cancel;
    }
    return QMessageBox::Cancel;//Should never get here, this is to stop the warnings.
}
/*!
 * \brief Warns the users that the edits the user made are invalid and cannot
 *      be kept.
 *
 * If the user has made changes that cannot be kept, they can either continue
 * with the operation and loose the changes or they can cancel the operation
 * and keep the changes.
 *
 * \return True if they wish to loose the edits and continue, false otherwise.
 */
bool servoboard_main::displayInvalidEditsWarning()
{
    //Edits can't be kept, ask what they want to do.
    QMessageBox warn(this);
    warn.setText(tr("The edits to the squence are invalid"));
    warn.setInformativeText(tr("Do you wish to lose the edits and continue?"));
    warn.setIcon(QMessageBox::Critical);
    warn.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    warn.setDefaultButton(QMessageBox::Yes);
    warn.setWindowTitle(tr("Invalid Editing"));
    int retval = warn.exec(); //Sychonous call, we need the return value, makes the box modal
    switch(retval)
    {
    case QMessageBox::Yes:
        return true;
        break;
    case QMessageBox::No:
        return false;
        break;
    default:
        qDebug() << "Something went wrong getting your response.";
        return false;
    }
    return false;//Should never get here, this is to stop the warnings.
}
/*!
 * \brief Notify the user that the new starting position was burnt into the
 *       mirco.
 */
void servoboard_main::displayBurnSuccess()
{
    QMessageBox warn(this);
    warn.setText(tr("New starting position burt into micro."));
    warn.setInformativeText(tr("Mirco will now reset"));
    warn.setIcon(QMessageBox::Question);
    warn.setStandardButtons(QMessageBox::Ok);
    warn.setDefaultButton(QMessageBox::Ok);
    warn.setWindowTitle("Set Start Position");
    warn.exec();//Not show, this is a synchonous call.
}
/*!
 * \brief Notify the user that a starting position was found in the file
 *      and ask if they wish to burn it to the micro.
 *
 * When a file is loaded it can have the starting position burnt into the
 * microcontroller, but since EEPROM can only be written to a certain number
 * of times before it stops working, the user may not want to burn the starting
 * position in everytime.
 *
 * \return True if the user wishes to have the starting position burnt into the
 *      mircocontroller.
 */
bool servoboard_main::displayBurnQuery()
{
    //Start position found in file, ask to burn to chip
    QMessageBox warn(this);
    warn.setText(tr("A starting position was found in the file"));
    warn.setInformativeText(tr("Do you wish to burn it to the micro?"));
    warn.setIcon(QMessageBox::Critical);
    warn.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    warn.setDefaultButton(QMessageBox::Yes);
    warn.setWindowTitle(tr("Burn Staring Position"));
    int retval = warn.exec(); //Sychonous call, we need the return value, makes the box modal
    switch(retval)
    {
    case QMessageBox::Yes:
        return true;
        break;
    case QMessageBox::No:
        return false;
        break;
    default:
        qDebug() << "Something went wrong getting your response.";
        return false;
    }
    return false;//Should never get here, this is to stop the warnings.

}
/*!
 * \brief Informs the user that a start position could not be burnt into the
 *      mirco.
 */
void servoboard_main::displaySetStartFailure()
{
    QMessageBox warn(this);
    warn.setText(tr("Start position was invalid or didn't exist"));
    warn.setInformativeText(tr("No changes were made to the micro state"));
    warn.setIcon(QMessageBox::Warning);
    warn.setStandardButtons(QMessageBox::Ok);
    warn.setDefaultButton(QMessageBox::Ok);
    warn.setWindowTitle("Set Start Position");
    warn.exec();//Not show, this is a synchonous call.
}
/*!
 * \brief Places the text in the text edit. Records that thte text had not
 *      been changed.
 *
 * This class has to track if the text in the text edit has been changed by the
 * user from when the controller last updated the text in the box.
 *
 * \param sequnce
 *      This is the text that will be displayed in the sequence box.
 */
void servoboard_main::displayNewSequence(QString sequence)
{
    //Display the new sequecne, store that text has not been edited yet.
    this->ui->txtSequence->setPlainText(sequence);
    this->hasTextChanged = false;
}
/*!
 * \brief Quickly checks to see if there is enough text in the text edit for
 *      there to be a sequence in the box.
 *
 * Sometimes the user wishes to clear the sequence in the box, or there is no
 * information in the text edit. This method is used to check quickly if it is
 * worth parsing the contents of the box looking for a sequence.
 *
 * \return True if there are enough characters in the text edit to be a
 *      sequence, false otherwise.
 */
bool servoboard_main::hasSequenceInText()
{
    //This doens't actually parse anything, just if there are enough characters it
    //assumes that it might be a sequence.
    if (this->ui->txtSequence->toPlainText().trimmed().length() > 8
        && !this->ui->txtSequence->toPlainText().trimmed().isEmpty()) //Smallest unit of readable text
    {
        return true;
    }
    else
    {

        return false;
    }
}
/*!
 * \brief Returns the information in the text edit as a string
 *
 * The information in the text edit box is returned as a string, with all of
 * style information removed from it.
 *
 * \return The information in the text edit, with all of the style information
 *      stripped.
 */
QString servoboard_main::currentSequenceText()
{
    return this->ui->txtSequence->toPlainText(); //Plain text strips the unneeded style info
}

/*!
 * \brief Selects all of the servo bundle check boxes
 *
 * This is triggered when the select all button on the user interface is clicked and
 * checks all of the servo bundles to be stored or played.
 */
void servoboard_main::on_btnSelectAll_clicked()
{
    // Select all the servo checkboxes
    for (int i=0; i< this->servoBundles.size(); i++)
    {
        this->servoBundles.at(i)->setChecked();
    }
}

/*!
 * \brief Cleats all of the servo bundle check boxes
 *
 * This is triggered when the clear all button is clicked and clears all of the check
 * boxes on the interface.
 */
void servoboard_main::on_btnClearAll_clicked()
{
    //Clear all the servo checkboxes
    for (int i=0; i<this->servoBundles.size(); i++)
    {
        this->servoBundles.at(i)->setUnchecked();
    }
}
/*!
 * \brief Checks if the sequence text box has been edited by the user since the
 *      last time that the controller updated the text in the box.
 *
 * \return True if the text has been changed, false otherwise.
 */
bool servoboard_main::hasSequenceChanged()
{
    //return if the user has typed something into the text box
    return this->hasTextChanged;
}

/*!
 * \brief Highlights the next line in the sequence when it is being played back.
 *
 * When a sequence is being played the line that was last sent to the servo
 * control board is highlighted. The syntax highlighting switches to only
 * showing the comments as another colour, while this method highlights the
 * currently playing line as red, with the lines that have already played as
 * blue. It advances over the comment lines without having to count then as a
 * line to be played.
 *
 * \return True if the next line was successfully highlighted. False otherwise.
 */
bool servoboard_main::highlightNextLine()
{
    //This highlights the previous lines blue, the active line red and the rest are black.
    QStringList lines = this->ui->txtSequence->toPlainText().split("\n",QString::SkipEmptyParts);
    if (lines.length() < 1 || this->lastLineHighlighed >= lines.length())
    {
        qDebug() << tr("Failed to highlight as there was no text left to highlight");
        return false;
    }
    if (this->lastLineHighlighed == 0)//If this is the first time we are highlighting.
    {
        delete this->highlighter;//Get rid of the full syntax highlighter
        //This highligher only highlights the comments.
        this->highlighter = new SequenceSyntaxHighlighter(ui->txtSequence->document());
    }
    this->ui->txtSequence->clear();
    this->ui->txtSequence->setTextColor(QColor(Qt::blue));
    int lineCount(0);
    //Move through the lines that have already been highlighted.
    while (lineCount < this->lastLineHighlighed)
    {
        this->ui->txtSequence->insertPlainText(lines.at(lineCount++) + "\n");
    }
    //Skip over comment lines, they are not highlighed.
    while (lines.at(lineCount).contains("#"))
    {
        this->ui->txtSequence->insertPlainText(lines.at(lineCount++) + "\n");
        this->lastLineHighlighed++;
    }
    //Highlight the active line.
    this->ui->txtSequence->setTextColor(QColor(Qt::red));
    this->ui->txtSequence->insertPlainText(lines.at(lineCount++)+ "\n");
    this->ui->txtSequence->setTextColor(QColor(Qt::black));
    //Put the lines that haven't been highlighted yet back in.
    while(lineCount < lines.length())
    {
        this->ui->txtSequence->insertPlainText(lines.at(lineCount++) + "\n");
    }
    this->lastLineHighlighed++;
    return true;
}
/*!
 * \brief Cleans up after a sequence has been played and restores the text edit
 *      to the normal state.
 *
 * When a sequence has been played back all of the member variables have to reset,
 * the style information has to be stripped from the text that was being diplayed
 * and the complete syntax highlighting must be restored.
 */
void servoboard_main::resetHighlighting()
{
    //Reset the internal iterator and go back to normal syntax highlighting.
    this->lastLineHighlighed = 0;
    //Get rid of the formatting currently attached to the text.
    QString temp = this->ui->txtSequence->toPlainText();
    this->ui->txtSequence->setTextColor(QColor(Qt::black));
    this->ui->txtSequence->clear();
    this->ui->txtSequence->setText(temp);
    //Get switch back to the complete syntax highlighter.
    delete this->highlighter;
    this->highlighter = new SequenceCompleteSyntaxHighlighter(ui->txtSequence->document());
}
/*!
 * \brief Sets the interface to the correct state for sequence playback to be
 *      playing by enabling and disabling the relevant buttons.
 */
void servoboard_main::setPlayingState()
{
    //Make buttons avaible reflect that the sequence is playing.
    this->ui->btnStopSequence->setEnabled(true);
    this->ui->btnPause->setEnabled(true);
    this->ui->btnPlaySequence->setEnabled(false);
}
/*!
 * \brief Sets the interface to the correct state for sequence playback to be
 *      paused by enabling and disabling the relevant buttons.
 */
void servoboard_main::setPausedState()
{
    //Get buttons showing for the sequence being paused.
    this->ui->btnStopSequence->setEnabled(true);
    this->ui->btnPause->setEnabled(false);
    this->ui->btnPlaySequence->setEnabled(true);

}
/*!
 * \brief Sets the interface to the correct state for sequence playback to be
 *      stopped by enabling and disabling the relevant buttons.
 */
void servoboard_main::setStoppedState()
{
    //Set up for the sequece being not playing.
    this->ui->btnStopSequence->setEnabled(false);
    this->ui->btnPause->setEnabled(false);
    this->ui->btnPlaySequence->setEnabled(true);

}
/*Public Methods*/

/*!
 * \brief This is used to update the servo bundles with their current position.
 *
 * Since there is no feedback mechanism built into the board, the only way that
 * the program can know what the current positions of the servos are is when it
 * sends a command. This is used to inform the user what the last command sent
 * was
 *
 * \param newPosition
 *      The position that was last sent to the servo control board.
 */
void servoboard_main::servoPositionChanged(Position *newPosition)
{
    if (!newPosition)
    {
        return;
    }
    for(int i(1); i <= 12; ++i)
    {
        if (newPosition->hasPositonDataFor(i))
        {
            this->servoBundles.at(i - 1)->setServoValue(newPosition->getPositionDataFor(i));
        }
    }
}
/*!
 * \brief Returns the current position in all of the servo bundles.
 *
 * \return The position in the servo bundles
 */
Position* servoboard_main::getHotModePosition()
{
    this->on_btnSelectAll_clicked();
    Position* p = this->makePositionFromSelected();
    return p;
}

/*Private Slots*/

/*!
 * \brief Opens advanced line options dialog along side the current window.
 *
 */
void servoboard_main::on_btnAdvancedLineOptions_clicked()
{
    if (!lineOptions)
    {
        this->lineOptions = new advancedLineOptionsDialog(this);
        connect(lineOptions,SIGNAL(dialogClosed(bool,bool,int,int,int)),
                SLOT(lineOptionsClosed(bool,bool,int,int,int)));
    }
    lineOptions->open();//Asynchronous call, allow for them to change servo positions while
    //using the dialog.
}
/*!
 * \brief Gets the data from the line options dialog when it is closed with
 *      line options.
 *
 * This method captures the information from the advanced line options dialog
 * and stores the choices for future use.
 *
 * \param accepted
 *      States if the choices from the dialog box are to be used or not.
 *
 * \param freeze
 *      If the user wishes to have a freeze/unfreeze command sent for the line.
 *
 * \param PWMSweep
 *      The user selected PWM Sweep value
 *
 * \param PWMDelay
 *      The user selected PWM delay value
 *
 * \param sequenceDelay
 *      The user selected delay value.
 *
 */
void servoboard_main::lineOptionsClosed(bool accepted, bool freeze,int PWMSweep, int PWMDelay, int sequenceDelay)
{
    lineOptions->close();
    delete lineOptions;
    lineOptions = 0;
    this->hasAdvancedLineOptions = accepted;
    this->isFreeze = freeze;
    this->PWMSweep = PWMSweep;
    this->PWMRepeatIndex = PWMDelay;
    this->sequenceDelay = sequenceDelay;
}
/*!
 * \brief When the user clicks store gather the position in the text boxes and
 *      alert whoever is interested (listening to the newPostionToSequence
 *      signal
 */
void servoboard_main::on_btnStore_clicked()
{
    Position* p = this->makePositionFromSelected();
    emit newPositionToSequence(p);
}
/*!
 * \brief Collects the values from selected servo bundles and makes a position
 *      out of the values
 *
 * There are number of places where the information from the servo bundles is
 * needed. This method gathers the information from the bundles and stores it
 * in a position.
 *
 * \return A position made from the information in the selected servo bundles
 */
Position* servoboard_main::makePositionFromSelected()
{
    Position* retval = new Position();
    for(int i(0); i < this->servoBundles.size(); ++i)
    {
        if (servoBundles.at(i)->isSelected())
        {
            retval->addServoPosition(servoBundles.at(i)->getServoNumber(),
                                     servoBundles.at(i)->getValue());
        }
    }
    if (this->ui->chkUseAdvanced->isChecked() &&
            this->hasAdvancedLineOptions)
    {
        retval->setFreeze(this->isFreeze);
        retval->addAdvancedPosition(Position::PWMRepeat,PWMRepeatIndex);
        retval->addAdvancedPosition(Position::PWMSweep,PWMSweep);
        retval->addAdvancedPosition(Position::SeqDelay,sequenceDelay);
    }
    return retval;
}
/*!
 * \brief Records that the user has made a change the to sequence text.
 */
void servoboard_main::on_txtSequence_textChanged()
{
    this->hasTextChanged = true;
}
/*!
 * \brief Informs interested parties that the user wished to play a sequence
 *      by using the playSequence signal.
 */
void servoboard_main::on_btnPlaySequence_clicked()
{
    this->setPlayingState();
    emit this->playSequence();
}
/*!
 * \brief Called when a servo bundle has the play button clicked, informs
 *      interested parties that the user wishes to have a new position
 *      sent.
 *
 * The method communicates that there should be a new position sent by using
 * the playPosition signal.
 *
 * \param servoNumber
 *      The index of the servo that the user wishes to send data to.
 *
 * \param servoValue
 *      The value that the user wishes to send to the servo.
 */
void servoboard_main::servoPlayButtonClicked(quint8 servoNumber, quint8 servoValue)
{
    Position *p = new Position();
    p->addServoPosition(servoNumber,servoValue);
    emit this->playPosition(p);
}
/*!
 * \brief Called when the Play Selected button is pressed, it gathers
 *      the position made from the selected bundles and emits the
 *      playPosition signal to any interested parties.
 */
void servoboard_main::on_btnPlaySelected_clicked()
{
    Position *p = this->makePositionFromSelected();
    emit this->playPosition(p);

}
/*!
 * \brief Informs interested parties that the user wished to pause playback if
 *      a sequence by using the pauseSequence signal.
 */
void servoboard_main::on_btnPause_clicked()
{
    this->setPausedState();
    emit this->pauseSequence();

}
/*!
 * \brief Informs interested parties that the user wished to stop plaback of
 *      a sequence by using the stopSequence signal.
 */
void servoboard_main::on_btnStopSequence_clicked()
{
    this->setStoppedState();
    emit this->stopSequence();
}
/*!
 * \brief Selects all of the servo bundles, makes a position from them and
 *      emits the setStartPosition signal to any interested parties.
 */
void servoboard_main::on_btnSetStartPosition_clicked()
{
    this->on_btnSelectAll_clicked();
    Position* p = this->makePositionFromSelected();
    emit this->setStartPosition(p);
}
/*!
 * \brief This is an experiemental feature, do not trust.
 */
void servoboard_main::on_btnHotModeStartStop_clicked()
{

    this->on_btnSelectAll_clicked();
    Position* p = this->makePositionFromSelected();
    emit this->hotModeToggle(p);
    if (this->ui->btnHotModeStartStop->text().endsWith("Stop, too hot"))
    {
        this->ui->btnHotModeStartStop->setText("Let's run it hot");
    }
    else
    {
        this->ui->btnHotModeStartStop->setText("Stop, too hot");
    }

}
/*!
 * \brief Load the last position sent to the servo control board.
 */

void servoboard_main::on_btnLoadCurrent_clicked()
{
    for (int i(0); i < 12; i++)
    {
        this->servoBundles.at(i)->currentToNext();
    }
}
