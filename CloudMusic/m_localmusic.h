#ifndef M_LOCALMUSIC_H
#define M_LOCALMUSIC_H

#include <QWidget>
#include <QGridLayout>
#include <QCloseEvent>
#include <QTableWidget>
#include <QSqlQuery>
#include "m_dbhelper.h"
#include <taglib2.0.1/include/taglib/tag.h>
#include <taglib2.0.1/include/taglib/fileref.h>
class M_DirCheck : public QWidget
{
    Q_OBJECT
public:
    M_DirCheck(QWidget *parent = nullptr);
    void AddDir(QString dir,bool enable);
    qint8 GetDirCount(void);
    QStringList GetEnablePath(void);
    void SetMusicPath(QStringList list){
        this->dirlist.clear();
        this->dirlist = list;
    }
    void SetEnablelist(QList<bool> list){
        this->ablelist.clear();
        this->ablelist = list;
    }
private:
    void InitFrm(void);
    void LoadDataBaseList(void);
    QStringList dirlist;
    QList<bool> ablelist;
    QGridLayout *gridlayout;
    void closeEvent(QCloseEvent *event) override;
private slots:
    void on_btnAdd_clicked();
    void on_btnOk_clicked();
    void on_checkbox_clicked(bool check);
signals:
    void DirStateChange(QString dir,bool enable);
    void MusicListupdate();

};

class M_LocalMusic : public QWidget
{
    Q_OBJECT
public:
    explicit M_LocalMusic(QWidget *parent = nullptr);
    void InitFrm(void);
    void LoadMusicList(void);
private:
    QStringList DirList;
    M_DirCheck *dircheck;
    QTableWidget *tabwidget;
    QLineEdit *txt_Search;//搜索框
    QStringList FullPath;//当前内容的完整路径
    QStringList Artist;//作者
    QStringList Album;//专辑

private:
    QStringList GetAppointFile(QString Path, QString type, QString condition = QString());
    void setCornerWidgetStyle(QString style);//设置左上角区域样式
    void TableLoad(QStringList list);
public:
    QString GetMusicInfo(QString filename);

private slots:
    void on_btn_CheckFile_clicked();
    void DoubleClicked(int row, int column);
    void on_btn_Search_clicked();
    void on_btn_playAll_clicked();
signals:
    void updatePlayerMusic(QString filename);
    void playlistchange(QStringList list);
};

#endif // M_LOCALMUSIC_H
