#ifndef M_LIKEMUSIC_H
#define M_LIKEMUSIC_H

#include <QObject>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QSqlQuery>
#include <QMessageBox>
#include <QGridLayout>

class M_LikeMusic : public QWidget
{
    Q_OBJECT
public:
    explicit M_LikeMusic(QWidget *parent = nullptr);


private:
    QTableWidget *tablewidget;
    QStringList songPath;

private:
    void InitWidget();

private slots:
    void removelikeList(int row,int column);
    void DoubleClicked(int row, int column);
    void UpdatelikeList();
public slots:
    void AddlikeList(QString path,QString song,QString singer,QString album,QString duration);
    void removelike(QString path, QString song, QString singer, QString album);
signals:
    void PlayChange(QString filename);
};

#endif // M_LIKEMUSIC_H
