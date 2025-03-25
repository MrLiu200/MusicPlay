#include "mymusic.h"
#include "ui_mymusic.h"
#include "m_dbhelper.h"
#include "m_search.h"
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QKeyEvent>
#include <iostream>
#include <QMediaMetaData>
#include <QTemporaryDir>
#include <taglib2.0.1/include/taglib/tag.h>
#include <taglib2.0.1/include/taglib/fileref.h>

MyMusic::MyMusic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MyMusic)
{
    ui->setupUi(this);
    Init();
}

MyMusic::~MyMusic()
{
    delete ui;
}

void MyMusic::keyPressEvent(QKeyEvent *event)
{
    if((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)){
        on_btnSearch_clicked();
    }else{
        QDialog::keyPressEvent(event);
    }
}

void MyMusic::Init()
{
    IsMute = false;
    SliderPosition = 0;
    currentPlayMode = new QAction;
    Player = new QMediaPlayer;
//    QObject::connect(Player, &QMediaPlayer::stateChanged, [=](QMediaPlayer::State state) {
//            if (state == QMediaPlayer::StoppedState && Player->error() != QMediaPlayer::NoError) {
//                qDebug() << "Error occurred:" << Player->error() << Player->errorString();
//                //如果播放出错，则跳过且播放下一曲
//                NextSong();
//            }
//        });

    mediaplaylist = new QMediaPlaylist;
    Player->setPlaylist(mediaplaylist);
//    currentValue = 20;
    manager = new QNetworkAccessManager;
//    connect(manager,QNetworkReply)
    connect(manager,SIGNAL(finished(QNetworkReply *)),this,SLOT(databack(QNetworkReply *)));
    InitDataBase();
    InitFrmTop();
    InitFrmMenu();
    InitFrmButtom();

//    m_localmusic
    M_LocalMusic *localmusic = new M_LocalMusic;
    connect(localmusic,&M_LocalMusic::updatePlayerMusic,this,&MyMusic::CurrentPlayChange);
    connect(localmusic,&M_LocalMusic::playlistchange,this,&MyMusic::PlayListChange);
    //likeMusic
    M_LikeMusic *likemusic = new M_LikeMusic;
    connect(likemusic,&M_LikeMusic::PlayChange,this,&MyMusic::CurrentPlayChange);
    connect(this,&MyMusic::addlike,likemusic,&M_LikeMusic::AddlikeList);
    connect(this,&MyMusic::removelike,likemusic,&M_LikeMusic::removelike);
    //searchmusic
    M_Search *searchmusic = new M_Search;
    connect(this,&MyMusic::AddSearchList,searchmusic,&M_Search::AddData);
    connect(searchmusic,&M_Search::PlayCurrentMusic,this,&MyMusic::SearchPlayChange);
    connect(searchmusic,&M_Search::PlayAllMusic,this,&MyMusic::SearchPlayAll);


    ui->stackedWidget->addWidget(searchmusic);
    ui->stackedWidget->addWidget(localmusic);
    ui->stackedWidget->addWidget(likemusic);
    QLabel* lable = new QLabel;
    ui->stackedWidget->addWidget(lable);
    ui->stackedWidget->setCurrentIndex(1);
//    connect(Player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(onMediaStatusChanged(QMediaPlayer::MediaStatus)));


    connect(mediaplaylist,&QMediaPlaylist::currentMediaChanged,this,&MyMusic::setmusicInfo);
    connect(Player,&QMediaPlayer::positionChanged,this,&MyMusic::updateposition);
    connect(ui->listWidget,&QListWidget::itemClicked,this,&MyMusic::PageChanged);

}

void MyMusic::InitFrmTop()
{
    ui->txtSearch->setPlaceholderText("在此搜索你想要的歌曲……");
    //    ui->txtSearch->setInputMask("000.000.000.000;0");
    ui->txtSearch->setClearButtonEnabled(true);
    ui->btnSearch->setToolTip("搜索");
    ui->btnSet->setToolTip("设置");
    ui->btnSkin->setToolTip("换肤");
    ui->btnHide->setToolTip("隐藏");
    ui->btnMax_min->setToolTip("最大化");
    ui->btnclose->setToolTip("退出");
    ui->btnSet->setFlat(true);
    QList<QPushButton*> list = ui->widgetTop->findChildren<QPushButton *>();
    foreach(QPushButton *btn,list){
        btn->setFlat(true);
    }
}

