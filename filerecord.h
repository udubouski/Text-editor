#ifndef FILERECORDE_H
#define FILERECORDE_H

#include <QtWidgets>

#include "char.h"

class FileOpenException: public QException
{
public:
    void raise() const { throw *this; }
    FileOpenException *clone() const { return new FileOpenException(*this); }
};

struct Record
{
    QByteArray data;
    QByteArray dataChanged;
    qint64 absPos;
};


class FileRecord: public QObject
{
    Q_OBJECT

public:
    FileRecord(QObject *parent = Q_NULLPTR);

    Text *read(QString file, QFont defFont);
    bool write(const Text *text, QString file);

private:
    void add_font_attrs(const QFont& font);
    void add_similar_font_text(QXmlStreamAttributes, QString text, Line&);


    QXmlStreamReader reader;
    QXmlStreamWriter writer;

};

#endif
