#ifndef SEQUENCECOMPLETESYNTAXHIGHLIGHTER_H
#define SEQUENCECOMPLETESYNTAXHIGHLIGHTER_H
/*! \class SequenceCompleteSyntaxHighlighter
 *
 * This is used to highlight the sequence that is currently displayed to
 * the user, and not being played back to the board. To change the colour
 * scheme that is being used, the constructor contains the settings that
 * can be changed.
 *
 */
#include <QSyntaxHighlighter>
#include "position.h"

class SequenceCompleteSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit SequenceCompleteSyntaxHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);

signals:
    
public slots:

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

    QRegExp commentExpression;
    QRegExp servoExpression;
    QTextCharFormat keywordFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat servoNumberFormat;
    QTextCharFormat servoDataFormat;
    QTextCharFormat errorLineFormat;
    QTextCharFormat normalFormat;
    
};

#endif // SEQUENCECOMPLETESYNTAXHIGHLIGHTER_H
