#include "m_localmusic.h"
#include <QDebug>
#include <QTableView>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>
#include <QLineEdit>
#include <QAction>
#include <QMenu>
#include <QCheckBox>
#include <QLabel>
#include <QDir>
#include <QSpacerItem>
#include <QTableWidget>
#include <QHeaderView>

M_DirCheck::M_DirCheck(QWidget *parent) : QWidget(parent)
{
    this->setWindowFlag(Qt::WindowCloseButtonHint,true);
    LoadDataBaseList();
    InitFrm();

}

void M_DirCheck::AddDir(QString dir, bool enable)
{
    QCheckBox *box = new QCheckBox(this);
    connect(box,&QCheckBox::clicked,this,&M_DirCheck::on_checkbox_clicked);
    box->setText(dir);
    box->setChecked(enable);
    dirlist.append(dir);
    ablelist.append(enable);
    int row = gridlayout->rowCount();

    gridlayout->addWidget(box,row,0);
    QStringList data;
    data.append(dir);
    data.append(QString("%1").arg(enable));
    M_DbHelper::AddDataToTable("LocalMusicDir",data);
}

qint8 M_DirCheck::GetDirCount()
{
    return dirlist.size();
}

QStringList M_DirCheck::GetEnablePath()
{
    QStringList ret;
    int ablecount = ablelist.size();
    for(int i=0;i<dirlist.size();i++){
        if(ablecount > i){
            if(ablelist.at(i)){
                ret.append(dirlist.at(i));
            }
        }
    }
    return ret;
}

void M_DirCheck::InitFrm()
{
    QWidget *btnwidget = new QWidget;
    QPushButton *btnAdd = new QPushButton;
    QPushButton *btnOk = new QPushButton;

    QLabel *label = new QLabel;
    QFont labfont("宋体",18);
    label->setText("请选择本地文件夹");
    label->setFont(labfont);
    label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    label->setSizePolicy(QSizePolicy::Policy::Minimum,QSizePolicy::Policy::Fixed);

    btnAdd->setText("添加文件夹");
    btnAdd->setFont(QFont("宋体",10));
    btnOk->setText("确认");
    btnOk->setFont(QFont("宋体",10));
    connect(btnOk,&QPushButton::clicked,this,&M_DirCheck::on_btnOk_clicked);
    connect(btnAdd,&QPushButton::clicked,this,&M_DirCheck::on_btnAdd_clicked);

    QHBoxLayout *Hlayout = new QHBoxLayout;
    Hlayout->addWidget(btnAdd);
    Hlayout->addWidget(btnOk);
    Hlayout->setContentsMargins(5,5,5,5);
    Hlayout->setSpacing(7);
    btnwidget->setLayout(Hlayout);
    btnwidget->setSizePolicy(QSizePolicy::Policy::Minimum,QSizePolicy::Policy::Fixed);


    gridlayout = new QGridLayout;

    for(int i=0;i<dirlist.size();i++){
        QCheckBox *box = new QCheckBox(this);
        box->setText(dirlist.at(i));
        box->setChecked(ablelist.at(i));
        connect(box,&QCheckBox::clicked,this,&M_DirCheck::on_checkbox_clicked);
        gridlayout->addWidget(box,i,0);
    }
    QWidget *dirwidget = new QWidget;
    dirwidget->setLayout(gridlayout);


    QVBoxLayout *Vlayout = new QVBoxLayout;
    Vlayout->addWidget(label);
    Vlayout->addWidget(dirwidget);
    Vlayout->addWidget(btnwidget);
    Vlayout->setContentsMargins(10,10,10,10);
    Vlayout->setSpacing(20);




    this->setLayout(Vlayout);



}

void M_DirCheck::LoadDataBaseList()
{
    this->dirlist.clear();
    this->ablelist.clear();
    QSqlQuery query;
    QString sql = "select * from LocalMusicDir";
    query.exec(sql);
    while(query.next()){
        this->dirlist.append(query.value(0).toString());
        this->ablelist.append(query.value(1).toBool());
    }
}

void M_DirCheck::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    qDebug() << "closeEvent";
    this->hide();
}

void M_DirCheck::on_btnAdd_clicked()
{
    qDebug()<<"btnAdd click";
#if 1
    QString dir = QFileDialog::getExistingDirectory();
    if(!dir.isEmpty()){
        AddDir(dir,true);
    }

#endif
}

