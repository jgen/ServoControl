#ifndef SEQUENCESYNTAXHIGHLIGHTER_H
#define SEQUENCESYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include "position.h"

class SequenceSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit SequenceSyntaxHighlighter(QTextDocument *parent = 0);

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
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat servoNumberFormat;
    QTextCharFormat servoDataFormat;


};

#endif // SEQUENCESYNTAXHIGHLIGHTER_H
