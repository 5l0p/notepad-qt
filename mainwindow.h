#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

class CodeEditor;
class SyntaxHighlighter;
class FindDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void openFile(const QString &filePath);

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void find();
    void replace();
    void goToLine();
    void documentModified();
    void updateStatusBar();

private:
    void createActions();
    void createMenus();
    void createStatusBar();
    void loadSettings();
    void saveSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void loadFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    CodeEditor *textEdit;
    SyntaxHighlighter *highlighter;
    FindDialog *findDialog;
    QString currentFile;
    QLabel *lineColLabel;

    void applyTheme(bool dark);

    // View menu actions (for settings persistence)
    QAction *darkModeAct;
    QAction *lineNumAct;
    QAction *wordWrapAct;
    QAction *syntaxAct;
    QAction *rainbowAct;
    QAction *statusBarAct;
};

#endif // MAINWINDOW_H
