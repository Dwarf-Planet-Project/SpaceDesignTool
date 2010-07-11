/*
 This program is free software; you can redistribute it and/or modify it under
 the terms of the European Union Public Licence - EUPL v.1.1 as published by
 the European Commission.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the European Union Public Licence - EUPL v.1.1
 for more details.

 You should have received a copy of the European Union Public Licence - EUPL v.1.1
 along with this program.

 Further information about the European Union Public Licence - EUPL v.1.1 can
 also be found on the world wide web at http://ec.europa.eu/idabc/eupl
 */

/*
 ------ Copyright (C) 2010 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Ricardo Noriega  ----------------------------------------------
 ------------------ email: ricardonor@gmail.com  ------------------------------------------

 */

#include "commanalysis.h"
#include <QDebug>
#include "Locations/environmentdialog.h"
#include "Payloads/receiverPayloadDialog.h"
#include <math.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Astro-Core/surfaceVelocity.h>


//Definition of constructors, the default one contains a warning.

using namespace Eigen;


bool tracking=true;//This allows to set a tracking system for the calculations, so the antenna of the groundstation follows the spacecraft!


CommAnalysis::CommAnalysis()
{
qDebug()<< "Warning: CommAnalysis object created without any participant to make the calculations";
}

CommAnalysis::CommAnalysis(ScenarioTransmitterPayloadType *transmitter, ScenarioReceiverPayloadType *receiver, ScenarioGroundStationEnvironment* environment, PropagatedScenario* propagatedScenario, int indexSC, int indexGS, int indexMA, bool flagTX, bool flagRX):m_transmitter(transmitter), m_receiver(receiver), m_propagatedScenario(propagatedScenario), m_environment(environment), m_indexSC(indexSC), m_indexGS(indexGS), m_indexMA(indexMA), m_flagTX(flagTX), m_flagRX(flagRX)
{

}

//Default destructor

CommAnalysis::~CommAnalysis(){

}

double lightSpeed=SPEED_OF_LIGHT;
double Pi=mypi;

////////////////////////////////////////////// FUNCTIONS ////////////////////////////////////////////////////////

double CommAnalysis::DopplerShift(){

    double dopplerShift;

    for(int i=0; i<m_propagatedScenario->spaceObjects().at(m_indexSC)->mission().at(m_indexMA)->trajectorySampleCount(); i++)
    {
        double frequency=m_transmitter->Budget()->FrequencyBand();
        double t=m_propagatedScenario->spaceObjects().at(m_indexSC)->mission().at(m_indexMA)->trajectorySampleTime(i);


        bool visibility;
        if(m_propagatedScenario->groundObjects().at(m_indexGS)->elevationAngle(m_propagatedScenario->spaceObjects().at(m_indexSC),t)>=5.0*DEG2RAD){
            visibility=true;
        } else{
            visibility=false;}

        if(visibility==true)
        {

            sta::StateVector samples;

            samples=m_propagatedScenario->spaceObjects().at(m_indexSC)->mission().at(m_indexMA)->trajectorySample(i);


            double latitudeGS=m_propagatedScenario->groundObjects().at(m_indexGS)->latitude;
            double longitudeGS=m_propagatedScenario->groundObjects().at(m_indexGS)->longitude;
            double altitudeGS=m_propagatedScenario->groundObjects().at(m_indexGS)->altitude;

            latitudeGS=DEG2RAD*(latitudeGS);
            longitudeGS=DEG2RAD*(longitudeGS);

            longitudeGS=longitudeGS+getGreenwichHourAngle(m_propagatedScenario->spaceObjects().at(m_indexSC)->mission().at(m_indexMA)->trajectorySampleTime(i)+2400000.5);

            Vector3d normPosition(cos(latitudeGS) * cos(longitudeGS),
                                  cos(latitudeGS) * sin(longitudeGS),
                                  sin(latitudeGS));

            Vector3d stationPos=normPosition.cwise() * STA_SOLAR_SYSTEM->lookup("Earth")->radii() + normPosition * altitudeGS;

            Vector3d toSpacecraft = (samples.position - stationPos).normalized();

            Vector3d velSat=samples.velocity;
            surfaceVelocity earthVel;
            double speedGS=earthVel.earthStationLinearVelocity(latitudeGS);
            Vector3d velGS(speedGS*sin(longitudeGS),speedGS*cos(longitudeGS),0);
            toSpacecraft.normalize();

            double radialVelToGS=((velGS-velSat).dot(toSpacecraft));



            radialVelToGS=radialVelToGS*1000; //The result is in Km/sec so I convert it to m/s

            dopplerShift=radialVelToGS*frequency/lightSpeed;
            dopplerList.append(dopplerShift);

        }
    }
    return dopplerShift;
}


