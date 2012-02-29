#ifndef SERVOBOARD_MAIN_H
#define SERVOBOARD_MAIN_H

#include <QWidget>
#include <QVector>
#include <QCheckBox>

namespace Ui {
    class servoboard_main;
}

class servoboard_main : public QWidget
{
    Q_OBJECT

public:
    explicit servoboard_main(QWidget *parent = 0);
    ~servoboard_main();

private slots:
    void on_btnSelectAll_clicked();
    void on_btnClearAll_clicked();

private:
    Ui::servoboard_main *ui;
    QVector<QCheckBox*> *servosEnabled;
};

#endif // SERVOBOARD_MAIN_H
