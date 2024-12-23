#include "notepad.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QToolBar>
#include <QAction>
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QTextEdit>
#include <QTimer>
#include <QListWidgetItem>
#include <QVBoxLayout>

Notepad::Notepad(QWidget *parent) : QMainWindow(parent), textEdit(new QTextEdit(this)), notesList(new QListWidget(this)) {
    // Setup UI
    QFrame *mainFrame = new QFrame(this);
    QHBoxLayout *layout = new QHBoxLayout(mainFrame);

    // Create a QToolBar for the actions
    QToolBar *toolbar = new QToolBar(this);
    toolbar->setMovable(false); // Prevent toolbar from being moved
    toolbar->setFixedHeight(50); // Make the toolbar longer (taller)

    // Add actions to the toolbar
    QAction *newAction = new QAction("New", this);
    connect(newAction, &QAction::triggered, this, &Notepad::newFile);
    toolbar->addAction(newAction);

    QAction *openAction = new QAction("Open", this);
    connect(openAction, &QAction::triggered, this, &Notepad::openFile);
    toolbar->addAction(openAction);
    
    QAction *saveAction = new QAction("Save", this);
    connect(saveAction, &QAction::triggered, this, &Notepad::saveFile);
    toolbar->addAction(saveAction);

    QAction *saveAsAction = new QAction("Save As", this);
    connect(saveAsAction, &QAction::triggered, this, &Notepad::saveFileAs);
    toolbar->addAction(saveAsAction);
    
    QAction *exitAction = new QAction("Exit", this);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    toolbar->addAction(exitAction);

    // Sidebar layout
    notesList->setFixedWidth(380); // Make the sidebar longer

    // Create layout for sidebar and toolbar
    QVBoxLayout *sidebarLayout = new QVBoxLayout();
    sidebarLayout->addWidget(toolbar); // Add toolbar at the top of sidebar
    sidebarLayout->addWidget(notesList);

    // Set the sidebar layout into the main layout
    layout->addLayout(sidebarLayout);

    // Text editor area
    layout->addWidget(textEdit);
    textEdit->setFixedWidth(500); // Make the sidebar longer
    textEdit->setFixedHeight(500); // Make the toolbar longer (taller)

    mainFrame->setLayout(layout);
    setCentralWidget(mainFrame);

    // Apply modern design styles
    setupModernDesign();

    // Check if it's the first launch
    checkFirstLaunch();

    // Set up the timer to refresh the notes list every 100ms
    QTimer *refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &Notepad::updateNotesList);
    refreshTimer->start(100); // 100 ms
}

Notepad::~Notepad() {
    // Clean up resources
    delete textEdit;
    delete notesList;
}

void Notepad::setupModernDesign() {
    setStyleSheet(
        "QFrame {"
        "    background: #f0f0f0;"
        "    border: none;"
        "}"
        "QListWidget {"
        "    background: #007bff;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 25px;"
        "    padding: 10px;"
        "}"
        "QListWidget::item {"
        "    border-radius: 15px;"
        "    padding: 5px;"
        "    margin: 5px;"
        "}"
        "QListWidget::item:selected {"
        "    background: #0056b3;"
        "    color: white;"
        "}"
        "QTextEdit {"
        "    background: white;"
        "    color: black;"
        "    border: none;"
        "    border-radius: 25px;"
        "    padding: 10px;"
        "}"
        "QToolBar {"
        "    background: #007bff;"
        "    border: none;"
        "    border-radius: 25px;"
        "    padding: 10px;"
        "}"
        "QToolBar QToolButton {"
        "    color: white;"
        "    padding: 5px 10px;"
        "}"
        "QToolBar QToolButton:hover {"
        "    background: #0056b3;"
        "    border-radius: 10px;"
        "}"
    );
}

void Notepad::checkFirstLaunch() {
    QSettings settings("PulseOS", "Notepad");
    QString notesDir = settings.value("notesDirectory").toString();

    if (notesDir.isEmpty() || !QDir(notesDir).exists()) {
        QString selectedDir = QFileDialog::getExistingDirectory(this, "Select Notes Storage Folder");
        if (!selectedDir.isEmpty()) {
            if (!QDir().mkpath(selectedDir)) {
                QMessageBox::warning(this, "Directory Error", "Unable to create or access the directory.");
                QApplication::quit();
            } else {
                settings.setValue("notesDirectory", selectedDir);
                QMessageBox::information(this, "Notes Directory", "Your notes will be stored in: " + selectedDir);
            }
        } else {
            QMessageBox::warning(this, "No Directory Selected", "No directory selected. The application will exit.");
            QApplication::quit();
        }
    } else {
        loadNotesFromDirectory(notesDir);
    }
}

void Notepad::loadNotesFromDirectory(const QString &dirPath) {
    QDir notesDir(dirPath);
    if (!notesDir.exists()) {
        QMessageBox::warning(this, "Directory Error", "Notes directory no longer exists.");
        return;
    }
    QStringList files = notesDir.entryList(QStringList() << "*.txt" << "*.rtf", QDir::Files);

    notesList->clear();  // Clear the existing items before reloading

    for (const QString &file : files) {
        notesList->addItem(file);
    }
}

void Notepad::updateNotesList() {
    QSettings settings("PulseOS", "Notepad");
    QString notesDir = settings.value("notesDirectory").toString();
    if (!notesDir.isEmpty()) {
        loadNotesFromDirectory(notesDir);
    }
}

void Notepad::newFile() {
    QString newNoteName = "NewNote.txt";
    if (notesList->findItems(newNoteName, Qt::MatchExactly).isEmpty()) {
        notesList->addItem(newNoteName);
    } else {
        int counter = 1;
        while (!notesList->findItems(newNoteName + QString::number(counter), Qt::MatchExactly).isEmpty()) {
            counter++;
        }
        notesList->addItem(newNoteName + QString::number(counter));
    }
    textEdit->clear();
    currentFile.clear();
}

void Notepad::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", QString(), "Text Files (*.txt);;Rich Text Files (*.rtf)");
    if (!fileName.isEmpty()) {
        QFileInfo fileInfo(fileName);

        if (fileInfo.suffix() == "txt" || fileInfo.suffix() == "rtf") {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                textEdit->setText(file.readAll());
                currentFile = fileName;
                notesList->addItem(fileInfo.fileName());
                file.close();
            } else {
                QMessageBox::warning(this, "Error", "Cannot open file: " + file.errorString());
            }
        }
    }
}

void Notepad::saveFile() {
    if (currentFile.isEmpty()) {
        saveFileAs();
        return;
    }

    QFile file(currentFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << textEdit->toPlainText();
        file.close();
    } else {
        QMessageBox::warning(this, "Error", "Cannot save file: " + file.errorString());
    }
}

void Notepad::saveFileAs() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save File", QString(), "Text Files (*.txt);;Rich Text Files (*.rtf)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << textEdit->toPlainText();
            file.close();
            currentFile = fileName;
        } else {
            QMessageBox::warning(this, "Error", "Cannot save file: " + file.errorString());
        }
    }
}

void Notepad::selectNote() {
    QListWidgetItem *item = notesList->currentItem();
    if (item) {
        QString fileName = item->text();
        QSettings settings("PulseOS", "Notepad");
        QString notesDir = settings.value("notesDirectory").toString();
        QString filePath = notesDir + "/" + fileName;

        QFile file(filePath);
        if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            textEdit->setText(file.readAll());
            currentFile = filePath;
            file.close();
        } else {
            QMessageBox::warning(this, "Error", "Cannot open the note.");
        }
    }
}