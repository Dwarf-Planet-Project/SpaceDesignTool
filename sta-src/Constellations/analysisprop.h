#ifndef ANALYSISPROP_H
#define ANALYSISPROP_H

#include "ui_analysisprop.h"
#include <QDialog>


class AnalysisPropDialog : public QDialog, private Ui_AnalysisPropDialog
{
    Q_OBJECT

public:
   AnalysisPropDialog(QDialog* parent = NULL);
   ~AnalysisPropDialog();

   void getCheckVal(bool& cov, bool& SOLink, bool& GOLink);

};

#endif // ANALYSISPROP_H
