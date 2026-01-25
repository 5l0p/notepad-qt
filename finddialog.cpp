#include "finddialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTextCursor>

FindDialog::FindDialog(QPlainTextEdit *editor, QWidget *parent)
    : QDialog(parent), textEdit(editor)
{
    setWindowTitle(tr("Find and Replace"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Find row
    QHBoxLayout *findLayout = new QHBoxLayout;
    findLayout->addWidget(new QLabel(tr("Find:")));
    findLineEdit = new QLineEdit;
    findLineEdit->setMinimumWidth(250);
    findLayout->addWidget(findLineEdit);
    mainLayout->addLayout(findLayout);

    // Replace row
    replaceWidget = new QWidget;
    QHBoxLayout *replaceLayout = new QHBoxLayout(replaceWidget);
    replaceLayout->setContentsMargins(0, 0, 0, 0);
    replaceLayout->addWidget(new QLabel(tr("Replace:")));
    replaceLineEdit = new QLineEdit;
    replaceLayout->addWidget(replaceLineEdit);
    mainLayout->addWidget(replaceWidget);

    // Options
    caseSensitiveCheck = new QCheckBox(tr("Case sensitive"));
    mainLayout->addWidget(caseSensitiveCheck);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    QPushButton *findPrevButton = new QPushButton(tr("Find Previous"));
    connect(findPrevButton, &QPushButton::clicked, this, &FindDialog::findPrevious);
    buttonLayout->addWidget(findPrevButton);

    QPushButton *findNextButton = new QPushButton(tr("Find Next"));
    findNextButton->setDefault(true);
    connect(findNextButton, &QPushButton::clicked, this, &FindDialog::findNext);
    buttonLayout->addWidget(findNextButton);

    replaceButton = new QPushButton(tr("Replace"));
    connect(replaceButton, &QPushButton::clicked, this, &FindDialog::replace);
    buttonLayout->addWidget(replaceButton);

    replaceAllButton = new QPushButton(tr("Replace All"));
    connect(replaceAllButton, &QPushButton::clicked, this, &FindDialog::replaceAll);
    buttonLayout->addWidget(replaceAllButton);

    QPushButton *closeButton = new QPushButton(tr("Close"));
    connect(closeButton, &QPushButton::clicked, this, &QDialog::close);
    buttonLayout->addWidget(closeButton);

    mainLayout->addLayout(buttonLayout);

    // Enter key triggers find
    connect(findLineEdit, &QLineEdit::returnPressed, this, &FindDialog::findNext);
    connect(replaceLineEdit, &QLineEdit::returnPressed, this, &FindDialog::findNext);
}

void FindDialog::showFind()
{
    setWindowTitle(tr("Find"));
    replaceWidget->hide();
    replaceButton->hide();
    replaceAllButton->hide();

    // Pre-fill with selected text
    QTextCursor cursor = textEdit->textCursor();
    if (cursor.hasSelection()) {
        findLineEdit->setText(cursor.selectedText());
    }

    findLineEdit->selectAll();
    findLineEdit->setFocus();
    show();
    raise();
    activateWindow();
}

void FindDialog::showReplace()
{
    setWindowTitle(tr("Find and Replace"));
    replaceWidget->show();
    replaceButton->show();
    replaceAllButton->show();

    // Pre-fill with selected text
    QTextCursor cursor = textEdit->textCursor();
    if (cursor.hasSelection()) {
        findLineEdit->setText(cursor.selectedText());
    }

    findLineEdit->selectAll();
    findLineEdit->setFocus();
    show();
    raise();
    activateWindow();
}

void FindDialog::findNext()
{
    find(true);
}

void FindDialog::findPrevious()
{
    find(false);
}

void FindDialog::find(bool forward)
{
    QString searchText = findLineEdit->text();
    if (searchText.isEmpty()) {
        return;
    }

    QTextDocument::FindFlags flags;
    if (!forward) {
        flags |= QTextDocument::FindBackward;
    }
    if (caseSensitiveCheck->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    QTextCursor cursor = textEdit->textCursor();

    // If searching forward and we have selection, start from end of selection
    if (forward && cursor.hasSelection()) {
        cursor.setPosition(cursor.selectionEnd());
        textEdit->setTextCursor(cursor);
    }

    bool found = textEdit->find(searchText, flags);

    if (!found) {
        // Wrap around
        cursor = textEdit->textCursor();
        if (forward) {
            cursor.movePosition(QTextCursor::Start);
        } else {
            cursor.movePosition(QTextCursor::End);
        }
        textEdit->setTextCursor(cursor);
        found = textEdit->find(searchText, flags);

        if (!found) {
            QMessageBox::information(this, tr("Find"),
                tr("'%1' not found.").arg(searchText));
        }
    }
}

void FindDialog::replace()
{
    QString searchText = findLineEdit->text();
    if (searchText.isEmpty()) {
        return;
    }

    QTextCursor cursor = textEdit->textCursor();

    // Check if current selection matches search text
    if (cursor.hasSelection()) {
        QString selected = cursor.selectedText();
        bool match = caseSensitiveCheck->isChecked()
            ? (selected == searchText)
            : (selected.compare(searchText, Qt::CaseInsensitive) == 0);

        if (match) {
            cursor.insertText(replaceLineEdit->text());
        }
    }

    // Find next occurrence
    findNext();
}

void FindDialog::replaceAll()
{
    QString searchText = findLineEdit->text();
    if (searchText.isEmpty()) {
        return;
    }

    QString replaceText = replaceLineEdit->text();
    QTextDocument::FindFlags flags;
    if (caseSensitiveCheck->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    // Start from beginning
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::Start);
    textEdit->setTextCursor(cursor);

    int count = 0;
    cursor.beginEditBlock();

    while (textEdit->find(searchText, flags)) {
        QTextCursor found = textEdit->textCursor();
        found.insertText(replaceText);
        count++;
    }

    cursor.endEditBlock();

    QMessageBox::information(this, tr("Replace All"),
        tr("Replaced %1 occurrence(s).").arg(count));
}
