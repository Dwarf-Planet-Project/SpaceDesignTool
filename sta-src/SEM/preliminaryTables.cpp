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

/*
 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Guido Ridolfi  -------------------------------------------------
 ------------------ E-mail: (guido.ridolfi@gmail.com) --------------------------------------
 ------------------ Author: Tiziana Sabatini  ----------------------------------------------
 ------------------ E-mail: (tiziana.sabatini@yahoo.it) ------------------------------------
 */

// Modified by Tiziana Sabatini, May 2009

#include "preliminaryTables.h"
#include <cmath>

using namespace std;


Percentages createTables(int choice, double PLMassPerc)
{
        Percentages Table;
        Percentages TableFinal;

        switch (choice)
              {
                 case 1:

                        Table.Payload=27.4;
                    Table.Structure=21.3;
                    Table.Thermal=3.6;
                    Table.Power=31.9;
                    Table.TTC=4.8;
                    Table.ADCS=6.9;
                    Table.Propulsion=3.8;
                    break;

                 case 2:

                        Table.Payload=21.2;
                Table.Structure=23.5;
                Table.Thermal=9.9;
                    Table.Power=32.1;
                    Table.TTC=4.7;
                    Table.ADCS=5.6;
                    Table.Propulsion=3.2;
                    break;

                 case 3:

                        Table.Payload=34.6;
                Table.Structure=18.9;
                Table.Thermal=2.1;
                    Table.Power=24.3;
                    Table.TTC=3.4;
                    Table.ADCS=4.5;
                    Table.Propulsion=6.1;
                    break;

                 case 4:

                        Table.Payload=24.4;
                Table.Structure=22.7;
                Table.Thermal=1.7;
                    Table.Power=24.6;
                    Table.TTC=12.7;
                    Table.ADCS=11.3;
                    Table.Propulsion=2.7;
                        break;

                 }
            double SUM=Table.Payload+Table.ADCS+Table.Power+Table.Propulsion+Table.Structure+Table.Thermal+Table.TTC;
                double diff=100-SUM;


                double Payload=Table.Payload+Table.Payload/SUM*(diff);
                double ADCS=Table.ADCS+Table.ADCS/SUM*(diff);
                double Power=Table.Power+Table.Power/SUM*(diff);
                double Propulsion=Table.Propulsion+Table.Propulsion/SUM*(diff);
                double Structure=Table.Structure+Table.Structure/SUM*(diff);
                double Thermal=Table.Thermal+Table.Thermal/SUM*(diff);
                double TTC=Table.TTC+Table.TTC/SUM*(diff);

                double diffPerc=Payload-PLMassPerc;
                double sumPartial=Table.ADCS+Table.Power+Table.Propulsion+Table.Structure+Table.Thermal+Table.TTC;

                TableFinal.Payload=PLMassPerc;
                TableFinal.ADCS=ADCS+ADCS/(sumPartial)*diffPerc;
                TableFinal.Power=Power+Power/(sumPartial)*diffPerc;
                TableFinal.Propulsion=Propulsion+Propulsion/(sumPartial)*diffPerc;
                TableFinal.Structure=Structure+Structure/(sumPartial)*diffPerc;
                TableFinal.Thermal=Thermal+Thermal/(sumPartial)*diffPerc;
                TableFinal.TTC=TTC+TTC/(sumPartial)*diffPerc;


        return(TableFinal);

}

