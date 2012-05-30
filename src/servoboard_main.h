#ifndef SERVOBOARD_MAIN_H
#define SERVOBOARD_MAIN_H

#include <QWidget>
#include <QVector>
#include <QCheckBox>
#include <QMessageBox>
#include "advancedlineoptionsdialog.h"

namespace Ui {
    class servoboard_main;
}

class servoboard_main : public QWidget
{
    Q_OBJECT

public:
    explicit servoboard_main(QWidget *parent = 0);
    ~servoboard_main();

    void disableButtons();
    void displayConnectionWarning();
    void enableButtons();

private slots:
    void lineOptionsClosed(bool,bool,int,int,int);

    void on_btnAdvancedLineOptions_clicked();
    void on_btnSelectAll_clicked();
    void on_btnClearAll_clicked();

    // this is a mess
    void on_sliderServo1_valueChanged(int);
    void on_spinServo1_valueChanged(int);
    void on_sliderServo2_valueChanged(int);
    void on_spinServo2_valueChanged(int);
    void on_sliderServo3_valueChanged(int);
    void on_spinServo3_valueChanged(int);
    void on_sliderServo4_valueChanged(int);
    void on_spinServo4_valueChanged(int);
    void on_sliderServo5_valueChanged(int);
    void on_spinServo5_valueChanged(int);
    void on_sliderServo6_valueChanged(int);
    void on_spinServo6_valueChanged(int);
    void on_sliderServo7_valueChanged(int);
    void on_spinServo7_valueChanged(int);
    void on_sliderServo8_valueChanged(int);
    void on_spinServo8_valueChanged(int);
    void on_sliderServo9_valueChanged(int);
    void on_spinServo9_valueChanged(int);
    void on_sliderServo10_valueChanged(int);
    void on_spinServo10_valueChanged(int);
    void on_sliderServo11_valueChanged(int);
    void on_spinServo11_valueChanged(int);
    void on_sliderServo12_valueChanged(int);
    void on_spinServo12_valueChanged(int);


private:
    void setValueForAll(unsigned char value);
    void setMaxValueForAll(unsigned char value);
    void setMinValueForAll(unsigned char value);


private:
    Ui::servoboard_main *ui;
    QVector<QCheckBox*> *servosEnabled;
    advancedLineOptionsDialog* lineOptions;

    // These should be user configurable.
    unsigned char MinValue;
    unsigned char MiddleValue;
    unsigned char MaxValue;
};

#endif // SERVOBOARD_MAIN_H
