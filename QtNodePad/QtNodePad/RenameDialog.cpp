#include "RenameDialog.h"

RenameDialog::RenameDialog(QWidget *parent)
	:QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	connect(ui.changeNameButton, &QPushButton::clicked, this, &RenameDialog::sltRenameFinsh);
}

RenameDialog::~RenameDialog()
{
}

void RenameDialog::showRenameDialog(const QString & fileName)
{
	m_fileName = fileName;
	ui.nameEdit->setText(fileName);
	QDialog::show();
	ui.nameEdit->setFocus();
	ui.nameEdit->selectAll();
	this->activateWindow();
}

void RenameDialog::sltRenameFinsh()
{
	auto name = ui.nameEdit->text();
	m_fileName = name.isEmpty() ? m_fileName : name;
	emit signalRename(m_fileName);
	this->close();
}
