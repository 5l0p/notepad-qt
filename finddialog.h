#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QPlainTextEdit>

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    FindDialog(QPlainTextEdit *editor, QWidget *parent = nullptr);
    void showFind();
    void showReplace();

private slots:
    void findNext();
    void findPrevious();
    void replace();
    void replaceAll();

private:
    void find(bool forward);

    QPlainTextEdit *textEdit;
    QLineEdit *findLineEdit;
    QLineEdit *replaceLineEdit;
    QCheckBox *caseSensitiveCheck;
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;
    QWidget *replaceWidget;
};

#endif // FINDDIALOG_H
