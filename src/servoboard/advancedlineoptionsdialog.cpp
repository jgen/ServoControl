#include "advancedlineoptionsdialog.h"
#include "ui_advancedlineoptionsdialog.h"

/*!
 * \brief This sets up the dialog box with the values are possible
 *      and initalizes the UI elements.
 *
 * The dialog is initally set up for use with the advanced line options,
 * but can be used for the global values by calling the show sequence
 * repeat method. The two dialogs were collapsed into once class since
 * the only difference is that the line options dialog doesn't need the
 * sequence repeat options.
 *
 */

advancedLineOptionsDialog::advancedLineOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::advancedLineOptionsDialog)
{
    ui->setupUi(this);
	/*! @todo
      I am hardcoding in the values for the dialog box for now
      but I would like to have them at some point factored out
      for use in other dialog boxes.
	*/
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
/*!
 * \brief This is used to indicate that the dialog will be used for global
 *      values to be set, not just line options.
 *
 * Once this method is called the dialog box will act as a though it is for
 * setting the global options instead of setting line options.
 */
void advancedLineOptionsDialog::showSequenceRepeat()
{
    //Hide the line options, and show the global ones.
    this->ui->rdbFreeze->setEnabled(false);
    this->ui->rdbNoFreeze->setEnabled(false);
    this->ui->cmbSequenceRepeat->show();
    this->ui->lblSeqRepeat->show();
    this->isRepeat = true;
}
/*!
 * \brief Gets the values what were set for when the dialog is not usesd as a
 *      modal dialog box.
 *
 * In some cases the results of the users are not needed right away in the main
 * program. This method allows access to the values that the user has set after
 * the dialog is closed.
 *
 * \param isFreeze
 *     This will contain if the user selected the freeze option in the dialog.
 *     Note that it will pass the value out by referece in the location given.
 *
 * \param sequenceDelay
 *     This will pass out the delay between sequences in seconds
 *
 * \param seqRepeat
 *     This will pass out the number of times the sequence is to repeat
 *     as a user faceing value. It will have to be converted before it can be
 *     can be sent to the board.
 *
 * \param PWMSweep
 *     This will pass out the PWM sweep part of the PWM change information
 *
 * \param PWMRepeat
 *     This will pass out the PWM repeat information as a user facing value.
 *
 * \return True if the user pressed the okay button, false otherwise. Note that
 *     if the return values are false, the other data may not be valid.
 */
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
/*!
 * \brief Run when the okay button is pressed to store the user entered values
 *      emit them if needed.
 *
 * This method will store the values in the input fields, record that the okay
 * button was pressed and emit a signal with all this information. The signal
 * will be emitted if it is needed or not, but can be ignored for global values
 */
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
/*!
 * \brief If the user clicks the cancel button it is recorded and the signal
 *      is sent to discard the changes.
 */
void advancedLineOptionsDialog::on_buttons_rejected()
{
    this->wasAccepted = false;
    this->emit dialogClosed(false,false,0,0,0);
}
