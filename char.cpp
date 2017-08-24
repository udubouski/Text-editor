#include "char.h"

Symbol::Symbol()
{
    font_ = QFont(QString("Monospace"), 14);
    font_.setBold(false);
    font_.setItalic(false);
}

Symbol::Symbol(QChar value)
{
    font_ = QFont(QString("Monospace"), 14);
    font_.setBold(false);
    font_.setItalic(false);
    value_ = QChar(value);
}

Symbol::Symbol(const QFont &font, QChar value)
{
    font_ = QFont(font);
    value_ = QChar(value);
}

Symbol::Symbol(const Symbol& symbol)
{
    font_ = symbol.font_;
    value_ = symbol.value_;
}

Symbol& Symbol::operator=(const Symbol& symbol)
{
    font_ = symbol.font_;
    value_ = symbol.value_;
    return *this;
}

Symbol::~Symbol()
{
}



Line::Line(QObject *parent)
    : QObject(parent)
{
    width_ = 0;
    height_ = 0;
}

Line::Line(int height, QObject *parent)
    : QObject(parent)
{
    width_ = 0;
    height_ = height;
}

Line::Line(const Line& line) :
    QObject(line.parent())
{
    content_ = line.content_;
    width_ = line.width_;
    height_ = line.height_;
}

Line& Line::operator=(const Line& line)
{
    setParent(line.parent());
    content_ = line.content_;
    width_ = line.width_;
    height_ = line.height_;
    return *this;
}

Line::~Line()
{
}

Symbol& Line::operator[](int pos)
{
    return content_[pos];
}


qint64 Line::getSymbShift(int s) const
{
    int width = 0;
    if(s > content_.size())
        s = content_.size();
    if(s > 0)
        for(int i = 0; i < s; ++i)
            width += content_[i].width();
    return width;
}

qint64 Line::getMaxHeight() const
{
    qint64 max = 0;
    if(isEmpty())
        return height_;
    foreach (const Symbol& symb, content_) {
        if(symb.height() > max)
            max = symb.height();
    }
    return max;
}

void Line::setHeight(qint64 height) {
    if(height > height_)
        height_ = height;
}

void Line::recountHeight() {
    int h = 0;
    foreach (Symbol s, content_) {
        if(s.height() > h)
        h = s.height();
    }
    height_ = h;
}

void Line::recountWidth() {
    int w = 0;
    foreach (Symbol s, content_) {
        w += s.width();
    }
    width_ = w;
}

Symbol Line::pop_front()
{
    Symbol symb = content_.first();
    content_.pop_front();
    width_ -= symb.width();
    reduce_height(symb.height());

    return symb;
}

Symbol Line::pop_back()
{
    Symbol symb = content_.last();
    content_.pop_back();
    width_ -= symb.width();
    reduce_height(symb.height());

    return symb;
}

void Line::push_front(const Symbol& symb)
{
    content_.push_front(symb);
    width_ += symb.width();
    raise_height(symb.height());
}

void Line::push_back(const Symbol& symb)
{
    content_.push_back(symb);
    width_ += symb.width();
    raise_height(symb.height());
}

void Line::insert(int pos, const Symbol& symb)
{
    content_.insert(content_.begin() + pos, symb);
    width_ += symb.width();
    raise_height(symb.height());
}

Symbol Line::erase(int pos)
{
    Symbol symb = content_[pos];
    width_ -= symb.width();
    int h = symb.height();
    content_.erase(content_.begin() + pos);
    reduce_height(h);
    return symb;
}

int Line::getSymbolBegin(int x, QPoint& pos) const
{
    qint64 shift = 0;
    int i = 0;
    if(x >= width_){
        shift = width_;
        i = size();
    }
    else if(x <= 0)
        shift = 0;
    else
    {
        while(shift < width_)
        {
            if(shift + content_.at(i).width() / 2 >= x)
                break;
            shift += content_.at(i++).width();
        }
    }
    pos.setX(i);
    return shift;

}

int Line::getDifference(int s) const
{
    if(isEmpty())
        return 0;
    s = s == content_.size() ? s - 1 : s;
    return content_[s].height() < height() ?
           (height() - content_[s].height()) * 0.8 :
           0;
}

Line Line::getNewLine(int pos)
{
    if(content_.isEmpty())
        return Line(height_, parent());
    int count = content_.size() - pos;
    qint64 height = content_[pos == content_.length() ? pos - 1 : pos].height();
    Line newLine = Line(height, parent());

    while(count--)
        newLine.push_back(erase(pos));

    return newLine;
}

void Line::draw(QPainter *painter, qint64 x, qint64 y) const
{
    foreach (const Symbol& symb, content_) {
        QFont f = symb.font();
        painter->setFont(symb.font());
        painter->drawText(x,
                  height() * 0.8 + y,
                  symb.value());
        x += symb.width();
    }
}

void Line::raise_height(int h)
{
    if(isEmpty())
        height_ = h;
    else if(h > height_)
        height_ = h;
}

