#ifndef SEQUENCESYNTAXHIGHLIGHTER_H
#define SEQUENCESYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include "position.h"
/*
 *This only highlights the comments in a sequence and leaves the rest untouched.
 *It is used when the sequence is being played back as it allows for the default
 *"pointer" highlighing to occur and then the comments are made green on top of that.
 */
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
