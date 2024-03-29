#pragma once

#include <QTabBar>

class DnDTabBar : public QTabBar
{
    Q_OBJECT

public:
    DnDTabBar(QWidget * parent = Q_NULLPTR);
    ~DnDTabBar();

signals:
    void openFileRequest(QString/*fileName*/,int/*tabIndex*/);
    void dragTabRequest(int/*tabIndex*/);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent (QMouseEvent *event) override;
    void mouseMoveEvent (QMouseEvent *event) override;
//    void mouseReleaseEvent (QMouseEvent *event) override;
};
