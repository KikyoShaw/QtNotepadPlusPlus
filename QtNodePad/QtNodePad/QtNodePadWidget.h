#pragma once

#pragma execution_character_set("utf-8")

#include <QtWidgets/QMainWindow>
#include "ui_QtNodePadWidget.h"

class QtNodePad : public QMainWindow
{
    Q_OBJECT

public:
    QtNodePad(QWidget *parent = Q_NULLPTR);
	~QtNodePad();

private:
    Ui::QtNodePadClass ui;
};
