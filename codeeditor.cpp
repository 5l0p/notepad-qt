#include "codeeditor.h"

#include <QPainter>
#include <QTextBlock>

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent), showLineNumbers(true), darkMode(false)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, &CodeEditor::blockCountChanged,
            this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest,
            this, &CodeEditor::updateLineNumberArea);

    updateLineNumberAreaWidth(0);
}

int CodeEditor::lineNumberAreaWidth()
{
    if (!showLineNumbers) {
        return 0;
    }

    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 10 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy) {
        lineNumberArea->scroll(0, dy);
    } else {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
                                      lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    if (!showLineNumbers) {
        return;
    }

    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), darkMode ? QColor(42, 42, 42) : QColor(240, 240, 240));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(darkMode ? QColor(160, 160, 160) : QColor(120, 120, 120));
            painter.drawText(0, top, lineNumberArea->width() - 5,
                           fontMetrics().height(),
                           Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::setLineNumbersVisible(bool visible)
{
    showLineNumbers = visible;
    updateLineNumberAreaWidth(0);
    lineNumberArea->setVisible(visible);
    lineNumberArea->update();
}

bool CodeEditor::lineNumbersVisible() const
{
    return showLineNumbers;
}

void CodeEditor::setDarkMode(bool dark)
{
    darkMode = dark;
    lineNumberArea->update();
}

bool CodeEditor::isDarkMode() const
{
    return darkMode;
}
