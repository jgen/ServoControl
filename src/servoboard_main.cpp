#include "servoboard_main.h"
#include "ui_servoboard_main.h"
#include <QDebug>

servoboard_main::servoboard_main(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::servoboard_main),
        lineOptions(0),
        hasTextChanged(false),
        hasAdvancedLineOptions(false),
        isFreeze(false),
        PWMSweep(0),
        PWMRepeatIndex(0),
        sequenceDelay(1)
{
    ui->setupUi(this);

    // Hardcoded values, yay!
    MinValue = 1;
    MiddleValue = 49;
    MaxValue = 97; // not sure about this one... Ask Eugen

    setValueForAll(MiddleValue);
    setMaxValueForAll(MaxValue);
    setMinValueForAll(MinValue);

    servosEnabled = new QVector<QCheckBox*>(0);
    servosEnabled->append( this->ui->chkServo1  );
    servosEnabled->append( this->ui->chkServo2  );
    servosEnabled->append( this->ui->chkServo3  );
    servosEnabled->append( this->ui->chkServo4  );
    servosEnabled->append( this->ui->chkServo5  );
    servosEnabled->append( this->ui->chkServo6  );
    servosEnabled->append( this->ui->chkServo7  );
    servosEnabled->append( this->ui->chkServo8  );
    servosEnabled->append( this->ui->chkServo9  );
    servosEnabled->append( this->ui->chkServo10 );
    servosEnabled->append( this->ui->chkServo11 );
    servosEnabled->append( this->ui->chkServo12 );

    this->highlighter = new SequenceSyntaxHighlighter(ui->txtSequence->document());
}

