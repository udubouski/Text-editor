#include "toolbar.h"

EditToolBar::EditToolBar(MenuComponents *components, QWidget *parent)
    : QToolBar(parent)
{
    this->components = components;
    fontBox = new QComboBox(this);
    sizeBox = new QComboBox(this);

    createToolBar();
}

EditToolBar::~EditToolBar()
{
}

void EditToolBar::changeToolBarFonts(const QFont& f)
{
    sizeBox->setCurrentText(QString::number(f.pointSize()));
    fontBox->setCurrentText(f.family());
    components->fontBoldAction->setChecked(f.bold());
    components->fontItalicAction->setChecked(f.italic());
}

void EditToolBar::createToolBar()
{
    components->addFileActions(this);
    addSeparator();
    components->addEditActions(this);
    addSeparator();

    fontBox->addItems(components->getFontList());
    addWidget(fontBox);

    #ifdef __linux__
        fontBox->setCurrentText("Ubuntu");
    #else
    #ifdef __WIN32__
        fontBox->setCurrentText("Serif");
    #endif
    #endif

    sizeBox->addItems(components->getFontSizeList());
    addWidget(sizeBox);
    sizeBox->setEditable(true);
    sizeBox->setCurrentText("12");

    addAction(components->fontBoldAction);
    addAction(components->fontItalicAction);
}
