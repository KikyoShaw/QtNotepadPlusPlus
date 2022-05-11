#include "GotoDialog.h"
#include <QRegExpValidator>

GotoDialog::GotoDialog(QWidget *parent)
	:QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	QRegExp regx("[0-9]+$");
	QValidator *validator = new QRegExpValidator(regx);
	ui.gotoEdit->setValidator(validator);

	connect(ui.cancelButton, &QPushButton::clicked, this, &GotoDialog::close);
	connect(ui.gotoButton, &QPushButton::clicked, this, &GotoDialog::sltGotoFinsh);
}

GotoDialog::~GotoDialog()
{
}

void GotoDialog::showGotoDialog()
{
	QDialog::show();
	ui.gotoEdit->setFocus();
	ui.gotoEdit->selectAll();
	this->activateWindow();
}

void GotoDialog::sltGotoFinsh()
{
	int row = 0;
	if (ui.gotoEdit->text().isEmpty())
		row = ui.gotoEdit->text().toInt();
	emit signalGoto(row);
}
