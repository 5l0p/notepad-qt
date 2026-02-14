#include "mainwindow.h"
#include "codeeditor.h"
#include "syntaxhighlighter.h"
#include "finddialog.h"

#include <QAction>
#include <QInputDialog>
#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QTextStream>
#include <QFont>
#include <QFontDialog>
#include <QPalette>
#include <QApplication>
#include <QStyle>
#include <QSettings>
#include <QDir>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    textEdit = new CodeEditor;
    setCentralWidget(textEdit);

    // Set a monospace font like classic Notepad
    QFont font("Monospace", 10);
    font.setStyleHint(QFont::Monospace);
    textEdit->setFont(font);

    // Syntax highlighting (monochrome)
    highlighter = new SyntaxHighlighter(textEdit->document());

    // Find dialog
    findDialog = new FindDialog(textEdit, this);

    createActions();
    createMenus();
    createStatusBar();

    connect(textEdit->document(), &QTextDocument::contentsChanged,
            this, &MainWindow::documentModified);
    connect(textEdit, &QPlainTextEdit::cursorPositionChanged,
            this, &MainWindow::updateStatusBar);

    setCurrentFile(QString());
    textEdit->setAcceptDrops(false);
    setAcceptDrops(true);
    resize(800, 600);
    loadSettings();
    applyTheme(darkModeAct->isChecked());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        saveSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();
        if (!urls.isEmpty()) {
            QString filePath = urls.first().toLocalFile();
            if (!filePath.isEmpty()) {
                openFile(filePath);
            }
        }
    }
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFile(QString());
    }
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open File"), QString(),
            tr("All Files (*);;Text Files (*.txt)"));
        if (!fileName.isEmpty()) {
            loadFile(fileName);
        }
    }
}

void MainWindow::openFile(const QString &filePath)
{
    if (maybeSave()) {
        loadFile(filePath);
    }
}

bool MainWindow::save()
{
    if (currentFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(currentFile);
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save As"), QString(),
        tr("All Files (*);;Text Files (*.txt)"));
    if (fileName.isEmpty()) {
        return false;
    }
    return saveFile(fileName);
}

void MainWindow::find()
{
    findDialog->showFind();
}

void MainWindow::replace()
{
    findDialog->showReplace();
}

void MainWindow::goToLine()
{
    int maxLine = textEdit->document()->blockCount();
    QTextCursor cursor = textEdit->textCursor();
    int currentLine = cursor.blockNumber() + 1;

    bool ok;
    int line = QInputDialog::getInt(this, tr("Go to Line"),
        tr("Line number (1-%1):").arg(maxLine),
        currentLine, 1, maxLine, 1, &ok);

    if (ok) {
        QTextBlock block = textEdit->document()->findBlockByLineNumber(line - 1);
        cursor.setPosition(block.position());
        textEdit->setTextCursor(cursor);
        textEdit->centerCursor();
    }
}

void MainWindow::documentModified()
{
    setWindowModified(textEdit->document()->isModified());
}

void MainWindow::updateStatusBar()
{
    QTextCursor cursor = textEdit->textCursor();
    int line = cursor.blockNumber() + 1;
    int col = cursor.columnNumber() + 1;
    lineColLabel->setText(tr("Ln %1, Col %2").arg(line).arg(col));
}

void MainWindow::applyTheme(bool dark)
{
    if (dark) {
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
        darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, Qt::white);
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        QApplication::setPalette(darkPalette);

        QPalette editorPalette = textEdit->palette();
        editorPalette.setColor(QPalette::Base, QColor(30, 30, 30));
        editorPalette.setColor(QPalette::Text, QColor(220, 220, 220));
        textEdit->setPalette(editorPalette);
    } else {
        QApplication::setPalette(QApplication::style()->standardPalette());

        QPalette editorPalette = textEdit->palette();
        editorPalette.setColor(QPalette::Base, Qt::white);
        editorPalette.setColor(QPalette::Text, Qt::black);
        textEdit->setPalette(editorPalette);
    }

    textEdit->setDarkMode(dark);
    highlighter->setDarkMode(dark);
}

