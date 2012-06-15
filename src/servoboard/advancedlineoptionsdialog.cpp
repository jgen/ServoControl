#include "advancedlineoptionsdialog.h"
#include "ui_advancedlineoptionsdialog.h"

advancedLineOptionsDialog::advancedLineOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::advancedLineOptionsDialog)
{
    ui->setupUi(this);
    //I am hardcoding in the values for the dialog box for now
    //but I would like to have them at some point factored out
    //for use in other dialog boxes.
    this->ui->cmbPWMRepeat->addItem(tr("No Sweeping"));
    this->ui->cmbPWMRepeat->addItem(tr("1"));
    this->ui->cmbPWMRepeat->addItem(tr("2"));
    this->ui->cmbPWMRepeat->addItem(tr("10"));
    this->ui->cmbPWMRepeat->addItem(tr("25"));
    this->ui->cmbPWMRepeat->addItem(tr("50"));
    this->ui->cmbPWMRepeat->addItem(tr("100"));
    this->ui->cmbPWMRepeat->addItem(tr("150"));
    this->ui->cmbPWMRepeat->addItem(tr("200"));

    this->ui->cmbSequenceRepeat->addItem(tr("1"));
    this->ui->cmbSequenceRepeat->addItem(tr("2"));
    this->ui->cmbSequenceRepeat->addItem(tr("10"));
    this->ui->cmbSequenceRepeat->addItem(tr("25"));
    this->ui->cmbSequenceRepeat->addItem(tr("50"));
    this->ui->cmbSequenceRepeat->addItem(tr("100"));
    this->ui->cmbSequenceRepeat->addItem(tr("150"));
    this->ui->cmbSequenceRepeat->addItem(tr("200"));

    this->ui->cmbPWMSweep->addItem("No Sweeping");
    for(int i = 1; i < 16; ++i)
    {
        this->ui->cmbSequenceDelay->addItem(tr("%1").arg(i));
        this->ui->cmbPWMSweep->addItem(tr("%1").arg(i));
    }
    this->ui->cmbPWMRepeat->setCurrentIndex(0);
    this->ui->cmbPWMSweep->setCurrentIndex(0);
    this->ui->cmbSequenceDelay->setCurrentIndex(0);
    this->ui->rdbFreeze->setChecked(true);

    //Hide the elements not needed for line options
    this->ui->cmbSequenceRepeat->hide();
    this->ui->lblSeqRepeat->hide();
    this->isRepeat = false;
}

advancedLineOptionsDialog::~advancedLineOptionsDialog()
{
    delete ui;
}
void advancedLineOptionsDialog::showSequenceRepeat()
{
    //Hide the line options, and show the global ones.
    this->ui->rdbFreeze->setEnabled(false);
    this->ui->rdbNoFreeze->setEnabled(false);
    this->ui->cmbSequenceRepeat->show();
    this->ui->lblSeqRepeat->show();
    this->isRepeat = true;
}

bool advancedLineOptionsDialog::getGlobalValues(bool &isFreeze, int &sequenceDelay,
                                                int &seqRepeat, int &PWMSweep, int &PWMRepeat)
{
    isFreeze = this->isFreeze;
    sequenceDelay = this->sequenceDelay;
    seqRepeat = this->seqRepeat;
    PWMSweep = this->PWMSweep;
    PWMRepeat = this->PWMRepeat;
    return this->wasAccepted;
}

void advancedLineOptionsDialog::on_buttons_accepted()
{
    //see if they want a freeze line.
    //It doesn't matter if this is visible, if it wasn't the value
    //will never be used.
    if (this->ui->rdbFreeze->isEnabled())
    {
        isFreeze = this->ui->rdbFreeze->isChecked();
    }
    else
    {
        isFreeze = false;
    }

    if(this->ui->cmbPWMSweep->currentIndex() == 0)
    {
        PWMSweep = 0;
    }
    else
    {
        //The string parsing is slow, but I am not sure it is
        //worth worrying about.
        //None of the return values for the parsing are being
        //check as the posible inputs are known and won't cause errors
        PWMSweep = (this->ui->cmbPWMSweep->itemText(
                    this->ui->cmbPWMSweep->currentIndex())).toInt();
    }
    if (this->ui->cmbPWMRepeat->currentIndex() ==0)
    {
        PWMRepeat = 0;
    }
    else
    {
        PWMRepeat = (this->ui->cmbPWMRepeat->itemText
                    ((this->ui->cmbPWMSweep->currentIndex()))).toInt();
    }
    bool ok = false;
    sequenceDelay = (this->ui->cmbSequenceDelay->itemText
                    (this->ui->cmbSequenceDelay->currentIndex())).toInt(&ok);
    //If this isn't show (line options) don't bother reading it.
    if (this->isRepeat)
    {
        this->seqRepeat = this->ui->cmbSequenceRepeat->itemText(
                this->ui->cmbSequenceRepeat->currentIndex()).toInt();
    }
    else
    {
        this->seqRepeat =0;
    }
    wasAccepted = true;
    //If this signal isn't needed emiting it will cause no problenm.
    emit this->dialogClosed(true,isFreeze,PWMSweep,PWMRepeat,sequenceDelay);
}
void advancedLineOptionsDialog::on_buttons_rejected()
{
    this->wasAccepted = false;
    this->emit dialogClosed(false,false,0,0,0);
}
