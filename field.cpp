#include "field.h"

TextField::TextField(QFont f, QWidget *parent)
    : QAbstractScrollArea(parent)
{
    field_ = new QWidget(this);

    setViewport(field_);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    verticalScrollBar()->setEnabled(true);
    horizontalScrollBar()->setEnabled(true);
    setMaximumSize(1368, 768);

    width = 0;
    field_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(horizontalScrollBar(), SIGNAL( valueChanged(int) ), SLOT( moveHViewPort(int) ) );
    connect(verticalScrollBar(), SIGNAL( valueChanged(int) ), SLOT( moveVViewPort(int) ) );

    edge_ = QPoint(5, 5);

    setFont(f);

    highlightningColor_ = QColor(0x96, 0x96, 0x96);
    palette().highlight().color();
    selectionBegin_ = edge_;
    selectionEnd_ = edge_;
    selectionPos_ = QPoint(0, 0);
    setSelected(false);
    curPos_ = QPoint(0, 0);

    cursor_ = new Cursor(viewport(), this);
    cursor_->setEdge(edge_);
    cursor_->setCursor(QPoint(0, 0), QFontMetrics(font()).height());
    cursor_->setColorCursor(viewport()->palette().color(QPalette::WindowText));
    cursor_->setColorBase(viewport()->palette().color(QPalette::Base));
    cursor_->setColorHighlighted(highlightningColor_);

    textLines_ = new Text(QFontMetrics(font()).height(), this);

    textBuffer_= new Text(QFontMetrics(font()).height(), this);

    setCapsLock(false);

    viewport()->update();
}

TextField::~TextField()
{
    delete field_;
    delete cursor_;
    delete textBuffer_;
    delete textLines_;
}

void TextField::changeCurrentFontSize(const QString& fontSize)
{
    setFocus();
    apply_font_func<int>(&QFont::setPointSize, fontSize.toInt());
    QFont f = QFont(font().family(), fontSize.toInt(), -1, font().italic());
    f.setBold(font().bold());
    setFont(f);
}

void TextField::changeCurrentFont(const QString& fontName)
{
    setFocus();
    apply_font_func<const QString&>(&QFont::setFamily, fontName);
    QFont f = QFont(fontName, font().pointSize(), -1, font().italic());
    f.setBold(font().bold());
    setFont(f);
}

void TextField::changeBold(bool bold)
{
    apply_font_func<bool>(&QFont::setBold, bold);
    QFont f = QFont(font().family(), font().pointSize(), -1, font().italic());
    f.setBold(bold);
    setFont(f);
}

void TextField::changeItalics(bool it)
{
    apply_font_func<bool>(&QFont::setItalic, it);
    QFont f = QFont(font().family(), font().pointSize(), -1, it);
    f.setBold(font().bold());
    setFont(f);
}

void TextField::moveHViewPort(int value)
{
    qint64 topLeftX = viewport()->rect().topLeft().x();
    qint64 topLeftY = viewport()->rect().topLeft().y();

    viewport()->move(topLeftX - value, topLeftY - verticalScrollBar()->value());
    viewport()->update();
}

void TextField::moveVViewPort(int value)
{
    qint64 topLeftX = viewport()->rect().topLeft().x();
    qint64 topLeftY = viewport()->rect().topLeft().y();

    viewport()->move(topLeftX - horizontalScrollBar()->value(), topLeftY - value);
    viewport()->update();
}

void TextField::scrollViewport(QPoint pos)
{
    QPoint shift = textLines_->getShiftByPos(pos.x(), pos.y(), pos);
    int marginX = 2 * edge_.x() + verticalScrollBar()->height();
    int marginY = edge_.y() + 0.2 * horizontalScrollBar()->height();

    int x = 0;
    int y = 0;

    if(shift.x() + marginX - horizontalScrollBar()->value() < 0 ||
       shift.x() + marginX - horizontalScrollBar()->value() > size().width())
    {
        x = shift.x() + marginX - size().width();
        x = x < 0 ? 0 : x;
        horizontalScrollBar()->setValue(x);
        emit horizontalScrollBar()->valueChanged(x);
    }

    if(shift.y() + marginY - verticalScrollBar()->value() < 0 ||
       shift.y() + marginY - verticalScrollBar()->value() > size().height())
    {
        y = shift.y() + marginY - size().width();
        y = y < 0 ? 0 : y;
        verticalScrollBar()->setValue(y);
    }
}

void TextField::setTextEditorView(Qt::GlobalColor color)
{
    QPalette pal = QPalette(field_->palette());
    pal.setBrush(backgroundRole(), QBrush(color));
    field_->setPalette(pal);
    field_->setAutoFillBackground(true);

    field_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    field_->setCursor(Qt::IBeamCursor);
}

