#include "QtNodePadWidget.h"
#include <QFileIconProvider>

QtNodePad::QtNodePad(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	// ʹ��windowϵͳ����ʼǱ�icon
	QFileIconProvider ip;
	QIcon icon = ip.icon(QFileInfo("C:\\Windows\\System32\\notepad.exe"));
	qApp->setWindowIcon(icon);
}

QtNodePad::~QtNodePad()
{

}
