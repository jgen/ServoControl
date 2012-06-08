#include "sequencesyntaxhighlighter.h"

SequenceSyntaxHighlighter::SequenceSyntaxHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
    HighlightingRule rule;
    this->keywordFormat.setForeground(Qt::yellow);
    this->commentFormat.setForeground(Qt::green);
    this->servoDataFormat.setForeground(Qt::magenta);
    this->servoNumberFormat.setFontItalic(true);
    this->commentExpression.setPattern("^#");
    this->errorLineFormat.setForeground(Qt::red);
}


/*Protected Methods*/

void SequenceSyntaxHighlighter::highlightBlock(const QString &text)
{
    int index = commentExpression.indexIn(text);
    if (index == 0)
    {
        this->setFormat(0,text.length(),commentFormat);
    }
   // Position p;
   // if (p.fromString(text)) //line is the correct format
   // {

   // }
   // else //Line is not valid format
//    {

   // }

}
