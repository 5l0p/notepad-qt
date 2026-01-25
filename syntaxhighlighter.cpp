#include "syntaxhighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent), enabled(true)
{
    HighlightingRule rule;

    // Keywords - bold
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
        // C/C++/Java/JS common keywords
        QStringLiteral("\\bbreak\\b"), QStringLiteral("\\bcase\\b"),
        QStringLiteral("\\bcatch\\b"), QStringLiteral("\\bclass\\b"),
        QStringLiteral("\\bconst\\b"), QStringLiteral("\\bcontinue\\b"),
        QStringLiteral("\\bdefault\\b"), QStringLiteral("\\bdelete\\b"),
        QStringLiteral("\\bdo\\b"), QStringLiteral("\\belse\\b"),
        QStringLiteral("\\benum\\b"), QStringLiteral("\\bexport\\b"),
        QStringLiteral("\\bextends\\b"), QStringLiteral("\\bfalse\\b"),
        QStringLiteral("\\bfor\\b"), QStringLiteral("\\bfunction\\b"),
        QStringLiteral("\\bif\\b"), QStringLiteral("\\bimport\\b"),
        QStringLiteral("\\bin\\b"), QStringLiteral("\\binstanceof\\b"),
        QStringLiteral("\\blet\\b"), QStringLiteral("\\bnew\\b"),
        QStringLiteral("\\bnull\\b"), QStringLiteral("\\bprivate\\b"),
        QStringLiteral("\\bprotected\\b"), QStringLiteral("\\bpublic\\b"),
        QStringLiteral("\\breturn\\b"), QStringLiteral("\\bstatic\\b"),
        QStringLiteral("\\bstruct\\b"), QStringLiteral("\\bswitch\\b"),
        QStringLiteral("\\btemplate\\b"), QStringLiteral("\\bthis\\b"),
        QStringLiteral("\\bthrow\\b"), QStringLiteral("\\btrue\\b"),
        QStringLiteral("\\btry\\b"), QStringLiteral("\\btypedef\\b"),
        QStringLiteral("\\btypename\\b"), QStringLiteral("\\bvar\\b"),
        QStringLiteral("\\bvirtual\\b"), QStringLiteral("\\bvoid\\b"),
        QStringLiteral("\\bvolatile\\b"), QStringLiteral("\\bwhile\\b"),
        // Type keywords
        QStringLiteral("\\bint\\b"), QStringLiteral("\\blong\\b"),
        QStringLiteral("\\bshort\\b"), QStringLiteral("\\bchar\\b"),
        QStringLiteral("\\bfloat\\b"), QStringLiteral("\\bdouble\\b"),
        QStringLiteral("\\bbool\\b"), QStringLiteral("\\bunsigned\\b"),
        QStringLiteral("\\bsigned\\b"), QStringLiteral("\\bauto\\b"),
        // Python keywords
        QStringLiteral("\\bdef\\b"), QStringLiteral("\\belif\\b"),
        QStringLiteral("\\bexcept\\b"), QStringLiteral("\\bfinally\\b"),
        QStringLiteral("\\bfrom\\b"), QStringLiteral("\\bglobal\\b"),
        QStringLiteral("\\blambda\\b"), QStringLiteral("\\bNone\\b"),
        QStringLiteral("\\bpass\\b"), QStringLiteral("\\braise\\b"),
        QStringLiteral("\\bwith\\b"), QStringLiteral("\\byield\\b"),
        QStringLiteral("\\band\\b"), QStringLiteral("\\bor\\b"),
        QStringLiteral("\\bnot\\b"), QStringLiteral("\\bis\\b"),
    };
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // Numbers - dark gray
    numberFormat.setForeground(QColor(80, 80, 80));
    rule.pattern = QRegularExpression(QStringLiteral("\\b[0-9]+\\.?[0-9]*\\b"));
    rule.format = numberFormat;
    highlightingRules.append(rule);

    // Preprocessor - bold dark gray
    preprocessorFormat.setForeground(QColor(60, 60, 60));
    preprocessorFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression(QStringLiteral("^\\s*#[^\n]*"));
    rule.format = preprocessorFormat;
    highlightingRules.append(rule);

    // Single-line strings - italic gray
    quotationFormat.setForeground(QColor(100, 100, 100));
    quotationFormat.setFontItalic(true);
    rule.pattern = QRegularExpression(QStringLiteral("\"[^\"]*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);
    rule.pattern = QRegularExpression(QStringLiteral("'[^']*'"));
    highlightingRules.append(rule);

    // Single-line comments - italic light gray
    singleLineCommentFormat.setForeground(QColor(140, 140, 140));
    singleLineCommentFormat.setFontItalic(true);
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);
    // Python/shell comments
    rule.pattern = QRegularExpression(QStringLiteral("#[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    // Multi-line comments
    multiLineCommentFormat.setForeground(QColor(140, 140, 140));
    multiLineCommentFormat.setFontItalic(true);
    commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}

void SyntaxHighlighter::setEnabled(bool enable)
{
    enabled = enable;
    rehighlight();
}

bool SyntaxHighlighter::isEnabled() const
{
    return enabled;
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    if (!enabled) {
        return;
    }

    // Apply single-line rules
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Handle multi-line comments
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1) {
        startIndex = text.indexOf(commentStartExpression);
    }

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;

        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + match.capturedLength();
        }

        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