void M_DirCheck::on_btnOk_clicked()
{
    Q_EMIT MusicListupdate();
    this->hide();
}

void M_DirCheck::on_checkbox_clicked(bool check)
{
    QCheckBox *box = (QCheckBox *)sender();
    QString text = box->text();
    //更新ablelist
    int index = dirlist.indexOf(text);
    if(ablelist.size() >= index){
        if(ablelist[index] != check){
            ablelist[index] = check;
            QSqlQuery query;
            QString str = QString("update LocalMusicDir set isenable = '%1' where musicdir = '%2'").arg(check).arg(text);
            query.exec(str);
        }

    }
}
M_LocalMusic::M_LocalMusic(QWidget *parent) : QWidget(parent)
{
    dircheck = new M_DirCheck;
    connect(dircheck,&M_DirCheck::MusicListupdate,this,&M_LocalMusic::LoadMusicList);
    FullPath.clear();
    InitFrm();
}

void M_LocalMusic::InitFrm()
{
    /*******************顶部导航栏 start ************************/
    QWidget *widgetTop = new QWidget(this);
//    widgetTop->setMaximumSize(QSize(600,150));
    QPushButton *btn_PlayAll = new QPushButton(widgetTop);
    QPushButton *btn_Refresh = new QPushButton(widgetTop);
    QFrame *frmSearch = new QFrame(widgetTop);
    QPushButton *btn_Search = new QPushButton(frmSearch);
    txt_Search = new QLineEdit(frmSearch);
    QPushButton *btn_CheckFile = new QPushButton(widgetTop);

    btn_PlayAll->setSizePolicy(QSizePolicy::Policy::Minimum,QSizePolicy::Policy::Fixed);
    btn_PlayAll->setText("全部播放");
    btn_PlayAll->setIcon(QIcon(":/Image/Image/play.ico"));
    btn_PlayAll->setIconSize(QSize(16,16));


    btn_Refresh->setIcon(QIcon(":/Image/Image/Refresh.ico"));
    btn_Refresh->setIconSize(QSize(16,16));
    btn_Refresh->setSizePolicy(QSizePolicy::Policy::Fixed,QSizePolicy::Policy::Fixed);
    connect(btn_Refresh,&QPushButton::clicked,this,&M_LocalMusic::LoadMusicList);
    btn_Search->setIcon(QIcon(":/Image/Image/search.ico"));
    btn_Search->setIconSize(QSize(16,16));
    btn_Search->setFlat(true);
    btn_Search->setSizePolicy(QSizePolicy::Policy::Minimum,QSizePolicy::Policy::Fixed);
    connect(btn_PlayAll,&QPushButton::clicked,this,&M_LocalMusic::on_btn_playAll_clicked);
    connect(btn_Search,&QPushButton::clicked,this,&M_LocalMusic::on_btn_Search_clicked);
    connect(txt_Search,&QLineEdit::textChanged,this,&M_LocalMusic::on_btn_Search_clicked);


    txt_Search->setPlaceholderText("搜 索 本 地 歌 曲 ……");
    txt_Search->setSizePolicy(QSizePolicy::Policy::Expanding,QSizePolicy::Policy::Fixed);

    QHBoxLayout *Hlayout = new QHBoxLayout;
    Hlayout->addWidget(txt_Search);
    Hlayout->addWidget(btn_Search);
    Hlayout->setSpacing(0);
    Hlayout->setContentsMargins(0,0,0,0);

    frmSearch->setFrameShape(QFrame::Box);
    frmSearch->setFrameShadow(QFrame::Raised);
    frmSearch->setLineWidth(1);
    frmSearch->setLayout(Hlayout);
    frmSearch->setSizePolicy(QSizePolicy::Policy::Preferred,QSizePolicy::Policy::Preferred);

    btn_CheckFile->setText("选择目录");
    connect(btn_CheckFile,&QPushButton::clicked,this,&M_LocalMusic::on_btn_CheckFile_clicked);

    QSpacerItem *spacer1 = new QSpacerItem(100,20);
    QSpacerItem *spacer2 = new QSpacerItem(100,20);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(5);
    layout->setContentsMargins(9,9,9,9);
    layout->addWidget(btn_PlayAll);
    layout->addWidget(btn_Refresh);
    layout->addItem(spacer1);
    layout->addWidget(frmSearch);
    layout->addItem(spacer2);
    layout->addWidget(btn_CheckFile);

    widgetTop->setLayout(layout);
    widgetTop->setSizePolicy(QSizePolicy::Policy::Minimum,QSizePolicy::Policy::Fixed);

    /*******************顶部导航栏 end ************************/

    /*******************底部内容栏 start ************************/
    //如果没有检索到内容，需要label提示，如果有，则需要表格显示
//    QLabel *labmain = new QLabel;
//    labmain->setText("Hello,Qt!");
//    labmain->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
//    labmain->setFont(QFont("宋体",30));
//    labmain->setSizePolicy(QSizePolicy::Policy::Preferred,QSizePolicy::Policy::Preferred);


    //表格
    tabwidget = new QTableWidget;
    QStringList Hlabels;
    Hlabels << "歌曲" << "歌手" << "专辑" << "时长";
    tabwidget->setColumnCount(4);
    tabwidget->setRowCount(0);
    tabwidget->setHorizontalHeaderLabels(Hlabels);//设置列标题栏

    // 设置各列的宽度
    tabwidget->setColumnWidth(0, 100); // 第一列宽度100
    tabwidget->setColumnWidth(1, 100);  // 第二列宽度100
    tabwidget->setColumnWidth(2, 100);  // 第三列宽度100
    tabwidget->setColumnWidth(3, 80);  // 第三列宽度80
    // 设置列的调整模式，使其填满表格宽度
    QHeaderView* header = tabwidget->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Stretch);//自动调整
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setSectionResizeMode(3, QHeaderView::Interactive);//固定大小

    // 获取表头视图并设置不可选择
    QHeaderView *horizontalHeader = tabwidget->horizontalHeader();
    horizontalHeader->setSectionsMovable(false);
    horizontalHeader->setSectionsClickable(false);

    QHeaderView *verticalHeader = tabwidget->verticalHeader();
    verticalHeader->setSectionsMovable(false);
    verticalHeader->setSectionsClickable(false);

    //一些其他设置
