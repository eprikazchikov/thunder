#include "mapconverter.h"

#include <QFile>

#include <bson.h>

MapConverterSettings::MapConverterSettings() {
    setType(MetaType::type<Actor *>());
}

QString MapConverterSettings::typeName() const {
    return "Map";
}

uint8_t MapConverter::convertFile(IConverterSettings *settings) {
    QFile src(settings->source());
    if(src.open(QIODevice::ReadOnly)) {
        string data = src.readAll().toStdString();
        src.close();

        Variant actor = readJson(data, settings);

        QFile file(settings->absoluteDestination());
        if(file.open(QIODevice::WriteOnly)) {
            ByteArray data = Bson::save(actor);
            file.write((const char *)&data[0], data.size());
            file.close();
            return 0;
        }
    }
    return 1;
}

IConverterSettings *MapConverter::createSettings() const {
    return new MapConverterSettings();
}
