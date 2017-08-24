#ifndef CARRIAGE_H
#define CARRIAGE_H

#include <QtWidgets>

class Cursor: QObject
{
    Q_OBJECT
public:
    explicit Cursor(QWidget *holder, QObject *parent = Q_NULLPTR);
    ~Cursor();

    void setCursor(QPoint position, int h);
    QPoint cursorPosition() const;

    inline QRect rectangle() const { return rect_; }
    inline void setRectangle(QRect rect) { rect_ = rect; }

    inline QWidget* holder() const { return holder_; }
    inline void setHolder(QWidget * holder) { holder_ = holder; }

    inline bool blink() const { return blink_; }
    inline void setBlink(bool blink) { blink_ = blink; }

    inline QColor colorCursor() const { return colorCursor_; }
    inline void setColorCursor(QColor color) { colorCursor_ = color; }

    inline QColor colorBase() const { return colorBase_; }
    inline void setColorBase(QColor color) { colorBase_ = color; }

    inline QColor colorHighlighted() const { return colorHighlighted_; }
    inline void setColorHighlighted(QColor color) { colorHighlighted_ = color; }

    inline QPoint getTopLeft() const { return rect_.topLeft(); }

    inline int x() const { return rect_.x(); }
    inline void setX(int x) { rect_.setX(x); }

    inline int y() const { return rect_.y(); }
    inline void setY(int y) { rect_.setY(y); }

    inline int width() const { return rect_.width(); }
    inline void setWidth(int CharHeight);

    inline int heigth() const { return rect_.height(); }
    inline void setHeigth(int heigth) { rect_.setHeight(heigth); }

    inline QPoint Edge() const { return edge_; }
    inline void setEdge(QPoint edge)
    {
        edge_ = edge;
        rect_.setX(rect_.x() + edge.x());
        rect_.setY(rect_.y() + edge.y());
    }

    void draw(QPainter *painter, bool highlighted);

signals:
    void currentAddressChanged(QPoint address);

public slots:
    void updateCursor();

private:
    QRect rect_;
    QWidget *holder_;

    QTimer timer_;

    QColor colorCursor_;
    QColor colorBase_;
    QColor colorHighlighted_;
    QPoint edge_;
    bool blink_;
};

#endif