double CommAnalysis::FreeSpaceLoss(){

    //The inputs for this function will be the frequency and the range between both participants
    double frequency=m_transmitter->Budget()->FrequencyBand();

    double freeSpaceLoss=0, freeSpaceLossDb;
    double range;
    int i;

    for(i=0; i<m_propagatedScenario->spaceObjects().at(m_indexSC)->mission().at(m_indexMA)->trajectorySampleCount(); i++)
    {
        double t=m_propagatedScenario->spaceObjects().at(m_indexSC)->mission().at(m_indexMA)->trajectorySampleTime(i);

        bool visibility;
        if(m_propagatedScenario->groundObjects().at(m_indexGS)->elevationAngle(m_propagatedScenario->spaceObjects().at(m_indexSC),t)>=5.0*DEG2RAD){
            visibility=true;
        } else{
            visibility=false;}



        if(visibility==true){
            range=m_propagatedScenario->groundObjects().at(m_indexGS)->getRange(m_propagatedScenario->spaceObjects().at(m_indexSC), t);
            mjdList.append(t);
            range=range*1000;
            rangeList.append(range);//In metres
            freeSpaceLoss=pow((4*Pi*range*frequency/lightSpeed), 2);  //This formula is defined to be in the denominator of the C/No when it is expressed in natural units
            freeSpaceLossDb=10*log10(freeSpaceLoss); //This value shall be preceded by a minus when it is in decibels
            freeSpaceLossList.append(freeSpaceLossDb);
        }


    }

    return freeSpaceLossDb;
}

double CommAnalysis::OxygenSpecificAttenuation(double frequency){ //THE FREQUENCY HAS TO BE IN GHZ!!!!!!!


    double oxSpecAtt;

    oxcheck=m_environment->OxChoice();

    if(oxcheck==QString("true")){

        oxSpecAtt=((7.1/(pow(frequency,2)+0.36))+(4.5/(pow((frequency-57),2)+0.98)))*pow(frequency,2)/1000;  //dB/Km
        /*oxSpecAtt=(7.1/(pow(frequency,2)+0.36));
        oxSpecAtt=oxSpecAtt+(4.5/(pow((frequency-57),2)+0.98));
        oxSpecAtt=oxSpecAtt*pow(frequency,2)/1000;*/
    }else if(oxcheck==QString("false")){
        oxSpecAtt=0;}

    return oxSpecAtt;
}


double CommAnalysis::WaterVapourSpecificAttenuation(double frequency, double latitude, double longitude){

    /*For applications which require an annual mean value, the surface water vapour densities exceeded
    for 50% of the year, shown for quick reference in Fig. 1, ITU-R P.836-3, should be used.*/


    double wpSpecAtt;
    double waterDensity;

    wvcheck=m_environment->WaterVapourChoice();

    if(wvcheck=="true"){
    QString pathLat("data/maps/surfwv_lat.txt");


    QFile fileLat(pathLat);

    if(!fileLat.exists())
        qDebug()<<" file error";
    fileLat.open(QIODevice::ReadOnly);

    QTextStream latStream(&fileLat);
    double latPoint;

    int i=0;
    do  {
            latStream>>latPoint;
            i++;
        }while(latPoint>latitude);

    fileLat.close();

    if(longitude<0)
        longitude=longitude+360;


    QString pathLon("data/maps/surfwv_lon.txt");

    QFile fileLon(pathLon);

    if(!fileLon.exists())
        qDebug()<<" file error";
    fileLon.open(QIODevice::ReadOnly);


    QTextStream lonStream(&fileLon);
    double lonPoint;


    int k;
    for(k=0; k<i; k++)
        lonStream>>lonPoint;


    int j=0;
    do  {
             lonStream>>lonPoint;
             j++;
         }while(lonPoint<longitude);

    fileLon.close();

    QString pathWaterVapour("data/maps/surfwv_50.txt");

    QFile fileWaterVapour(pathWaterVapour);

    if(!fileWaterVapour.exists())
        qDebug()<<" file error";
    fileWaterVapour.open(QIODevice::ReadOnly);

    QTextStream waterVapourStream(&fileWaterVapour);

    int z=0;
    int target=i+j;

    for(z; z<target; z++)
    {
        waterVapourStream>>waterDensity;
    }



    wpSpecAtt=(0.067+(3/(pow(((frequency)-22.3),2)+7.3)))*waterDensity*pow((frequency),2)/10000; //This result is in dB/Km
    } else if(wvcheck=="false")
        wpSpecAtt=0.0;




    return wpSpecAtt;
}