void MyMusic::InitFrmMain()
{

}

void MyMusic::InitFrmMenu()
{

    ui->btnTitle->setVisible(false);
    ui->labtitle->setFont(QFont("华文彩云",18));
    ui->labtitle->setText("简易播放器");
//https://music.163.com/song?id=1974443814&userid=446072620
//https://music.163.com/song?id=1929370102&userid=446072620


//    ui->btnTitle->setEnabled(false);
    QFont font("宋体",30);
    QColor TextColor = QColor(200,200,200);
    QList<QListWidgetItem> itemlist = QList<QListWidgetItem> ();
    QStringList namelist;
    namelist << "本地音乐" << "我喜欢的" << "我的收藏";
    QList<QIcon> iconlist;
    iconlist.append(QIcon(":/Image/Image/music_host.ico"));
    iconlist.append(QIcon(":/Image/Image/like.ico"));
    iconlist.append(QIcon(":/Image/Image/collect_enable.ico"));

    for(int i=0;i<namelist.size();i++){
        QListWidgetItem *Item = new QListWidgetItem;
        Item->setText(namelist.at(i));

        Item->setFont(font);
        Item->setForeground(QBrush(TextColor));
//        Item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        Item->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
        Item->setIcon(iconlist.at(i));
        ui->listWidget->addItem(Item);
    }
    ui->listWidget->setIconSize(QSize(32,32));
    ui->listWidget->setSpacing(3);
}

void MyMusic::InitFrmButtom()
{
    //设置鼠标悬停时显示的内容
    ui->tbtnPause->setToolTip("播放");
    ui->tbtnLike->setToolTip("喜欢");
    ui->tbtnList->setToolTip("当前列表");
    ui->tbtnnext->setToolTip("下一曲");
    ui->tbtnPrevious->setToolTip("上一曲");
    ui->tbtnMode->setToolTip("循环播放");
    ui->tbtncollect->setToolTip("收藏");
    ui->btnvolume->setToolTip("静音");
    ui->labalbum->setVisible(false);
    ui->MusicInfo_Icon->setVisible(false);
    ui->labcurrentPlay->setMinimumWidth(180);
    ui->labcurrentPlay->setMaximumWidth(180);
    QList<QToolButton *> toollist = ui->frmButtom->findChildren<QToolButton*>();
    foreach(QToolButton *tbtn, toollist){
        connect(tbtn,&QToolButton::clicked,this,&MyMusic::ButtomToolClick);
    }

    //添加播放模式
    QMenu *menu = new QMenu;
    QAction *random = new QAction(QIcon(":/Image/Image/random.ico"),"随机播放");
    QAction *loop = new QAction(QIcon(":/Image/Image/loop.ico"),"循环播放");
    QAction *loop_one = new QAction(QIcon(":/Image/Image/loop_one.ico"),"单曲循环");
    QAction *order_playback = new QAction(QIcon(":/Image/Image/order_playback.ico"),"顺序播放");
    random->setObjectName("random");
    loop->setObjectName("loop");
    loop_one->setObjectName("loop_one");
    order_playback->setObjectName("order_playback");
    menu->addAction(random);
    menu->addAction(loop);
    menu->addAction(loop_one);
    menu->addAction(order_playback);
    connect(menu,&QMenu::triggered,this,&MyMusic::PlayModeChange);
    ui->tbtnMode->setMenu(menu);
    ui->tbtnMode->setPopupMode(QToolButton::InstantPopup);
    currentPlayMode = order_playback;

    //音量控制
    ui->Slider_Volume->setMaximum(100);
    ui->Slider_Volume->setMinimum(0);
    ui->Slider_Volume->setValue(20);
    Player->setVolume(20);

    //进度条样式
    QString SliderStyle = "QSlider::groove:horizontal{"
                          "border: 0px solid #bbb;"
                          "}"
                          "QSlider:sub-page:horizontal{"
                          "background:rgb(117,250,97);"
                          "border-radius: 6px;"
                          "margin-top:8px;"
                          "margin-bottom:8px;"
                          "}"
                          "QSlider::add-page:horizontal{"
                          "background: rgb(200,200,200);"
                          "border: 0px solid #777;"
                          "border-radius: 6px;"
                          "margin-top: 9px;"
                          "margin-bottom: 9px;"
                          "}"
                          "QSlider::handle:horizontal{"
                          "background: rgb(200,200,200);"
                          "width:5px;"
                          "border:1px solid rgb(200,200,200);"
                          "border-radius: 2px;"
                          "margin-top: 6px;"
                          "margin-bottom: 6px;"
                          "}"
                          "QSlider::handle:horizontal:hover{"
                          "background: rgb(255,255,255);"
                          "width: 10px;"
                          "border: 1px solid rgb(255,255,255);"
                          "border-radius: 5px;"
                          "margin-top: 4px;"
//                          "margin-buttom: 4px;"
                          "}";

    ui->SliderCurrent->setStyleSheet(SliderStyle);

}