//    tabwidget->setShowGrid(false);//设置不显示网格
    tabwidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    tabwidget->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式

//    tabwidget->setStyleSheet("QTableWidget { border: 0px solid black; "
//                             "background-color:transparent;}"
//                             "QTableWidget::item { background-color: transparent; "
//                             "color: blue;}"
//                             "QHeaderView::section { background-color: transparent;}");
//    // 访问左上角区域并设置样式
//    setCornerWidgetStyle("background-color: transparent;");
    connect(tabwidget,&QTableWidget::cellDoubleClicked,this,&M_LocalMusic::DoubleClicked);

    //窗体
    QWidget *mainwidget = new QWidget;
    QGridLayout *buttomlayout = new QGridLayout;
//    buttomlayout->addWidget(labmain);
    buttomlayout->addWidget(tabwidget);

    mainwidget->setLayout(buttomlayout);

    /*******************底部内容栏 end ************************/

    QGridLayout *mainlayout = new QGridLayout;
    mainlayout->addWidget(widgetTop);
    mainlayout->addWidget(mainwidget);
    this->setLayout(mainlayout);

    //加载列表
    LoadMusicList();
}

void M_LocalMusic::LoadMusicList()
{
    //先从目录中获取到所有的mp3文件
    QStringList MusicInfo = QStringList();

    QStringList Paths = dircheck->GetEnablePath();
    int count = Paths.size();
    for(int i=0;i<count;i++){
        MusicInfo += GetAppointFile(Paths.at(i),"mp3");
    }
    TableLoad(MusicInfo);
}

QStringList M_LocalMusic::GetAppointFile(QString Path, QString type, QString condition)
{
    QStringList retlist;
    QDir dir(Path);
    FullPath.clear();
    //获取path下所有文件夹和文件
    dir.setFilter(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot); //文件夹/文件/不包含./和../

    //排序文件夹优先
    dir.setSorting(QDir::DirsFirst);

    //获取文件夹下所有文件(文件夹+文件)
    QFileInfoList list = dir.entryInfoList();

    if(list.size()==0)
    {
        return QStringList();
    }

    //遍历
    for(int i=0;i<list.size();i++)
    {
        QFileInfo fileInfo = list.at(i);
        if(fileInfo.isDir())
        {
            GetAppointFile(fileInfo.filePath(),type);
        }
        else {
            if(fileInfo.suffix()==type)
            {
                if(!condition.isEmpty()){//如果存在限定词，则需要判断
                    QString name = list.at(i).fileName();
                    if(name.contains(condition,Qt::CaseSensitive)){
//                        retlist.append(name);
                        FullPath.append(list.at(i).filePath());
                        retlist.append(GetMusicInfo(list.at(i).filePath()));
                    }
                }else{
//                    retlist.append(list.at(i).fileName());
                    FullPath.append(list.at(i).filePath());
                    retlist.append(GetMusicInfo(list.at(i).filePath()));
                }
            }
        }
    }
    return retlist;
}

