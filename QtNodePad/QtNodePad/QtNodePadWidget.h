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

	//打开文件或者创建空文件
	void openFile(const QString& path);

	//是否保存
	bool isModified() const;

private:
	void updateWindowTitle();
	bool askSaveFile();
	void createFindDialog();

private slots:
	//文件保存
	bool sltActionSaveFile();

	//文本内容操作
	void sltPlainTextEditTextChanged();
	void sltPlainTextEdiSelectionChanged();
	void sltPlainTextEditUndoAvailable(bool ava);
	void sltPlainTextEditCursorPositionChanged();
	void sltPlainTextEditCustomContextMenuRequested(const QPoint &p);

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
	//退出
	void sltActionExit();

	//撤销
	void sltActionUndo();
	//剪切
	void sltActionCut();
	//复制
	void sltActionCopy();
	//粘贴
	void sltActionPaste();
	//删除
	void sltActionDelete();

	//必应搜索
	void sltActionBingSearch();
	//搜索
	void sltActionFind();
	//下一个
	void sltActionFindNext();
	//上一个
	void sltActionFindPrev();

	//替换
	void sltActionReplace();
	//全选
	void sltActionSelectAll();
	//日期添加
	void sltActionAddDate();
	//换行
	void sltActionWrap();
	//字体
	void sltActionFontHandle();

	//放大
	void sltActionZoomIn();
	//缩小
	void sltActionZoomOut();
	//恢复默认
	void sltActionZoomDefault();

	//输入法操作
	void sltActionInputHandle();
	//阅读顺序
	void sltActionReadHandle();

	//帮助
	void sltActionHelp();
	//主页
	void sltActionGithub();

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

	//搜索控件
	FindDialog* m_findDialog = nullptr;
};
