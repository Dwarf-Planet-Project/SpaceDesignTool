#include "analysisprop.h"

AnalysisPropDialog::AnalysisPropDialog(QDialog* parent){
    setupUi(this);
    progressBar->hide();
    connect(OKButton, SIGNAL(clicked()), this, SLOT(close()));
}

AnalysisPropDialog::~AnalysisPropDialog(){
}

void AnalysisPropDialog::getCheckVal(bool& cov, bool& SOLink, bool& GOLink){
    cov = covCheckBox->isChecked();
    SOLink = SOLinkCheckBox->isChecked();
    GOLink = GOLinkCheckBox->isChecked();
}

