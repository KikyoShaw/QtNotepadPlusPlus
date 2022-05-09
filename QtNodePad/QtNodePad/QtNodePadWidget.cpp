#include "QtNodePadWidget.h"
#include <QFileIconProvider>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>

QtNodePad::QtNodePad(QWidget *parent)
    : QMainWindow(parent)
	, m_settings("NewNotePad")
{
    ui.setupUi(this);

	//底部工具化文本控件初始化
	ui.statusBar->addPermanentWidget(new QLabel(this), 6);
	m_posLabel = new QLabel("第 1 行，第 1 列", this);
	ui.statusBar->addPermanentWidget(m_posLabel, 3);
	m_zoomLabel = new QLabel("100%", this);
	ui.statusBar->addPermanentWidget(m_zoomLabel, 1);
	m_lineLabel = new QLabel("Windows (CRLF)", this);
	ui.statusBar->addPermanentWidget(m_lineLabel, 3);
	m_codecLabel = new QLabel("GBK", this);
	ui.statusBar->addPermanentWidget(m_codecLabel, 1);

	// 使用window系统本身笔记本icon
	QFileIconProvider ip;
	QIcon icon = ip.icon(QFileInfo("C:\\Windows\\System32\\notepad.exe"));
	qApp->setWindowIcon(icon);
}

QtNodePad::~QtNodePad()
{

}

void QtNodePad::openFile(const QString & path)
{

}

bool QtNodePad::isModified() const
{
	return ui.mainTextEdit->toPlainText() != savedContent;
}

void QtNodePad::updateWindowTitle()
{
	this->setWindowTitle((isModified() ? "*" : "") + fileName + " - 记事本");
}

bool QtNodePad::askSaveFile()
{
	if (!isModified())
		return true;

	// 有未保存的更改
	int btn = QMessageBox::question(this, "记事本", "你想更改保存到 " + (fileName.isEmpty() ? "无标题" : fileName) + " 吗？", "保存(&S)", "不保存(&N)", "取消");
	if (btn == 2) // 取消
		return false;
	if (btn == 0) // 保存
	{
		return sltActionSaveFile();
	}
	return true;
}

bool QtNodePad::sltActionSaveFile()
{
	if (filePath.isEmpty()) // 没有路径，另存为
	{
		QString recentPath = m_settings.value("recent/filePath").toString();
		QString path = QFileDialog::getSaveFileName(this, "另存为", recentPath, "*.txt");
		if (path.isEmpty())
			return false;
		m_settings.setValue("recent/filePath", path);
		filePath = path;
		fileName = QFileInfo(path).baseName();
	}

	// 写出文件
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly))
		return false;
	QTextStream ts(&file);
	ts.setCodec("GBK");
	savedContent = ui.mainTextEdit->toPlainText();
	ts << savedContent;
	file.close();
	updateWindowTitle();
	return true;
}
