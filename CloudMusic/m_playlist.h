#ifndef M_PLAYLIST_H
#define M_PLAYLIST_H

#include <QObject>
#include <QWidget>
#include <QListWidget>
#include <QDebug>
#include <QSqlQuery>

class M_PlayList : public QWidget
{
    Q_OBJECT
public:
    explicit M_PlayList(QWidget *parent = nullptr);

private:
    QListWidget *listwidget;

signals:

};

#endif // M_PLAYLIST_H
