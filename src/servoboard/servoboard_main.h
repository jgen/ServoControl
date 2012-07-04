#ifndef SERVOBOARD_MAIN_H
#define SERVOBOARD_MAIN_H
/*! \class servoboard_main
 *
 * \brief The view class for the servoboard tab, instatiated in the servoboard
 *     tab, it is modeled after the form from the program written by Eugen.
 *
 * View class for sending information to the servoboard, including creating and
 * loading sequences of positions.
 *
 * Must stay alive for the duration of program execution as it has no way to
 * save its state on tab switches.
 *
 * It is resposible for the creation of dialog boxes that will be displayed with various
 * warnings to the user.
 *
 */
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
    //I don't think we need this enum.
    enum actionRequested{Save,Revert, Cancel};

    explicit servoboard_main(QWidget *parent = 0);
    ~servoboard_main();

    void disableButtons(); //!< Disables all input fields
    void displayConnectionWarning();//!< When no serial port is connected
    bool displaySaveFormatWaring();//!< Used to warn about legacy format
    void displaySetStartFailure();//!< Notify set starting position failed
    void displayBurnSuccess();//!< Nofity new start position was burnt in.
    bool displayBurnQuery();//!< Ask if a new starting position should be burnt from the file.
    void enableButtons();//!< Enable all input fields


    void displayNewSequence(QString sequence);//!< Dumps the string in the text field
    /// Check if there is enough text to make a sequence in the box.
    bool hasSequenceInText(); //Not sure if it is a great idea but hey.
    QString currentSequenceText();//!< Get the text from the sequence box.
    bool hasSequenceChanged();//!< Checks to see if user typed in the text box.
    QMessageBox::StandardButton displayKeepChangesWarning(); //!< Ask if text box changes are to be kept.
    bool displayInvalidEditsWarning();//!< Warn that user typed positions are invalid.

    bool highlightNextLine();//!< For highlighting when sequence is playing, will not wrap around
    void resetHighlighting();//!< Resets iterator for highlighting, used for wrapping around.

    void setStoppedState();//!< Sets buttons for when sequence playback is stopped.
    void setPlayingState();//!< Sequence playing
    void setPausedState();//!< Sequence paused

signals:
    void newPositionToSequence(Position* p);//!< Used to indicate the sequence needs a new position.
    void playSequence();//!< Play sequence button clicked
    void playPosition(Position* p);//!< Single postion playback requested
    void pauseSequence();//!< Pause sequence button clicked
    void stopSequence();//!< Stop sequence button clicked.
    void setStartPosition(Position* p); //!< The set start position button was clicked.

public slots:
    void servoPositionChanged(Position* newPosition);

private slots:
    /// Recieved from servoBundle, used to request a single servo to be moved.
    void servoPlayButtonClicked(quint8 servoNumber,quint8 servoValue);
    void on_btnPlaySelected_clicked();//!< Request a single position to be played
    void on_btnPlaySequence_clicked();//!< Start playing the sequence in the text box.
    void on_txtSequence_textChanged();//!< Private handler for textbox change from user.
    void on_btnStore_clicked();//!< When a sequence is requested to be stored to the sequence
    void lineOptionsClosed(bool,bool,int,int,int);//!< Used to get return from advanceLineOptionsDialog
    void on_btnAdvancedLineOptions_clicked();//!< Opens advanced line options dialog.
    void on_btnSelectAll_clicked();//!< Checks checkboxes in all servoBundles
    void on_btnClearAll_clicked();//!< Clears all checkboxes in servoBundles

    void on_btnPause_clicked();//!< Pause the currently playing sequence.
    void on_btnStopSequence_clicked();//!< Stop the currently playing sequence.

    void on_btnSetStartPosition_clicked(); //!< Set start position was clicked.

private:
    void initBundles(); //!< Makes connections and store servobundles.

private:
    Ui::servoboard_main *ui;

    //These store the return values from the advanced line options dialog.
    bool hasAdvancedLineOptions;
    bool isFreeze;
    int PWMSweep;
    int PWMRepeatIndex;
    int sequenceDelay;

    int lastLineHighlighed;//Used for highlighting on sequence plays
    //This is used for syntax highlighting the sequences when not playing
    QSyntaxHighlighter* highlighter;


    QVector<ServoControlBundle*> servoBundles; //The widgets for inputing servo info
    advancedLineOptionsDialog* lineOptions;
    bool hasTextChanged;//!< Stores if text box has been edited since last read.
    Position* makePositionFromSelected();//Makes a position based on the selected boxes.
};

#endif // SERVOBOARD_MAIN_H
