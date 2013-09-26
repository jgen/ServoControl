#ifndef SERVOCONTROLBUNDLE_H
#define SERVOCONTROLBUNDLE_H

#include <QtWidgets\QWidget>
#include "ui_servocontrolbundle.h"

class ServoControlBundle : public QWidget
{
    Q_OBJECT
public:
    explicit ServoControlBundle(QWidget *parent = 0);
    void setServoNumber(quint8 servoNumber);
    quint8 getServoNumber();
    void setServoValue(quint8 currentValue);
    bool isSelected();
    bool isActive();
    void setActive();
    void setInactive();
    int getValue();

    void currentToNext();

    void setChecked();
    void setUnchecked();

    
signals:
    void valueChanged(quint8 value);
    void playClicked(quint8 servoNumber, quint8 value);
    
public slots:
private:
    Ui::ServoControlBundle ui;
    int servoNumber;


private slots:
    void on_btn_play_clicked();
    void on_verticalSlider_valueChanged(int value);
};

#endif // SERVOCONTROLBUNDLE_H
