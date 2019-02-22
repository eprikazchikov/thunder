#ifndef BUILDER_H
#define BUILDER_H

#include <QDirIterator>
#include <QDebug>

class Builder : public QObject {
    Q_OBJECT
public:
    Builder         ();

signals:
    void            packDone            ();
    void            moveDone            (const QString &target);

public slots:
    void            package             (const QString &target);
    void            onImportFinished    ();
};

#endif // BUILDER_H