void TextField::clear()
{
    selectAll();
    QPoint p = _handle_backspace();
    _set_cursor_points(p);
}

void TextField::setText(const Text* text) {
    if(textLines_)
        delete textLines_;
    textLines_ = new Text(*text);
    textLines_->setParent(this);
}

void TextField::keyPressEvent(QKeyEvent *event)
{
    int x = cursor_->x() - edge_.x();
    int y = cursor_->y() - edge_.y();
    QPoint p = QPoint(x, y);
    QPoint pos = curPos_;

    if(event->matches(QKeySequence::Copy))
        copy();
    else if(event->matches(QKeySequence::Cut))
        cut();
    else if(event->matches(QKeySequence::Paste)){
        paste();
        return;
    }
    else if(event->matches(QKeySequence::SelectAll)){
        selectAll();
        return;
    }
    else if(event->key() == Qt::Key_Return)
        p =_handle_enter();
    else if(event->key() == Qt::Key_Backspace)
        p = _handle_backspace();
    else if(event->key() == Qt::Key_CapsLock)
        setCapsLock(capsLock() ? false : true);
    else{
        if((event->key() >= 0x20 && event->key() <= 0x7E) ||
               (event->key() >= 0x410 && event->key() <= 0x42f) ||
               (event->key() == 1000021))
        {
            if(isSelected())
                _erase_highlighted_text();

            QChar in_char = event->text().at(0);
           if((QApplication::keyboardModifiers() == Qt::NoModifier)){
                if(capsLock())
                    in_char = in_char.toUpper();
            }
            else if(QApplication::keyboardModifiers() == Qt::ShiftModifier)
                    if(capsLock())
                        in_char = in_char.toLower();

            textLines_->insert(curPos_.x(), curPos_.y(), Symbol(font(), in_char));
            p = textLines_->getShiftByPos(curPos_.x() + 1, curPos_.y(), pos);
        }
        else if(event->matches(QKeySequence::MoveToNextChar))
                    p = textLines_->getShiftByPos(curPos_.x() + 1, getCurPosY(), pos);
        else if(event->matches(QKeySequence::MoveToEndOfLine))
            p = textLines_->getShiftByPos((*textLines_)[curPos_.y()].size(), curPos_.y(), pos);
        else if(event->matches(QKeySequence::MoveToPreviousChar))
            p = textLines_->getShiftByPos(curPos_.x() - 1, getCurPosY(), pos);
        else if(event->matches(QKeySequence::MoveToStartOfLine))
            p = textLines_->getShiftByPos(0, curPos_.y(), pos);
        else if(event->matches(QKeySequence::MoveToNextLine)){
            if(curPos_.y() != textLines_->length() - 1){
                pos = QPoint(curPos_.x(), curPos_.y() + 1 >= textLines_->length() ?
                              textLines_->length() - 1 :
                                curPos_.y() + 1);
                x = (*textLines_)[pos.y()].getSymbolBegin(x, pos);
                y = (*textLines_).getLineShift(pos.y(), pos.x());
                p = QPoint(x, y);
            }
        }
        else if(event->matches(QKeySequence::MoveToPreviousLine)){
            pos = QPoint(curPos_.x(), curPos_.y() - 1 <= 0 ? 0 : curPos_.y() - 1);
            p = QPoint((*textLines_)[pos.y()].getSymbolBegin(x, pos),
                       (*textLines_).getLineShift(pos.y(), pos.x()));
        }
        else if(event->matches(QKeySequence::MoveToStartOfDocument))
            p = textLines_->getShiftByPos(0, 0, pos);
        else if(event->matches(QKeySequence::MoveToEndOfDocument)) {
            p = textLines_->getShiftByPos((*textLines_)[(*textLines_).length() - 1].size(),
                    (*textLines_).length(), pos);
        }
        else
            return;
        setCurrentPos(pos);
     }
     _set_cursor_points(p);
     resize_field(textLines_->width(), textLines_->height());
     scrollViewport(curPos_);
}

void TextField::mouseMoveEvent(QMouseEvent * event)
{
    QPoint pos = curPos_;
    QPoint p = textLines_->getShiftByCoord(QPoint(event->x() - edge_.x(), event->y() - edge_.y()),
                                           pos);
    setCurrentPos(pos);

    _change_cursor(p);
    _set_selection_end(QPoint(p.x(), (*textLines_).getLineRoof(getCurPosY())));
    setSelected(true);
}

void TextField::mousePressEvent(QMouseEvent * event)
{
    if(event->button() == Qt::MouseButton::LeftButton){
        QPoint pos = curPos_;
        QPoint curPoint = QPoint(event->x() - edge_.x(), event->y() - edge_.y());
        QPoint p = textLines_->getShiftByCoord(curPoint, pos);
        setCurrentPos(pos);
        _set_cursor_points(p);
    }
}

