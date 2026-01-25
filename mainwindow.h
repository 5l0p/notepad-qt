#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void openFile(const QString &filePath);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void documentModified();
    void updateStatusBar();

private:
    void createActions();
    void createMenus();
    void createStatusBar();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void loadFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QPlainTextEdit *textEdit;
    QString currentFile;
    QLabel *lineColLabel;
};

#endif // MAINWINDOW_H
