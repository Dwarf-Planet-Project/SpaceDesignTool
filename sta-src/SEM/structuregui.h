#ifndef STRUCTUREGUI_H
#define STRUCTUREGUI_H

#include "ui_StructureGUI.h"
#include <QDialog>

#include "SemMain.h"

class StructureGUI : public QDialog , private Ui::StructureGUI
{
Q_OBJECT
public:
        StructureGUI(SemMain * SC, QWidget * parent = 0, Qt::WindowFlags f = 0 );
        ~StructureGUI();

protected slots:
	void on_StructureGUISCShapeComboBox_activated(const QString&);
        void on_StructureGUIMaterialComboBox_activated(const QString&);
        void on_StructureGUISavePushButton_clicked();

private:
        SemMain * SCVirtual;
//        void RefreshStructureGUI();


};
#endif //STRUCTUREGUI_H
