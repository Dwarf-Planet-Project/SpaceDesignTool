#include "manoeuvrestreedrag.h"


ManoeuvresTreeDrag::ManoeuvresTreeDrag(QWidget* parent){
   // parent->acceptDrops();
   this->setDragDropMode(QAbstractItemView::InternalMove);


}


ManoeuvresTreeDrag::~ManoeuvresTreeDrag(){}

void ManoeuvresTreeDrag::mouseMoveEvent(QMouseEvent *event)
{
    // if not left button - return
    if (!(event->buttons() & Qt::LeftButton)) return;

    // if no item selected, return (else it would crash)
    if (currentItem() == NULL) return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    // construct list of QUrls
    // other widgets accept this mime type, we can drop to them
    QList<QTreeWidgetItem> list;
    //list.append(currentItem()->text()); // only QUrl in list will be text of actual item

    // mime stuff
    //mimeData->setUserData(list);
    drag->setMimeData(mimeData);

    // start drag
    drag->start(Qt::CopyAction | Qt::MoveAction);
}
