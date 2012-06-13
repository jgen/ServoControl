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
    this->initBundles();
    // Hardcoded values, yay!
    MinValue = 1;
    MiddleValue = 49;
    MaxValue = 97; // not sure about this one... Ask Eugen


    this->highlighter = new SequenceCompleteSyntaxHighlighter(ui->txtSequence->document());
}

servoboard_main::~servoboard_main()
{
    delete ui;
    for (int i(0); i < this->servoBundles.size();i++)
    {
        delete servoBundles[i];
    }

}

void servoboard_main::initBundles()
{

    ServoControlBundle* t;
    for (int i(1); i <= 12; ++i)
    {
        t = new ServoControlBundle(this);
        t->setServoNumber(i);
       ui->gridMainLayout->addWidget(t,(i/7) +1,(i-1)%6 ,1,1);
        this->servoBundles.append(t);
    }
    this->servoBundles.squeeze();//Make sure we aren't wasting memory
}

void servoboard_main::disableButtons()
{
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
void servoboard_main::enableButtons()
{
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
    return this->ui->txtSequence->toPlainText();
}

/* Select all the servo checkboxes */
void servoboard_main::on_btnSelectAll_clicked()
{
    for (int i=0; i< this->servoBundles.size(); i++)
    {
        this->servoBundles.at(i)->setChecked();
    }
}

/* Clear all the servo checkboxes */
void servoboard_main::on_btnClearAll_clicked()
{
    for (int i=0; i<this->servoBundles.size(); i++)
    {
        this->servoBundles.at(i)->setUnchecked();
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
    QStringList lines = this->ui->txtSequence->toPlainText().split("\n",QString::SkipEmptyParts);
    if (lines.length() < 1 || this->lastLineHighlighed >= lines.length())
    {
        qDebug() << tr("Failed to highligh as there was no text left to highlight");
        return false;
    }
    if (this->lastLineHighlighed == 0)
    {
        delete this->highlighter;
        this->highlighter = new SequenceSyntaxHighlighter(ui->txtSequence->document());
    }
    this->ui->txtSequence->clear();
    this->ui->txtSequence->setTextColor(QColor(Qt::blue));
    int lineCount(0);
    while (lineCount < this->lastLineHighlighed)
    {
        if (lines.at(lineCount).startsWith("#"))
        {
            this->ui->txtSequence->insertPlainText(lines.at(lineCount++) + "\n");
        }
        else
        {
            this->ui->txtSequence->insertPlainText(lines.at(lineCount++) + "\n");
        }
    }
    while (lines.at(lineCount).contains("#"))
    {
        this->ui->txtSequence->insertPlainText(lines.at(lineCount++) + "\n");
        this->lastLineHighlighed++;
    }
    this->ui->txtSequence->setTextColor(QColor(Qt::red));
    this->ui->txtSequence->insertPlainText(lines.at(lineCount++)+ "\n");
    this->ui->txtSequence->setTextColor(QColor(Qt::black));
    while(lineCount < lines.length())
    {        
        if (lines.at(lineCount).startsWith("#"))
        {
            this->ui->txtSequence->insertPlainText(lines.at(lineCount++) + "\n");
        }
        else
        {
            this->ui->txtSequence->insertPlainText(lines.at(lineCount++) + "\n");
        }
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
    delete this->highlighter;
    this->highlighter = new SequenceCompleteSyntaxHighlighter(ui->txtSequence->document());
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
    for(int i(0); i < this->servoBundles.size(); ++i)
    {
        if (servoBundles.at(i)->isSelected())
        {
            retval->addServoPosition(servoBundles.at(i)->getServoNumber(),
                                     servoBundles.at(i)->getValue());
        }
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
