#ifndef SEQUENCECOMPLETESYNTAXHIGHLIGHTER_H
#define SEQUENCECOMPLETESYNTAXHIGHLIGHTER_H

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
