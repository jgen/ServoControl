#ifndef ADVANCEDLINEOPTIONSDIALOG_H
#define ADVANCEDLINEOPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
    class advancedLineOptionsDialog;
}

class advancedLineOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit advancedLineOptionsDialog(QWidget *parent = 0);
    ~advancedLineOptionsDialog();

private:
    Ui::advancedLineOptionsDialog *ui;

private slots:
    void on_buttons_rejected();
    void on_buttons_accepted();
signals:
    void dialogClosed(bool accepted, bool freeze,int PWMSweep, int PWMDelay, int sequenceDelay);
};

#endif // ADVANCEDLINEOPTIONSDIALOG_H
