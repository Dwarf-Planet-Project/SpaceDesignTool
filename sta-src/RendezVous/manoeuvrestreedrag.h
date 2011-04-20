#ifndef MANOEUVRESTREEDRAG_H
#define MANOEUVRESTREEDRAG_H

#include <QtGui>
#include <QTreeWidget>
#include <QAbstractItemView>

class ManoeuvresTreeDrag : public QTreeWidget
{
    Q_OBJECT

public:
    ManoeuvresTreeDrag(QWidget* parent);
    ~ManoeuvresTreeDrag();

private:

    void mouseMoveEvent(QMouseEvent *event);

};
#endif // MANOEUVRESTREEDRAG_H