void MyMusic::InitDataBase()
{
   //查询文件是否存在
   QString Filename = qApp->applicationDirPath() + "/Music.db";
   QFile file(Filename);
   if(!file.exists()){
       M_DbHelper::CreateDataBaseFile("music.db",qApp->applicationDirPath());
   }

   //连接数据库
   if(M_DbHelper::ConnectDataBase("QSQLITE",Filename)){//连接成功
       if(!M_DbHelper::IsEmpty_Table("currentList")){//当前播放列表
           QStringList data;
           data.append("music_id integer");
           data.append("musicname text");
           data.append("singername text");
           data.append("album_id text");
           data.append("music_duration text");
           data.append("mv_id text");
           M_DbHelper::CreateNewTable("currentList",data);
       }else{
           //如果存在则查询所有信息，并将其放在界面上
       }
       if(!M_DbHelper::IsEmpty_Table("likeList")){//我的喜欢
           QStringList data;
           data.append("path text");
           data.append("song text");
           data.append("singer text");
           data.append("album text");
           data.append("duration text");
           M_DbHelper::CreateNewTable("likeList",data);
       }else{
           //填充界面
       }
       if(!M_DbHelper::IsEmpty_Table("collentList")){//我的收藏
           QStringList data;
           data.append("music_id text");
           data.append("song text");
           data.append("singer text");
           data.append("album text");
           data.append("duration text");
           M_DbHelper::CreateNewTable("collentList",data);
       }else{
           //填充界面

       }
       if(!M_DbHelper::IsEmpty_Table("LocalMusicDir")){//本地音乐目录
           QStringList data;
           data.append("musicdir text");
           data.append("isenable bool");
           bool ok = M_DbHelper::CreateNewTable("LocalMusicDir",data);
           if(ok){
               QStringList data;
               data.append("我的音乐");
               data.append(QString("%1").arg(true));
               M_DbHelper::AddDataToTable("LocalMusicDir",data);
               data.clear();
               data.append("下载");
               data.append(QString("%1").arg(true));
               M_DbHelper::AddDataToTable("LocalMusicDir",data);
           }
       }
   }

}

void MyMusic::NextSong()
{
    PlayStop();
    if (mediaplaylist->currentIndex() < mediaplaylist->mediaCount() - 1) {
        mediaplaylist->next();
        PlaySong();
    } else {
        mediaplaylist->setCurrentIndex(0);
        PlaySong();
        qDebug() << "No more tracks in playlist.";
    }
}

void MyMusic::PreviousSong()
{
    PlayStop();
    if (mediaplaylist->currentIndex() < mediaplaylist->mediaCount() - 1) {
        mediaplaylist->previous();
        PlaySong();
    } else {
        mediaplaylist->setCurrentIndex(mediaplaylist->mediaCount() - 1);
        PlaySong();
        qDebug() << "No more tracks in playlist.";
    }
}

void MyMusic::PlaySong()
{
    ui->tbtnPause->setIcon(QIcon(":/Image/Image/pause.ico"));
    ui->tbtnPause->setToolTip("暂停");
    Player->play();
}

