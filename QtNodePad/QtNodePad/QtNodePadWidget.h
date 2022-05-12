#pragma once

#pragma execution_character_set("utf-8")

#include <QtWidgets/QMainWindow>
#include <QLabel>
#include <QSettings>
#include <QPrinter>
#include "ui_QtNodePadWidget.h"

class FindDialog;
class RenameDialog;
class QPrintDialog;
class QPageSetupDialog;
class GotoDialog;

class QtNodePad : public QMainWindow
{
    Q_OBJECT

public:
    QtNodePad(QWidget *parent = Q_NULLPTR);
	~QtNodePad();

	//���ļ����ߴ������ļ�
	void openFile(const QString& path);

	//�Ƿ񱣴�
	bool isModified() const;

private:
	void updateWindowTitle();
	bool askSaveFile();
	void createFindDialog();
	//�ļ�����
	bool saveFile();
	//��ת��ָ����
	void gotoRow(int row);

private slots:
	//�ı����ݲ���
	void sltPlainTextEditTextChanged();
	void sltPlainTextEdiSelectionChanged();
	void sltPlainTextEditUndoAvailable(bool ava);
	void sltPlainTextEditCursorPositionChanged();
	void sltPlainTextEditCustomContextMenuRequested(const QPoint &p);

	//�½�
	void sltActionNewCreate();
	//�½�����
	void sltActionCreateWindow();
	//���ı�
	void sltActionOpenFile();
	//�����ı�
	void sltActionSaveText();
	//���Ϊ
	void sltActionSaveOther();
	//�˳�
	void sltActionExit();
	//������
	void sltActionRename();
	//�������ļ���
	void sltActionOpenFolder();
	//��ӡ
	void sltActionPrinter();
	//ҳ������
	void sltActionPageSetup();

	//����
	void sltActionUndo();
	//����
	void sltActionCut();
	//����
	void sltActionCopy();
	//ճ��
	void sltActionPaste();
	//ɾ��
	void sltActionDelete();

	//��Ӧ����
	void sltActionBingSearch();
	//����
	void sltActionFind();
	//��һ��
	void sltActionFindNext();
	//��һ��
	void sltActionFindPrev();
	//ת��
	void sltActionGoto();

	//�滻
	void sltActionReplace();
	//ȫѡ
	void sltActionSelectAll();
	//�������
	void sltActionAddDate();
	//����
	void sltActionWrap();
	//����
	void sltActionFontHandle();

	//�Ŵ�
	void sltActionZoomIn();
	//��С
	void sltActionZoomOut();
	//�ָ�Ĭ��
	void sltActionZoomDefault();

	//���뷨����
	void sltActionInputHandle();
	//�Ķ�˳��
	void sltActionReadHandle();
	//�ײ�������
	void sltActionStatus();
	//������ѡ
	void sltChineseReElection();

	//ת�ɴ�д
	void sltActionToUpper();
	//ת��Сд
	void sltActionToLower();

	//����
	void sltActionHelp();
	//��ҳ
	void sltActionGithub();
	//����
	void sltActionAboutNotePad();

protected:
	virtual void showEvent(QShowEvent* e) override;
	virtual void closeEvent(QCloseEvent* e) override;

private:
    Ui::QtNodePadClass ui;
	QSettings m_settings;

	//�ļ�����
	QString filePath;
	QString fileName;
	QString savedContent;
	int zoomSize = 100;

	//�ײ����߻��ı��ؼ�
	QLabel* m_posLabel = nullptr;
	QLabel* m_zoomLabel = nullptr;
	QLabel* m_lineLabel = nullptr;
	QLabel* m_codecLabel = nullptr;

	//�����ؼ�
	FindDialog* m_findDialog = nullptr;
	//�������ؼ�
	RenameDialog *m_renameDialog = nullptr;
	//��ת����
	GotoDialog *m_gotoDialog = nullptr;
	//��ӡ������
	QPrinter Printer;
	//��ӡ������
	QPrintDialog *m_printDialog = nullptr;
	//ҳ��������
	QPageSetupDialog *m_pageSetupDialog = nullptr;
};
