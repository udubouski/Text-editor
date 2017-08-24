#include "component.h"

MenuComponents::MenuComponents(QWidget *parent)
    : QWidget(parent)
{
    createFileActions();
    createEditActions();
    createFontActions();
    fontTypeMenu = new QMenu(tr("Font"), this);
    fontSizeMenu = new QMenu(tr("Size"), this);

    QFontDatabase fdb;
    fonts << fdb.families();

    fontSizes << "4" << "8" << "12" << "14" << "16" << "18" << "20"
              << "24" << "28" << "32" << "36" << "40" << "48";

    QList<QAction*> fontActions;
    foreach (QString font, fonts) {
        fontActions.push_back(new QAction(font, this));
    }
    fontTypeMenu->addActions(fontActions);

    QList<QAction*> fontSizeActions;
    foreach (QString fontSize, fontSizes) {
        fontSizeActions.push_back(new QAction(fontSize, this));
    }
    fontSizeMenu->addActions(fontSizeActions);
}

MenuComponents::~MenuComponents()
{
}

void MenuComponents::createFileActions()
{
    newAction = new QAction(tr("New"), this);
    newAction->setIcon(QIcon(":/icons/icons/new.png"));
    newAction->setShortcut(QKeySequence::New);

    openAction = new QAction(tr("Open"), this);
    openAction->setIcon(QIcon(":/icons/icons/open.png"));
    openAction->setShortcut(QKeySequence::Open);

    saveAction = new QAction(tr("Save"), this);
    saveAction->setIcon(QIcon(":/icons/icons/save.png"));
    saveAction->setShortcut(Qt::CTRL + Qt::Key_S);

    saveAsAction = new QAction(tr("Save As"), this);
    saveAsAction->setIcon(QIcon(":/icons/icons/saveas.png"));
    saveAsAction->setShortcut(QKeySequence::SaveAs);

    for(int i = 0; i < MAX_RECENT_FILES; ++i)
    {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
    }

    exitAction = new QAction(tr("Exit"), this);
    exitAction->setIcon(QIcon(":/icons/icons/exit.png"));
    exitAction->setShortcut(QKeySequence::Quit);

}

void MenuComponents::createEditActions()
{
    cutAction = new QAction(tr("Cut"), this);
    cutAction->setIcon(QIcon(":/icons/icons/cut.png"));
    cutAction->setShortcut(QKeySequence::Cut);

    copyAction = new QAction(tr("&Copy"), this);
    copyAction->setIcon(QIcon(":/icons/icons/copy.png"));
    copyAction->setShortcut(QKeySequence::Copy);

    pasteAction = new QAction(tr("Paste"), this);
    pasteAction->setIcon(QIcon(":/icons/icons/paste.png"));
    pasteAction->setShortcut(Qt::CTRL + Qt::Key_V);

    deleteAction = new QAction(tr("Delete"), this);
    deleteAction->setIcon(QIcon(":/icons/icons/delete.png"));
    deleteAction->setShortcut(QKeySequence::Delete);
}

void MenuComponents::createFontActions()
{
    fontBoldAction = new QAction(tr("Bold"), this);
    fontBoldAction->setIcon(QIcon(":/icons/icons/bold.png"));
    fontBoldAction->setCheckable(true);
    fontItalicAction = new QAction(tr("Italics"), this);
    fontItalicAction->setIcon(QIcon(":/icons/icons/italic.png"));
    fontItalicAction->setCheckable(true);

}

void MenuComponents::addFileActions(QWidget *menu)
{
    menu->addAction(newAction);
    menu->addAction(openAction);
    menu->addAction(saveAction);
    menu->addAction(saveAsAction);
}

void MenuComponents::addEditActions(QWidget *menu)
{
    menu->addAction(cutAction);
    menu->addAction(copyAction);
    menu->addAction(pasteAction);

}

void MenuComponents::addFontActions(QWidget *menu)
{
    menu->addAction(fontBoldAction);
    menu->addAction(fontItalicAction);
}

void MenuComponents::addDropDawnFontActions(QMenu *menu)
{
    menu->addMenu(fontTypeMenu);
    menu->addMenu(fontSizeMenu);
    menu->addAction(fontBoldAction);
    menu->addAction(fontItalicAction);
}

void MenuComponents::addRecentFilesActions(QWidget *menu)
{
    for(int i = 0; i < MAX_RECENT_FILES; ++i)
        menu->addAction(recentFileActions[i]);
}

void MenuComponents::addExitAction(QWidget *menu)
{
    menu->addAction(exitAction);
}
