#include "servocontrolbundle.h"
#include <QDebug>

/*!
 * \brief Creates the bundle, intializes the UI elements.
 *
 * \param parent
 *      The parent of the bundle, the widget that it will be displayed on.
 */
ServoControlBundle::ServoControlBundle(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);
}
/*Public Methods*/
/*!
 * \brief Sets the servo number for the bundle and diplays the changes
 *
 * \param servoNumber
 *      The index number of the servo that this bundle will control.
 */
void ServoControlBundle::setServoNumber(quint8 servoNumber)
{
    this->servoNumber = servoNumber;
    this->ui.groupBox->setTitle(tr("Servo %1").arg(servoNumber));
}
/*!
 * \brief Returns the servo number that this bundle controls.
 *
 * \return The servo number that this bundle controls.
 */
quint8 ServoControlBundle::getServoNumber()
{
    return this->servoNumber;
}
/*!
 * \brief Sets the current position of the servo
 *
 * \param currentValue
 *      The current position value of the servo this bundle represents.
 */
void ServoControlBundle::setServoValue(quint8 currentValue)
{
    this->ui.lbl_position->setText(tr("%1").arg(currentValue));
}
/*!
 * \brief Returns if the bundle is selected for use in a position that is being
 *      created.
 *
 * \return True if the bundle is selected, false otherwise.
 */
bool ServoControlBundle::isSelected()
{
    return this->ui.chk_enable->isChecked();
}
/*!
 * \brief Returns if the bundle is able to accept user input.
 *
 * \return True is the bundle can currenly accept user input.
 */
bool ServoControlBundle::isActive()
{
    return this->ui.btn_play->isEnabled();
}
/*!
 * \brief Sets the bundle as able to accept user input.
 */
void ServoControlBundle::setActive()
{
    this->ui.chk_enable->setEnabled(true);
    this->ui.btn_play->setEnabled(true);
    this->ui.label->setEnabled(true);
    this->ui.lbl_position->setEnabled(true);
    this->ui.spn_value->setEnabled(true);
    this->ui.verticalSlider->setEnabled(true);
}
/*!
 * \brief Sets then bundle as unable to accept user input.
 */
void ServoControlBundle::setInactive()
{
    this->ui.chk_enable->setEnabled(false);
    this->ui.btn_play->setEnabled(false);
    this->ui.label->setEnabled(false);
    this->ui.lbl_position->setEnabled(false);
    this->ui.spn_value->setEnabled(false);
    this->ui.verticalSlider->setEnabled(false);
}
/*!
 * \brief Finds the user entered value and returns it.
 *
 * \return The value that the user has set for the servo to be moved to.
 */
int ServoControlBundle::getValue()
{
    return this->ui.spn_value->value();
}
/*!
 * \brief Sets the bundle as selected for use in a position.
 */
void ServoControlBundle::setChecked()
{
    this->ui.chk_enable->setChecked(true);
}
/*!
 * \brief Takes the position that was last sent to the board and loads it into the text box.
 */
void ServoControlBundle::currentToNext()
{
    this->ui.verticalSlider->setValue(this->ui.lbl_position->text().toInt());
}

/*!
 * \brief Sets a bundle as unselected for use in a position.
 */
void ServoControlBundle::setUnchecked()
{
    this->ui.chk_enable->setChecked(false);
}

/*Private slots*/
/*!
 * \brief Ties the values on the slider and the spin edit together
 */
void ServoControlBundle::on_verticalSlider_valueChanged(int value)
{
    emit this->valueChanged(value);
}
/*!
 * \brief Lets interested parties know that the user wishes to send the
 *      the data to the board.
 */
void ServoControlBundle::on_btn_play_clicked()
{
    emit this->playClicked(this->servoNumber,this->ui.spn_value->value());
}
