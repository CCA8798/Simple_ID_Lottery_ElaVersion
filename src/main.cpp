#include "mw_header.h"

#include <QApplication>
#include "ElaApplication.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    eApp->init();
    mainWindow w;
    w.show();
    return a.exec();
}
