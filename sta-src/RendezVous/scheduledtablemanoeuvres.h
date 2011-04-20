#ifndef SCHEDULEDTABLEMANOEUVRES_H
#define SCHEDULEDTABLEMANOEUVRES_H


#include <QTableWidget>

class ScheduledTableManoeuvres : public QTableWidget{
    Q_OBJECT

public:
    ScheduledTableManoeuvres(QWidget *parent);
    ~ScheduledTableManoeuvres();
//private:
   /* void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);*/

};

#endif // SCHEDULEDTABLEMANOEUVRES_H
