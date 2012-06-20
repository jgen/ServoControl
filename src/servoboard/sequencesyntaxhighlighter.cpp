#include "sequencesyntaxhighlighter.h"

SequenceSyntaxHighlighter::SequenceSyntaxHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
    this->commentExpression.setPattern("^#");
    //Make sure to change this is SequenceCompleteSyntaxHighlighter as well
    this->commentFormat.setForeground(Qt::darkGreen);

}


/*Protected Methods*/

void SequenceSyntaxHighlighter::highlightBlock(const QString &text)
{
    int index = commentExpression.indexIn(text);
    if (index == 0) //Only touch comment lines.
    {
        this->setFormat(0,text.length(),commentFormat);
    }
}