void MyMusic::PlayStop()
{
    if(Player->state() == QMediaPlayer::PlayingState){
        Player->stop();
        ui->tbtnPause->setIcon(QIcon(":/Image/Image/play.ico"));
        ui->tbtnPause->setToolTip("播放");
    }
}

void MyMusic::GetCurrentMusicInfo(QString &song, QString &singer, QString &album, QString &Duration)
{
    QStringList list = ui->labcurrentPlay->text().split("--");
    if (list.size() >= 2) {
        song = list.at(0);
        singer = list.at(1);
    } else {
        song = "";
        singer = "";
    }
    Duration = ui->labtimelength->text();
    album = ui->labalbum->text();
}

void MyMusic::UpdateMusicInfo(QString title, QString singer, QString album,QString Duration,qint64 len,bool IsLike)
{
    ui->labtimecurrent->setText("00:00");
    ui->labtimelength->setText(Duration);
    ui->labalbum->setText(album);
    ui->labcurrentPlay->setText(title + "--" + singer);

    ui->SliderCurrent->setMaximum(len);
    ui->SliderCurrent->setMinimum(0);
    //该歌曲是否喜欢
    if(IsLike){
        ui->tbtnLike->setIcon(QIcon(":/Image/Image/like.ico"));
        ui->tbtnLike->setToolTip("取消喜欢");
    }else{
        ui->tbtnLike->setIcon(QIcon(":/Image/Image/unlike.ico"));
        ui->tbtnLike->setToolTip("喜欢");
    }
}

void MyMusic::PlayModeChange(QAction *action)
{
    QString name = action->objectName();
    QString iconstr = QString(":/Image/Image/%1.ico").arg(name);
    QString tiptext = action->text();
    QIcon icon(iconstr);
    if(!icon.isNull()){
        ui->tbtnMode->setIcon(QIcon(iconstr));
        ui->tbtnMode->setToolTip(tiptext);
        currentPlayMode = action;
    }
    //不同模式进行不同处理
    if(name == "random"){//随机播放
        mediaplaylist->setPlaybackMode(QMediaPlaylist::Random);
        qDebug()<<"随机播放";
    }else if(name == "loop"){//顺序循环
        mediaplaylist->setPlaybackMode(QMediaPlaylist::Loop);
        qDebug()<<"顺序循环";
    }else if(name == "loop_one"){//单曲循环
        mediaplaylist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        qDebug()<<"单曲循环";
    }else if(name == "order_playback"){//顺序播放
        mediaplaylist->setPlaybackMode(QMediaPlaylist::Sequential);
        qDebug()<<"顺序播放";
    }
}

void MyMusic::on_Slider_Volume_valueChanged(int value)
{
    Player->setVolume(value);
    if((value == 0) && (!IsMute)){
        ui->btnvolume->setIcon(QIcon(":/Image/Image/mute.ico"));
        IsMute = true;
    }
    if((IsMute)&&(value != 0)){
        ui->btnvolume->setIcon(QIcon(":/Image/Image/volume.ico"));
        IsMute = false;
    }
}

void MyMusic::on_btnvolume_clicked()
{
    static qint8 TestValue = 0;
    if(IsMute){
        ui->btnvolume->setIcon(QIcon(":/Image/Image/volume.ico"));
        ui->Slider_Volume->setValue(TestValue);
        ui->btnvolume->setToolTip("静音");
        IsMute = false;
        Player->setMuted(IsMute);//set mute, should test
    }else{
        ui->btnvolume->setIcon(QIcon(":/Image/Image/mute.ico"));
        TestValue = ui->Slider_Volume->value();
        ui->btnvolume->setToolTip("取消静音");
        ui->Slider_Volume->setValue(0);
        Player->setVolume(0);
        IsMute = true;
        Player->setMuted(IsMute);
    }
}

void MyMusic::on_btnSearch_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    QString text = ui->txtSearch->text();
    qDebug()<<"搜索内容: " << text;
    QNetworkRequest r;
    QString str = "http://music.163.com/api/search/get/web?csrf_token=hlpretag=&hlposttag=&s={"+text+"}&type=1&offset=0&total=true&limit=20";

