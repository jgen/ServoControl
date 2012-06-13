#ifndef SERVOBOARD_MAIN_H
#define SERVOBOARD_MAIN_H

#include <QWidget>
#include <QVector>
#include <QCheckBox>
#include <QMessageBox>
#include <QString>
#include <QTextDocument>
#include <QColor>

#include "sequencesyntaxhighlighter.h"
#include "sequencecompletesyntaxhighlighter.h"
#include "advancedlineoptionsdialog.h"
#include "position.h"
#include "servocontrolbundle.h"

namespace Ui {
    class servoboard_main;
}

class servoboard_main : public QWidget
{
    Q_OBJECT

public:
    enum actionRequested{Save,Revert, Cancel};

    explicit servoboard_main(QWidget *parent = 0);
    ~servoboard_main();

    void disableButtons();
    void displayConnectionWarning();
    bool displaySaveFormatWaring();
    void enableButtons();


    void displayNewSequence(QString sequence);
    bool hasSequenceInText(); //Not sure if it is a great idea but hey
    QString currentSequenceText();
    bool hasSequenceChanged();
    bool userSuppressChangeNotification();
    QMessageBox::StandardButton displayKeepChangesWarning();
    bool displayInvalidEditsWarning();

    bool highlightNextLine();
    void resetHighlighting();

    void setStoppedState();
    void setPlayingState();
    void setPausedState();

signals:
    void newPositionToSequence(Position* p);
    void playSequence();
    void playPosition(Position* p);
    void pauseSequence();
    void stopSequence();

private slots:
    void servoPlayButtonClicked(quint8 servoNumber,quint8 servoValue);
    void on_btnPlaySelected_clicked();
    void on_btnPlaySequence_clicked();
    void on_txtSequence_textChanged();
    void on_btnStore_clicked();
    void lineOptionsClosed(bool,bool,int,int,int);
    void on_btnAdvancedLineOptions_clicked();
    void on_btnSelectAll_clicked();
    void on_btnClearAll_clicked();


    void on_btnPause_clicked();

    void on_btnStopSequence_clicked();

private:
    void initBundles();


private:
    bool hasAdvancedLineOptions;
    bool isFreeze;
    int PWMSweep;
    int PWMRepeatIndex;
    int sequenceDelay;
    int lastLineHighlighed;
    QSyntaxHighlighter* highlighter;


    Ui::servoboard_main *ui;
    QVector<ServoControlBundle*> servoBundles;
    //QVector<QCheckBox*> *servosEnabled;
    advancedLineOptionsDialog* lineOptions;
    bool hasTextChanged;
    Position* makePositionFromSelected();

    // These should be user configurable.
    unsigned char MinValue;
    unsigned char MiddleValue;
    unsigned char MaxValue;
};

#endif // SERVOBOARD_MAIN_H
