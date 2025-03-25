#ifndef M_SEARCH_H
#define M_SEARCH_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QSpacerItem>
#include <QLayout>
class M_Search : public QWidget
{
    Q_OBJECT
public:
    explicit M_Search(QWidget *parent = nullptr);
private:
    QTableWidget *tablewidget;
    QStringList MusicListId;

private:
    void Init();



private slots:
    void doubleclick(const QModelIndex &index);
public slots:
    void AddData(QStringList list);
signals:
    void PlayCurrentMusic(int index);
    void PlayAllMusic();
};

#endif // M_SEARCH_H