double CommAnalysis::AtmosphericAttenuation(double frequency, double wpSpecAtt, double oxSpecAtt){

    double h0=6;//equivalent height of a uniform medium for oxygen calculations (Km)
    double hw; //equivalent height for water vapour calculations
    double atmosphericAtt;
    frequency=frequency/1000000000;
    hw=2.2+(3/(pow((frequency-22.3),2)+3));//Km

    int i;
    double elevationAngle;
    for(i=0; i<m_propagatedScenario->spaceObjects().at(m_indexSC)->mission().at(m_indexMA)->trajectorySampleCount(); i++)
    {
        double t=m_propagatedScenario->spaceObjects().at(m_indexSC)->mission().at(m_indexMA)->trajectorySampleTime(i);

        if(tracking==true){
            elevationAngle=m_propagatedScenario->groundObjects().at(m_indexGS)->elevationAngle(m_propagatedScenario->spaceObjects().at(m_indexSC),t);}
        else if(tracking==false){

            if(m_flagTX==false && m_flagRX==true)
                elevationAngle=m_transmitter->Transmitter()->PointingDirection()->elevation();
            else if(m_flagRX==false && m_flagTX==true)
                elevationAngle=m_receiver->Receiver()->PointingDirection()->elevation();

        } //This takes the elevation angle of the participant that is on the ground station. Satellites cannot carry antennas with tracking systems since there is not attitude control in STA

        bool visibility;
        if(m_propagatedScenario->groundObjects().at(m_indexGS)->elevationAngle(m_propagatedScenario->spaceObjects().at(m_indexSC),t)>=5.0*DEG2RAD){
            visibility=true;
        } else{
            visibility=false;}


        if(visibility==true)
        {

            elevationAngleList.append(elevationAngle);
            oxAttList.append(h0*oxSpecAtt*exp(-(hw/h0))/sin(elevationAngle));
            wvAttList.append((hw*wpSpecAtt)/sin(elevationAngle));

            atmosphericAtt=(((h0*oxSpecAtt*exp(-(hw/h0)))+(hw*wpSpecAtt)))/sin(elevationAngle);
            atmosphericAttList.append(atmosphericAtt);


        }

    } 

    return atmosphericAtt;

}



