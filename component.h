#ifndef COMPONENT_H
#define COMPONENT_H

#include <QtWidgets>

class MenuComponents : public QWidget
{
    Q_OBJECT

public:
    explicit MenuComponents(QWidget *parent = Q_NULLPTR);
    ~MenuComponents();

    void addFileActions(QWidget *menu);
    void addRecentFilesActions(QWidget *menu);
    void addEditActions(QWidget *menu);
    void addFontActions(QWidget *menu);
    void addDropDawnFontActions(QMenu *menu);
    void addExitAction(QWidget *menu);

    enum { MAX_RECENT_FILES = 5 };

    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *recentFileActions[MAX_RECENT_FILES];
    QAction *exitAction;

    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *deleteAction;

    QAction *fontBoldAction;
    QAction *fontItalicAction;

    QMenu *fontTypeMenu;
    QMenu *fontSizeMenu;

    inline const QStringList& getFontList() const { return fonts; }
    inline const QStringList& getFontSizeList() const { return fontSizes; }

public slots:

private:
    QString resentFileNames[MAX_RECENT_FILES];
    void createFileActions();
    void createEditActions();
    void createFontActions();

    QStringList fonts;
    QStringList fontSizes;
};

#endif
