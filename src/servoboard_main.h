#ifndef SERVOBOARD_MAIN_H
#define SERVOBOARD_MAIN_H

#include <QWidget>

namespace Ui {
    class servoboard_main;
}

class servoboard_main : public QWidget
{
    Q_OBJECT

public:
    explicit servoboard_main(QWidget *parent = 0);
    ~servoboard_main();

private:
    Ui::servoboard_main *ui;
};

#endif // SERVOBOARD_MAIN_H