double CommAnalysis::RainAttenuation(double latitude, double longitude){//This is the position of the groundstation, the parent object should be read

    //This function is following the recommendation ITU-R P.618-10 for Rain Attenuation estimation (section 2.2)

    ////////////////////////////// DATA AND COEFFICIENTS FOR THIS FUNCTION ////////////////////////////////////////



    double akh[4]={-5.3398, -0.3531, -0.23789, -0.94158};
    double bkh[4]={-0.10008, 1.26970, 0.86036, 0.64552};
    double ckh[4]={1.13098, 0.45400, 0.15354, 0.16817};

    double akv[4]={-3.80595, -3.44965, -0.39902, 0.50167};
    double bkv[4]={0.56934, -0.22911, 0.73042, 1.07319};
    double ckv[4]={0.81061, 0.51059, 0.11899, 0.27195};

    double aah[5]={-0.14318, 0.29591, 0.32177, -5.37610, 16.1721};
    double bah[5]={1.82442, 0.77564, 0.63773, -0.96230, -3.29980};
    double cah[5]={-0.55187, 0.19822, 0.13164, 1.47828, 3.43990};

    double aav[5]={-0.07771, 0.56727, -0.20238, -48.2991, 48.5833};
    double bav[5]={2.33840, 0.95545, 1.14520, 0.791669, 0.791459};
    double cav[5]={-0.76284, 0.54039, 0.26809, 0.116226, 0.116479};


    double logf;
    double frequency;

    if(m_flagTX==false && m_flagRX==true){
        logf=log10(m_transmitter->Budget()->FrequencyBand()/1000000000);
        frequency= m_transmitter->Budget()->FrequencyBand()/1000000000;
    }
    else if(m_flagRX==false && m_flagTX==true){
        logf=log10(m_receiver->Budget()->FrequencyBand()/1000000000);
        frequency= m_receiver->Budget()->FrequencyBand()/1000000000;
    }



    double logkh= (akh[0]*exp(-pow((logf-bkh[0])/ckh[0],2)))+(akh[1]*exp(-pow((logf-bkh[1])/ckh[1],2)))+(akh[2]*exp(-pow((logf-bkh[2])/ckh[2],2)))+(akh[3]*exp(-pow((logf-bkh[3])/ckh[3],2)))-(logf*0.18961)+0.71147;
    double logkv= (akv[0]*exp(-pow((logf-bkv[0])/ckv[0],2)))+(akv[1]*exp(-pow((logf-bkv[1])/ckv[1],2)))+(akv[2]*exp(-pow((logf-bkv[2])/ckv[2],2)))+(akv[3]*exp(-pow((logf-bkv[3])/ckv[3],2)))-(logf*0.16398)+0.63297;

    double alphah=(aah[0]*exp(-pow((logf-bah[0])/cah[0],2)))+(aah[1]*exp(-pow((logf-bah[1])/cah[1],2)))+(aah[2]*exp(-pow((logf-bah[2])/cah[2],2)))+(aah[3]*exp(-pow((logf-bah[3])/cah[3],2)))+(aah[4]*exp(-pow((logf-bah[4])/cah[4],2)))+(logf*0.67849)-1.95537;
    double alphav=(aav[0]*exp(-pow((logf-bav[0])/cav[0],2)))+(aav[1]*exp(-pow((logf-bav[1])/cav[1],2)))+(aav[2]*exp(-pow((logf-bav[2])/cav[2],2)))+(aav[3]*exp(-pow((logf-bav[3])/cav[3],2)))+(aav[4]*exp(-pow((logf-bav[4])/cav[4],2)))-(logf*0.053739)+0.83433;

    double kh=pow(10, logkh);

    double kv=pow(10, logkv);

    double tilt;

    if(m_flagTX==false && m_flagRX==true)
        tilt=m_transmitter->Transmitter()->EMproperties()->TiltAngle();
    else if(m_flagRX==false && m_flagTX==true)
        tilt=m_receiver->Receiver()->EMproperties()->TiltAngle();


    //////////////////////////////////////STEP 1: Determination of rain height given in ITU-R P.839//////////////////////////////

    QString pathLat("data/maps/ESALAT.TXT");


    QFile fileLat(pathLat);

    if(!fileLat.exists())
        qDebug()<<" file error";
    fileLat.open(QIODevice::ReadOnly);

    QTextStream latStream(&fileLat);
    double latPoint;

    int i1=0;
    do  {
        latStream>>latPoint;
        i1++;

    }while(latPoint>latitude);


    fileLat.close();

    if(longitude<0)
        longitude=longitude+360;

    QString pathLon("data/maps/ESALON.TXT");

    QFile fileLon(pathLon);

    if(!fileLon.exists())
        qDebug()<<" file error";
    fileLon.open(QIODevice::ReadOnly);


    QTextStream lonStream(&fileLon);
    double lonPoint;


    int k1=0;
    for(k1; k1<i1; k1++)
        lonStream>>lonPoint;

    int j1=0;
    do  {
        lonStream>>lonPoint;
        j1++;
    }while(lonPoint<longitude);


    fileLon.close();

    QString pathHeight("data/maps/ESA0HEIGHT.TXT");

    QFile fileHeight(pathHeight);

    if(!fileHeight.exists())
        qDebug()<<" file error";
    fileHeight.open(QIODevice::ReadOnly);

    QTextStream heightStream(&fileHeight);
    double heightPoint;

    int z1=0;
    int target1=i1+j1;

    for(z1; z1<target1; z1++)
        heightStream>>heightPoint;

    double hr;

    hr=heightPoint+0.36; //in Km

    //This step is working properly

    ///////////////////////////////////////////STEP 4: Get the rainfall rate exceeded for 0.01% of an average year//////////////////////////////////////////////

    QString pathLatRain("data/maps/ESARAIN_LAT_v5.TXT");


    QFile fileLatRain(pathLatRain);

    if(!fileLatRain.exists())
        qDebug()<<" file error";
    fileLatRain.open(QIODevice::ReadOnly);

    QTextStream latStreamRain(&fileLatRain);
    double latPointRain;

    int i2=0;
    do  {
        latStreamRain>>latPointRain;
        i2++;
    }while(latPointRain>latitude);

    fileLatRain.close();


    if(longitude<0)
        longitude=longitude+360;


    QString pathLonRain("data/maps/ESARAIN_LON_v5.TXT");

    QFile fileLonRain(pathLonRain);

    if(!fileLonRain.exists())
        qDebug()<<" file error";
    fileLonRain.open(QIODevice::ReadOnly);


    QTextStream lonStreamRain(&fileLonRain);
    double lonPointRain;


    int k2=0;
    for(k2; k2<i2; k2++)
        lonStreamRain>>lonPointRain;

    int j2=0;
    do  {
        lonStreamRain>>lonPointRain;
        j2++;

    }while(lonPointRain<longitude);
    fileLonRain.close();


    QString pathRain001("data/maps/R0_01.TXT");

    QFile fileRain001(pathRain001);

    if(!fileRain001.exists())
        qDebug()<<" file error";
    fileRain001.open(QIODevice::ReadOnly);

    QTextStream rain001Stream(&fileRain001);
    double r001;

    int t=0;
    int target2=i2+j2;

    for(t; t<target2; t++)
        rain001Stream>>r001;


    //////////////////////////////////////////STEP 2, 3: Get the slant-path range length, below the rain height and its projection////////////////////////////////////////////

    //////////////////////////////////////////STEP 5: get the Specific Attenuation in dB/Km according to ITU-R P.838-3////////////////////////////////////////////////////////

    ///////////////////////////////////////// STEP 6: Calculate the horizontal reduction factor for 0.01% of the time ////////////////////////////////////////////////////////

    //////////////////////////////////////////STEP 7: Calculate the vertical reduction factor for 0.01% of the time //////////////////////////////////////////////////////////

    //////////////////////////////////////////STEP 8: The effective path length //////////////////////////////////////////////////////////

    //////////////////////////////////////////STEP 9: The predicted attenuation exceeded for 0.01% of an average year //////////////////////////////////////////////////////////

    //////////////////////////////////////////STEP 10: Other percentages //////////////////////////////////////////////////////////////////////////////////////////////////////

    int counter;
    double elevationAngle;
    double finalAtt;



    for(counter=0; counter<m_propagatedScenario->spaceObjects().at(m_indexSC)->mission().at(m_indexMA)->trajectorySampleCount(); counter++)
    {
        double t=m_propagatedScenario->spaceObjects().at(m_indexSC)->mission().at(m_indexMA)->trajectorySampleTime(counter);



            if(tracking==true){
                elevationAngle=m_propagatedScenario->groundObjects().at(m_indexGS)->elevationAngle(m_propagatedScenario->spaceObjects().at(m_indexSC),t);}
            else if(tracking==false){

                if(m_flagTX==false && m_flagRX==true)
                    elevationAngle=m_transmitter->Transmitter()->PointingDirection()->elevation();
                else if(m_flagRX==false && m_flagTX==true)
                    elevationAngle=m_receiver->Receiver()->PointingDirection()->elevation();

            } //This takes the elevation angle of the participant that is on the ground station. Satellites cannot carry antennas with tracking systems since there is not attitude control in STA

            bool visibility;
            if(m_propagatedScenario->groundObjects().at(m_indexGS)->elevationAngle(m_propagatedScenario->spaceObjects().at(m_indexSC),t)>=5.0*DEG2RAD){
                visibility=true;
            } else{
                visibility=false;}


            double gsAlt=m_propagatedScenario->groundObjects().at(m_indexGS)->altitude;
            gsAlt=gsAlt/1000;//Now gsAlt is in Km

            if(visibility==true )
            {
                raincheck=m_environment->Rain()->RainChoice();

                if(raincheck=="true"){

                double slantPath;
                slantPath=(hr-gsAlt)/sin(elevationAngle);

                double Lg=slantPath*cos(elevationAngle); //Horizontal projection of the slant path length


                //////////////////////// This belongs to step 5 //////////////////////

                double ktot=(kh+kv+((kh-kv)*pow(cos(elevationAngle),2)*cos(2*tilt)))/2;

                double alphatot=((kh*alphah)+(kv*alphav)+((kh*alphah-kv*alphav)*pow(cos(elevationAngle),2)*cos(2*tilt)))/(2*ktot);

                double specAttenuation= ktot*pow(r001, alphatot); //Since it is specific attenuation, the units are dB/Km.

                ////////////////////////// This belongs to step 6 /////////////////////

                double horiz_reduc=1/(1+(0.78*sqrt(Lg*specAttenuation/frequency))-(0.38*(1-exp(-2*Lg))));


                ////////////////////////// This belongs to step 7 ////////////////////////

                double zeta=atan((hr-gsAlt)/(Lg*horiz_reduc));

                double Lr;
                double chi;

                if(zeta>elevationAngle)
                    Lr=(Lg*horiz_reduc)/cos(elevationAngle);
                else
                    Lr=slantPath;

                if(fabs(latitude)<36)
                    chi=36-fabs(latitude);
                else
                    chi=0;


                chi=chi*DEG2RAD;
                double ver_reduc=1/(1+(sqrt(sin(elevationAngle))*((31*(1-exp(-(elevationAngle/(1+chi))))*(sqrt((Lr*specAttenuation))/pow(frequency, 2)))-0.45)));

                ////////////////////////// This belongs to step 8 /////////////////////////

                double Le=Lr*ver_reduc;

                ////////////////////////// This belongs to step 9 /////////////////////////

                double estimatedAtt001=specAttenuation*Le;

                //////////////////////// This belongs to step 10 //////////////////////////

                double beta;
                double percentage=m_environment->Rain()->PercentageExceededLimit();
                if(percentage>1 || fabs(latitude)>=36)
                    beta=0;
                else if(percentage<1 && fabs(latitude)<36 && elevationAngle>=(25*DEG2RAD))
                    beta=-0.005*(fabs(latitude)-36);
                else
                    beta=-0.005*(fabs(latitude)-36)+1.8-(4.25*sin(elevationAngle));

                finalAtt=estimatedAtt001*pow((percentage/0.01),(-(0.655+(0.033*log(percentage))-(0.045*log(estimatedAtt001))-(beta*sin(elevationAngle)*(1-percentage))))); //In dB!!

            }else if(raincheck=="false"){
                finalAtt=0;
            }

            rainAttenuationList.append(finalAtt);

        }

        
    }
    return finalAtt;
}



