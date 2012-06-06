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
    isFreeze = this->ui->rdbFreeze->isChecked();

    if(this->ui->cmbPWMSweep->currentIndex() == 0)
    {
        PWMSweep = 0;
    }
    else
    {
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
    /*qDebug() << "text: " << ui->cmbSequenceDelay->itemText((this->ui->cmbSequenceDelay->currentIndex()))
             << "index: " << this->ui->cmbSequenceDelay->currentIndex()
             << "ok : " << ok << "Delay: " << sequenceDelay;*/
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
    emit this->dialogClosed(true,isFreeze,PWMSweep,PWMRepeat,sequenceDelay);
}
void advancedLineOptionsDialog::on_buttons_rejected()
{
    this->wasAccepted = false;
    this->emit dialogClosed(false,false,0,0,0);
}
