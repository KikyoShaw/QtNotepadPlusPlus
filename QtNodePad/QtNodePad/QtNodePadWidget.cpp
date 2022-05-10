#include "QtNodePadWidget.h"
#include <QFileIconProvider>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QProcess>
#include <QDesktopServices>
#include <QTextBlock>
#include <QDateTime>
#include "FindDialog.h"

QtNodePad::QtNodePad(QWidget *parent)
    : QMainWindow(parent)
	, m_settings("NewNotePad")
{
    ui.setupUi(this);

	// ��ȡ����
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

	// �ָ�����
	QString fs;
	if (!(fs = m_settings.value("font").toString()).isEmpty())
	{
		QFont f;
		f.fromString(fs);
		ui.mainTextEdit->setFont(f);
	}

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

	//����
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
	connect(ui.action_R_2, &QAction::triggered, this, &QtNodePad::sltActionReplace);
	connect(ui.action_A_2, &QAction::triggered, this, &QtNodePad::sltActionSelectAll);
	connect(ui.action_D, &QAction::triggered, this, &QtNodePad::sltActionAddDate);
	connect(ui.action_W_2, &QAction::triggered, this, &QtNodePad::sltActionWrap);

	connect(ui.action_I, &QAction::triggered, this, &QtNodePad::sltActionZoomIn);
	connect(ui.action_O_2, &QAction::triggered, this, &QtNodePad::sltActionZoomOut);
	connect(ui.action_R_4, &QAction::triggered, this, &QtNodePad::sltActionZoomDefault);

	connect(ui.action_L_2, &QAction::triggered, this, &QtNodePad::sltActionInputHandle);
	connect(ui.action_R_3, &QAction::triggered, this, &QtNodePad::sltActionReadHandle);

	connect(ui.action_H, &QAction::triggered, this, &QtNodePad::sltActionHelp);
	connect(ui.action_F_4, &QAction::triggered, this, &QtNodePad::sltActionGithub);
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
		fileName = "�ޱ���";
	}
	else
	{
		QFile file(path);
		if (!file.exists())
			return;

		fileName = QFileInfo(path).baseName();

		// ��ȡ�ļ�
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
	this->setWindowTitle((isModified() ? "*" : "") + fileName + " - ���±�");
}

bool QtNodePad::askSaveFile()
{
	if (!isModified())
		return true;

	// ��δ����ĸ���
	int btn = QMessageBox::question(this, "���±�", "�Ƿ񱣴浽 " + (fileName.isEmpty() ? "�ޱ���" : fileName) + " ��", "����(&S)", "������(&N)", "ȡ��");
	if (btn == 2) // ȡ��
		return false;
	if (btn == 0) // ����
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

		// �滻 �߼��Ϸ�Ϊ�����ҡ�ѡ�С��滻
		const QString& selectedText = ui.mainTextEdit->textCursor().selectedText();
		if ((m_findDialog->isCaseSensitive() && selectedText != findText)
			|| selectedText.toLower() != findText.toLower())
		{
			// ���ѡ�еĴʲ���findText���������һ��
			sltActionFindNext();
		}
		else
		{
			// ��ѡ�У����滻ѡ�е�
			QTextCursor tc = ui.mainTextEdit->textCursor();
			tc.insertText(replaceText);
			ui.mainTextEdit->setTextCursor(tc);
			sltActionFindNext(); // ������һ��
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
		// ui.mainTextEdit->setTextCursor(tc);     // ��������仰�������滻֮ǰ��λ��
		// ui.mainTextEdit->setPlainText(content); // ����ᵼ���޷����������һ����ù��λ�õ���ͷ
	});
}

void QtNodePad::sltPlainTextEditTextChanged()
{
	if (fileName.isEmpty())
		fileName = "�ޱ���";
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
}

void QtNodePad::sltPlainTextEditUndoAvailable(bool ava)
{
	ui.action_U_2->setEnabled(ava);
}

void QtNodePad::sltPlainTextEditCursorPositionChanged()
{
	QTextCursor tc = ui.mainTextEdit->textCursor();

	QTextLayout* ly = tc.block().layout();
	int posInBlock = tc.position() - tc.block().position(); // ��ǰ�����block�ڵ����λ��
	int line = ly->lineForTextPosition(posInBlock).lineNumber() + tc.block().firstLineNumber();

	int col = tc.columnNumber(); // �ڼ���
	// int row = tc.blockNumber(); // �ڼ��Σ��޷�ʶ��WordWrap�ĵڼ���
	m_posLabel->setText("�� " + QString::number(line + 1) + " �У��� " + QString::number(col + 1) + " ��");
}

void QtNodePad::sltPlainTextEditCustomContextMenuRequested(const QPoint & p)
{
	QMenu* menu = new QMenu();

	QMenu* insertUnicodeControlCharsMenu = new QMenu("���� Unicode �����ַ�(&I)", menu);
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
		// TODO: ����ʾҲ��ʾ����
		connect(action, &QAction::triggered, ui.mainTextEdit, [=] {
			// TODO: ���� Unicode �����ַ�
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
	QString path = QFileDialog::getOpenFileName(this, "��", recentPath, "*.txt");
	if (path.isEmpty())
		return;

	openFile(path);
}

void QtNodePad::sltActionSaveText()
{
	if (!sltActionSaveFile())
		QMessageBox::warning(this, "���±�", "����ʧ�ܣ�");
	else
		QMessageBox::information(this, "���±�", "����ɹ���");
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
	// !������ת��UTF-8�����浽txt�Ǳ���ΪGBK
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
		&& ui.mainTextEdit->toPlainText().contains(text)) // û�ҵ������Դ�ͷ��ʼ
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
		ui.action_L_2->setText("�ر����뷨(&L)");
	}
	else
	{
		ui.mainTextEdit->setReadOnly(true);
		ui.action_L_2->setText("�����뷨(&O)");
	}
}

void QtNodePad::sltActionReadHandle()
{
	auto direction = ui.action_R_3->isChecked() ? Qt::RightToLeft : Qt::LeftToRight;
	ui.mainTextEdit->setLayoutDirection(direction);
}

void QtNodePad::sltActionHelp()
{
	QDesktopServices::openUrl(QUrl("https://github.com/KikyoShaw/QtNotepadPlusPlus"));
}

void QtNodePad::sltActionGithub()
{
	QDesktopServices::openUrl(QUrl("https://github.com/KikyoShaw"));
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
