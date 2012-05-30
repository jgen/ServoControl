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
    this->ui->cmbPWMRepeat->addItem(tr("5"));
    this->ui->cmbPWMRepeat->addItem(tr("10"));
    this->ui->cmbPWMRepeat->addItem(tr("25"));
    this->ui->cmbPWMRepeat->addItem(tr("50"));
    this->ui->cmbPWMRepeat->addItem(tr("100"));
    this->ui->cmbPWMRepeat->addItem(tr("150"));
    this->ui->cmbPWMRepeat->addItem(tr("200"));

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
}

advancedLineOptionsDialog::~advancedLineOptionsDialog()
{
    delete ui;
}

void advancedLineOptionsDialog::on_buttons_accepted()
{
    bool isFreeze = this->ui->rdbFreeze->isChecked();
    int PWMSweep;
    if(this->ui->cmbPWMSweep->currentIndex() == 0)
    {
        PWMSweep = 0;
    }
    else
    {
        PWMSweep = (this->ui->cmbPWMSweep->itemText(
                    this->ui->cmbPWMSweep->currentIndex())).toInt();
    }


    int PWMRepeat;
    if (this->ui->cmbPWMRepeat->currentIndex() ==0)
    {
        PWMRepeat = 0;
    }
    else
    {
        PWMRepeat = (this->ui->cmbPWMRepeat->itemText
                            ((this->ui->cmbPWMSweep->currentIndex()))).toInt();
    }

    int sequenceDelay = (this->ui->cmbSequenceDelay->itemText
                        (this->ui->cmbSequenceDelay->currentIndex())).toInt();

    emit this->dialogClosed(true,isFreeze,PWMSweep,PWMRepeat,sequenceDelay);
}
void advancedLineOptionsDialog::on_buttons_rejected()
{
    this->emit dialogClosed(false,false,0,0,0);


}