void Line::reduce_height(int h)
{
    int heighest = isEmpty() ? height_ : 0;
    if(h == height_)
    {
        foreach(Symbol symb, content_)
        {
            if(symb.height() > heighest)
                heighest = symb.height();
        }
        if(heighest < height_)
        {
            height_ = heighest;
        }
    }
}



Text::Text(QObject *parent)
    : QObject(parent)
{
    height_ = 0;
}

Text::Text(int h, QObject *parent)
    : QObject(parent)
{
    Line line = Line(h, this);
    height_ = 0;
    insert(0, line);
}

Text::Text(const Text& text) :
    QObject(text.parent())
{
    setParent(text.parent());
    content_ = text.content_;
    height_ = text.height_;
}

Text::~Text()
{
}

Text& Text::operator=(const Text& text)
{
    setParent(text.parent());
    content_ = text.content_;
    height_ = text.height_;
    return *this;
}

Line& Text::operator[](int pos)
{
    return content_[pos];
}


qint64 Text::width() const
{
    int widthest = 0;
    foreach (const Line& l, content_) {
        if(l.getWidth() > widthest)
            widthest = l.getWidth();
    }
    return widthest;
}

void Text::recountHeight() {
    int h = 0;
    foreach (Line l, content_) {
        h += l.height();
    }
    height_ = h;
}

Line Text::erase(int pos)
{
    reduce_height(content_[pos].height());
    Line line = content_[pos];
    content_.erase(content_.begin() + pos);
    return line;
}

void Text::insert(int pos, const Line& line)
{
    content_.insert(content_.begin() + pos, line);
    raise_height(line.height());
}

void Text::insert(int posX, int posY, const Symbol &symb)
{
    int h = symb.height() - content_[posY].height();
    content_[posY].insert(posX, symb);
    if(h > 0)
        raise_height(h);
    return ;
}

Line& Text::pop_front()
{
    Line& line = content_.first();
    content_.pop_front();
    reduce_height(line.height());
    return line;
}

Line& Text::pop_back()
{
    Line& line = content_.last();
    content_.pop_back();
    reduce_height(line.height());
    return line;
}

void Text::push_front(const Line &line)
{
    content_.push_front(line);
    raise_height(line.height());
}

void Text::push_back(const Line &line)
{
    content_.push_back(line);
    raise_height(line.height());
}

Symbol Text::getSymbol(int i, int j)
{
    if(i < 0) i = 0;
    else if(i >= content_.length()) i = content_.length() - 1;
    if(content_[i].isEmpty())
        return Symbol();
    if(j < 0) j = 0;
    else if(j >= content_[i].length()) j = content_[i].length() - 1;

    return content_[i][j];
}

void Text::eraseSymbol(int l, int s, QPoint& pos)
{
    if(!s && l){
        int _p = content_[l - 1].size();
        for(int i = 0; i < content_[l].length(); ++i)
            content_[l - 1].push_back(content_[l][i]);
        reduce_height(content_[l].height());
        content_.erase(content_.begin() + l);
        pos = QPoint(_p, l - 1);
        }
    else if(s){
        int h = content_[l].height();
        content_[l].erase(s - 1);
        if(!content_[l].isEmpty()){
            reduce_height(h - content_[l].getMaxHeight());
        pos = QPoint(s - 1, l);
        }
    }
}

void Text::deleteText(const QPoint &begin, const QPoint &end)
{
    if(begin.y() < end.y())
    {
        int i = begin.y();
        content_[i].getNewLine(begin.x());
        for(i++; i < end.y(); ++i){
            erase(begin.y() + 1);
        }
        for(int j = 0; j < end.x(); ++j)
            content_[begin.y() + 1].erase(0);

        while(content_[begin.y() + 1].length())
            content_[begin.y()].push_back(content_[begin.y() + 1].erase(0));
        erase(begin.y() + 1);
    }
    else
    {
        for(int j = begin.x(); j < end.x(); ++j)
            content_[begin.y()].erase(begin.x());
    }
}

int Text::getLineShift(int l , int s) const
{
    int Y = getLineRoof(l);
    if(s <= 0)
        s = 0;
    else
        s = s >= content_[l].size() ? content_[l].size() - 1 : s;
    Y += l > 0 ? content_[l].getDifference(s) : content_[0].getDifference(s);
    return Y;
}

int Text::getLineRoof(int l) const
{
    int Y = 0;
    if(l >= content_.size())
        l = content_.size() - 1;
    if(l >= 0)
    {
        LineList::const_iterator it = content_.begin();
        for(; it != content_.begin() + l; ++ it)
            Y += it->height();
    }
    return Y;
}

QPoint Text::getShiftByCoord(QPoint point, QPoint& pos) const
{
    qint64 shiftY = 0;
    qint64 shiftX = 0;
    qint64 i = 0;
    if(point.y() >=  height_)
    {
        shiftX = content_.last().getWidth();
        shiftY = height_ - content_.last().height();
        pos.setX(content_[content_.size() - 1].size());
        pos.setY(content_.size() - 1);
    }
    else
    {
        if(point.y() <= 0)
            shiftY = 0;
        else
            while(shiftY < height_)
            {
                if(shiftY + content_.at(i).height() > point.y())
                    break;
                    shiftY += content_.at(i++).height();
            }
        shiftX = content_.at(i).getSymbolBegin(point.x(), pos);
        pos.setY(i);
    }
    shiftY += content_[pos.y()].getDifference(pos.x());
    return QPoint(shiftX, shiftY);
}

