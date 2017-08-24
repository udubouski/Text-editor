#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets>

#include "menu.h"
#include "toolbar.h"
#include "field.h"
#include "filerecord.h"

class Widget : public QMainWindow
{
    Q_OBJECT


public:
    explicit Widget();
    ~Widget();

public slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();

    void closeApp();

    void cutText();
    void copyText();
    void pasteText();
    void deleteText();

    void changeCurrentFont(QAction*);
    void changeFontSize(QAction*);
    void setBoldText();
    void setItalicText();

protected:
    void contextMenuEvent(QContextMenuEvent* pe);
    void closeEvent(QCloseEvent * closeEvent);

    void createStatusBar();

private:
    bool loadFile(const QString &openFileName);
    bool saveFile(const QString &openFileName);

    bool agreedToContinue();
    void setCurrentFileName(const QString &fileName);

    MenuComponents *menuComponents;
    Menu *menu;
    EditToolBar *editToolBar;

    TextField *textField;

    FileRecord fileRecorder;

    QFont defaultFont;

    QString currentFileName;
    QTextEdit textEdit;
};

#endif
