#ifndef CHAR_H
#define CHAR_H

#include <QtWidgets>

class Symbol;
class Line;
class Text;

typedef QList<Symbol> SymbolList;
typedef QList<Line> LineList;

class Symbol
{
public:
    Symbol();
    Symbol(QChar value);
    explicit Symbol(QFont font);
    explicit Symbol(const QFont& font, QChar value);
    Symbol(const Symbol&);
    Symbol& operator=(const Symbol&);
    ~Symbol();

    inline bool bold() const { return font_.bold(); }
    inline void setBold(bool bold) { font_.setBold(bold); }

    inline const QFont& font() const { return font_; }
    inline void setFont(const QFont& font) { font_ = QFont(font); }

    inline bool italic() const { return font_.italic(); }
    inline void setItalic(bool italic) { font_.setItalic(italic); }

    inline qint64 height() const { return QFontMetrics(font_).height(); }
    inline qint64 width() const { return QFontMetrics(font_).width(value_); }

    inline QChar value() const { return value_; }
    inline void setValue(QChar value) { value_ = value; }

private:
    QFont font_;

    QChar value_;
};

class Line : public QObject
{
    Q_OBJECT

public:
    explicit Line(QObject *parent = Q_NULLPTR);
    explicit Line(int height, QObject *parent = Q_NULLPTR);
    Line(const Line&);
    Line& operator=(const Line&);
    ~Line();

    inline int height() const { return height_; }
    qint64 getMaxHeight() const;
    inline void setHeight(qint64 height);
    void recountHeight();

    inline int getWidth() const { return width_; }
    inline int length() const { return content_.length(); }
    inline int size() const { return content_.size(); }
    void recountWidth();

    inline bool isEmpty() const { return !content_.size(); }

    Symbol pop_front();
    Symbol pop_back();
    void push_front(const Symbol&);
    void push_back(const Symbol&);
    void insert(int pos, const Symbol&);
    Symbol erase(int pos);

    qint64 getSymbShift(int s) const;
    int getSymbolBegin(int x, QPoint &pos) const;
    inline int getDifference(int s) const;
    Line getNewLine(int pos);
    void draw(QPainter *painter, qint64 x, qint64 y) const;

    Symbol& operator[](int);
    inline const Symbol& at(int pos) const { return content_[pos]; }

private:
    void raise_height(int);
    void reduce_height(int);

    SymbolList content_;

    qint64 width_;
    qint64 height_;
    qint64 maxHeight_;
};


class Text: public QObject
{
    Q_OBJECT

public:
    explicit Text(QObject *parent = Q_NULLPTR);
    explicit Text(int h, QObject *parent = Q_NULLPTR);
    Text(const Text&);
    ~Text();

    Text& operator=(const Text&);
    Line& operator[](int);

    inline const Line& at(int pos) const{ return content_[pos]; }

    inline qint64 height() const { return height_; }
    qint64 width() const;
    void recountHeight();
    inline int length() const { return content_.length(); }

    int getLineShift(int l, int s) const;
    int getLineRoof(int l) const;

    Line erase(int pos);
    void insert(int pos, const Line &);
    void insert(int posX, int posY, const Symbol &);
    Line &pop_front();
    Line &pop_back();
    void push_front( const Line &);
    void push_back( const Line &);

    Symbol getSymbol(int i, int j);
    void eraseSymbol(int x, int y, QPoint &pos);
    void deleteText(const QPoint& begin,const QPoint& end);

    QPoint getShiftByCoord(QPoint p, QPoint &pos) const;
    QPoint getShiftByPos(int x, int y, QPoint &pos) const;
    qint64 draw(QPainter *painter, QPoint curPos, QPoint edge) const;

    void copyPart(Text* res, QPoint beginPos, QPoint endPos);
    void cutPart(Text* res, QPoint beginPos, QPoint endPos);
    void insertPart(Text* source, QPoint &pos);

    template <class T>
    using qFontF = void (QFont::*) (T);

    template <class Argument>
    void fontF(qFontF<Argument> func, QPoint begin, QPoint end, Argument arg)
    {
        if(begin.y() < end.y())
        {
            for(int j = begin.x(); j < content_[begin.y()].size(); ++j)
            {
                QFont f = content_[begin.y()][j].font();
                (f.*func)(arg);
                content_[begin.y()][j].setFont(f);
            }

            for(int i = begin.y() + 1; i < end.y(); ++i)
                for(int j = 0; j < content_[i].length(); ++j)
                {
                    QFont f = content_[i][j].font();
                    (f.*func)(arg);
                    content_[i][j].setFont(f);
                }
            for(int j = 0; j < end.x(); ++j)
            {
                QFont f = content_[end.y()][j].font();
                (f.*func)(arg);
                content_[end.y()][j].setFont(f);
            }
            for(int i = begin.y(); i <= end.y(); ++i){
                content_[i].recountHeight();
                content_[i].recountWidth();
            }
        }
        else{
            for(int j = begin.x(); j < end.x(); ++j)
            {
                QFont f = content_[begin.y()][j].font();
                (f.*func)(arg);
                content_[begin.y()][j].setFont(f);
            }
            content_[begin.y()].recountHeight();
            content_[begin.y()].recountWidth();
        }
        recountHeight();
    }

private:
    void raise_height(int);
    void reduce_height(int);

    LineList content_;
    qint64 height_;
};

#endif
