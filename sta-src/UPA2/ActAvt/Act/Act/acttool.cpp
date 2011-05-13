/*
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org.
 */

#include "acttool.h"

using namespace std;

ActTool::ActTool(QWidget *parent) :
    QDialog(parent),
    m_scenario(NULL),
    m_propscenario(NULL),
    analysis(NULL)//,
    //m_ui(new Ui::ActTool)
{
    //m_ui->setupUi(this);
    setupUi(this);
    connect(SpaceObjectCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateComboBox(int)));
    connect(TrajectoryCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSpinBox(int)));
}

ActTool::~ActTool()
{
    if(analysis != NULL)
    {
        delete analysis;
    }
    //delete m_ui;
}

void ActTool::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        //m_ui->retranslateUi(this);
        retranslateUi(this);
        break;
    default:
        break;
    }
}

// Set the pointer to the scenario
void ActTool::setScenario(SpaceScenario* scenario, PropagatedScenario* propscenario)
{
    m_scenario = scenario;
    m_propscenario = propscenario;
    analysis = new act::ActAnalysis(scenario, propscenario);

    int vehicleindex=0;
    // Process each participant
        foreach (QSharedPointer<ScenarioParticipantType> participant, m_scenario->AbstractParticipant())
        {
            // For space vehicles, we need to propagate trajectories
            if (dynamic_cast<ScenarioSC*>(participant.data()))
            {
                ScenarioSC* vehicle = dynamic_cast<ScenarioSC*>(participant.data());
                SpaceObjectCombo->addItem(vehicle->Name(), vehicleindex);

            }
            else if (dynamic_cast<ScenarioREV*>(participant.data()))
            {
                ScenarioREV* entryVehicle = dynamic_cast<ScenarioREV*>(participant.data());
                SpaceObjectCombo->addItem(entryVehicle->Name(), vehicleindex);

            }
            vehicleindex++;
        }
    SpaceObjectCombo->setCurrentIndex(0);
}

