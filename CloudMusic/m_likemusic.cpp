#include "m_likemusic.h"
#include <QHeaderView>
M_LikeMusic::M_LikeMusic(QWidget *parent) : QWidget(parent)
{
    InitWidget();
    UpdatelikeList();
}

void M_LikeMusic::InitWidget()
{
    tablewidget = new QTableWidget;
    tablewidget->setColumnCount(5);
    tablewidget->setRowCount(0);
    //设置列标签
    QStringList lables;
    lables << "" << "歌曲" << "歌手" << "专辑" << "时长";
    tablewidget->setHorizontalHeaderLabels(lables);
    //设置列宽度
    tablewidget->setColumnWidth(0,30);
    tablewidget->setColumnWidth(1,100);
    tablewidget->setColumnWidth(2,100);
    tablewidget->setColumnWidth(3,100);
    tablewidget->setColumnWidth(4,50);
    // 设置列的调整模式，喜欢和时长为固定大小，其余的随界面自动填充
    QHeaderView* header = tablewidget->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Interactive);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setSectionResizeMode(3, QHeaderView::Stretch);
    header->setSectionResizeMode(4, QHeaderView::Interactive);

    // 获取表头视图并设置不可选择
    QHeaderView *horizontalHeader = tablewidget->horizontalHeader();
    horizontalHeader->setSectionsMovable(false);
    horizontalHeader->setSectionsClickable(false);
    QHeaderView *verticalHeader = tablewidget->verticalHeader();
    verticalHeader->setSectionsMovable(false);
    verticalHeader->setSectionsClickable(false);

    //禁止编辑
    tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //整行选中
    tablewidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    QGridLayout *gridlayout = new QGridLayout;
    gridlayout->setContentsMargins(0,0,0,0);
    gridlayout->addWidget(tablewidget);

    this->setLayout(gridlayout);

    connect(tablewidget,&QTableWidget::cellClicked,this,&M_LikeMusic::removelikeList);
    connect(tablewidget,&QTableWidget::cellDoubleClicked,this,&M_LikeMusic::DoubleClicked);
}

void M_LikeMusic::removelikeList(int row, int column)
{
    if(column != 0) return;

    //获取歌曲信息
    QString song = tablewidget->item(row,1)->text();
    QString singer = tablewidget->item(row,2)->text();
    QString album = tablewidget->item(row,3)->text();

    QString sql = QString("delete from likeList where song = '%1' and singer = '%2' and album = '%3';")
            .arg(song).arg(singer).arg(album);
    QSqlQuery query;
    query.exec(sql);

    tablewidget->removeRow(row);
    QMessageBox::information(this,"提示",QString("%1 已取消喜欢").arg(song));
//    UpdatelikeList();
}

void M_LikeMusic::DoubleClicked(int row, int column)
{
    if(column == 0) return;
    QString str = songPath.at(row);
    PlayChange(str);
}

void M_LikeMusic::UpdatelikeList()
{
    //清空tablewidget
    tablewidget->clearContents();
    songPath.clear();
    //加载数据库
    QStringList songs,singers,album,duration;
    QString sql = "select * from likeList";
    QSqlQuery query;
    query.exec(sql);
    while(query.next()){
        songPath.append(query.value(0).toString());
        songs.append(query.value(1).toString());
        singers.append(query.value(2).toString());
        album.append(query.value(3).toString());
        duration.append(query.value(4).toString());
    }

    //添加至窗口
    tablewidget->setRowCount(songs.size());
    QTableWidgetItem *item = new QTableWidgetItem;
    item->setIcon(QIcon(":/Image/Image/like.ico"));
    item->setToolTip("取消喜欢");
    for(int i=0;i<songs.size();i++){
        tablewidget->setItem(i,0,new QTableWidgetItem(QIcon(":/Image/Image/like.ico"),NULL));
        tablewidget->setItem(i,1,new QTableWidgetItem(songs.at(i)));
        tablewidget->setItem(i,2,new QTableWidgetItem(singers.at(i)));
        tablewidget->setItem(i,3,new QTableWidgetItem(album.at(i)));
        tablewidget->setItem(i,4,new QTableWidgetItem(duration.at(i)));
    }

}

void M_LikeMusic::AddlikeList(QString path, QString song, QString singer, QString album, QString duration)
{
    QSqlQuery query;
    QString sql = QString("insert or ignore into likeList values('%1','%2','%3','%4','%5');")
            .arg(path).arg(song).arg(singer).arg(album).arg(duration);
    query.exec(sql);
    UpdatelikeList();
}

void M_LikeMusic::removelike(QString path, QString song, QString singer, QString album)
{
    QString sql = QString("delete from likeList where path = '%1' and song = '%2' and singer = '%3' and album = '%4';")
            .arg(path).arg(song).arg(singer).arg(album);
    QSqlQuery query;
    query.exec(sql);

    QMessageBox::information(this,"提示",QString("%1 已取消喜欢").arg(song));
    UpdatelikeList();
}
