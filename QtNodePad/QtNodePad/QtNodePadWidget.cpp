#include "QtNodePadWidget.h"
#include <QFileIconProvider>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QProcess>
#include <QDesktopServices>
#include "FindDialog.h"

QtNodePad::QtNodePad(QWidget *parent)
    : QMainWindow(parent)
	, m_settings("NewNotePad")
{
    ui.setupUi(this);

	// 读取设置
	if (!m_settings.value("wordWrap", true).toBool())
	{
		//ui->actionWord_Wrap_W->setChecked(false);
		ui.mainTextEdit->setWordWrapMode(QTextOption::NoWrap);
	}
	if (!m_settings.value("statusBar", true).toBool())
	{
		this->statusBar()->hide();
		//ui->actionStatus_Bar_S->setChecked(false);
	}

	// 恢复字体
	QString fs;
	if (!(fs = m_settings.value("font").toString()).isEmpty())
	{
		QFont f;
		f.fromString(fs);
		ui.mainTextEdit->setFont(f);
	}

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

	//操作
	connect(ui.mainTextEdit, &QPlainTextEdit::selectionChanged, this, &QtNodePad::sltPlainTextEdiSelectionChanged);

	connect(ui.action_N, &QAction::triggered, this, &QtNodePad::sltActionNewCreate);
	connect(ui.action_W, &QAction::triggered, this, &QtNodePad::sltActionCreateWindow);
	connect(ui.action_O, &QAction::triggered, this, &QtNodePad::sltActionOpenFile);
	connect(ui.action_S, &QAction::triggered, this, &QtNodePad::sltActionSaveText);
	connect(ui.action_A, &QAction::triggered, this, &QtNodePad::sltActionSaveOther);
	connect(ui.action_C, &QAction::triggered, this, &QtNodePad::sltActionExit);

	connect(ui.action_U_2, &QAction::triggered, this, &QtNodePad::sltActionUndo);
	connect(ui.action_C_2, &QAction::triggered, this, &QtNodePad::sltActionCopy);
	connect(ui.action_P_2, &QAction::triggered, this, &QtNodePad::sltActionPaste);
	connect(ui.action_T, &QAction::triggered, this, &QtNodePad::sltActionCut);
	connect(ui.action_L, &QAction::triggered, this, &QtNodePad::sltActionDelete);

	connect(ui.action_Bing_2, &QAction::triggered, this, &QtNodePad::sltActionBingSearch);
	connect(ui.action_F_2, &QAction::triggered, this, &QtNodePad::sltActionFind);
	connect(ui.action_N_2, &QAction::triggered, this, &QtNodePad::sltActionFindNext);
	connect(ui.action_V, &QAction::triggered, this, &QtNodePad::sltActionFindPrev);
}

QtNodePad::~QtNodePad()
{

}

void QtNodePad::openFile(const QString & path)
{
	filePath = path;

	if (path.isEmpty())
	{
		savedContent = "";
		fileName = "无标题";
	}
	else
	{
		QFile file(path);
		if (!file.exists())
			return;

		fileName = QFileInfo(path).baseName();

		// 读取文件
		if (!file.open(QIODevice::ReadOnly))
			return;

		savedContent = QString::fromLocal8Bit(file.readAll());
	}
	ui.mainTextEdit->setPlainText(savedContent);
	updateWindowTitle();
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
	int btn = QMessageBox::question(this, "记事本", "是否保存到 " + (fileName.isEmpty() ? "无标题" : fileName) + " 吗？", "保存(&S)", "不保存(&N)", "取消");
	if (btn == 2) // 取消
		return false;
	if (btn == 0) // 保存
	{
		return sltActionSaveFile();
	}
	return true;
}

void QtNodePad::createFindDialog()
{
	m_findDialog = new FindDialog(m_settings, this);

	connect(m_findDialog, &FindDialog::signalShow, this, [=] {
		ui.action_N_2->setEnabled(true);
		ui.action_V->setEnabled(true);
	});
	connect(m_findDialog, &FindDialog::signalHide, this, [=] {
		ui.action_N_2->setEnabled(false);
		ui.action_V->setEnabled(false);
	});
	/* connect(m_findDialog, &FindDialog::signalTextChanged, this, [=](const QString& text){
		this->findText = text;
	}); */
	connect(m_findDialog, &FindDialog::signalFindNext, this, &QtNodePad::sltActionFindNext);
	connect(m_findDialog, &FindDialog::signalFindPrev, this, &QtNodePad::sltActionFindPrev);
	connect(m_findDialog, &FindDialog::signalReplaceNext, this, [=] {
		const QString& findText = m_findDialog->getFindText();
		const QString& replaceText = m_findDialog->getReplaceText();
		if (findText.isEmpty())
			return;

		// 替换 逻辑上分为：查找、选中、替换
		const QString& selectedText = ui.mainTextEdit->textCursor().selectedText();
		if ((m_findDialog->isCaseSensitive() && selectedText != findText)
			|| selectedText.toLower() != findText.toLower())
		{
			// 如果选中的词不是findText，则查找下一个
			sltActionFindNext();
		}
		else
		{
			// 已选中，则替换选中的
			QTextCursor tc = ui.mainTextEdit->textCursor();
			tc.insertText(replaceText);
			ui.mainTextEdit->setTextCursor(tc);
			sltActionFindNext(); // 查找下一个
		}
		});
	connect(m_findDialog, &FindDialog::signalReplaceAll, this, [=] {
		const QString& findText = m_findDialog->getFindText();
		const QString& replaceText = m_findDialog->getReplaceText();
		if (findText.isEmpty())
			return;

		QString content = ui.mainTextEdit->toPlainText();
		QTextCursor tc = ui.mainTextEdit->textCursor();
		tc.setPosition(0);
		tc.setPosition(content.length(), QTextCursor::KeepAnchor);
		content.replace(findText, replaceText);
		tc.insertText(content);
		// ui.mainTextEdit->setTextCursor(tc);     // 不调用这句话，保留替换之前的位置
		// ui.mainTextEdit->setPlainText(content); // 这个会导致无法撤销，而且会重置光标位置到开头
	});
}

