#include "sequencecompletesyntaxhighlighter.h"

/**
 * This sets up the colour scheme that will be used and the patterns that are used to match the
 * different parts of a squence.
 */
SequenceCompleteSyntaxHighlighter::SequenceCompleteSyntaxHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
    HighlightingRule rule;
    this->keywordFormat.setForeground(Qt::darkCyan);
    //Make sure to change this in SequenceSyntaxHighlighter as well
    this->commentFormat.setForeground(Qt::darkGreen);
    this->commentExpression.setPattern("^#");
    this->servoExpression.setPattern("0[0-1][0-9],0[0-9][0-9]");
    this->servoExpression.setPatternSyntax(QRegExp::Wildcard);
    this->servoDataFormat.setForeground(QColor(155,2,6));
    this->servoNumberFormat.setFontItalic(true);

    this->errorLineFormat.setForeground(Qt::red);
}

/*Protected Methods*/

/**
 * This is called each time that a block is drawn on the screen.
 */
void SequenceCompleteSyntaxHighlighter::highlightBlock(const QString &text)
{
    int index = 0;
    Position p;
    if (!text.startsWith("#") &&
            p.fromString(text)) //line is the correct format
    {
        if (text.contains("PWMRep"))
        {
           index = text.indexOf("PWMRep");
           this->setFormat(index,6,keywordFormat);
           index += 8; //Move past this section so it isn't highlighted twice.
        }
        if (text.contains("PWMSweep"))
        {
            index = text.indexOf("PWMSweep");
            this->setFormat(index,8,keywordFormat);
            index += 10; //Move past this section

        }
        while(index < text.length() &&  index != -1)//Walk through the positions
        {
            index = servoExpression.indexIn(text,index);
            if (index == -1) break;
            this->setFormat(index,3,servoNumberFormat);
            this->setFormat(index +4,3,servoDataFormat);//skip the comma
            index += 5;//Get past what we just highlighted
        }
        if (text.contains("SeqDelay"))
        {
            index = text.indexOf("SeqDelay");
            this->setFormat(index,8,keywordFormat);
            index += 10;
        }
    }
    else //Line is not valid format
    {
        //This will cause comments to get highlighed twice, but that isn't a concern.
        this->setFormat(0,text.length(),errorLineFormat);
    }
    index = commentExpression.indexIn(text);
    if (index == 0)
    {
        this->setFormat(0,text.length(),commentFormat);
    }

}