void TextField::paintEvent(QPaintEvent *)
{
    QPainter painter(viewport());
    if(selectionBegin_ != selectionEnd_)
    {
        const QPoint& beginSelect = minPoint(selectionBegin_, selectionEnd_);
        const QPoint& endSelect = maxPoint(selectionBegin_, selectionEnd_);
        _fill_highlightning_rect(painter, beginSelect, endSelect);
        cursor_->draw(&painter, true);
    }
    else
        cursor_->draw(&painter, false);
    width = textLines_->draw(&painter, curPos_, edge_);
}

void TextField::resizeEvent(QResizeEvent *)
{
    static int width = 0;
    static int height = 0;

    int marginX = 2 * edge_.x() + verticalScrollBar()->height();
    int marginY = edge_.y() + 0.2 * horizontalScrollBar()->height();

    int w = 0;
    int h = 0;

    if(width <= size().width()){
        w = size().width();
        width = w;
    }
    else{
        width = viewport()->size().width();
        w = width + marginX;
    }

    if(height  < size().height()){
        h = size().height();
        height = h;
    }
    else{
        h = height + marginY;
        height = h;
    }
    if(width != w || height != h){
        field_->setFixedSize(w, h);
    }

    horizontalScrollBar()->setPageStep(viewport()->size().height());
    verticalScrollBar()->setPageStep(viewport()->size().height());
    horizontalScrollBar()->setRange(0, viewport()->size().width() - size().width());
    verticalScrollBar()->setRange(0, viewport()->size().height() - size().height());
}

int TextField::getCurPosX() const
{
    if((*textLines_)[curPos_.y()].isEmpty())
        return 0;
    return curPos_.x() == (*textLines_)[curPos_.y()].length() ?
            curPos_.x() - 1 :
            curPos_.x();
}

int TextField::getCurPosY() const
{
    return curPos_.y();
}

void TextField::setCurrentPos(const QPoint& curPos) {
    curPos_ = curPos;
    emit posChanged(curPos_);
}

void TextField::copy()
{
    textLines_->copyPart(textBuffer_,
                minPoint(curPos_, selectionPos_),
                maxPoint(curPos_, selectionPos_));
}

void TextField::cut()
{
    textLines_->cutPart(textBuffer_,
                minPoint(curPos_, selectionPos_),
                maxPoint(curPos_, selectionPos_));
    setCurrentPos(minPoint(curPos_, selectionPos_));
    _set_cursor_points(textLines_->getShiftByPos(curPos_.x(), curPos_.y(), curPos_));
}

void TextField::paste()
{
    if(isSelected())
        _erase_highlighted_text();
    QPoint pos = curPos_;
    textLines_->insertPart(textBuffer_, pos);
    int x = (*textLines_)[curPos_.y()].getSymbShift(pos.x());
    int y = textLines_->getLineShift(pos.y(), pos.x());
    setCurrentPos(pos);
    _set_cursor_points(QPoint(x, y));
}

void TextField::selectAll()
{
    setSelected(true);
    selectionBegin_ = QPoint(0, 0);
    selectionEnd_ = _get_end_document();
    selectionPos_ = QPoint(0, 0);
    _change_cursor((*textLines_).getShiftByPos(
                        (*textLines_)[textLines_->length() - 1].length(),
                        textLines_->length() - 1,
                        curPos_));
}

void TextField::resize_field(qint64 w, qint64 h)
{
    if(w + 2 * edge_.x() + verticalScrollBar()->height() < size().width())
        w = size().width();
    else
        w = w + 2 * edge_.x() + 0.1 * size().width();
    if(h + edge_.y() + 0.2 * horizontalScrollBar()->height() < size().height())
        h = size().height();
    else
        h += edge_.y() + 0.2 * size().height();
    field_->setFixedSize(w, h);
}

QPoint TextField::_get_end_document()
{
    Line* lasLine = &(*textLines_)[textLines_->length() - 1];
    return QPoint(lasLine->getWidth(), textLines_->height() - lasLine->height());
}

QPoint TextField::_handle_backspace()
{
   QPoint p;
    if(isSelected()){
        _erase_highlighted_text();
    }
    else{
        textLines_->eraseSymbol(curPos_.y(), curPos_.x(), curPos_);
    }
    p = textLines_->getShiftByPos(curPos_.x(), curPos_.y(), curPos_);
    return p;
}

