#include "menu.h"

Menu::Menu(MenuComponents *components, QWidget *parent)
    : QMenuBar(parent)
{
    this->components = components;
    fileMenu = new QMenu(tr("File"), this) ;
    editMenu = new QMenu(tr("Edit"), this);
    contextMenu = new QMenu(this);
    createMenu();
    createContextMenu();
}

Menu::~Menu()
{
}

void Menu::createMenu()
{
    components->addFileActions(fileMenu);
    fileMenu->addSeparator();
    components->addRecentFilesActions(fileMenu);
    fileMenu->addSeparator();
    components->addExitAction(fileMenu);
    addMenu(fileMenu);

    components->addEditActions(editMenu);
    editMenu->addSeparator();
    components->addDropDawnFontActions(editMenu);
    addMenu(editMenu);
}

void Menu::createContextMenu()
{
    components->addFileActions(contextMenu);
    contextMenu->addSeparator();
    components->addEditActions(contextMenu);
    contextMenu->addSeparator();
    components->addDropDawnFontActions(contextMenu);
}

QMenu* Menu::getContextMenu() const
{
    return contextMenu;
}
