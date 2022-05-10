#pragma once

#pragma execution_character_set("utf-8")

#include <QDialog>
#include "ui_RenameDialog.h"

class RenameDialog : public QDialog
{
	Q_OBJECT

public:
	explicit RenameDialog(QWidget *parent = nullptr);
	~RenameDialog();

	void showRenameDialog(const QString &fileName);

signals:
	void signalRename(const QString &fileName);

private slots:
	void sltRenameFinsh();

private:
	Ui::RenameDialog ui;

	QString m_fileName;
};
