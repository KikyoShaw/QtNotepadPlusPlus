#pragma once

#pragma execution_character_set("utf-8")

#include <QtWidgets/QMainWindow>
#include <QLabel>
#include <QSettings>
#include "ui_QtNodePadWidget.h"

class FindDialog;

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

private slots:
	bool sltActionSaveFile();

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
};
