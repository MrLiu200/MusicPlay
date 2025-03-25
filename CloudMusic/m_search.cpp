#include "m_search.h"
#include <QHeaderView>
#include <QDebug>
M_Search::M_Search(QWidget *parent) : QWidget(parent)
{
    Init();
}

void M_Search::Init()
{
    //ui
    //上面一部分
    QPushButton *playall = new QPushButton;
    playall->setText("播放全部");
    playall->setSizePolicy(QSizePolicy::Policy::Minimum,QSizePolicy::Policy::Fixed);
    connect(playall,&QPushButton::clicked,this,&M_Search::PlayAllMusic);
    QPushButton *collectall = new QPushButton;
    collectall->setText("收藏全部");
    collectall->setSizePolicy(QSizePolicy::Policy::Minimum,QSizePolicy::Policy::Fixed);
    QSpacerItem *space = new QSpacerItem(100,20,QSizePolicy::Expanding, QSizePolicy::Minimum);
    QHBoxLayout *Hlayout = new QHBoxLayout;
    Hlayout->setSpacing(5);
    Hlayout->setContentsMargins(0,0,0,0);
    Hlayout->addWidget(playall);
    Hlayout->addWidget(collectall);
    Hlayout->addSpacerItem(space);

    QWidget *widgettop = new QWidget;
    widgettop->setLayout(Hlayout);
    widgettop->setSizePolicy(QSizePolicy::Policy::Preferred,QSizePolicy::Policy::Preferred);

    //下面就是一个列表视图
    tablewidget = new QTableWidget(0,4);
    QStringList HLabel;
    HLabel << "歌曲" << "歌手" << "专辑" << "时长";
    tablewidget->setHorizontalHeaderLabels(HLabel);
    // 设置各列的宽度
    tablewidget->setColumnWidth(0, 100); // 第一列宽度100
    tablewidget->setColumnWidth(1, 100);  // 第二列宽度100
    tablewidget->setColumnWidth(2, 100);  // 第三列宽度100
    tablewidget->setColumnWidth(3, 80);  // 第三列宽度80
    tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    tablewidget->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中
    // 获取表头视图并设置不可选择
    QHeaderView *horizontalHeader = tablewidget->horizontalHeader();
    horizontalHeader->setSectionsMovable(false);
    horizontalHeader->setSectionsClickable(false);

    QHeaderView *verticalHeader = tablewidget->verticalHeader();
    verticalHeader->setSectionsMovable(false);
    verticalHeader->setSectionsClickable(false);
    // 设置列的调整模式，使其填满表格宽度
    QHeaderView* header = tablewidget->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Stretch);//自动调整
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setSectionResizeMode(3, QHeaderView::Interactive);//固定大小
    QVBoxLayout *Vlayout = new QVBoxLayout;
    Vlayout->addWidget(widgettop);
    Vlayout->addWidget(tablewidget);
    this->setLayout(Vlayout);

    //variable
    MusicListId.clear();


    //signal
    connect(tablewidget,&QTableWidget::doubleClicked,this,&M_Search::doubleclick);
}

void M_Search::AddData(QStringList list)
{
    if(list.isEmpty()) return;
    MusicListId.clear();
    int count = list.size();
    tablewidget->setRowCount(count);
    //将数据拆分下来
    for(int i=0;i<list.size();i++){
        QStringList strlist = list.at(i).split("-");
        tablewidget->setItem(i,0,new QTableWidgetItem(strlist.at(0)));
        tablewidget->setItem(i,1,new QTableWidgetItem(strlist.at(1)));
        tablewidget->setItem(i,2,new QTableWidgetItem(strlist.at(2)));
        //时长需要处理
        int length = strlist.at(3).toInt()/1000;
        int min = length / 60;
        int sec = length % 60;
        //判断是否小于0，如果小于0还要补0
        QString len = QString("%1:%2").arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));
        tablewidget->setItem(i,3,new QTableWidgetItem(len));
        MusicListId.append(strlist.at(4));
    }
}

void M_Search::doubleclick(const QModelIndex &index)
{
    int count = MusicListId.size();
    if(index.row() > count) return;
//    qDebug()<<"点击了第 " << index.row() << "行！";
//    QString song = tablewidget->item(index.row(),0)->text();
//    QString singer = tablewidget->item(index.row(),1)->text();
//    QString album = tablewidget->item(index.row(),2)->text();
//    QString duration = tablewidget->item(index.row(),3)->text();
    Q_EMIT PlayCurrentMusic(index.row());
}
