#include "QtNodePadWidget.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	//设置字体相关
	QFont f(a.font());
	f.setFamily("微软雅黑");
	a.setFont(f);

	a.setApplicationName("QtNotePad");
	a.setApplicationVersion("v0.1");
	a.setApplicationDisplayName("记事本");

    QtNodePad w;
    w.show();
    return a.exec();
}
