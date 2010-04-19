#ifndef PARAMETRIZATIONDIALOG_H
#define PARAMETRIZATIONDIALOG_H

#include "ui_parametrizedgeometry.h"
#include <QDialog>

class ParametrizationDialog : public QDialog , private Ui::ParametrizationDialog
{
Q_OBJECT
public:
	ParametrizationDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~ParametrizationDialog();
protected slots:
	void on_GeometryTabWidget_currentChanged(int);
	void on_biConeLineEdit1_textChanged(const QString&);
	void on_biConeLineEdit2_textChanged(const QString&);
	void on_biConeLineEdit3_textChanged(const QString&);
	void on_biConeLineEdit4_textChanged(const QString&);
	void on_biConeLineEdit5_textChanged(const QString&);
	void on_probeLineEdit1_textChanged(const QString&);
	void on_probeLineEdit2_textChanged(const QString&);
	void on_probeLineEdit3_textChanged(const QString&);
	void on_probeLineEdit4_textChanged(const QString&);
	void on_blConeLineEdit1_textChanged(const QString&);
	void on_blConeLineEdit2_textChanged(const QString&);
	void on_blConeLineEdit3_textChanged(const QString&);
	void on_capsuleLineEdit1_textChanged(const QString&);
	void on_capsuleLineEdit2_textChanged(const QString&);
	void on_capsuleLineEdit3_textChanged(const QString&);
	void on_capsuleLineEdit4_textChanged(const QString&);
	void on_geometryFileEdit_textChanged(const QString&);
	void on_generateGeometryButton_clicked();
	void on_buttonBox_helpRequested();
	void on_buttonBox_clicked(QAbstractButton*);
	
};
#endif //PARAMETRIZATIONDIALOG_H
