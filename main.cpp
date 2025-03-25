#include <QApplication>
//#include "mydialog.h"
#include "mymusic.h"

int main(int argc, char*argv[])
{
    QApplication app(argc,argv);
    MyMusic a;
    a.show();
    return app.exec();
}