double CommAnalysis::SystemTempCalculations(){

    double antennaTemp, skyTemp, groundTemp, rainAttenuation, systemTemp, equivalentTemp, rxFigureNoise, rxFeederLoss, thermoTempFeeder;



    rxFigureNoise=pow(10, m_receiver->Receiver()->SystemTemperature()->RxNoiseFigure()/10);//Now in natural units
    rxFeederLoss=pow(10, m_receiver->Receiver()->FeederLossRx()/10); //Now in natural units
    thermoTempFeeder=m_receiver->Receiver()->SystemTemperature()->ThermoFeeder();

    double elevationAngle;
    antennaTempChoice=m_receiver->Receiver()->SystemTemperature()->choiceTantenna();

    for (int j=0; j<elevationAngleList.length(); j++){

        elevationAngle=elevationAngleList[j];
        if(antennaTempChoice=="calculated")
        {
               if(m_flagRX==true)
                    antennaTemp=290;
                else if(m_flagRX==false)
                {

                        skyTemp=4+275*(1-pow(10,(-atmosphericAttList[j]/10)));
                        rainAttenuation=pow(10, rainAttenuationList[j]/10);

                        if (elevationAngle<=-10*DEG2RAD){ groundTemp=290;}
                        else if (elevationAngle>-10*DEG2RAD && elevationAngle<=0*DEG2RAD){ groundTemp=150;}
                        else if (elevationAngle>0*DEG2RAD && elevationAngle<=10*DEG2RAD){ groundTemp=50;}
                        else if (elevationAngle>10*DEG2RAD && elevationAngle<=90*DEG2RAD){ groundTemp=10;}

                        antennaTemp=(skyTemp/rainAttenuation)+groundTemp+275*(1-(1/rainAttenuation));

                 }
         }else if(antennaTempChoice=="constant"){
             antennaTemp=m_receiver->Receiver()->SystemTemperature()->Tantenna();
         }
        antennaTempList.append(antennaTemp);

        equivalentTemp=290*(rxFigureNoise-1);

    systemTemp=(antennaTemp/rxFeederLoss)+equivalentTemp+(thermoTempFeeder*(1-(1/rxFeederLoss))); //In Kelvin
    systemTempList.append(systemTemp);

    }

  return systemTemp;
}

