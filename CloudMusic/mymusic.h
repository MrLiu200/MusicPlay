#ifndef MYMUSIC_H
#define MYMUSIC_H

#include <QDialog>
#include <QMediaPlayer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMediaPlaylist>
#include <QListWidgetItem>
#include "m_localmusic.h"
#include "m_likemusic.h"
namespace Ui {
class MyMusic;
}

class MyMusic : public QDialog
{
    Q_OBJECT

public:
    explicit MyMusic(QWidget *parent = nullptr);
    ~MyMusic();
protected:
    void keyPressEvent(QKeyEvent *event) override;
private:
    Ui::MyMusic *ui;
    bool IsPlay;//是否已经播放
    bool IsMute;//是否已经静音
//    qint8 currentValue;
    QAction *currentPlayMode;
    QMediaPlayer *Player;
    QNetworkAccessManager* manager;
    QList<int> PlayList;
    QMediaPlaylist *mediaplaylist;
    int playID;
    qint32 SliderPosition;
    QStringList SearchMusicList;
private:


private:
    //初始化
    void Init();
    void InitFrmTop();
    void InitFrmMain();
    void InitFrmMenu();
    void InitFrmButtom();
    void InitDataBase();
    //功能
    void NextSong();                    //下一曲
    void PreviousSong();                //上一曲
    void PlaySong();                    //开始播放
    void PlayStop();                    //停止播放
    void GetCurrentMusicInfo(QString &song, QString &singer, QString &album, QString &Duration);   //加载音乐信息
    void UpdateMusicInfo(QString title, QString singer, QString album, QString Duration, qint64 len, bool IsLike);//界面更新音乐信息



private slots:
    void PlayModeChange(QAction* action);                   //播放模式切换
    void on_Slider_Volume_valueChanged(int value);          //音量控制
    void on_btnvolume_clicked();                            //静音控制
    void on_btnSearch_clicked();                            //搜索歌曲
    void databack(QNetworkReply *reply);                    //网络数据回传
    void ButtomToolClick();
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void CurrentPlayChange(QString filename);
    void PlayListChange(QStringList list);
    void setmusicInfo(const QMediaContent& media);
    void updateposition(qint64 position);
    void on_SliderCurrent_sliderMoved(int position);
    void on_SliderCurrent_sliderReleased();
    void on_SliderCurrent_sliderPressed();
    void PageChanged(QListWidgetItem *Item);
    void SearchPlayChange(int index);
    void SearchPlayAll();
signals:
    void addlike(QString path,QString song, QString singer, QString album, QString duration);
    void removelike(QString path, QString song, QString singer, QString album);
    void AddSearchList(QStringList list);
};

#endif // MYMUSIC_H
