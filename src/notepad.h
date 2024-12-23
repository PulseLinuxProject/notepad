#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <QMainWindow>
#include <QTextEdit>
#include <QListWidget>
#include <QFrame>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDir>
#include <QFileInfo>
#include <QTimer>
#include <QListWidgetItem>

class Notepad : public QMainWindow
{
    Q_OBJECT

public:
    explicit Notepad(QWidget *parent = nullptr);
    ~Notepad();  // Destructor declaration

private:
    void setupLayout();
    void setupModernDesign();
    void checkFirstLaunch();
    void loadNotesFromDirectory(const QString &dirPath);
    void updateNotesList();
    void showContextMenu(const QPoint &pos);
    void deleteNoteFile(QListWidgetItem *item);
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void selectNote();

    QTextEdit *textEdit;
    QListWidget *notesList;
    QString currentFile;
};

#endif // NOTEPAD_H