//    QString str = "http://music.163.com/api/search/get/web?csrf_token=hlpretag=&hlposttag=&s={海阔天空}&type=1&offset=0&total=true&limit=20";
    r.setUrl(str);
    manager->get(r);

}

void MyMusic::databack(QNetworkReply *reply)
{
    //读取所有信息
     QByteArray searchInfo=reply->readAll();
    //错误信息
    QJsonParseError err;
    //将返回的文本信息转换为json文档对象
    QJsonDocument json_recv = QJsonDocument::fromJson(searchInfo,&err);

    //判断是否符合语法
    if(err.error != QJsonParseError::NoError){
        qDebug() <<"格式错误"<< err.errorString();
        return;
    }

    QJsonObject totalObject = json_recv.object();

    // 列出json里所有的key
    QStringList keys = totalObject.keys();
    //如果keys包含"result"
    if(keys.contains("result")){

        //就将带 result 的内容提取后转换为对象
        QJsonObject resultObject = totalObject["result"].toObject();

        //保存result所有key
        QStringList resultKeys = resultObject.keys();

        //如果key为songs
//        QString singerName;
//        int musicDuration;
//        QString albumName;
//        QStringList infolist;
        SearchMusicList.clear();
        if(resultKeys.contains("songs")){

            QJsonArray array = resultObject["songs"].toArray();

            //获取歌曲中的信息
            for(auto i : array){
                QJsonObject object = i.toObject();
                QString albumName;                                      // 专辑名
                QString singerName;                                     // 歌手名
                int musicId = object["id"].toInt();                     // 音乐id
                int musicDuration = object["duration"].toInt();         // 音乐长度
                QString musicName = object["name"].toString();          // 音乐名
                int mvId = object["mvid"].toInt();                      // mvid
                QStringList artistsKeys = object.keys();
                if(artistsKeys.contains("artists")){
                    QJsonArray artistsArray = object["artists"].toArray();
                    for(int j=0;j<artistsArray.size();j++){
                         QJsonObject object2 = artistsArray.at(j).toObject();
                         singerName += object2["name"].toString();
                         if(j != artistsArray.size()-1){
                             singerName += "、";
                         }
                    }
//                    for(auto j : artistsArray){
//                        QJsonObject object2 = j.toObject();
//                        singerName += object2["name"].toString();
//                    }
                }
                //包含专辑
                if(artistsKeys.contains("album")){
                    QJsonObject albumObjct = object["album"].toObject();
                    albumName = albumObjct["name"].toString();
                }

                //拼接数据然后触发信号展示
                QString str = musicName + "-" +singerName + "-" + albumName + "-" + QString::number(musicDuration) + "-" + QString::number(musicId);
                SearchMusicList.append(str);
                //音乐的url地址
                QString Musicurl = QString("https://music.163.com/song/media/outer/url?id=%1").arg(musicId);
                QStringList MusicList;
                QList<int>MusicDurationList;
                MusicList.append(Musicurl);
                MusicDurationList.append(musicDuration);

                QString st = QString::number(musicId)+
                        "  音乐名："+musicName+
                        "  歌手名："+singerName+
                        "  总时长："+QString::number(musicDuration)+
                        "  专辑名："+albumName;
                qDebug()<<st;
            }
            if(!SearchMusicList.isEmpty()){
                Q_EMIT AddSearchList(SearchMusicList);
            }
        }
    }
}