void ActTool::on_SaveButton_clicked()
{
    /*QFileDialog DirDialog(this);
    DirDialog.setFileMode(QFileDialog::Directory);
    DirDialog.setOption(QFileDialog::ShowDirsOnly, true);
    DirDialog.exec();
    QString DirName = DirDialog.selectedFiles().at(0);
    if(DirName == NULL)
        DirName=QString("./");

    foreach(SpaceObject* obj, m_nominalscenario->spaceObjects())
    {
        QString FileName = DirName+"/"+obj->name()+".txt";
        QFile fileout(FileName);
        if (!fileout.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream out(&fileout);
        out << obj->name() << endl;

        foreach (MissionArc* arc, obj->mission())
        {
            QFile filein(arc->ephemerisFile());
            if (!filein.open(QIODevice::ReadOnly | QIODevice::Text))
                return;

            QTextStream in(&filein);
            while (!in.atEnd()) {
                QString line = in.readLine();
                out << line << endl;
            }
        }
    }*/

    MatrixXd cov(6,6);
    MatrixXi Param2i(6,6);
    MatrixXi Param3i(6,6);
    MatrixXi Param4i(6,6);
    MatrixXd samplesin(2,6);

    cov << 0.000000052796333e-8,   0.000000061274246e-8,  -0.000000124165062e-8,   0.000000000857219e-8,  -0.000000008396362e-8,  -0.000146423367682e-8,
            0.000000061274246e-8,   0.000000106297385e-8,  -0.000000175903670e-8,  -0.000000005522117e-8,  -0.000000022619233e-8,  -0.000169150629331e-8,
           -0.000000124165062e-8,  -0.000000175903670e-8,   0.000000497210777e-8,  -0.000000031795235e-8,   0.000000046138752e-8,   0.000324528311897e-8,
            0.000000000857219e-8,  -0.000000005522117e-8,  -0.000000031795235e-8,   0.000000291090472e-8,  -0.000000061875985e-8,  -0.000005268961404e-8,
           -0.000000008396362e-8,  -0.000000022619233e-8,  0.000000046138752e-8,  -0.000000061875985e-8,   0.000000033684362e-8,   0.000022212525280e-8,
           -0.000146423367682e-8,  -0.000169150629331e-8,  0.000324528311897e-8,  -0.000005268961404e-8,   0.000022212525280e-8,   0.408365825525108e-8;

//    cov << 99.8,        0,           0,           0,  -0.003636,  -6.346e-08,
//               0,     5441,     0.09496,     -0.3961,          0,           0,
//               0,  0.09496,   1.657e-06,  -6.912e-06,          0,           0,
//               0,  -0.3961,  -6.912e-06,   2.883e-05,          0,           0,
//       -0.003636,        0,           0,           0,  1.325e-07,   2.312e-12,
//      -6.346e-08,        0,           0,           0,  2.312e-12,   4.035e-17;

//    cov << 100, 0,  0,  0,  0,          0,
//            0,  0,  0,  0,  0,          0,
//            0,  0,  0,  0,  0,          0,
//            0,  0,  0,  0,  0,          0,
//            0,  0,  0,  0, 0,          0,
//            0,  0,  0,  0,  0,  3.043e-06;

    QList<double> times;
    QList<MatrixXd> samples;

    analysis->setCurrentSpaceObject(SpaceObjectCombo->currentIndex());
    analysis->setCurrentArc(TrajectoryCombo->currentIndex());

    SpaceObject* propvehicle = m_propscenario->spaceObjects().at(SpaceObjectCombo->itemData(SpaceObjectCombo->currentIndex()).toInt());
    times << propvehicle->mission().at(TrajectoryCombo->itemData(TrajectoryCombo->currentIndex()).toInt())->beginning();
    times << propvehicle->mission().at(TrajectoryCombo->itemData(TrajectoryCombo->currentIndex()).toInt())->ending();

//    times << InitialTimeSpinBox->value();
//    times << FinalTimeSpinBox->value();

    analysis->setTimeSteps(times);

    sta::StateVector InitialState = propvehicle->mission().at(TrajectoryCombo->itemData(TrajectoryCombo->currentIndex()).toInt())->trajectorySample(0);
    sta::StateVector FinalState = propvehicle->mission().at(TrajectoryCombo->itemData(TrajectoryCombo->currentIndex()).toInt())->trajectorySample(propvehicle->mission().at(TrajectoryCombo->itemData(TrajectoryCombo->currentIndex()).toInt())->trajectorySampleCount()-1);

    samplesin << InitialState.position(0), InitialState.position(1), InitialState.position(2), InitialState.velocity(0), InitialState.velocity(1), InitialState.velocity(2),
                 InitialState.position(0)+1000.0, InitialState.position(1), InitialState.position(2), InitialState.velocity(0), InitialState.velocity(1), InitialState.velocity(2);

//    analysis->setLinear();
//    analysis->InitializeCov(cov,sta::COORDSYS_EME_J2000, act::ELEM_CARTESIAN);

    //analysis->InitializeSamples(samplesin,sta::COORDSYS_EME_J2000, act::ELEM_CARTESIAN);

    analysis->setNonLinear();
    analysis->InitializeDist(act::DIST_NORMAL,cov, Param2i, Param3i, Param4i, 1000 ,sta::COORDSYS_EME_J2000, act::ELEM_CARTESIAN);



    analysis->propagate();


    QList<MatrixXd> cov2 = analysis->covariance();
    samples = analysis->samples();

    QFile outfile(QString("debug.txt"));

    if (!outfile.open(QIODevice::WriteOnly | QIODevice::Text))
             exit(0);

    QTextStream out(&outfile);

    out.setFieldWidth(16);
    out.setFieldAlignment(QTextStream::AlignRight);
    out.setPadChar(' ');

    out.setRealNumberPrecision(8);
    out.setRealNumberNotation(QTextStream::ScientificNotation);

    out << "Initial Covariance";
    out.setFieldWidth(0);
    out << endl;
    out.setFieldWidth(16);
    for(unsigned int j=0; j<cov2[0].rows();j++)
    {
        for(unsigned int k=0; k< cov2[0].cols(); k++)
        {
            out << cov2[0](j,k);
        }
        out.setFieldWidth(0);
        out << endl;
        out.setFieldWidth(16);

    }

    out << "Final Covariance";
    out.setFieldWidth(0);
    out << endl;
    out.setFieldWidth(16);
    for(unsigned int j=0; j<cov2.last().rows();j++)
    {
        for(unsigned int k=0; k< cov2.last().cols(); k++)
        {
            out << cov2.last()(j,k);
        }
        out.setFieldWidth(0);
        out << endl;
        out.setFieldWidth(16);

    }



//    for(unsigned int i=0;i<samples.size() ;i++)
//    {
//        out << "Time Step " << i;
//        out.setFieldWidth(0);
//        out << endl;
//        out.setFieldWidth(16);
//        for(unsigned int j=0; j<samples[i].rows();j++)
//        {
//            for(unsigned int k=0; k< samples[i].cols(); k++)
//            {
//                out << samples[i](j,k);
//            }
//            out.setFieldWidth(0);
//            out << endl;
//            out.setFieldWidth(16);

//        }



//    }

//    out << "Initial Condition Nominal";
//    out.setFieldWidth(0);
//    out << endl;
//    out.setFieldWidth(16);

//    out << InitialState.position(0);
//    out << InitialState.position(1);
//    out << InitialState.position(2);
//    out << InitialState.velocity(0);
//    out << InitialState.velocity(1);
//    out << InitialState.velocity(2);
//    out.setFieldWidth(0);
//    out << endl;
//    out.setFieldWidth(16);

//    out << "Final Condition Nominal";
//    out.setFieldWidth(0);
//    out << endl;
//    out.setFieldWidth(16);
//    out << FinalState.position(0);
//    out << FinalState.position(1);
//    out << FinalState.position(2);
//    out << FinalState.velocity(0);
//    out << FinalState.velocity(1);
//    out << FinalState.velocity(2);
//    out.setFieldWidth(0);
//    out << endl;
//    out.setFieldWidth(16);

     outfile.close();



//    for(unsigned int i=0;i<3 ;i++)
//    {
//        cout << setprecision(12) << InitialConditions.velocity(i) << " " ;
//    }
//    cout << endl;
//    cout << endl;

//    cout << "State Transition Matrix:" << endl;
//    cout << STM << endl;
////    for (unsigned int i=0;i<6;i++)
////    {
////        for(unsigned int j=0;j<6;j++)
////        {
////            cout << setprecision(12) << STM(i,j) << " ";
////        }
////        cout << endl;
////    }

//    cout << "Covariance Matrix:" << endl;
//    cout << CovMat.cov() << endl;
//    cout << endl;
//    cout << "Propagated Covariance Matrix:" << endl;
//    cout << CovMat.cov()*STM*CovMat.cov().transpose() << endl;
}

