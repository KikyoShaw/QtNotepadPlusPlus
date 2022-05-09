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

	//�ײ����߻��ı��ؼ���ʼ��
	ui.statusBar->addPermanentWidget(new QLabel(this), 6);
	m_posLabel = new QLabel("�� 1 �У��� 1 ��", this);
	ui.statusBar->addPermanentWidget(m_posLabel, 3);
	m_zoomLabel = new QLabel("100%", this);
	ui.statusBar->addPermanentWidget(m_zoomLabel, 1);
	m_lineLabel = new QLabel("Windows (CRLF)", this);
	ui.statusBar->addPermanentWidget(m_lineLabel, 3);
	m_codecLabel = new QLabel("GBK", this);
	ui.statusBar->addPermanentWidget(m_codecLabel, 1);

	// ʹ��windowϵͳ����ʼǱ�icon
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
	this->setWindowTitle((isModified() ? "*" : "") + fileName + " - ���±�");
}

bool QtNodePad::askSaveFile()
{
	if (!isModified())
		return true;

	// ��δ����ĸ���
	int btn = QMessageBox::question(this, "���±�", "������ı��浽 " + (fileName.isEmpty() ? "�ޱ���" : fileName) + " ��", "����(&S)", "������(&N)", "ȡ��");
	if (btn == 2) // ȡ��
		return false;
	if (btn == 0) // ����
	{
		return sltActionSaveFile();
	}
	return true;
}

bool QtNodePad::sltActionSaveFile()
{
	if (filePath.isEmpty()) // û��·�������Ϊ
	{
		QString recentPath = m_settings.value("recent/filePath").toString();
		QString path = QFileDialog::getSaveFileName(this, "���Ϊ", recentPath, "*.txt");
		if (path.isEmpty())
			return false;
		m_settings.setValue("recent/filePath", path);
		filePath = path;
		fileName = QFileInfo(path).baseName();
	}

	// д���ļ�
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
