#ifndef ENTRYMASS_H
#define ENTRYMASS_H

#include "ui_entrymass.h"
#include "Scenario/staschema.h"
#include "scenariotree.h"


class EntryMassDialog : public QDialog, private Ui_EntryMassDialog
{
    Q_OBJECT

public:
    EntryMassDialog(ScenarioTree* parent=NULL);
    ~EntryMassDialog(){}

    bool loadValues(ScenarioREVWeights* geometry);
    bool saveValues(ScenarioREVWeights* geometry);

};

#endif // ENTRYMASS_H
