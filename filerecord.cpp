#include "filerecord.h"

FileRecord::FileRecord(QObject *parent):
    QObject(parent)
{
}

Text* FileRecord::read(QString file, QFont defFont)
{
    QFile inFile(file);

    if(!inFile.open(QIODevice::ReadOnly))
        throw FileOpenException();

    Text *text = new Text();

    if(file.endsWith(".xml"))
    {
        reader.setDevice(&inFile);
        reader.readNext();
        reader.readNext();
        reader.readNext();
        do{
            Line line = Line(reader.attributes()[0].value().toInt(), text);
            reader.readNext();

            do{
                QXmlStreamAttributes attrs = reader.attributes();
                reader.readNext();
                if(reader.isEndElement() && reader.name().toString() == "font"){
                    reader.readNext();
                    break;
                }
                QString str = reader.text().toString();
                add_similar_font_text(attrs, str, line);

                reader.readNext();
                reader.readNext();
                if(reader.isEndElement() && reader.name().toString() == "line")
                    break;
            } while(true);

            text->push_back(line);

            reader.readNext();
        } while(!(reader.isEndElement() && reader.name().toString() == "Text"));
    }
    else if(file.endsWith(".txt"))
    {
        int height = QFontMetrics(defFont).height();
        bool endsWithEmpty = true;
        QTextCodec* codec = QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForLocale(codec);

        while(!inFile.atEnd())
        {
            Line line = Line(height, text);
            QByteArray byteLine = inFile.readLine();
            QString t;
            if(byteLine.endsWith('\n')){
                byteLine.remove(byteLine.length() - 1, 1);
                endsWithEmpty = true;
            }
            else endsWithEmpty = false;
            t = codec->toUnicode(byteLine);
            foreach (const QChar& s, t) {

                line.push_back(Symbol(defFont, s));
            }
            text->push_back(line);
        }
        if(endsWithEmpty)
            text->push_back(Line(height, text));
    }
    inFile.close();
    return text;
}

bool FileRecord::write(const Text* text, QString file)
{
    QFile outFile(file);

    if(!outFile.open(QIODevice::WriteOnly))
        return false;

    if(file.endsWith(".xml"))
    {
        writer.setDevice(&outFile);
        writer.writeStartDocument();
        QFont curFont;
        QString simText;
        writer.writeStartElement(QString("Text"));
        for(int i = 0; i < text->length(); ++i)
        {
            writer.writeStartElement(QString("line"));
            writer.writeAttribute(QString("height"), QString(QString::number(text->at(i).height())));

            if(!text->at(i).isEmpty())
                curFont = text->at(i).at(0).font();

            writer.writeStartElement(QString("font"));
            add_font_attrs(curFont);

            for(int j = 0; j < text->at(i).length(); ++j)
            {
                if(curFont != text->at(i).at(j).font())
                {
                    writer.writeCharacters(simText);
                    writer.writeEndElement();
                    simText = QString();

                    curFont = text->at(i).at(j).font();
                    writer.writeStartElement(QString("font"));
                    add_font_attrs(curFont);
                }
                simText += text->at(i).at(j).value();
            }
            writer.writeCharacters(simText);
            simText = QString();
            writer.writeEndElement();
            writer.writeEndElement();
        }
        writer.writeEndElement();
        writer.writeEndDocument();
    }
    else if(file.endsWith(".txt"))
    {
        QTextStream outStream(&outFile);
        for(int i = 0; i < text->length() - 1; ++i)
        {
            for(int j = 0; j < text->at(i).length(); ++j)
                outStream << text->at(i).at(j).value();
            outStream << '\n';
        }
        if(!text->at(text->length() - 1).isEmpty())
            for(int j = 0; j < text->at(text->length() - 1).length(); ++j)
                outStream << text->at(text->length() - 1).at(j).value();
    }
    outFile.close();
    return true;
}

void FileRecord::add_font_attrs(const QFont& font)
{
    writer.writeAttribute(QString("family"), QString(font.family()));
    writer.writeAttribute(QString("size"), QString(QString::number(font.pointSize())));
    writer.writeAttribute(QString("bold"), QString( font.bold() ? "true" : "false" ));
    writer.writeAttribute(QString("italic"), QString( font.italic() ? "true" : "false" ));
}

void FileRecord::add_similar_font_text(QXmlStreamAttributes attributes, QString text, Line& line)
{
    QString family = attributes[0].value().toString();
    int size = attributes[1].value().toInt();
    bool bold = attributes[2].value() == "true" ? true : false;
    bool italic = attributes[3].value() == "true" ? true : false;

    QFont font = QFont(family, size, -1, italic);
    font.setBold(bold);

    foreach (QChar ch, text) {
        line.push_back(Symbol(font, ch));
    }
}