void MainWindow::createActions()
{
    // Actions are created inline in createMenus for simplicity
}

void MainWindow::createMenus()
{
    // File menu
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *newAct = fileMenu->addAction(tr("&New"), this, &MainWindow::newFile);
    newAct->setShortcuts(QKeySequence::New);

    QAction *newWindowAct = fileMenu->addAction(tr("New &Window"), []() {
        QProcess::startDetached(QCoreApplication::applicationFilePath(), QStringList());
    });
    newWindowAct->setShortcut(QKeySequence(tr("Ctrl+Shift+N")));

    QAction *openAct = fileMenu->addAction(tr("&Open..."), this, &MainWindow::open);
    openAct->setShortcuts(QKeySequence::Open);

    QAction *saveAct = fileMenu->addAction(tr("&Save"), this, &MainWindow::save);
    saveAct->setShortcuts(QKeySequence::Save);

    QAction *saveAsAct = fileMenu->addAction(tr("Save &As..."), this, &MainWindow::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);

    // Edit menu
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    QAction *undoAct = editMenu->addAction(tr("&Undo"), textEdit, &QPlainTextEdit::undo);
    undoAct->setShortcuts(QKeySequence::Undo);

    QAction *redoAct = editMenu->addAction(tr("&Redo"), textEdit, &QPlainTextEdit::redo);
    redoAct->setShortcuts(QKeySequence::Redo);

    editMenu->addSeparator();

    QAction *cutAct = editMenu->addAction(tr("Cu&t"), textEdit, &QPlainTextEdit::cut);
    cutAct->setShortcuts(QKeySequence::Cut);

    QAction *copyAct = editMenu->addAction(tr("&Copy"), textEdit, &QPlainTextEdit::copy);
    copyAct->setShortcuts(QKeySequence::Copy);

    QAction *pasteAct = editMenu->addAction(tr("&Paste"), textEdit, &QPlainTextEdit::paste);
    pasteAct->setShortcuts(QKeySequence::Paste);

    QAction *deleteAct = editMenu->addAction(tr("&Delete"), [this]() {
        textEdit->textCursor().removeSelectedText();
    });
    deleteAct->setShortcut(QKeySequence::Delete);

    editMenu->addSeparator();

    QAction *selectAllAct = editMenu->addAction(tr("Select &All"), textEdit, &QPlainTextEdit::selectAll);
    selectAllAct->setShortcuts(QKeySequence::SelectAll);

    editMenu->addSeparator();

    QAction *findAct = editMenu->addAction(tr("&Find..."), this, &MainWindow::find);
    findAct->setShortcuts(QKeySequence::Find);

    QAction *replaceAct = editMenu->addAction(tr("&Replace..."), this, &MainWindow::replace);
    replaceAct->setShortcut(QKeySequence(tr("Ctrl+H")));

    QAction *goToLineAct = editMenu->addAction(tr("&Go to Line..."), this, &MainWindow::goToLine);
    goToLineAct->setShortcut(QKeySequence(tr("Ctrl+G")));

    // View menu
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    darkModeAct = viewMenu->addAction(tr("&Dark Mode"));
    darkModeAct->setCheckable(true);
    darkModeAct->setChecked(true);
    connect(darkModeAct, &QAction::toggled, this, &MainWindow::applyTheme);

    viewMenu->addSeparator();

    lineNumAct = viewMenu->addAction(tr("&Line Numbers"));
    lineNumAct->setCheckable(true);
    lineNumAct->setChecked(true);
    connect(lineNumAct, &QAction::toggled, [this](bool checked) {
        textEdit->setLineNumbersVisible(checked);
    });

    wordWrapAct = viewMenu->addAction(tr("&Word Wrap"));
    wordWrapAct->setCheckable(true);
    wordWrapAct->setChecked(true);
    connect(wordWrapAct, &QAction::toggled, [this](bool checked) {
        textEdit->setLineWrapMode(checked ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
    });

    syntaxAct = viewMenu->addAction(tr("&Syntax Highlighting"));
    syntaxAct->setCheckable(true);
    syntaxAct->setChecked(true);
    connect(syntaxAct, &QAction::toggled, [this](bool checked) {
        highlighter->setEnabled(checked);
    });

    rainbowAct = viewMenu->addAction(tr("&Rainbow Brackets"));
    rainbowAct->setCheckable(true);
    rainbowAct->setChecked(false);
    connect(rainbowAct, &QAction::toggled, [this](bool checked) {
        highlighter->setRainbowBracketsEnabled(checked);
    });

    statusBarAct = viewMenu->addAction(tr("S&tatus Bar"));
    statusBarAct->setCheckable(true);
    statusBarAct->setChecked(true);
    connect(statusBarAct, &QAction::toggled, [this](bool checked) {
        statusBar()->setVisible(checked);
    });

    viewMenu->addSeparator();

    viewMenu->addAction(tr("&Font..."), [this]() {
        bool ok;
        QFont font = QFontDialog::getFont(&ok, textEdit->font(), this);
        if (ok) {
            textEdit->setFont(font);
        }
    });
}

void MainWindow::createStatusBar()
{
    lineColLabel = new QLabel(tr("Ln 1, Col 1"));
    statusBar()->addPermanentWidget(lineColLabel);
    statusBar()->showMessage(tr("Ready"));
}

bool MainWindow::maybeSave()
{
    if (!textEdit->document()->isModified()) {
        return true;
    }

    QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("QtCode"),
        tr("The document has been modified.\n"
           "Do you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save) {
        return save();
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
    return true;
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QtCode"),
            tr("Cannot write file %1:\n%2.")
            .arg(fileName)
            .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << textEdit->toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QtCode"),
            tr("Cannot read file %1:\n%2.")
            .arg(fileName)
            .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    textEdit->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    currentFile = fileName;
    textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = currentFile;
    if (currentFile.isEmpty()) {
        shownName = "Untitled";
    } else {
        shownName = strippedName(currentFile);
    }
    setWindowTitle(tr("%1[*]").arg(shownName));
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::loadSettings()
{
    QString configPath = QDir::homePath() + "/.config/5l0p/qtcode.conf";
    QSettings settings(configPath, QSettings::IniFormat);
    settings.beginGroup("View");
    darkModeAct->setChecked(settings.value("darkMode", true).toBool());
    lineNumAct->setChecked(settings.value("lineNumbers", true).toBool());
    wordWrapAct->setChecked(settings.value("wordWrap", true).toBool());
    syntaxAct->setChecked(settings.value("syntaxHighlighting", true).toBool());
    rainbowAct->setChecked(settings.value("rainbowBrackets", false).toBool());
    statusBarAct->setChecked(settings.value("statusBar", true).toBool());
    settings.endGroup();
}

void MainWindow::saveSettings()
{
    QString configPath = QDir::homePath() + "/.config/5l0p/qtcode.conf";
    QDir().mkpath(QDir::homePath() + "/.config/5l0p");
    QSettings settings(configPath, QSettings::IniFormat);
    settings.beginGroup("View");
    settings.setValue("darkMode", darkModeAct->isChecked());
    settings.setValue("lineNumbers", lineNumAct->isChecked());
    settings.setValue("wordWrap", wordWrapAct->isChecked());
    settings.setValue("syntaxHighlighting", syntaxAct->isChecked());
    settings.setValue("rainbowBrackets", rainbowAct->isChecked());
    settings.setValue("statusBar", statusBarAct->isChecked());
    settings.endGroup();
}