void MyMusic::ButtomToolClick()
{
    QToolButton *btn = (QToolButton *)sender();
    QString tooltip = btn->toolTip();
    if(tooltip == "暂停"){
        btn->setIcon(QIcon(":/Image/Image/play.ico"));
        btn->setToolTip("播放");
        if(Player->state() == QMediaPlayer::PlayingState){
            Player->pause();
        }
    }else if(tooltip == "播放"){
        btn->setIcon(QIcon(":/Image/Image/pause.ico"));
        btn->setToolTip("暂停");
        if(Player->state() == QMediaPlayer::PausedState){
            Player->play();
        }
    }else if(tooltip == "上一曲"){
        PlayStop();
        PreviousSong();
        //查找上一曲
    }else if(tooltip == "下一曲"){
        PlayStop();
        //查找下一曲
        NextSong();
    }else if(tooltip == "喜欢"){
        btn->setIcon(QIcon(":/Image/Image/like.ico"));
        btn->setToolTip("取消喜欢");
        //获取文件信息添加到喜欢列表
        QString path,song,singer,album,duration;
        GetCurrentMusicInfo(song,singer,album,duration);
        path = mediaplaylist->currentMedia().request().url().toString();
        Q_EMIT addlike(path,song,singer,album,duration);
    }else if(tooltip == "取消喜欢"){
        btn->setIcon(QIcon(":/Image/Image/unlike.ico"));
        btn->setToolTip("喜欢");
        //从喜欢列表中删除
        QString path,song,singer,album,duration;
        GetCurrentMusicInfo(song,singer,album,duration);
        path = mediaplaylist->currentMedia().request().url().toString();
        Q_EMIT removelike(path,song,singer,album);
    }else if(tooltip == "收藏"){
        btn->setIcon(QIcon(":/Image/Image/collect_enable.ico"));
        btn->setToolTip("取消收藏");
        //添加至收藏列表
    }else if(tooltip == "取消收藏"){
        btn->setIcon(QIcon(":/Image/Image/collect_disable.ico"));
        btn->setToolTip("收藏");
        //从收藏列表中删除
    }else if(tooltip == "当前播放列表"){
        //弹出当前播放列表
    }
}

void MyMusic::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia){
        QString ret;
        QStringList metadatalist = Player->availableMetaData();
        // Get the size of the list
        int list_size = metadatalist.size();

        //qDebug() << player->isMetaDataAvailable() << list_size;

        // Define variables to store metadata key and value
        QString metadata_key;
        QVariant var_data;

        for (int indx = 0; indx < list_size; indx++)
        {
            // Get the key from the list
            metadata_key = metadatalist.at(indx);

            // Get the value for the key
            var_data = Player->metaData(metadata_key);

            qDebug() << metadata_key << var_data.toString();
        }
    }
}

void MyMusic::CurrentPlayChange(QString filename)
{
    QString url;
    //判断数据来源，本地还是网络
    QRegularExpression re("id=(\\d+)");
    QRegularExpressionMatch match = re.match(filename);
    url = filename;
    if (match.hasMatch()){//如果来源与网络
        //从数据库中获取到名称、歌手、专辑、时长
        QString sql = QString("select * from likeList where path = '%1'").arg(filename);
        QSqlQuery query;
        query.exec(sql);
        if(query.next()){
            QString song = query.value(1).toString();
            QString singer = query.value(2).toString();
            QString album = query.value(3).toString();
            QString duration = query.value(4).toString();
            QString path = query.value(0).toString();
            int id = 0;
            id = match.captured(1).toInt();
            QString str = song + "-" +singer + "-" + album + "-" + duration + "-" + QString::number(id);
            SearchMusicList.append(str);
        }
    }
    PlayStop();
    int index = mediaplaylist->currentIndex();
    mediaplaylist->insertMedia(index+1,QUrl(url));
    mediaplaylist->setCurrentIndex(index+1);
    PlaySong();
}

void MyMusic::PlayListChange(QStringList list)
{
    if(Player->state() != QMediaPlayer::StoppedState){
        Player->stop();
    }
    mediaplaylist->clear();
    for(int i=0;i<list.size();i++){
        mediaplaylist->addMedia(QUrl(list.at(i)));
    }
    //找出当前播放模式，重新进行设置
    PlayModeChange(currentPlayMode);
    PlaySong();
}