servoboard_main::~servoboard_main()
{
    delete ui;
    delete servosEnabled;
}
void servoboard_main::disableButtons()
{
    this->ui->btnAdvancedLineOptions->setEnabled(false);
    this->ui->btnClearAll->setEnabled(false);
    this->ui->btnPlaySelected->setEnabled(false);
    this->ui->btnSelectAll->setEnabled(false);
    this->ui->btnServo1->setEnabled(false);
    this->ui->btnServo2->setEnabled(false);
    this->ui->btnServo3->setEnabled(false);
    this->ui->btnServo4->setEnabled(false);
    this->ui->btnServo5->setEnabled(false);
    this->ui->btnServo6->setEnabled(false);
    this->ui->btnServo7->setEnabled(false);
    this->ui->btnServo8->setEnabled(false);
    this->ui->btnServo9->setEnabled(false);
    this->ui->btnServo10->setEnabled(false);
    this->ui->btnServo11->setEnabled(false);
    this->ui->btnServo12->setEnabled(false);

    this->ui->chkServo1->setEnabled(false);
    this->ui->chkServo2->setEnabled(false);
    this->ui->chkServo3->setEnabled(false);
    this->ui->chkServo4->setEnabled(false);
    this->ui->chkServo5->setEnabled(false);
    this->ui->chkServo6->setEnabled(false);
    this->ui->chkServo7->setEnabled(false);
    this->ui->chkServo8->setEnabled(false);
    this->ui->chkServo9->setEnabled(false);
    this->ui->chkServo10->setEnabled(false);
    this->ui->chkServo11->setEnabled(false);
    this->ui->chkServo12->setEnabled(false);

    this->ui->sliderServo1->setEnabled(false);
    this->ui->sliderServo2->setEnabled(false);
    this->ui->sliderServo3->setEnabled(false);
    this->ui->sliderServo4->setEnabled(false);
    this->ui->sliderServo5->setEnabled(false);
    this->ui->sliderServo6->setEnabled(false);
    this->ui->sliderServo7->setEnabled(false);
    this->ui->sliderServo8->setEnabled(false);
    this->ui->sliderServo9->setEnabled(false);
    this->ui->sliderServo10->setEnabled(false);
    this->ui->sliderServo11->setEnabled(false);
    this->ui->sliderServo12->setEnabled(false);

    this->ui->spinServo1->setEnabled(false);
    this->ui->spinServo2->setEnabled(false);
    this->ui->spinServo3->setEnabled(false);
    this->ui->spinServo4->setEnabled(false);
    this->ui->spinServo5->setEnabled(false);
    this->ui->spinServo6->setEnabled(false);
    this->ui->spinServo7->setEnabled(false);
    this->ui->spinServo8->setEnabled(false);
    this->ui->spinServo9->setEnabled(false);
    this->ui->spinServo10->setEnabled(false);
    this->ui->spinServo11->setEnabled(false);
    this->ui->spinServo12->setEnabled(false);

    this->ui->btnStore->setEnabled(false);
    this->ui->btnPlaySequence->setEnabled(false);
    this->ui->chkUseAdvanced->setEnabled(false);
    this->ui->btnPause->setEnabled(false);
    this->ui->btnStopSequence->setEnabled(false);

}
void servoboard_main::enableButtons()
{
    this->ui->btnAdvancedLineOptions->setEnabled(true);
    this->ui->btnClearAll->setEnabled(true);
    this->ui->btnPlaySelected->setEnabled(true);
    this->ui->btnSelectAll->setEnabled(true);
    this->ui->btnServo1->setEnabled(true);
    this->ui->btnServo2->setEnabled(true);
    this->ui->btnServo3->setEnabled(true);
    this->ui->btnServo4->setEnabled(true);
    this->ui->btnServo5->setEnabled(true);
    this->ui->btnServo6->setEnabled(true);
    this->ui->btnServo7->setEnabled(true);
    this->ui->btnServo8->setEnabled(true);
    this->ui->btnServo9->setEnabled(true);
    this->ui->btnServo10->setEnabled(true);
    this->ui->btnServo11->setEnabled(true);
    this->ui->btnServo12->setEnabled(true);




    this->ui->chkServo1->setEnabled(true);
    this->ui->chkServo2->setEnabled(true);
    this->ui->chkServo3->setEnabled(true);
    this->ui->chkServo4->setEnabled(true);
    this->ui->chkServo5->setEnabled(true);
    this->ui->chkServo6->setEnabled(true);
    this->ui->chkServo7->setEnabled(true);
    this->ui->chkServo8->setEnabled(true);
    this->ui->chkServo9->setEnabled(true);
    this->ui->chkServo10->setEnabled(true);
    this->ui->chkServo11->setEnabled(true);
    this->ui->chkServo12->setEnabled(true);

    this->ui->sliderServo1->setEnabled(true);
    this->ui->sliderServo2->setEnabled(true);
    this->ui->sliderServo3->setEnabled(true);
    this->ui->sliderServo4->setEnabled(true);
    this->ui->sliderServo5->setEnabled(true);
    this->ui->sliderServo6->setEnabled(true);
    this->ui->sliderServo7->setEnabled(true);
    this->ui->sliderServo8->setEnabled(true);
    this->ui->sliderServo9->setEnabled(true);
    this->ui->sliderServo10->setEnabled(true);
    this->ui->sliderServo11->setEnabled(true);
    this->ui->sliderServo12->setEnabled(true);

    this->ui->spinServo1->setEnabled(true);
    this->ui->spinServo2->setEnabled(true);
    this->ui->spinServo3->setEnabled(true);
    this->ui->spinServo4->setEnabled(true);
    this->ui->spinServo5->setEnabled(true);
    this->ui->spinServo6->setEnabled(true);
    this->ui->spinServo7->setEnabled(true);
    this->ui->spinServo8->setEnabled(true);
    this->ui->spinServo9->setEnabled(true);
    this->ui->spinServo10->setEnabled(true);
    this->ui->spinServo11->setEnabled(true);
    this->ui->spinServo12->setEnabled(true);

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
    QMessageBox warn(this);
    warn.setText(tr("Some advanced features cannot be saved in this format"));
    warn.setInformativeText(tr("Do you wish to continue saving?"));
    warn.setIcon(QMessageBox::Warning);
    warn.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    warn.setDefaultButton(QMessageBox::Cancel);
    warn.setWindowTitle("Older Format");
    int retval = warn.exec();
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
    QMessageBox warn(this);
    warn.setText(tr("The sequence apears to have been edited by hand"));
    warn.setInformativeText(tr("Do you wish to keep the changes?"));
    warn.setIcon(QMessageBox::Warning);
    warn.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    warn.setDefaultButton(QMessageBox::Yes);
    warn.setWindowTitle("Changes Made to Squence Text");
    int retval = warn.exec();
    switch(retval)
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
    QMessageBox warn(this);
    warn.setText(tr("The edits to the squence are invaid"));
    warn.setInformativeText(tr("Do you wish to lose the edits and continue?"));
    warn.setIcon(QMessageBox::Critical);
    warn.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    warn.setDefaultButton(QMessageBox::Yes);
    warn.setWindowTitle(tr("Invalid Editing"));
    int retval = warn.exec();
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

void servoboard_main::displayNewSequence(QString sequence)
{
    this->ui->txtSequence->setPlainText(sequence);
    this->hasTextChanged = false;
}
bool servoboard_main::hasSequenceInText()
{
    if (this->ui->txtSequence->toPlainText().length() > 8) //Smallest unit of readable text
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
    return this->ui->txtSequence->toPlainText();
}

/* Select all the servo checkboxes */
void servoboard_main::on_btnSelectAll_clicked()
{
    for (int i=0; i<servosEnabled->size(); i++) {
        servosEnabled->at(i)->setChecked(true);
    }
}

/* Clear all the servo checkboxes */
void servoboard_main::on_btnClearAll_clicked()
{
    for (int i=0; i<servosEnabled->size(); i++) {
        servosEnabled->at(i)->setChecked(false);
    }
}

bool servoboard_main::hasSequenceChanged()
{
    return this->hasTextChanged;
}

bool servoboard_main::userSuppressChangeNotification()
{
}

bool servoboard_main::highlightNextLine()
{
    QStringList lines = this->ui->txtSequence->toPlainText().split("\n");
    if (lines.length() < 1 || this->lastLineHighlighed >= lines.length())
    {
        qDebug() << tr("Failed to highligh as there was no text left to highlight");
        return false;
    }
    this->ui->txtSequence->clear();
    this->ui->txtSequence->setTextColor(QColor(Qt::blue));
    int lineCount(0);
    while (lineCount < this->lastLineHighlighed)
    {
        this->ui->txtSequence->insertPlainText(lines.at(lineCount++) + "\n");
    }
    this->ui->txtSequence->setTextColor(QColor(Qt::red));
    this->ui->txtSequence->insertPlainText(lines.at(lineCount++)+ "\n");
    this->ui->txtSequence->setTextColor(QColor(Qt::black));
    while(lineCount < lines.length())
    {
        this->ui->txtSequence->insertPlainText(lines.at(lineCount++) + "\n");
    }
    this->lastLineHighlighed++;
    return true;
}

void servoboard_main::resetHighlighting()
{
    this->lastLineHighlighed = 0;
    QString temp = this->ui->txtSequence->toPlainText();
    this->ui->txtSequence->setTextColor(QColor(Qt::black));
    this->ui->txtSequence->clear();
    this->ui->txtSequence->setText(temp);
}

void servoboard_main::setPlayingState()
{
    this->ui->btnStopSequence->setEnabled(true);
    this->ui->btnPause->setEnabled(true);
    this->ui->btnPlaySequence->setEnabled(false);
}
void servoboard_main::setPausedState()
{
    this->ui->btnStopSequence->setEnabled(true);
    this->ui->btnPause->setEnabled(false);
    this->ui->btnPlaySequence->setEnabled(true);

}
void servoboard_main::setStoppedState()
{
    this->ui->btnStopSequence->setEnabled(false);
    this->ui->btnPause->setEnabled(false);
    this->ui->btnPlaySequence->setEnabled(true);

}

/*
 * Convenience funtion
 *
 * Sets the value for all servos controls
 */
void servoboard_main::setValueForAll(unsigned char value)
{
    // I wish there was an easier way to do this.. (templates maybe?)
    ui->spinServo1->setValue(value);
    ui->sliderServo1->setValue(value);
    ui->spinServo2->setValue(value);
    ui->sliderServo2->setValue(value);
    ui->spinServo3->setValue(value);
    ui->sliderServo3->setValue(value);
    ui->spinServo4->setValue(value);
    ui->sliderServo4->setValue(value);
    ui->spinServo5->setValue(value);
    ui->sliderServo5->setValue(value);
    ui->spinServo6->setValue(value);
    ui->sliderServo6->setValue(value);
    ui->spinServo7->setValue(value);
    ui->sliderServo7->setValue(value);
    ui->spinServo8->setValue(value);
    ui->sliderServo8->setValue(value);
    ui->spinServo9->setValue(value);
    ui->sliderServo9->setValue(value);
    ui->spinServo10->setValue(value);
    ui->sliderServo10->setValue(value);
    ui->spinServo11->setValue(value);
    ui->sliderServo11->setValue(value);
    ui->spinServo12->setValue(value);
    ui->sliderServo12->setValue(value);
}

/*
 * Convenience funtion
 *
 * Sets the maximum value for all servos controls
 */
void servoboard_main::setMaxValueForAll(unsigned char value)
{
    // I wish there was an easier way to do this.. (templates maybe?)
    ui->spinServo1->setMaximum(value);
    ui->sliderServo1->setMaximum(value);
    ui->spinServo2->setMaximum(value);
    ui->sliderServo2->setMaximum(value);
    ui->spinServo3->setMaximum(value);
    ui->sliderServo3->setMaximum(value);
    ui->spinServo4->setMaximum(value);
    ui->sliderServo4->setMaximum(value);
    ui->spinServo5->setMaximum(value);
    ui->sliderServo5->setMaximum(value);
    ui->spinServo6->setMaximum(value);
    ui->sliderServo6->setMaximum(value);
    ui->spinServo7->setMaximum(value);
    ui->sliderServo7->setMaximum(value);
    ui->spinServo8->setMaximum(value);
    ui->sliderServo8->setMaximum(value);
    ui->spinServo9->setMaximum(value);
    ui->sliderServo9->setMaximum(value);
    ui->spinServo10->setMaximum(value);
    ui->sliderServo10->setMaximum(value);
    ui->spinServo11->setMaximum(value);
    ui->sliderServo11->setMaximum(value);
    ui->spinServo12->setMaximum(value);
    ui->sliderServo12->setMaximum(value);
}

/*
 * Convenience funtion
 *
 * Sets the minimum value for all servos controls
 */
void servoboard_main::setMinValueForAll(unsigned char value)
{
    // I wish there was an easier way to do this.. (templates maybe?)
    ui->spinServo1->setMinimum(value);
    ui->sliderServo1->setMinimum(value);
    ui->spinServo2->setMinimum(value);
    ui->sliderServo2->setMinimum(value);
    ui->spinServo3->setMinimum(value);
    ui->sliderServo3->setMinimum(value);
    ui->spinServo4->setMinimum(value);
    ui->sliderServo4->setMinimum(value);
    ui->spinServo5->setMinimum(value);
    ui->sliderServo5->setMinimum(value);
    ui->spinServo6->setMinimum(value);
    ui->sliderServo6->setMinimum(value);
    ui->spinServo7->setMinimum(value);
    ui->sliderServo7->setMinimum(value);
    ui->spinServo8->setMinimum(value);
    ui->sliderServo8->setMinimum(value);
    ui->spinServo9->setMinimum(value);
    ui->sliderServo9->setMinimum(value);
    ui->spinServo10->setMinimum(value);
    ui->sliderServo10->setMinimum(value);
    ui->spinServo11->setMinimum(value);
    ui->sliderServo11->setMinimum(value);
    ui->spinServo12->setMinimum(value);
    ui->sliderServo12->setMinimum(value);
}



/*
    Handlers for the Sliders and Spin boxes
 */

void servoboard_main::on_sliderServo1_valueChanged(int value)
{
    ui->spinServo1->setValue(value);
}
void servoboard_main::on_spinServo1_valueChanged(int value)
{
    ui->sliderServo1->setValue(value);
}


void servoboard_main::on_sliderServo2_valueChanged(int value)
{
    ui->spinServo2->setValue(value);
}
void servoboard_main::on_spinServo2_valueChanged(int value)
{
    ui->sliderServo2->setValue(value);
}


void servoboard_main::on_sliderServo3_valueChanged(int value)
{
    ui->spinServo3->setValue(value);
}
void servoboard_main::on_spinServo3_valueChanged(int value)
{
    ui->sliderServo3->setValue(value);
}


void servoboard_main::on_sliderServo4_valueChanged(int value)
{
    ui->spinServo4->setValue(value);
}
void servoboard_main::on_spinServo4_valueChanged(int value)
{
    ui->sliderServo4->setValue(value);
}


void servoboard_main::on_sliderServo5_valueChanged(int value)
{
    ui->spinServo5->setValue(value);
}
void servoboard_main::on_spinServo5_valueChanged(int value)
{
    ui->sliderServo5->setValue(value);
}


void servoboard_main::on_sliderServo6_valueChanged(int value)
{
    ui->spinServo6->setValue(value);
}
void servoboard_main::on_spinServo6_valueChanged(int value)
{
    ui->sliderServo6->setValue(value);
}


void servoboard_main::on_sliderServo7_valueChanged(int value)
{
    ui->spinServo7->setValue(value);
}
void servoboard_main::on_spinServo7_valueChanged(int value)
{
    ui->sliderServo7->setValue(value);
}


void servoboard_main::on_sliderServo8_valueChanged(int value)
{
    ui->spinServo8->setValue(value);
}
void servoboard_main::on_spinServo8_valueChanged(int value)
{
    ui->sliderServo8->setValue(value);
}


void servoboard_main::on_sliderServo9_valueChanged(int value)
{
    ui->spinServo9->setValue(value);
}
void servoboard_main::on_spinServo9_valueChanged(int value)
{
    ui->sliderServo9->setValue(value);
}


void servoboard_main::on_sliderServo10_valueChanged(int value)
{
    ui->spinServo10->setValue(value);
}
void servoboard_main::on_spinServo10_valueChanged(int value)
{
    ui->sliderServo10->setValue(value);
}


void servoboard_main::on_sliderServo11_valueChanged(int value)
{
    ui->spinServo11->setValue(value);
}
void servoboard_main::on_spinServo11_valueChanged(int value)
{
    ui->sliderServo11->setValue(value);
}


void servoboard_main::on_sliderServo12_valueChanged(int value)
{
    ui->spinServo12->setValue(value);
}
void servoboard_main::on_spinServo12_valueChanged(int value)
{
    ui->sliderServo12->setValue(value);
}


void servoboard_main::on_btnAdvancedLineOptions_clicked()
{
    if (!lineOptions)
    {
        this->lineOptions = new advancedLineOptionsDialog(this);
        connect(lineOptions,SIGNAL(dialogClosed(bool,bool,int,int,int)),
                SLOT(lineOptionsClosed(bool,bool,int,int,int)));
    }
    lineOptions->open();

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
    if (servosEnabled->at(0)->isChecked())
    {
        retval->addServoPosition(1,ui->spinServo1->value());
    }
    if (servosEnabled->at(1)->isChecked())
    {
        retval->addServoPosition(2,ui->spinServo1->value());
    }
    if (servosEnabled->at(2)->isChecked())
    {
        retval->addServoPosition(3,ui->spinServo1->value());
    }
    if (servosEnabled->at(3)->isChecked())
    {
        retval->addServoPosition(4,ui->spinServo1->value());
    }
    if (servosEnabled->at(4)->isChecked())
    {
        retval->addServoPosition(5,ui->spinServo1->value());
    }
    if (servosEnabled->at(5)->isChecked())
    {
        retval->addServoPosition(6,ui->spinServo1->value());
    }
    if (servosEnabled->at(6)->isChecked())
    {
        retval->addServoPosition(7,ui->spinServo1->value());
    }
    if (servosEnabled->at(7)->isChecked())
    {
        retval->addServoPosition(8,ui->spinServo1->value());
    }
    if (servosEnabled->at(8)->isChecked())
    {
        retval->addServoPosition(9,ui->spinServo1->value());
    }
    if (servosEnabled->at(9)->isChecked())
    {
        retval->addServoPosition(10,ui->spinServo1->value());
    }
    if (servosEnabled->at(10)->isChecked())
    {
        retval->addServoPosition(11,ui->spinServo1->value());
    }
    if (servosEnabled->at(11)->isChecked())
    {
        retval->addServoPosition(12,ui->spinServo1->value());
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

void servoboard_main::on_btnServo1_clicked()
{
    Position* p = new Position();
    p->addServoPosition(1,ui->spinServo1->value());
    emit this->playPosition(p);
}

void servoboard_main::on_btnServo2_clicked()
{
    Position* p = new Position();
    p->addServoPosition(2,ui->spinServo2->value());
    emit this->playPosition(p);
}

void servoboard_main::on_btnServo3_clicked()
{
    Position* p = new Position();
    p->addServoPosition(3,ui->spinServo3->value());
    emit this->playPosition(p);
}

void servoboard_main::on_btnServo4_clicked()
{
    Position* p = new Position();
    p->addServoPosition(4,ui->spinServo4->value());
    emit this->playPosition(p);
}

void servoboard_main::on_btnServo5_clicked()
{
    Position* p = new Position();
    p->addServoPosition(5,ui->spinServo5->value());
    emit this->playPosition(p);
}

void servoboard_main::on_btnServo6_clicked()
{
    Position* p = new Position();
    p->addServoPosition(6,ui->spinServo6->value());
    emit this->playPosition(p);
}

void servoboard_main::on_btnServo7_clicked()
{
    Position* p = new Position();
    p->addServoPosition(7,ui->spinServo7->value());
    emit this->playPosition(p);
}

void servoboard_main::on_btnServo8_clicked()
{
    Position* p = new Position();
    p->addServoPosition(8,ui->spinServo8->value());
    emit this->playPosition(p);
}

void servoboard_main::on_btnServo9_clicked()
{
    Position* p = new Position();
    p->addServoPosition(9,ui->spinServo9->value());
    emit this->playPosition(p);
}

void servoboard_main::on_btnServo10_clicked()
{
    Position* p = new Position();
    p->addServoPosition(10,ui->spinServo10->value());
    emit this->playPosition(p);
}

void servoboard_main::on_btnServo11_clicked()
{
    Position* p = new Position();
    p->addServoPosition(11,ui->spinServo11->value());
    emit this->playPosition(p);
}

void servoboard_main::on_btnServo12_clicked()
{
    Position* p = new Position();
    p->addServoPosition(12,ui->spinServo12->value());
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
