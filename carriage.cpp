#include "carriage.h"

Cursor::Cursor(QWidget *holder, QObject *parent)
    : QObject(parent)
{
    rect_.setRect(0, 0, 2, 14);
    blink_ = true;
    holder_ = holder;

    connect(&timer_, SIGNAL( timeout() ), SLOT( updateCursor() ) );

    timer_.setInterval(500);
    timer_.start();
}

Cursor::~Cursor()
{
}

void Cursor::setCursor(QPoint pos, int h)
{
    blink_ = false;
    holder_->update();

    setWidth(h);
    setHeigth(h);

    rect_.setRect(pos.x() + edge_.x(), pos.y() + edge_.y(), rect_.width(), rect_.height());

    blink_ = true;
    holder_->update();
}

QPoint Cursor::cursorPosition() const
{
    return QPoint(rect_.x(), rect_.y());
}

void Cursor::setWidth(int CharHeight)
{
    rect_.setWidth(static_cast<int>(log(CharHeight ) / log(16)) + 1);
}

void Cursor::draw(QPainter *painter, bool highlighted)
{
    if(blink_)
        painter->fillRect(rect_, colorCursor_);
    else
        painter->fillRect(rect_, highlighted ? colorHighlighted_ : colorBase_);
}

void Cursor::updateCursor()
{
    blink_ = !blink_;
    holder_->update();
}