void MyMusic::setmusicInfo(const QMediaContent &media)
{
    QString filename = media.request().url().toString();
    if(filename.isEmpty()) return;
    if(filename.contains("id=")){//如果是网络流
        for(int i=0;i<SearchMusicList.size();i++){
            QStringList temp = SearchMusicList.at(i).split("-");
            QString str = QString("https://music.163.com/song/media/outer/url?id=%1").arg(temp.at(4));
            if(str == filename){
                QString song = temp.at(0);
                QString singer = temp.at(1);
                QString album = temp.at(2);
                int length = temp.at(3).toInt()/1000;
                int min = length / 60;
                int sec = length % 60;
                //判断是否小于0，如果小于0还要补0
                QString len = QString("%1:%2").arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));

                //查询是否被喜欢和收藏
                bool Islike = false;
                QSqlQuery query;
                QString str = QString("select * from likeList where path = 'https://music.163.com/song/media/outer/url?id=%1'").arg(temp.at(4));
                query.exec(str);
                if(query.next()){
                    Islike = true;
                }
                UpdateMusicInfo(song,singer,album,len,length,Islike);
                break;
            }
        }
    }else{
        QTemporaryDir tempDir;
        if (!tempDir.isValid())
        {
            qWarning() << "Failed to create temporary directory";
            return;
        }

        // 复制文件到临时目录
        QString tempFilePath = tempDir.path() + QDir::separator() + QFileInfo(filename).fileName();
        if (!QFile::copy(filename, tempFilePath))
        {
            qWarning() << "Failed to copy file to temporary directory";
            return;
        }

        TagLib::FileRef file(tempFilePath.toStdWString().c_str());
        if (!file.isNull() && file.tag())
        {
            TagLib::Tag *tag = file.tag();
            TagLib::AudioProperties *properties = file.audioProperties();

            QString title = QString::fromStdWString(tag->title().toWString());
            QString artist = QString::fromStdWString(tag->artist().toWString());
            QString album = QString::fromStdWString(tag->album().toWString());
            int length = properties ? properties->length() : 0;
            QString Duration = QString("%1:%2").arg((length/60), 2, 10, QChar('0')).arg((length%60), 2, 10, QChar('0'));
            //查询是否被喜欢和收藏
            bool Islike = false;
            QSqlQuery query;
            QString str = QString("select * from likeList where path = '%1'").arg(filename);
            query.exec(str);
            if(query.next()){
                Islike = true;
            }
            UpdateMusicInfo(title,artist,album,Duration,length,Islike);
        }
    }

}

void MyMusic::updateposition(qint64 position)
{
    int seconds = (position / 1000) % 60;
    int minutes = (position / 1000) / 60;
    QString len = QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
    ui->labtimecurrent->setText(len);
    ui->SliderCurrent->setValue(position/1000);

}

void MyMusic::on_SliderCurrent_sliderMoved(int position)
{
    SliderPosition = position;
}

void MyMusic::on_SliderCurrent_sliderReleased()
{
    Player->setPosition(SliderPosition*1000);
}

void MyMusic::on_SliderCurrent_sliderPressed()
{
    SliderPosition = ui->SliderCurrent->value();
}

void MyMusic::PageChanged(QListWidgetItem *Item)
{
    QString text = Item->text();
    if(text == "本地音乐"){
        ui->stackedWidget->setCurrentIndex(1);
    }else if(text == "我喜欢的"){
        ui->stackedWidget->setCurrentIndex(2);
    }else if(text == "我的收藏"){

    }
}

void MyMusic::SearchPlayChange(int index)
{
    //获取list内容
    if(SearchMusicList.isEmpty()) return;
    QStringList list = SearchMusicList.at(index).split("-");
    QString id = list.at(4);
    QString song = list.at(0);
    QString singer = list.at(1);
    QString album = list.at(2);
    int length = list.at(3).toInt()/1000;
    int min = length / 60;
    int sec = length % 60;
    //判断是否小于0，如果小于0还要补0
    QString len = QString("%1:%2").arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));

    CurrentPlayChange(id);
    //查询是否被喜欢和收藏
    bool Islike = false;
    QSqlQuery query;
    QString str = QString("select * from likeList where path = 'https://music.163.com/song/media/outer/url?id=%1'").arg(id);
    query.exec(str);
    if(query.next()){
        Islike = true;
    }

    UpdateMusicInfo(song,singer,album,len,length,Islike);
}

void MyMusic::SearchPlayAll()
{
    if(SearchMusicList.isEmpty()) return;
    QStringList idList;
    //取出id
    for(int i=0;i<SearchMusicList.size();i++){
        QStringList temp = SearchMusicList.at(i).split("-");
        QString str = QString("https://music.163.com/song/media/outer/url?id=%1").arg(temp.at(4));
        idList.append(str);
    }
    if(!idList.isEmpty()){
        PlayListChange(idList);
    }
}