QPoint Text::getShiftByPos(int x, int y, QPoint& pos) const
{
    int X = 0;
    int Y = 0;

    if(y < 0)
        y = 0;
    else if(y >= content_.size())
        y = content_.size() - 1;
    if(x < 0){
        if(y > 0){
            x = content_[y - 1].size();
            X = content_[y - 1].getSymbShift(content_[y - 1].length());
            y = y - 1;
            Y = getLineShift(y, content_[y].length());
        }
        else{
            x = 0;
            Y += content_[y].getDifference(0);
        }
    }
    else if(x > content_[y].size()){
        if(y < content_.size() - 1){
            x = 0;
            X = content_[++y].getSymbShift(0);
            Y = getLineShift(y, x);
        }
        else{
            x--;
            X = content_[y].getWidth();
            Y = getLineShift(y, x);
        }
    }
    else{
        X = content_[y].getSymbShift(x);
        Y = getLineShift(y, x);
    }
    pos.setX(x);
    pos.setY(y);
    return QPoint(X, Y);
}

qint64 Text::draw(QPainter *painter, QPoint curPos, QPoint edge) const
{
    qint64 x = edge.x();
    qint64 y = edge.y();
    int widthest = 0;
    foreach(const Line& line, content_) {
        if(line.getWidth() > widthest)
            widthest = line.getWidth();
        line.draw(painter, x, y);
        x = edge.x();
        y += line.height();
    }
    return widthest;
}

void Text::copyPart(Text* res, QPoint beginPos, QPoint endPos)
{
    if(res != Q_NULLPTR)
        delete res;

    res = new Text(parent());
    Line line(this);
    if(beginPos.y() < endPos.y())
    {
        if(content_[beginPos.y()].isEmpty() && beginPos.y() < endPos.y())
            res->push_back(Line(content_[beginPos.y()].height(), this));
        else{
            for(int j = beginPos.x(); j < content_[beginPos.y()].size(); ++j)
                line.push_back(Symbol(content_[beginPos.y()][j]));
            res->push_back(line);
        }

        for(int i = beginPos.y() + 1; i < endPos.y(); ++i)
            res->push_back(Line(content_[i]));

        line = Line(this);
        for(int j = 0; j < endPos.x(); ++j)
            line.push_back(Symbol(content_[endPos.y()][j]));
        if(!line.isEmpty())
            res->push_back(line);
    }
    else if(beginPos.y() == endPos.y()){
        for(int j = beginPos.x(); j < endPos.x(); ++j)
            line.push_back(Symbol(content_[beginPos.y()][j]));
        res->push_back(line);
    }
}

void Text::cutPart(Text* res, QPoint beginPos, QPoint endPos)
{
    if(res != Q_NULLPTR)
        delete res;

    res = new Text(parent());
    Line line(this);
    if(beginPos.y() < endPos.y())
    {
        int secondPos = beginPos.y() + 1;
        res->push_back(content_[beginPos.y()].getNewLine(beginPos.x()));

        for(int i = secondPos; i < endPos.y(); ++i)
            res->push_back(erase(secondPos));

        line = Line(this);
        for(int j = 0; j < endPos.x(); ++j)
            line.push_back(content_[secondPos].erase(0));
        if(!line.isEmpty())
            res->push_back(line);

        for(int j = 0; j < content_[secondPos].size(); ++j)
            content_[beginPos.y()].push_back(content_[secondPos][j]);
        erase(secondPos);
    }
    else if(beginPos.y() == endPos.y()){
        for(int j = beginPos.x(); j < endPos.x(); ++j)
            line.push_back(content_[beginPos.y()].erase(beginPos.x()));
        res->push_back(line);
    }
}

void Text::insertPart(Text* source, QPoint& pos)
{
    Line line = content_[pos.y()].getNewLine(pos.x());
    if((*source)[0].isEmpty() && (*source)[source->length() - 1].isEmpty()){
        insert(pos.y(), (*source)[0]);
        pos.setY(pos.y() + 1);
    }
    else
        for(int j = 0; j < (*source)[0].length(); ++j)
            content_[pos.y()].push_back(Symbol((*source)[0][j]));

    for(int j = 1; j < source->length(); ++j)
        insert(pos.y() + j, Line((*source)[j]));

    for(int j = 0; j < line.length(); ++j)
        content_[pos.y() + source->length() - 1].push_back(Symbol(line[j]));
    pos.setY(pos.y() + source->length() - 1);

    if(source->length() > 1)
        pos.setX((*source)[source->length() - 1].length());
    else
        pos.setX(pos.x() + (*source)[0].length());
}

void Text::raise_height(int h)
{
    height_ += h;
}

void Text::reduce_height(int h)
{
    height_ -= h;
}
