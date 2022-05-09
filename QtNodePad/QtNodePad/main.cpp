#include "QtNodePadWidget.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtNodePad w;
    w.show();
    return a.exec();
}
