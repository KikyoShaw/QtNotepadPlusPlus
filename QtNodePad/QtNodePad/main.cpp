#include "QtNodePadWidget.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	//�����������
	QFont f(a.font());
	f.setFamily("΢���ź�");
	a.setFont(f);

	a.setApplicationName("QtNotePad");
	a.setApplicationVersion("v0.1");
	a.setApplicationDisplayName("���±�");

    QtNodePad w;
    w.show();
    return a.exec();
}