double CommAnalysis::Modulations(double EbNo){ //the EbNo is passed in dB

    QString modType=m_transmitter->Transmitter()->Modulation()->ModulationType();
    double ber, a, b;

    if(modType==QString("BPSK")){
        a=sqrt(EbNo);
        b=erfc(a);
        ber=0.5*b;
    }
    else if (modType=="DE-BPSK"){
        a=sqrt(EbNo);
        ber=erfc(a);
    }
    else if (modType=="D-BPSK"){
        a=exp((-EbNo));
        ber=0.5*a;
    }
    else if (modType=="QPSK"){
        a=sqrt(EbNo);
        b=erfc(a);
        ber=0.5*b;
    }
    else if (modType=="DE-QPSK"){
        a=sqrt(EbNo);
        ber=erfc(a);
    }
    else if (modType=="D-QPSK"){
        a=exp((-EbNo));
        ber=0.5*a;
    }
    else if (modType=="OQPSK"){
        a=sqrt(EbNo);
        b=erfc(a);
        ber=0.5*b;
    }


    return ber;
}



void CommAnalysis::CommReports(){

    double frequency;

    if(m_flagTX==false && m_flagRX==true){
        frequency= m_transmitter->Budget()->FrequencyBand()/1000000000;
    }
    else if(m_flagRX==false && m_flagTX==true){
        frequency= m_receiver->Budget()->FrequencyBand()/1000000000;
    }


    ///////////////////// FUNCTIONS CALLED TO CALCULATE DATA /////////////////////////////////

    CommAnalysis::DopplerShift();
    CommAnalysis::FreeSpaceLoss();

    double oxAttenuation;
    oxAttenuation=CommAnalysis::OxygenSpecificAttenuation(frequency);
    double wpAttenuation;
    wpAttenuation=CommAnalysis::WaterVapourSpecificAttenuation(frequency, m_propagatedScenario->groundObjects().at(m_indexGS)->latitude,m_propagatedScenario->groundObjects().at(m_indexGS)->longitude);//Ricardo

    CommAnalysis::AtmosphericAttenuation(frequency, wpAttenuation, oxAttenuation);
    CommAnalysis::RainAttenuation(m_propagatedScenario->groundObjects().at(m_indexGS)->latitude,m_propagatedScenario->groundObjects().at(m_indexGS)->longitude);
    CommAnalysis::SystemTempCalculations();


    /////////////// FIXED VALUES THAT COME FROM TRANSMITTER AND RECEIVER //////////////////////

    double potTxDb, gainTxDb, txFeederLossDb, txDepointingLossDb, txDataRate;

     potTxDb=10*log10(m_transmitter->Transmitter()->TransmittingPower());
     gainTxDb=m_transmitter->Transmitter()->EMproperties()->GainMax();
     txFeederLossDb=m_transmitter->Transmitter()->FedderLossTx();
     txDepointingLossDb=m_transmitter->Transmitter()->DepointingLossTx();

     double gainRxDb, rxFeederLossDb, rxDepointingLossDb, polLoss, rcvdPower, fluxDensity;

     if(m_transmitter->Transmitter()->EMproperties()->Polarisation()=="rightCircular" && m_receiver->Receiver()->EMproperties()->Polarisation()=="leftCircular")
         polLoss=20;//in dB
     else if(m_receiver->Receiver()->EMproperties()->Polarisation()=="rightCircular" && m_transmitter->Transmitter()->EMproperties()->Polarisation()=="leftCircular")
         polLoss=20;//in dB
     else{
         double txAngle=m_transmitter->Transmitter()->EMproperties()->TiltAngle();
         double rxAngle=m_receiver->Receiver()->EMproperties()->TiltAngle();
         polLoss=-20*(log10(cos(fabs(txAngle-rxAngle))));
     }

    double transmittedFrequency;

     gainRxDb=m_receiver->Receiver()->EMproperties()->GainMax();
     rxFeederLossDb=m_receiver->Receiver()->FeederLossRx();
     rxDepointingLossDb=m_receiver->Receiver()->DepointingLossRx();
     transmittedFrequency=m_transmitter->Budget()->FrequencyBand();
     //Calculate some fix parameters

     double eirp;
     double overLapBWfactor, txBW, rxBW;


     eirp=potTxDb+gainTxDb-(txFeederLossDb+txDepointingLossDb);//The units are in dBW
     txBW=m_transmitter->Transmitter()->EMproperties()->BandWidth();
     rxBW=m_receiver->Receiver()->EMproperties()->BandWidth();
     txDataRate=m_transmitter->Transmitter()->Modulation()->DataRate();

     if(txBW>=rxBW)
         overLapBWfactor=rxBW/txBW;
     else if(rxBW>txBW)
         overLapBWfactor=txBW/rxBW;

     double propLoss;
     double carrierToNoise;
     double EbNo, BER;
     QList<double> rcvdFrequencyList;
     QList<double> pathLossList;
     QList<double> GoverTList;



     QFile reportComm1("reportComm1.txt");
     QFile reportComm2("reportComm2.txt");
     QFile reportComm3("reportComm3.txt");

     reportComm1.open(QIODevice::ReadWrite|QIODevice::WriteOnly);
     reportComm2.open(QIODevice::ReadWrite|QIODevice::WriteOnly);
     reportComm3.open(QIODevice::ReadWrite|QIODevice::WriteOnly);

     QTextStream streamReportComm1(&reportComm1);
     QTextStream streamReportComm2(&reportComm2);
     QTextStream streamReportComm3(&reportComm3);

     streamReportComm1.setRealNumberPrecision(16);
     streamReportComm2.setRealNumberPrecision(16);
     streamReportComm3.setRealNumberPrecision(16);

     streamReportComm1<<"MJD  "<<"EIRP    "<<"Rcvd.Freq.  "<<"DopplerShift    "<<"Rcvd.Power  "<<"FluxDensity  "<<"OverlapBWfactor  "<<endl;
     streamReportComm2<<"MJD         "<<"FSL    "<<"OxLoss  "<<"WvLoss    "<<"RainLoss  "<<"AtmosLoss    "<<"PropLoss "<<endl;
     streamReportComm3<<"MJD   "<<"G/T    "<<"C/No    "<<"C/N    "<<"Eb/No  "<<"BER "<<endl;
     for(int p=0; p<mjdList.length(); p++){

         rcvdFrequencyList.append(transmittedFrequency+dopplerList[p]);
         //rcvdPower=eirp-freeSpaceLossList[p];
         fluxDensity=eirp-(10*log10(4*pow(rangeList[p],2)*Pi));
         pathLossList.append(freeSpaceLossList[p]+atmosphericAttList[p]+rainAttenuationList[p]);
         GoverTList.append(gainRxDb-(rxFeederLossDb+rxDepointingLossDb+polLoss)-(10*log10(systemTempList[p])));//The units are dBK⁻¹
         carrierToNoiseDensityList.append(eirp-pathLossList[p]+GoverTList[p]+228.6+(10*log10(overLapBWfactor)));//The units are dBHz, the 228.6 factor is in dBW/HzK and it is the Bolztman constant in decibels


         propLoss=rainAttenuationList[p]+atmosphericAttList[p]+freeSpaceLossList[p];
         rcvdPower=eirp-propLoss;
         carrierToNoise=carrierToNoiseDensityList[p]-(10*log10(txBW));
         EbNo=carrierToNoiseDensityList[p]-(10*log10(txDataRate));
         BER=CommAnalysis::Modulations(EbNo);

         streamReportComm1<<mjdList[p]<<"\t"<<eirp<<"\t"<<rcvdFrequencyList[p]<<"\t"<<dopplerList[p]<<"\t"<<rcvdPower<<"\t"<<fluxDensity<<"\t"<<overLapBWfactor<<endl;
         streamReportComm2<<mjdList[p]<<"\t"<<freeSpaceLossList[p]<<"\t"<<oxAttList[p]<<"\t"<<wvAttList[p]<<"\t"<<rainAttenuationList[p]<<"\t"<<atmosphericAttList[p]<<"\t"<<propLoss<<endl;
         streamReportComm3<<mjdList[p]<<"\t"<<GoverTList[p]<<"\t"<<carrierToNoiseDensityList[p]<<"\t"<<carrierToNoise<<"\t"<<EbNo<<"\t"<<BER<<endl;

     }


       reportComm1.close();
       reportComm2.close();
       reportComm3.close();

}
