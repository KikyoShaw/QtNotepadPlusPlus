#include "QtNodePadWidget.h"
#include <QFileIconProvider>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QProcess>
#include <QDesktopServices>
#include <QTextBlock>
#include <QDateTime>
#include <QFontDialog>
#include <QPrintDialog>
#include <QPageSetupDialog>
#include "GotoDialog.h"
#include "FindDialog.h"
#include "RenameDialog.h"

QtNodePad::QtNodePad(QWidget *parent)
    : QMainWindow(parent)
	, m_settings("NewNotePad"), fileName("无标题")
{
    ui.setupUi(this);

	// 读取设置
	if (!m_settings.value("wordWrap", true).toBool())
	{
		ui.action_W_2->setChecked(false);
		ui.mainTextEdit->setWordWrapMode(QTextOption::NoWrap);
	}
	if (!m_settings.value("statusBar", true).toBool())
	{
		this->statusBar()->hide();
		ui.action_S_2->setChecked(false);
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

	//最近操作的标题
	filePath = m_settings.value("recent/filePath").toString();
	fileName = QFileInfo(filePath).baseName();
	updateWindowTitle();

	//操作
	connect(ui.mainTextEdit, &QPlainTextEdit::selectionChanged, this, &QtNodePad::sltPlainTextEdiSelectionChanged);
	connect(ui.mainTextEdit, &QPlainTextEdit::textChanged, this, &QtNodePad::sltPlainTextEditTextChanged);
	connect(ui.mainTextEdit, &QPlainTextEdit::undoAvailable, this, &QtNodePad::sltPlainTextEditUndoAvailable);
	connect(ui.mainTextEdit, &QPlainTextEdit::cursorPositionChanged, this, &QtNodePad::sltPlainTextEditCursorPositionChanged);
	connect(ui.mainTextEdit, &QPlainTextEdit::customContextMenuRequested, this, &QtNodePad::sltPlainTextEditCustomContextMenuRequested);

	connect(ui.action_N, &QAction::triggered, this, &QtNodePad::sltActionNewCreate);
	connect(ui.action_W, &QAction::triggered, this, &QtNodePad::sltActionCreateWindow);
	connect(ui.action_O, &QAction::triggered, this, &QtNodePad::sltActionOpenFile);
	connect(ui.action_S, &QAction::triggered, this, &QtNodePad::sltActionSaveText);
	connect(ui.action_A, &QAction::triggered, this, &QtNodePad::sltActionSaveOther);
	connect(ui.action_R, &QAction::triggered, this, &QtNodePad::sltActionRename);

	connect(ui.action_U_2, &QAction::triggered, this, &QtNodePad::sltActionUndo);
	connect(ui.action_C_2, &QAction::triggered, this, &QtNodePad::sltActionCopy);
	connect(ui.action_P_2, &QAction::triggered, this, &QtNodePad::sltActionPaste);
	connect(ui.action_T, &QAction::triggered, this, &QtNodePad::sltActionCut);
	connect(ui.action_L, &QAction::triggered, this, &QtNodePad::sltActionDelete);

	connect(ui.action_Bing_2, &QAction::triggered, this, &QtNodePad::sltActionBingSearch);
	connect(ui.action_F_2, &QAction::triggered, this, &QtNodePad::sltActionFind);
	connect(ui.action_N_2, &QAction::triggered, this, &QtNodePad::sltActionFindNext);
	connect(ui.action_V, &QAction::triggered, this, &QtNodePad::sltActionFindPrev);
	connect(ui.action_R_2, &QAction::triggered, this, &QtNodePad::sltActionReplace);
	connect(ui.action_A_2, &QAction::triggered, this, &QtNodePad::sltActionSelectAll);
	connect(ui.action_D, &QAction::triggered, this, &QtNodePad::sltActionAddDate);
	connect(ui.action_W_2, &QAction::triggered, this, &QtNodePad::sltActionWrap);
	connect(ui.action_F_3, &QAction::triggered, this, &QtNodePad::sltActionFontHandle);
	connect(ui.action_F, &QAction::triggered, this, &QtNodePad::sltActionOpenFolder);
	connect(ui.action_P, &QAction::triggered, this, &QtNodePad::sltActionPrinter);
	connect(ui.action_U, &QAction::triggered, this, &QtNodePad::sltActionPageSetup);
	connect(ui.action_G_2, &QAction::triggered, this, &QtNodePad::sltActionGoto);
	
	connect(ui.action_I, &QAction::triggered, this, &QtNodePad::sltActionZoomIn);
	connect(ui.action_O_2, &QAction::triggered, this, &QtNodePad::sltActionZoomOut);
	connect(ui.action_R_4, &QAction::triggered, this, &QtNodePad::sltActionZoomDefault);

	connect(ui.action_L_2, &QAction::triggered, this, &QtNodePad::sltActionInputHandle);
	connect(ui.action_R_3, &QAction::triggered, this, &QtNodePad::sltActionReadHandle);
	connect(ui.action_S_2, &QAction::triggered, this, &QtNodePad::sltActionStatus);
	connect(ui.action_H_2, &QAction::triggered, this, &QtNodePad::sltChineseReElection);
	connect(ui.action_U_3, &QAction::triggered, this, &QtNodePad::sltActionToUpper);
	connect(ui.action_L_3, &QAction::triggered, this, &QtNodePad::sltActionToLower);
	
	connect(ui.action_H, &QAction::triggered, this, &QtNodePad::sltActionHelp);
	connect(ui.action_F_4, &QAction::triggered, this, &QtNodePad::sltActionGithub);
	connect(ui.action_A_3, &QAction::triggered, this, &QtNodePad::sltActionAboutNotePad);
	connect(ui.action_X, &QAction::triggered, this, &QtNodePad::sltActionExit);
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
		return saveFile();
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

void QtNodePad::sltPlainTextEditTextChanged()
{
	if (fileName.isEmpty())
		fileName = "无标题";
	updateWindowTitle();

	bool empty = ui.mainTextEdit->toPlainText().isEmpty();
	ui.action_F_2->setEnabled(!empty);
	ui.action_R_2->setEnabled(!empty);
	ui.action_N_2->setEnabled(!empty && m_findDialog && m_findDialog->isVisible());
	ui.action_V->setEnabled(!empty && m_findDialog && m_findDialog->isVisible());
}

void QtNodePad::sltPlainTextEdiSelectionChanged()
{
	bool selected = ui.mainTextEdit->textCursor().hasSelection();
	ui.action_Bing_2->setEnabled(selected);
	ui.action_C_2->setEnabled(selected);
	ui.action_T->setEnabled(selected);
	ui.action_L->setEnabled(selected);
	ui.action_H_2->setEnabled(selected);
	ui.action_U_3->setEnabled(selected);
	ui.action_L_3->setEnabled(selected);
}

void QtNodePad::sltPlainTextEditUndoAvailable(bool ava)
{
	ui.action_U_2->setEnabled(ava);
}

void QtNodePad::sltPlainTextEditCursorPositionChanged()
{
	QTextCursor tc = ui.mainTextEdit->textCursor();

	QTextLayout* ly = tc.block().layout();
	int posInBlock = tc.position() - tc.block().position(); // 当前光标在block内的相对位置
	int row = ly->lineForTextPosition(posInBlock).lineNumber() + tc.block().firstLineNumber();

	int col = tc.columnNumber(); // 第几列
	// int row = tc.blockNumber(); // 第几段，无法识别WordWrap的第几行
	m_posLabel->setText("第 " + QString::number(row + 1) + " 行，第 " + QString::number(col + 1) + " 列");
}

void QtNodePad::sltPlainTextEditCustomContextMenuRequested(const QPoint & p)
{
	QMenu* menu = new QMenu();

	QMenu* insertUnicodeControlCharsMenu = new QMenu("插入 Unicode 控制字符(&I)", menu);
	QList<QPair<QString, QString>> unicodeControlChars{
		{"LRM", "&Left-to-right mark"},
		{"RLM", "&Right-to-left mark"},
		{"ZWJ", "Zero width joiner"},
		{"ZWNJ", "Zero width &non-joiner"},
		{"LRE", "Start of left-to-right &embedding"},
		{"RLE", "Start of right-to-left e&mbedding"},
		{"LRO", "Start of left-to-right &override"},
		{"RLO", "Start of right-to-left o&verride"},
		{"PDF", "&Pop directional formatting"},
		{"NADS", "N&ational digit shapes substitution"},
		{"NODS", "Nominal (European) &digit shapes"},
		{"ASS", "Activate &symmetric swapping"},
		{"ISS", "Inhibit s&ymmetric swapping"},
		{"AAFS", "Activate Arabic &form shaping"},
		{"IAFS", "Inhibit Arabic form s&haping"},
		{"RS", "Record Separator (&Block separator)"},
		{"US", "Unit Separator (&Segment separator)"}
	};
	for (auto p : unicodeControlChars)
	{
		QAction* action = new QAction(p.first, insertUnicodeControlCharsMenu);
		action->setToolTip(p.second);
		// TODO: 把提示也显示出来
		connect(action, &QAction::triggered, ui.mainTextEdit, [=] {
			// TODO: 插入 Unicode 控制字符
			// ui->plainTextEdit->insertPlainText("\u202c");
			});
		insertUnicodeControlCharsMenu->addAction(action);
	}

	menu->addAction(ui.action_U_2);
	menu->addSeparator();
	menu->addAction(ui.action_C_2);
	menu->addAction(ui.action_P_2);
	menu->addAction(ui.action_T);
	menu->addAction(ui.action_L);
	menu->addSeparator();
	menu->addAction(ui.action_A_2);
	menu->addSeparator();
	menu->addAction(ui.action_I);
	menu->addAction(ui.action_O_2);
	menu->addAction(ui.action_R_4);
	menu->addSeparator();
	menu->addAction(ui.action_R_3);
	menu->addAction(ui.action_Unicode_S);
	menu->addMenu(insertUnicodeControlCharsMenu);
	menu->addSeparator();
	menu->addAction(ui.action_L_2);
	menu->addAction(ui.action_H_2);
	menu->addSeparator();
	menu->addAction(ui.action_Bing_2);

	menu->exec(QCursor::pos());
	menu->deleteLater();
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
	if (!saveFile())
		QMessageBox::warning(this, "记事本", "保存失败！");
	else
		QMessageBox::information(this, "记事本", "保存成功！");
}

void QtNodePad::sltActionSaveOther()
{
	QString temp = filePath;
	filePath = "";
	if (!saveFile())
		filePath = temp;
}

void QtNodePad::sltActionExit()
{
	if (!askSaveFile())
		return;

	this->close();
}

void QtNodePad::sltActionRename()
{
	if (m_renameDialog == nullptr) {
		m_renameDialog = new RenameDialog(this);
		connect(m_renameDialog, &RenameDialog::signalRename, this, [=](const QString &name) {
			fileName = name;
			updateWindowTitle();
		});
	}
	m_renameDialog->showRenameDialog(fileName);
}

void QtNodePad::sltActionOpenFolder()
{
	QFileInfo fileInfo(filePath);
	if (!fileInfo.isFile()) {
		QMessageBox::warning(this, "记事本", "请先保存文件！");
		return;
	}

	/*QString pathStr = filePath.left(filePath.lastIndexOf("/"));
	pathStr.replace("/", "//");
	bool ok = QDesktopServices::openUrl(QUrl(pathStr));
	if (!ok)
	{
		QMessageBox::warning(this, "记事本", "打开文件所在文件夹失败！");
	}*/
	QString pathStr = filePath.replace("/", "\\");
	QString cmd = QString("explorer.exe /select,\"%1\"").arg(pathStr);
	QProcess::startDetached(cmd);
}

void QtNodePad::sltActionPrinter()
{
	if (m_printDialog == nullptr) {
		m_printDialog = new QPrintDialog(&Printer);
	}
	m_printDialog->exec();
}

void QtNodePad::sltActionPageSetup()
{
	if (m_pageSetupDialog == nullptr) {
		m_pageSetupDialog = new QPageSetupDialog(&Printer);
	}
	m_pageSetupDialog->exec();
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

void QtNodePad::sltActionGoto()
{
	if (m_gotoDialog == nullptr) {
		m_gotoDialog = new GotoDialog(this);
		connect(m_gotoDialog, &GotoDialog::signalGoto, this, [=](int row) {
			if (row == 0) {
				QMessageBox::warning(this, "转到指定行", "请输入正确的跳转行数！");
				return;
			}
			int _row = ui.mainTextEdit->document()->lineCount();
			if (row > _row) {
				QMessageBox::warning(this, "转到指定行", "行数超过了总行数！");
				return;
			}

			gotoRow(row);
			m_gotoDialog->close();
		});
	}
	m_gotoDialog->showGotoDialog();
}

void QtNodePad::sltActionReplace()
{
	if (!m_findDialog)
	{
		createFindDialog();
	}
	m_findDialog->openFind(true);
}

void QtNodePad::sltActionSelectAll()
{
	ui.mainTextEdit->selectAll();
}

void QtNodePad::sltActionAddDate()
{
	ui.mainTextEdit->insertPlainText(QDateTime::currentDateTime().toString("hh:mm yyyy/MM/dd"));
}

void QtNodePad::sltActionWrap()
{
	if (ui.mainTextEdit->wordWrapMode() == QTextOption::NoWrap)
	{
		ui.mainTextEdit->setWordWrapMode(QTextOption::WordWrap);
		ui.action_W_2->setChecked(true);
		m_settings.setValue("wordWrap", true);
	}
	else
	{
		ui.mainTextEdit->setWordWrapMode(QTextOption::NoWrap);
		ui.action_W_2->setChecked(false);
		m_settings.setValue("wordWrap", false);
	}
}

void QtNodePad::sltActionFontHandle()
{
	bool ok;
	QFont f = QFontDialog::getFont(&ok, ui.mainTextEdit->font(), this, "字体");
	if (!ok)
		return;

	ui.mainTextEdit->setFont(f);
	m_settings.setValue("font", f.toString());
}

void QtNodePad::sltActionZoomIn()
{
	if (zoomSize >= 500)
		return;

	ui.mainTextEdit->zoomIn(1);
	zoomSize += 10;
	m_zoomLabel->setText(QString::number(zoomSize) + "%");
}

void QtNodePad::sltActionZoomOut()
{
	if (zoomSize <= 10)
		return;

	ui.mainTextEdit->zoomOut(1);
	zoomSize -= 10;
	m_zoomLabel->setText(QString::number(zoomSize) + "%");
}

void QtNodePad::sltActionZoomDefault()
{
	QString fs;
	if (!(fs = m_settings.value("font").toString()).isEmpty())
	{
		QFont f;
		f.fromString(fs);
		ui.mainTextEdit->setFont(f);
	}
	else
	{
		ui.mainTextEdit->setFont(qApp->font());
	}

	zoomSize = 100;
	m_zoomLabel->setText(QString::number(zoomSize) + "%");
}

void QtNodePad::sltActionInputHandle()
{
	if (ui.mainTextEdit->isReadOnly())
	{
		ui.mainTextEdit->setReadOnly(false);
		ui.action_L_2->setText("关闭输入法(&L)");
	}
	else
	{
		ui.mainTextEdit->setReadOnly(true);
		ui.action_L_2->setText("打开输入法(&O)");
	}
}

void QtNodePad::sltActionReadHandle()
{
	auto direction = ui.action_R_3->isChecked() ? Qt::RightToLeft : Qt::LeftToRight;
	ui.mainTextEdit->setLayoutDirection(direction);
}

void QtNodePad::sltActionStatus()
{
	if (this->statusBar()->isHidden())
	{
		this->statusBar()->show();
		ui.action_S_2->setChecked(true);
		m_settings.setValue("statusBar", true);
	}
	else
	{
		this->statusBar()->hide();
		ui.action_S_2->setChecked(false);
		m_settings.setValue("statusBar", false);
	}
}

void QtNodePad::sltChineseReElection()
{
	//QTextCursor cursor = ui.mainTextEdit->textCursor();
	//cursor.movePosition(QTextCursor::End);
	//ui.mainTextEdit->setTextCursor(cursor);
	ui.mainTextEdit->moveCursor(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
}

void QtNodePad::sltActionToUpper()
{
	//QTextCursor tc = ui.mainTextEdit->textCursor();
	//auto text = tc.selectedText().toUpper();
	//tc.removeSelectedText();
	//ui.mainTextEdit->insertPlainText(text);
	//tc.setPosition(ui.mainTextEdit->toPlainText().length());
	//ui.mainTextEdit->setTextCursor(tc);
	auto text = ui.mainTextEdit->textCursor().selectedText().toUpper();
	ui.mainTextEdit->textCursor().removeSelectedText();
	ui.mainTextEdit->insertPlainText(text);
}

void QtNodePad::sltActionToLower()
{
	auto text = ui.mainTextEdit->textCursor().selectedText().toLower();
	ui.mainTextEdit->textCursor().removeSelectedText();
	ui.mainTextEdit->insertPlainText(text);
}

void QtNodePad::sltActionHelp()
{
	QDesktopServices::openUrl(QUrl("https://github.com/KikyoShaw/QtNotepadPlusPlus"));
}

void QtNodePad::sltActionGithub()
{
	QDesktopServices::openUrl(QUrl("https://github.com/KikyoShaw"));
}

void QtNodePad::sltActionAboutNotePad()
{
	QMessageBox::about(this, "关于", "高仿NotePad++的Qt版");
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

bool QtNodePad::saveFile()
{
	//先判断文件是否存在
	QFileInfo fileInfo(filePath);
	if (!fileInfo.isFile()) {
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

void QtNodePad::gotoRow(int row)
{
	QTextCursor tc = ui.mainTextEdit->textCursor();
	int position = ui.mainTextEdit->document()->findBlockByNumber(row - 1).position();
	tc.setPosition(position, QTextCursor::MoveAnchor);
	ui.mainTextEdit->setTextCursor(tc);
}
