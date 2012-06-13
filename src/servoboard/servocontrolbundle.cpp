#include "servocontrolbundle.h"

ServoControlBundle::ServoControlBundle(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);
}
/*Public Methods*/

void ServoControlBundle::setServoNumber(quint8 servoNumber)
{
    this->servoNumber = servoNumber;
    this->ui.groupBox->setTitle(tr("Servo %1").arg(servoNumber));
}

quint8 ServoControlBundle::getServoNumber()
{
    return this->servoNumber;
}

void ServoControlBundle::setServoValue(quint8 currentValue)
{
    this->ui.lbl_position->setText(tr("%1").arg(currentValue));
}

bool ServoControlBundle::isSelected()
{
    return this->ui.chk_enable->isChecked();
}

bool ServoControlBundle::isActive()
{
    return this->ui.btn_play->isEnabled();
}

void ServoControlBundle::setActive()
{
    this->ui.chk_enable->setEnabled(true);
    this->ui.btn_play->setEnabled(true);
    this->ui.label->setEnabled(true);
    this->ui.lbl_position->setEnabled(true);
    this->ui.spn_value->setEnabled(true);
    this->ui.verticalSlider->setEnabled(true);
}
void ServoControlBundle::setInactive()
{
    this->ui.chk_enable->setEnabled(false);
    this->ui.btn_play->setEnabled(false);
    this->ui.label->setEnabled(false);
    this->ui.lbl_position->setEnabled(false);
    this->ui.spn_value->setEnabled(false);
    this->ui.verticalSlider->setEnabled(false);
}
int ServoControlBundle::getValue()
{
    return this->ui.spn_value->value();
}

void ServoControlBundle::setChecked()
{
    this->ui.chk_enable->setChecked(true);
}

void ServoControlBundle::setUnchecked()
{
    this->ui.chk_enable->setChecked(false);
}

/*Private slots*/

void ServoControlBundle::on_verticalSlider_valueChanged(int value)
{
    emit this->valueChanged(value);
}

void ServoControlBundle::on_btn_play_clicked()
{
    emit this->playClicked(this->servoNumber,this->ui.spn_value->value());
}