QPoint TextField::_handle_enter()
{
    if(isSelected())
        _erase_highlighted_text();
    Line line = (*textLines_)[getCurPosY()].getNewLine(curPos_.x());
    setCurrentPos(QPoint(0, curPos_.y() + 1));
    textLines_->insert(getCurPosY(), line);
    return QPoint((*textLines_)[curPos_.y()].getSymbShift(getCurPosX()),
            (*textLines_).getLineShift(getCurPosY(), getCurPosX()));
}

void TextField::_fill_highlightning_rect(QPainter &painter, const QPoint &begin, const QPoint &end)
{
    int  beginPos = selectionPos_.y() < curPos_.y() ?
                selectionPos_.y() :
                curPos_.y();
    int endPos = selectionPos_.y() > curPos_.y() ?
                selectionPos_.y() :
                curPos_.y();

    if(beginPos < endPos){
        int xBegin = begin.x() + edge_.x();
        int yBegin = begin.y() + edge_.y();
        int xEnd = (*textLines_)[beginPos].getWidth() + edge_.x() + 5;
        int yEnd = begin.y() + (*textLines_)[beginPos].height() + edge_.y();
        painter.fillRect(QRect(QPoint(xBegin, yBegin), QPoint(xEnd, yEnd)),
                         highlightningColor_);

        xBegin = edge_.x();
        int yPos = beginPos + 1;
        for(; yPos < endPos; ++yPos)
        {
            yBegin = yEnd;
            yEnd += (*textLines_)[yPos].height();
            if((*textLines_)[yPos].isEmpty())
            {
                painter.fillRect(QRect(QPoint(xBegin, yBegin), QPoint(10, yEnd)),
                                 highlightningColor_);
                continue;
            }
            xEnd = (*textLines_)[yPos].getWidth() + edge_.x() + 5;

            painter.fillRect(QRect(QPoint(xBegin, yBegin),
                                   QPoint(xEnd, yEnd)),
                             highlightningColor_);
        }

        yBegin = yEnd;
        xEnd = end.x() + edge_.x();
        yEnd += (*textLines_)[yPos].height();
        painter.fillRect(QRect(QPoint(xBegin, yBegin),
                           QPoint(xEnd, yEnd)),
                           highlightningColor_);
    }
    else{
        painter.fillRect(QRect(QPoint(begin.x() + edge_.x(), begin.y() + edge_.y()),
                               QPoint(end.x() + edge_.x(), end.y() + edge_.y() + (*textLines_)[beginPos].height())),
                         highlightningColor_);
    }
}

void TextField::_set_selection_begin(QPoint p)
{
    selectionBegin_.setX(p.x());
    selectionBegin_.setY(p.y());
}

void TextField::_set_selection_end(QPoint p)
{
    selectionEnd_.setX(p.x());
    selectionEnd_.setY(p.y());
}

void TextField::_set_selection_pos(QPoint p)
{
    selectionPos_.setX(p.x());
    selectionPos_.setY(p.y());
}

void TextField::_erase_highlighted_text()
{
    textLines_->deleteText(minPoint(curPos_, selectionPos_),
                          maxPoint(curPos_, selectionPos_));
    _reset_selection();
    setSelected(false);
}

const QPoint& TextField::minPoint(const QPoint &p1, const QPoint &p2) const
{
    if(p1.y() < p2.y() )
        return p1;
    else if(p2.y() < p1.y())
        return p2;
     return p1.x() <= p2.x() ? p1 : p2;
}

const QPoint& TextField::maxPoint(const QPoint &p1, const QPoint &p2) const
{
    if(p1.y() > p2.y() )
        return p1;
    else if(p2.y() > p1.y())
        return p2;
     return p1.x() >= p2.x() ? p1 : p2;
}

void TextField::_reset_selection()
{
    setCurrentPos(minPoint(curPos_, selectionPos_));
    selectionPos_ = curPos_;
    selectionBegin_ = minPoint(selectionBegin_, selectionEnd_);
    selectionEnd_ = selectionBegin_;
}

void TextField::_set_cursor_points(QPoint p)
{
    _change_cursor(p);

    QPoint p_roof = QPoint(p.x(), (*textLines_).getLineRoof(getCurPosY()));
    _set_selection_pos(curPos_);
    _set_selection_begin(p_roof);
    _set_selection_end(p_roof);
    if(!(*textLines_)[getCurPosY()].isEmpty()){
        setFont(QFont((*textLines_)[getCurPosY()][curPos_.x() ? curPos_.x() - 1 : 0].font()));
        emit fontChanged(font());
    }
}

void TextField::_change_cursor(QPoint p)
{
    int y = getCurPosY();
    int x = getCurPosX();
    cursor_->setCursor(p, (*textLines_)[y].isEmpty() ?
                           (*textLines_)[y].height() :
                           (*textLines_)[y][x].height());
}
