#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    SyntaxHighlighter(QTextDocument *parent = nullptr);
    void setEnabled(bool enabled);
    bool isEnabled() const;
    void setRainbowBracketsEnabled(bool enabled);
    bool rainbowBracketsEnabled() const;

protected:
    void highlightBlock(const QString &text) override;

private:
    void highlightBrackets(const QString &text, int initialDepth);

    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat preprocessorFormat;
    QVector<QTextCharFormat> bracketFormats;

    bool enabled;
    bool rainbowBrackets;
};

#endif // SYNTAXHIGHLIGHTER_H
