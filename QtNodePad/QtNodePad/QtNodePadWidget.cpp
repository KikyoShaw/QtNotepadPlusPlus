#include "QtNodePadWidget.h"
#include <QFileIconProvider>

QtNodePad::QtNodePad(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	// 使用window系统本身笔记本icon
	QFileIconProvider ip;
	QIcon icon = ip.icon(QFileInfo("C:\\Windows\\System32\\notepad.exe"));
	qApp->setWindowIcon(icon);
}

QtNodePad::~QtNodePad()
{

}
