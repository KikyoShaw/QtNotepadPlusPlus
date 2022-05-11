#pragma once

#pragma execution_character_set("utf-8")

#include <QDialog>
#include "ui_GotoDialog.h"

class GotoDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GotoDialog(QWidget *parent = nullptr);
	~GotoDialog();

	void showGotoDialog();

signals:
	void signalGoto(int row);

private slots:
	void sltGotoFinsh();

private:
	Ui::GotoDialog ui;
};
