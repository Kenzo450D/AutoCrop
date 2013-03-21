#ifndef AUTOCROP_H
#define AUTOCROP_H

//Qt includes

#include <QObject>
#include <QString>
#include <QRect>

class AutoCrop : public QObject
{
    Q_OBJECT

public:
    explicit AutoCrop(QObject* const parent);
    ~AutoCrop();

    void setImagePath(const QString& path);

    QRect autoOuterCrop();

    QRect autoInnerCrop();

    QString output() const;

    void ShowOutput(QRect);

Q_SIGNALS:
    void signalProgress(int);

private:

    void postProgress(int, const QString&);

private:

    class Private;
    Private* const d;

};

#endif /* AUTOCROP_H */
