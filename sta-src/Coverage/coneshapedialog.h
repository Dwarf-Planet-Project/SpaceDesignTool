#ifndef CONESHAPEDIALOG_H
#define CONESHAPEDIALOG_H

#include "ui_ConeShape.h"
#include <QDialog>

class ConeShapeDialog : public QDialog , private Ui::ConeProperties
{
Q_OBJECT
public:
	ConeShapeDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~ConeShapeDialog();
protected slots:
	void on_OkButton_clicked();
	void on_CancelButton_clicked();
	void on_VertAngleLineEdit_textChanged(const QString&);
	void on_HorAngleLineEdit_textChanged(const QString&);
	void on_ConeAngleLineEdit_textChanged(const QString&);
	
};
#endif //CONESHAPEDIALOG_H