void QtNodePad::sltPlainTextEdiSelectionChanged()
{
	bool selected = ui.mainTextEdit->textCursor().hasSelection();
	ui.action_Bing_2->setEnabled(selected);
	ui.action_C_2->setEnabled(selected);
	ui.action_T->setEnabled(selected);
	ui.action_L->setEnabled(selected);
	/*ui->actionReselect_Chinese->setEnabled(selected);*/
}

void QtNodePad::sltActionNewCreate()
{
	if (!askSaveFile())
		return;

	openFile("");
}

void QtNodePad::sltActionCreateWindow()
{
	QProcess pro(this);
	pro.startDetached(QApplication::applicationFilePath(), { "-new" });
}

void QtNodePad::sltActionOpenFile()
{
	if (!askSaveFile())
		return;

	QString recentPath = m_settings.value("recent/filePath").toString();
	QString path = QFileDialog::getOpenFileName(this, "打开", recentPath, "*.txt");
	if (path.isEmpty())
		return;

	openFile(path);
}

void QtNodePad::sltActionSaveText()
{
	if (!sltActionSaveFile())
		QMessageBox::warning(this, "记事本", "保存失败！");
	else
		QMessageBox::information(this, "记事本", "保存成功！");
}

void QtNodePad::sltActionSaveOther()
{
	QString temp = filePath;
	filePath = "";
	if (!sltActionSaveFile())
		filePath = temp;
}

void QtNodePad::sltActionExit()
{
	if (!askSaveFile())
		return;

	this->close();
}

void QtNodePad::sltActionUndo()
{
	ui.mainTextEdit->undo();
}

void QtNodePad::sltActionCut()
{
	ui.mainTextEdit->cut();
}

void QtNodePad::sltActionCopy()
{
	ui.mainTextEdit->copy();
}

void QtNodePad::sltActionPaste()
{
	ui.mainTextEdit->paste();
}

void QtNodePad::sltActionDelete()
{
	QTextCursor tc = ui.mainTextEdit->textCursor();
	int pos = tc.position();
	if (pos >= ui.mainTextEdit->toPlainText().length())
		return;
	tc.setPosition(pos + 1, QTextCursor::MoveMode::KeepAnchor);
	tc.removeSelectedText();
}

void QtNodePad::sltActionBingSearch()
{
	// !这里是转到UTF-8，保存到txt是保存为GBK
	QByteArray key = ui.mainTextEdit->textCursor().selectedText().toUtf8().toPercentEncoding();
	QDesktopServices::openUrl(QUrl("https://cn.bing.com/search?q=" + key + "&form=NPCTXT"));
}

void QtNodePad::sltActionFind()
{
	if (!m_findDialog)
	{
		createFindDialog();
	}
	m_findDialog->openFind(false);
}

void QtNodePad::sltActionFindNext()
{
	const QString& text = m_findDialog->getFindText();
	if (text.isEmpty())
		return;

	QTextDocument::FindFlags flags;
	if (m_findDialog->isCaseSensitive())
		flags |= QTextDocument::FindCaseSensitively;
	bool rst = ui.mainTextEdit->find(text, flags);
	if (!rst && m_findDialog->isLoop()
		&& ui.mainTextEdit->toPlainText().contains(text)) // 没找到，尝试从头开始
	{
		QTextCursor tc = ui.mainTextEdit->textCursor();
		tc.setPosition(0);
		ui.mainTextEdit->setTextCursor(tc);
		sltActionFindNext();
	}
}

void QtNodePad::sltActionFindPrev()
{
	const QString& text = m_findDialog->getFindText();
	if (text.isEmpty())
		return;

	QTextDocument::FindFlags flags = QTextDocument::FindBackward;
	if (m_findDialog->isCaseSensitive())
		flags |= QTextDocument::FindCaseSensitively;
	bool rst = ui.mainTextEdit->find(text, flags);
	if (!rst && m_findDialog->isLoop()
		&& ui.mainTextEdit->toPlainText().contains(text))
	{
		QTextCursor tc = ui.mainTextEdit->textCursor();
		tc.setPosition(ui.mainTextEdit->toPlainText().length());
		ui.mainTextEdit->setTextCursor(tc);
		sltActionFindPrev();
	}
}

void QtNodePad::showEvent(QShowEvent * e)
{
	this->restoreGeometry(m_settings.value("QtNodePad/geometry").toByteArray());
	this->restoreState(m_settings.value("QtNodePad/state").toByteArray());

	QMainWindow::showEvent(e);
}

void QtNodePad::closeEvent(QCloseEvent * e)
{
	if (!askSaveFile())
	{
		e->ignore();
		return;
	}
	m_settings.setValue("QtNodePad/geometry", this->saveGeometry());
	m_settings.setValue("QtNodePad/state", this->saveState());

	QMainWindow::closeEvent(e);
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