void M_LocalMusic::setCornerWidgetStyle(QString style)
{
    QWidget *cornerWidget = tabwidget->findChild<QWidget *>("qt_scrollarea_cornerwidget");
    if (cornerWidget) {
        cornerWidget->setStyleSheet(style);
    }
}

void M_LocalMusic::TableLoad(QStringList list)
{
    tabwidget->clearContents();
    int count = list.size();
    tabwidget->setRowCount(count);
    for(int i=0;i<count;i++){
        QStringList Info = list.at(i).split("-");
        QString song = Info.at(0);
        QString singer = Info.at(1);
        QString album = Info.at(2);
        int length = Info.at(3).toInt();
        int min = length / 60;
        int sec = length % 60;
        //判断是否小于0，如果小于0还要补0
        QString len = QString("%1:%2").arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));
//        QString len = QString("%1:%2").arg(min).arg(sec);
        tabwidget->setItem(i,0,new QTableWidgetItem(song));
        tabwidget->setItem(i,1,new QTableWidgetItem(singer));
        tabwidget->setItem(i,2,new QTableWidgetItem(album));
        tabwidget->setItem(i,3,new QTableWidgetItem(len));
    }

    for (int row = 0; row < tabwidget->rowCount(); ++row) {
        for (int col = 0; col < tabwidget->columnCount(); ++col) {
            QTableWidgetItem *item = tabwidget->item(row, col);
            if (item) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }

}

QString M_LocalMusic::GetMusicInfo(QString filename)
{
    QString ret;
    TagLib::FileRef file(filename.toStdWString().c_str());
    if (!file.isNull() && file.tag())
    {
        TagLib::Tag *tag = file.tag();
        TagLib::AudioProperties *properties = file.audioProperties();

        QString title = QString::fromStdWString(tag->title().toWString());
        QString artist = QString::fromStdWString(tag->artist().toWString());
        QString album = QString::fromStdWString(tag->album().toWString());
        QString year = QString::number(tag->year());
        QString length = properties ? QString::number(properties->length()) : "Unknown";
#ifdef __DEBUG__
        qDebug() << "Title:" << title;
        qDebug() << "Artist:" << artist;
        qDebug() << "Album:" << album;
        qDebug() << "Year:" << year;
        qDebug() << "Duration:" << length << "seconds";
        qDebug() << "-----------------------------------";
#endif
        ret = title + "-" + artist + "-" + album + "-" + length;
    }
    return ret;
}


void M_LocalMusic::on_btn_CheckFile_clicked()
{
    if(dircheck->isHidden()){
        dircheck->show();
    }
}

void M_LocalMusic::DoubleClicked(int row, int column)
{
    QTableWidgetItem *item = tabwidget->item(row,0);
    QString filename = item->text();
    QString filepath;

    for(int i=0;i<FullPath.size();i++){
        if(FullPath.at(i).contains(filename,Qt::CaseSensitive)){
            filepath = FullPath.at(i);
            Q_EMIT updatePlayerMusic(filepath);
            break;
        }
    }
}

void M_LocalMusic::on_btn_Search_clicked()
{
    //获取搜索框的内容，去所有使能的文件夹内进行搜索
    QString Search_text = txt_Search->text();
    //先从目录中获取到所有的mp3文件
    QStringList MusicInfo = QStringList();

    QStringList Paths = dircheck->GetEnablePath();
    int count = Paths.size();
    for(int i=0;i<count;i++){
        MusicInfo += GetAppointFile(Paths.at(i),"mp3",Search_text);
    }
    TableLoad(MusicInfo);
//    setCornerWidgetStyle("background-color: transparent;");

}

void M_LocalMusic::on_btn_playAll_clicked()
{
    if(!FullPath.isEmpty()){
        Q_EMIT playlistchange(FullPath);
    }
}

