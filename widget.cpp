#include "widget.h"

Widget::Widget() :
    QMainWindow()
{
    menuComponents = new MenuComponents;

    menu = new Menu(menuComponents, this);
    setMenuBar(menu);

    editToolBar = new EditToolBar(menuComponents, this);
    addToolBar(editToolBar);

    defaultFont = QFont(editToolBar->getFontType(), editToolBar->getFontSize());

    textField = new TextField(defaultFont, this);
    textField->setFocus();
    setCentralWidget(textField);
    textField->setTextEditorView(Qt::white);

    setCurrentFileName("");

    connect(menuComponents->newAction, SIGNAL( triggered() ), SLOT( newFile() ) );
    connect(menuComponents->openAction, SIGNAL( triggered() ), SLOT( open() ) );
    connect(menuComponents->saveAction, SIGNAL( triggered() ), SLOT( save() ) );
    connect(menuComponents->saveAsAction, SIGNAL( triggered() ), SLOT( saveAs() ) );
    connect(menuComponents->exitAction, SIGNAL( triggered() ), SLOT( closeApp() ) );
    connect(menuComponents->cutAction, SIGNAL( triggered() ), SLOT( cutText() ) );
    connect(menuComponents->copyAction, SIGNAL( triggered() ), SLOT( copyText() ) );
    connect(menuComponents->pasteAction, SIGNAL( triggered() ), SLOT( pasteText() ) );
    connect(menuComponents->deleteAction, SIGNAL( triggered() ), SLOT( deleteText() ) );
    connect(menuComponents->fontTypeMenu, SIGNAL( triggered(QAction*) ), SLOT( changeCurrentFont(QAction*) ) );
    connect(menuComponents->fontSizeMenu, SIGNAL( triggered(QAction*) ), SLOT( changeFontSize(QAction*) ) );

    connect(editToolBar->getFontBox(), SIGNAL( activated(QString) ), textField, SLOT( changeCurrentFont(QString) ) );
    connect(editToolBar->getSizeBox(), SIGNAL(activated(QString)), textField, SLOT( changeCurrentFontSize(QString) ) );
    connect(menuComponents->fontBoldAction, SIGNAL( triggered() ), SLOT( setBoldText() ) );
    connect(menuComponents->fontItalicAction, SIGNAL( triggered() ), SLOT( setItalicText() ) );

    connect(textField, SIGNAL( fontChanged(const QFont&) ), editToolBar, SLOT( changeToolBarFonts(const QFont&) ) );

    setWindowTitle(tr("TextEditor"));

    textField->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    textField->resize(sizeHint());
    setMinimumSize(400, 200);
    setMaximumSize(1368, 768);
    resize(700, 500);
    show();
}

Widget::~Widget()
{

}

void Widget::contextMenuEvent(QContextMenuEvent* mouse_pointer)
{
    menu->getContextMenu()->exec(mouse_pointer->globalPos());
}

void Widget::closeEvent(QCloseEvent * closeEvent)
{
    if(agreedToContinue()){
        closeEvent->accept();
    }
    else
        closeEvent->ignore();
}

bool Widget::agreedToContinue()
 {
     if (!textEdit.document()->isModified())
         return true;
     QMessageBox::StandardButton answer = QMessageBox::warning(this,
                       tr("The document has been modified"),
                       tr("Do you want to save your changes?"),
                       QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

     if (answer == QMessageBox::Save)
         return save();
     else if (answer == QMessageBox::Cancel)
         return false;
     return true;
 }

void Widget::newFile()
{
    if(agreedToContinue()){
        textField->clear();
        setCurrentFileName("");
    }
}

void Widget::open()
{
    if(agreedToContinue()){
        QString openFileName = QFileDialog::getOpenFileName(this,
                                                tr("Open file"), "/media/file",
                                                tr("Text files (*.txt);;Xml files (*.xml)"));
        if(!openFileName.isEmpty())
            loadFile(openFileName);

    }
}

bool Widget::save()
{
    if(currentFileName.isEmpty())
        return saveAs();
    else
        return saveFile(currentFileName);
}

bool Widget::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                tr("Save file"), "/media/file",
                                                tr("Text files (*.txt);;Xml files (*.xml)"));
    if(fileName.isEmpty())
        return false;
    if(!fileName.endsWith(".txt") && !fileName.endsWith(".xml"))
        fileName += ".xml";
    return saveFile(fileName);
}

bool Widget::loadFile(const QString &fileName)
{
    try{
        textField->setText(fileRecorder.read(fileName, defaultFont));

        textField->setCurrentPos(QPoint(0, 0));
        setCurrentFileName(fileName);
    }
    catch(FileOpenException &)
    {
        return false;
    }

    return true;
}

bool Widget::saveFile(const QString &fileName)
{
    try{
        fileRecorder.write(textField->getText(), fileName);
        setCurrentFileName(fileName);
    }
    catch(FileOpenException &)
    {
        return false;
    }

    return true;
}

void Widget::closeApp()
{
    close();
}

void Widget::cutText()
{
    textField->cut();
}

void Widget::copyText()
{
    textField->copy();
}

void Widget::pasteText()
{
    textField->paste();
}

void Widget::deleteText()
{
     textField->clear();
}

void Widget::changeFontSize(QAction* action)
{
    textField->changeCurrentFontSize(action->text());
}

void Widget::changeCurrentFont(QAction* action)
{
    textField->changeCurrentFont(action->text());
}

void Widget::setBoldText()
{
    bool checked = menuComponents->fontBoldAction->isChecked() ? true : false;
    menuComponents->fontBoldAction->setChecked(checked);
    textField->changeBold(checked);
}

void Widget::setItalicText()
{
    bool checked = menuComponents->fontItalicAction->isChecked() ? true : false;
    menuComponents->fontItalicAction->setChecked(checked);
    textField->changeItalics(checked);
}

void Widget::setCurrentFileName(const QString &fileName)
{
    currentFileName = fileName;

    QString shownName;
    if(currentFileName.isEmpty())
        shownName = tr("untitled.txt");
    else
        shownName = QFileInfo(currentFileName).fileName();
}