void ActTool::updateComboBox(int currentvehicleindex)
{
    if(currentvehicleindex >= 0)
    {
        TrajectoryCombo->clear();
        QSharedPointer<ScenarioParticipantType> participant = m_scenario->AbstractParticipant().at(SpaceObjectCombo->itemData(currentvehicleindex).toInt());


        if (dynamic_cast<ScenarioSC*>(participant.data()))
        {
            ScenarioSC* vehicle = dynamic_cast<ScenarioSC*>(participant.data());

            const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList = vehicle->SCMission()->TrajectoryPlan()->AbstractTrajectory();


            int trajectoryindex=0;

            if (!trajectoryList.isEmpty())
            {
                foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory, trajectoryList)
                {
                    if (dynamic_cast<ScenarioLoiteringType*>(trajectory.data()))    // Loitering
                    {
                        ScenarioLoiteringType* loitering = dynamic_cast<ScenarioLoiteringType*>(trajectory.data());

                        TrajectoryCombo->addItem(loitering->ElementIdentifier()->Name(), trajectoryindex);

                    }
                    else if (dynamic_cast<ScenarioLoiteringTLEType*>(trajectory.data()))    // Loitering
                    {
                        ScenarioLoiteringTLEType* loitering = dynamic_cast<ScenarioLoiteringTLEType*>(trajectory.data());
                        TrajectoryCombo->addItem(loitering->ElementIdentifier()->Name(), trajectoryindex);

                    }
                    else if (dynamic_cast<ScenarioDeltaVType*>(trajectory.data()))    // Loitering
                    {
                        ScenarioDeltaVType* loitering = dynamic_cast<ScenarioDeltaVType*>(trajectory.data());
                        TrajectoryCombo->addItem(loitering->ElementIdentifier()->Name(), trajectoryindex);

                    }
                    else if (dynamic_cast<ScenarioExternalType*>(trajectory.data()))    // Loitering
                    {
                        ScenarioExternalType* loitering = dynamic_cast<ScenarioExternalType*>(trajectory.data());
                        TrajectoryCombo->addItem(loitering->ElementIdentifier()->Name(), trajectoryindex);


                    }


                    trajectoryindex++;
                }
            }
            TrajectoryCombo->setCurrentIndex(0);


        }
        else if (dynamic_cast<ScenarioREV*>(participant.data()))
        {
            ScenarioREV* entryVehicle = dynamic_cast<ScenarioREV*>(participant.data());

            const QList<QSharedPointer<ScenarioAbstractTrajectoryType> >& trajectoryList = entryVehicle->REVMission()->REVTrajectoryPlan()->AbstractTrajectory();


            int trajectoryindex=0;

            if (!trajectoryList.isEmpty())
            {
                foreach (QSharedPointer<ScenarioAbstractTrajectoryType> trajectory, trajectoryList)
                {
                    if (dynamic_cast<ScenarioEntryArcType*>(trajectory.data()))    // Loitering
                    {
                        ScenarioEntryArcType* loitering = dynamic_cast<ScenarioEntryArcType*>(trajectory.data());

                        TrajectoryCombo->addItem(loitering->ElementIdentifier()->Name(), trajectoryindex);

                    }


                    trajectoryindex++;
                }
            }
            TrajectoryCombo->setCurrentIndex(0);



        }





    }

}

void ActTool::updateSpinBox(int currenttrajectoryindex)
{
    if(currenttrajectoryindex >= 0)
    {

        SpaceObject* propvehicle = m_propscenario->spaceObjects().at(SpaceObjectCombo->itemData(SpaceObjectCombo->currentIndex()).toInt());
        InitialTimeSpinBox->setMinimum(propvehicle->mission().at(currenttrajectoryindex)->beginning());
        InitialTimeSpinBox->setMaximum(propvehicle->mission().at(currenttrajectoryindex)->ending());
        FinalTimeSpinBox->setMinimum(propvehicle->mission().at(currenttrajectoryindex)->beginning());
        FinalTimeSpinBox->setMaximum(propvehicle->mission().at(currenttrajectoryindex)->ending());
        InitialTimeSpinBox->setValue(InitialTimeSpinBox->minimum());
        FinalTimeSpinBox->setValue(InitialTimeSpinBox->maximum());

    }
}
