#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtNodePadWidget.h"

class QtNodePad : public QMainWindow
{
    Q_OBJECT

public:
    QtNodePad(QWidget *parent = Q_NULLPTR);

private:
    Ui::QtNodePadClass ui;
};
