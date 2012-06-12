#include "sequencesyntaxhighlighter.h"

SequenceSyntaxHighlighter::SequenceSyntaxHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
    this->commentExpression.setPattern("^#");
    this->commentFormat.setForeground(Qt::darkGreen);

}


/*Protected Methods*/

void SequenceSyntaxHighlighter::highlightBlock(const QString &text)
{
    int index = commentExpression.indexIn(text);
    if (index == 0)
    {
        this->setFormat(0,text.length(),commentFormat);
    }
}
