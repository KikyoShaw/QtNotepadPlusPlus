#pragma once

#pragma execution_character_set("utf-8")

#include <QDialog>
#include <QSettings>
#include "ui_finddialog.h"

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QSettings& settings, QWidget *parent = nullptr);
    ~FindDialog() override;

    void openFind(bool replace);

signals:
    void signalShow();
    void signalHide();
    void signalFindNext();
    void signalFindPrev();
    void signalReplaceNext();
    void signalReplaceAll();

public:
    const QString getFindText() const;
    const QString getReplaceText() const;
    bool isCaseSensitive() const;
    bool isLoop() const;

private slots:
	void sltFindNext();
	void sltReplace();
	void sltReplaceAll();
	void sltCaseSensitiveCheck();
	void sltLoopCheck();
	void sltUp();
	void sltDown();
	void sltFindEditReturnPressed();
	void sltReplaceEditReturnPressed();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    Ui::FindDialog ui;
    QSettings& settings;
};
