#include "servoboard_main.h"
#include "ui_servoboard_main.h"

servoboard_main::servoboard_main(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::servoboard_main)
{
    ui->setupUi(this);

    servosEnabled = new QVector<QCheckBox*>(0);
    servosEnabled->append( this->ui->chkServo1  );
    servosEnabled->append( this->ui->chkServo2  );
    servosEnabled->append( this->ui->chkServo3  );
    servosEnabled->append( this->ui->chkServo4  );
    servosEnabled->append( this->ui->chkServo5  );
    servosEnabled->append( this->ui->chkServo6  );
    servosEnabled->append( this->ui->chkServo7  );
    servosEnabled->append( this->ui->chkServo8  );
    servosEnabled->append( this->ui->chkServo9  );
    servosEnabled->append( this->ui->chkServo10 );
    servosEnabled->append( this->ui->chkServo11 );
    servosEnabled->append( this->ui->chkServo12 );
}

servoboard_main::~servoboard_main()
{
    delete ui;
    delete servosEnabled;
}

/* Select all the servo checkboxes */
void servoboard_main::on_btnSelectAll_clicked()
{
    for (int i=0; i<servosEnabled->size(); i++) {
        servosEnabled->at(i)->setChecked(true);
    }
}

/* Clear all the servo checkboxes */
void servoboard_main::on_btnClearAll_clicked()
{
    for (int i=0; i<servosEnabled->size(); i++) {
        servosEnabled->at(i)->setChecked(false);
    }
}



