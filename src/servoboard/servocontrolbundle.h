#ifndef SERVOCONTROLBUNDLE_H
#define SERVOCONTROLBUNDLE_H

#include <QWidget>
#include "ui_servocontrolbundle.h"


class ServoControlBundle : public QWidget
{
    Q_OBJECT
public:
    explicit ServoControlBundle(QWidget *parent = 0);
    
signals:
    
public slots:
private:
    Ui::ServoControlBundle ui;

};

#endif // SERVOCONTROLBUNDLE_H
