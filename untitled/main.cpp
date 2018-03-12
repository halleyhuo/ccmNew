#include "mainwindow.h"
#include <QApplication>
#include "HmiXtp.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    CHmiXtpIf* XTPIf = new CHmiXtpIf();
    CHmiXtpIf::Sigleton(XTPIf);
    CHmiXtpIf::receive();
    w.BindSigSlot(XTPIf);


    return a.exec();
}
