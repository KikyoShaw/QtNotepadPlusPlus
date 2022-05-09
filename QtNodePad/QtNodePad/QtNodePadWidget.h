#pragma once

#pragma execution_character_set("utf-8")

#include <QtWidgets/QMainWindow>
#include <QLabel>
#include <QSettings>
#include "ui_QtNodePadWidget.h"

class QtNodePad : public QMainWindow
{
    Q_OBJECT

public:
    QtNodePad(QWidget *parent = Q_NULLPTR);
	~QtNodePad();

	//打开文件或者创建空文件
	void openFile(const QString& path);

	//是否保存
	bool isModified() const;

private:
	void updateWindowTitle();
	bool askSaveFile();

private slots:
	bool sltActionSaveFile();

	//新建
	void sltActionNewCreate();
	//新建窗口
	void sltActionCreateWindow();
	//打开文本
	void sltActionOpenFile();
	//保存文本
	void sltActionSaveText();
	//另存为
	void sltActionSaveOther();

protected:
	virtual void showEvent(QShowEvent* e) override;
	virtual void closeEvent(QCloseEvent* e) override;

private:
    Ui::QtNodePadClass ui;
	QSettings m_settings;

	//文件属性
	QString filePath;
	QString fileName;
	QString savedContent;
	int zoomSize = 100;

	//底部工具化文本控件
	QLabel* m_posLabel = nullptr;
	QLabel* m_zoomLabel = nullptr;
	QLabel* m_lineLabel = nullptr;
	QLabel* m_codecLabel = nullptr;
};
