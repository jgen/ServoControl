#include "sequencesyntaxhighlighter.h"
/*!
 * Sets up the colours to be used in the highlighting and the patterns that
 * will be highlighted. In this case the only pattern of interest is with a
 * '#' starting on a line for a comment.
 */
SequenceSyntaxHighlighter::SequenceSyntaxHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
    this->commentExpression.setPattern("^#");
    //Make sure to change this is SequenceCompleteSyntaxHighlighter as well
    this->commentFormat.setForeground(Qt::darkGreen);

}


/*Protected Methods*/
/*!
 * This will highlight the comments, there is no checking done for lines that
 * are not in the correct position format.
 */
void SequenceSyntaxHighlighter::highlightBlock(const QString &text)
{
    int index = commentExpression.indexIn(text);
    if (index == 0) //Only touch comment lines.
    {
        this->setFormat(0,text.length(),commentFormat);
    }
}
