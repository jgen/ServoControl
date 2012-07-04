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

}
void servoboard_main::displayConnectionWarning()
{
    QMessageBox warn(this);
    warn.setText(tr("You must connect to the serial port first"));
    warn.setIcon(QMessageBox::Critical);
    warn.exec();
}

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

void servoboard_main::displayNewSequence(QString sequence)
{
    //Display the new sequecne, store that text has not been edited yet.
    this->ui->txtSequence->setPlainText(sequence);
    this->hasTextChanged = false;
}
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
QString servoboard_main::currentSequenceText()
{
    return this->ui->txtSequence->toPlainText(); //Plain text strips the unneeded style info
}


void servoboard_main::on_btnSelectAll_clicked()
{
    // Select all the servo checkboxes
    for (int i=0; i< this->servoBundles.size(); i++)
    {
        this->servoBundles.at(i)->setChecked();
    }
}


void servoboard_main::on_btnClearAll_clicked()
{
    //Clear all the servo checkboxes
    for (int i=0; i<this->servoBundles.size(); i++)
    {
        this->servoBundles.at(i)->setUnchecked();
    }
}

bool servoboard_main::hasSequenceChanged()
{
    //return if the user has typed something into the text box
    return this->hasTextChanged;
}


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
    //Move thought the lines that have already been highlighted.
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

void servoboard_main::setPlayingState()
{
    //Make buttons avaible reflect that the sequence is playing.
    this->ui->btnStopSequence->setEnabled(true);
    this->ui->btnPause->setEnabled(true);
    this->ui->btnPlaySequence->setEnabled(false);
}
void servoboard_main::setPausedState()
{
    //Get buttons showing for the sequence being paused.
    this->ui->btnStopSequence->setEnabled(true);
    this->ui->btnPause->setEnabled(false);
    this->ui->btnPlaySequence->setEnabled(true);

}
void servoboard_main::setStoppedState()
{
    //Set up for the sequece being not playing.
    this->ui->btnStopSequence->setEnabled(false);
    this->ui->btnPause->setEnabled(false);
    this->ui->btnPlaySequence->setEnabled(true);

}
/*Public Methods*/

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

/*Private Slots*/
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

void servoboard_main::on_btnStore_clicked()
{
    Position* p = this->makePositionFromSelected();
    emit newPositionToSequence(p);
}
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
void servoboard_main::on_txtSequence_textChanged()
{
    this->hasTextChanged = true;
}

void servoboard_main::on_btnPlaySequence_clicked()
{
    this->setPlayingState();
    emit this->playSequence();
}

void servoboard_main::servoPlayButtonClicked(quint8 servoNumber, quint8 servoValue)
{
    Position *p = new Position();
    p->addServoPosition(servoNumber,servoValue);
    emit this->playPosition(p);
}

void servoboard_main::on_btnPlaySelected_clicked()
{
    Position *p = this->makePositionFromSelected();
    emit this->playPosition(p);

}

void servoboard_main::on_btnPause_clicked()
{
    this->setPausedState();
    emit this->pauseSequence();

}

void servoboard_main::on_btnStopSequence_clicked()
{
    this->setStoppedState();
    emit this->stopSequence();
}

void servoboard_main::on_btnSetStartPosition_clicked()
{
    this->on_btnSelectAll_clicked();
    Position* p = this->makePositionFromSelected();
    emit this->setStartPosition(p);
}
