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
    void showSequenceRepeat();
    bool getGlobalValues(bool& isFreeze, int& seqenceDelay, int& seqRepeat,
                         int& PWMSweep, int& PWMRepeat);

private:
    Ui::advancedLineOptionsDialog *ui;
    bool wasAccepted;
    bool isFreeze;
    int sequenceDelay;
    int seqRepeat;
    int PWMSweep;
    int PWMRepeat;

private slots:
    void on_buttons_rejected();
    void on_buttons_accepted();
signals:
    //0 for PWN[sweep|repeat] 0 means no value, 1 for sequence delay
    void dialogClosed(bool accepted, bool freeze,int PWMSweep, int PWMDelay, int sequenceDelay);
};

#endif // ADVANCEDLINEOPTIONSDIALOG_H
