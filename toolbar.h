#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QtWidgets>

#include "component.h"

class EditToolBar : public QToolBar
{
    Q_OBJECT

public:
    explicit EditToolBar(MenuComponents * components, QWidget *parent = Q_NULLPTR);

    const QComboBox* getFontBox() const { return fontBox; }
    const QComboBox* getSizeBox() const { return sizeBox; }
    int getFontSize() const { return sizeBox->currentText().toInt(); }
    const QString getFontType() const { return fontBox->currentText(); }

    ~EditToolBar();

public slots:
    void changeToolBarFonts(const QFont&);

protected:
    void createToolBar();

private:
    MenuComponents *components;
    QComboBox *fontBox;
    QComboBox *sizeBox;
};


#endif
