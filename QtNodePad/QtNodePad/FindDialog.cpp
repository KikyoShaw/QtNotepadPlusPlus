#include "finddialog.h"

FindDialog::FindDialog(QSettings &settings, QWidget *parent) :
    QDialog(parent),
    settings(settings)
{
    ui.setupUi(this);
    this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    // 读取数据
    ui.findEdit->setText(settings.value("find/findText").toString());
    ui.replaceEdit->setText(settings.value("find/replaceText").toString());
    ui.caseSensitiveCheck->setChecked(settings.value("find/caseSensitive").toBool());
    ui.loopCheck->setChecked(settings.value("find/loop").toBool());
    if (!settings.value("find/down", true).toBool())
        ui.upRadio->setChecked(true);

	connect(ui.findNextButton, &QPushButton::clicked, this, &FindDialog::sltFindNext);
	connect(ui.replaceButton, &QPushButton::clicked, this, &FindDialog::sltReplace);
	connect(ui.replaceAllButton, &QPushButton::clicked, this, &FindDialog::sltReplaceAll);
	connect(ui.cancelButton, &QPushButton::clicked, this, &FindDialog::close);

	connect(ui.caseSensitiveCheck, &QCheckBox::toggled, this, &FindDialog::sltCaseSensitiveCheck);
	connect(ui.loopCheck, &QCheckBox::toggled, this, &FindDialog::sltLoopCheck);
	connect(ui.downRadio, &QRadioButton::toggled, this, &FindDialog::sltDown);
	connect(ui.upRadio, &QRadioButton::toggled, this, &FindDialog::sltUp);
}

FindDialog::~FindDialog()
{
}

void FindDialog::openFind(bool replace)
{
    ui.label_3->setVisible(replace);
    ui.replaceEdit->setVisible(replace);
    ui.replaceButton->setVisible(replace);
    ui.replaceAllButton->setVisible(replace);
    ui.groupBox->setVisible(!replace);
    QDialog::show(); // open/exec会导致模态
    ui.findEdit->setFocus();
    ui.findEdit->selectAll();
    this->adjustSize();

    if (!replace)
        setWindowTitle(QStringLiteral("查找"));
    else
        setWindowTitle(QStringLiteral("替换"));
}

const QString FindDialog::getFindText() const
{
    return ui.findEdit->text();
}

const QString FindDialog::getReplaceText() const
{
    return ui.replaceEdit->text();
}

bool FindDialog::isCaseSensitive() const
{
    return ui.caseSensitiveCheck->isChecked();
}

bool FindDialog::isLoop() const
{
    return ui.loopCheck->isChecked();
}

void FindDialog::sltFindNext()
{
    if (ui.upRadio->isChecked())
        emit signalFindPrev();
    else
        emit signalFindNext();
    settings.setValue("find/findText", ui.findEdit->text());
}

void FindDialog::sltReplace()
{
    emit signalReplaceNext();
    settings.setValue("find/replaceText", ui.replaceEdit->text());
}

void FindDialog::sltReplaceAll()
{
    emit signalReplaceAll();
    settings.setValue("find/replaceText", ui.replaceEdit->text());
}

void FindDialog::sltCaseSensitiveCheck()
{
    settings.setValue("find/caseSensitive", ui.caseSensitiveCheck->isChecked());
}

void FindDialog::sltLoopCheck()
{
    settings.setValue("find/loop", ui.loopCheck->isChecked());
}

void FindDialog::sltUp()
{
    settings.setValue("find/down", false);
}

void FindDialog::sltDown()
{
    settings.setValue("find/down", true);
}

void FindDialog::showEvent(QShowEvent *event)
{
    emit signalShow();
    adjustSize();
    QWidget::showEvent(event);
}

void FindDialog::hideEvent(QHideEvent *event)
{
    emit signalHide();
    QWidget::hideEvent(event);
}

void FindDialog::sltFindEditReturnPressed()
{
    sltFindNext();
}

void FindDialog::sltReplaceEditReturnPressed()
{
    sltReplace();
}
