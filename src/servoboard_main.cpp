#include "servoboard_main.h"
#include "ui_servoboard_main.h"

servoboard_main::servoboard_main(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::servoboard_main)
{
    ui->setupUi(this);
}

servoboard_main::~servoboard_main()
{
    delete ui;
}
