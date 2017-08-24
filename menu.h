#ifndef MENU_H
#define MENU_H

#include <QtWidgets>
#include "component.h"

class Menu : public QMenuBar
{
    Q_OBJECT

public:
    explicit Menu(MenuComponents * components, QWidget *parent = Q_NULLPTR);
    ~Menu();

    QMenu* getContextMenu() const;

protected:
    void createMenu();
    void createContextMenu();

private:
    MenuComponents *components;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *contextMenu;

};

#endif
