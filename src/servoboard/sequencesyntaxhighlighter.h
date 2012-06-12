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
    QRegExp commentExpression;
    QTextCharFormat commentFormat;



};

#endif // SEQUENCESYNTAXHIGHLIGHTER_H
