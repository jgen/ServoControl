#ifndef ADVANCEDLINEOPTIONSDIALOG_H
#define ADVANCEDLINEOPTIONSDIALOG_H

#include <QDialog>
#include <QDebug>

namespace Ui {
    class advancedLineOptionsDialog;
}

/*
 *This same class is used for both global and line settings which means
 *that it has essentially two interfaces.
 *
 *The reason for resuing this in two seperate locations is because code
 *reuse is good and Adam hates designer so he didn't want to make the
 *same ui twice.
 *
 *It assumes that it is being used for the line options, and if it is
 *being used for global settings, then it must be told that explicitly.
 *
 *Global values are returned using the getGlobalValues method, which is
 *called synchronously which means that the dialog must be modal (cannot
 *access the other parts of the ui when the dialog is up). This is done
 *by calling the intherited exec method. The choice to do this was made
 *because when editing global values, it make no sense to update the lines
 *or servo positions.
 *
 *Line values are returned using the signal, dialogClosed, which allows
 *for the dialog to be called asynchronously. The call is made using the
 *show() method, and then the result will be recieved when the dialog is
 *closed. This will allow for the user to edit the current line positions
 *while choosing the values.
 */

class advancedLineOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    //Set the parent so it knows what event loop to use
    explicit advancedLineOptionsDialog(QWidget *parent = 0);
    ~advancedLineOptionsDialog();
    //This sets it up as for the global values, unless this is closed it
    //is for line options only
    void showSequenceRepeat();
    //Returns whether the options were accepted or rejected
    //The rest of the values are passed in the parameters.
    //PWMSweep and SequenceRepeat return the user visible value
    // ie 200 not 7
    bool getGlobalValues(bool& isFreeze, int& seqenceDelay, int& seqRepeat,
                         int& PWMSweep, int& PWMRepeat);

private:
    Ui::advancedLineOptionsDialog *ui;
    //We need all these as there may be a delay from the box closing
    //to the values being read.
    bool wasAccepted;
    bool isFreeze;
    bool isRepeat;
    int sequenceDelay;
    int seqRepeat;
    int PWMSweep;
    int PWMRepeat;

private slots:
    void on_buttons_rejected();
    void on_buttons_accepted();
signals:
    //0 for PWN[sweep|repeat] 0 means no value
    //PWMSweeep returns the user visible value (ie 200 not 7)
    void dialogClosed(bool accepted, bool freeze,int PWMSweep, int PWMDelay, int sequenceDelay);
};

#endif // ADVANCEDLINEOPTIONSDIALOG_H