Percentages createTablesPower(double PLpower, double PLPowerPerc)
{
        Percentages TablePower;
        Percentages TablePowerFinal;

        const int Pmin=10;
        const int Pmax=10000;

        if(PLpower<Pmin)

        {
                TablePower.Payload=20;
                TablePower.ADCS=15;
                TablePower.Power=30;
                TablePower.Propulsion=5;
                TablePower.Structure=0;
                TablePower.Thermal=5;
                TablePower.TTC=25;
        }
        else

        {
                if(PLpower>Pmax)
                        {
                                TablePower.Payload=80;
                                TablePower.ADCS=5;
                                TablePower.Power=5;
                                TablePower.Propulsion=1;
                                TablePower.Structure=0;
                                TablePower.Thermal=1;
                                TablePower.TTC=8;
                        }
                else

                {
                        TablePower.Payload=20*(1+log(PLpower/Pmin));
                        TablePower.ADCS=5*(1+1/3*log(PLpower/Pmin));
                        TablePower.Power=5*(1+1/3*log(PLpower/Pmin));
                        TablePower.Propulsion=1*(1+1/3*log(PLpower/Pmin));
                        TablePower.Structure=0;
                        TablePower.Thermal=1*(1+1/3*log(PLpower/Pmin));
                        TablePower.TTC=8*(1+1/3*log(PLpower/Pmin));
                }

        }
        double SUM=TablePower.ADCS+TablePower.Power+TablePower.Propulsion+TablePower.Structure+TablePower.Thermal+TablePower.TTC;

        TablePowerFinal.Payload=PLPowerPerc;
        TablePowerFinal.ADCS=TablePower.ADCS-TablePower.ADCS/SUM*(PLPowerPerc-TablePower.Payload);
        TablePowerFinal.Power=TablePower.Power-TablePower.Power/SUM*(PLPowerPerc-TablePower.Payload);
        TablePowerFinal.Propulsion=TablePower.Propulsion-TablePower.Propulsion/SUM*(PLPowerPerc-TablePower.Payload);
        TablePowerFinal.Structure=TablePower.Structure-TablePower.Structure/SUM*(PLPowerPerc-TablePower.Payload);
        TablePowerFinal.Thermal=TablePower.Thermal-TablePower.Thermal/SUM*(PLPowerPerc-TablePower.Payload);
        TablePowerFinal.TTC=TablePower.TTC-TablePower.TTC/SUM*(PLPowerPerc-TablePower.Payload);

        return(TablePowerFinal);
}

Breackdown createTablesMassBreackdown(Percentages TableFinal ,double PLMass)

{
        Breackdown MassBreackdown;

        MassBreackdown.Payload=PLMass;
        MassBreackdown.Structure=PLMass/TableFinal.Payload*TableFinal.Structure;
        MassBreackdown.Thermal=PLMass/TableFinal.Payload*TableFinal.Thermal;
        MassBreackdown.Power=PLMass/TableFinal.Payload*TableFinal.Power;
        MassBreackdown.TTC=PLMass/TableFinal.Payload*TableFinal.TTC;
        MassBreackdown.ADCS=PLMass/TableFinal.Payload*TableFinal.ADCS;
        MassBreackdown.Propulsion=PLMass/TableFinal.Payload*TableFinal.Propulsion;
        MassBreackdown.Total=MassBreackdown.Payload+MassBreackdown.Structure+MassBreackdown.Thermal+MassBreackdown.Power+MassBreackdown.TTC+MassBreackdown.ADCS+MassBreackdown.Propulsion;
        return (MassBreackdown);
}

Breackdown createTablesPowerBreackdown(Percentages TablePowerFinal ,double PLPower)

{
        Breackdown PowerBreackdown;

        PowerBreackdown.Payload=PLPower;
        PowerBreackdown.Structure=PLPower/TablePowerFinal.Payload*TablePowerFinal.Structure;
        PowerBreackdown.Thermal=PLPower/TablePowerFinal.Payload*TablePowerFinal.Thermal;
        PowerBreackdown.Power=PLPower/TablePowerFinal.Payload*TablePowerFinal.Power;
        PowerBreackdown.TTC=PLPower/TablePowerFinal.Payload*TablePowerFinal.TTC;
        PowerBreackdown.ADCS=PLPower/TablePowerFinal.Payload*TablePowerFinal.ADCS;
        PowerBreackdown.Propulsion=PLPower/TablePowerFinal.Payload*TablePowerFinal.Propulsion;
        PowerBreackdown.Total=PowerBreackdown.Payload+PowerBreackdown.Structure+PowerBreackdown.Thermal+PowerBreackdown.Power+PowerBreackdown.TTC+PowerBreackdown.ADCS+PowerBreackdown.Propulsion;
        return (PowerBreackdown);
}
