
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

 ------ Copyright (C) 2009 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Valentino Zuccarelli  -------------------------------------------------
 ------------------ E-mail: (Valentino.Zuccarelli@gmail.com) ----------------------------
 */

#include "threebodyParametersComputation.h"
#include "Astro-Core/stabody.h"
#include "Scenario/scenariobody.h"

double getGravParam_user (double mi_1, double mi_2)
{
	double grav_param;
	grav_param=(mi_2/(mi_1+mi_2)); 
        return grav_param;
}

int getSystem (int id_1, int id_2)
{
    if (id_1==10 && id_2==199)
        return 1;                    //Sun-Mercury
    else if (id_1==10 && id_2==299)
        return 2;                    //Sun-Venus
    else if (id_1==10 && id_2==399)
        return 3;                    //Sun-Earth
    else if (id_1==10 && id_2==499)
        return 4;                    //Sun-Mars
    else if (id_1==10 && id_2==599)
        return 5;                    //Sun-Jupiter
    else if (id_1==10 && id_2==699)
        return 6;                    //Sun-Saturn
    else if (id_1==10 && id_2==799)
        return 7;                    //Sun-Uranus
    else if (id_1==10 && id_2==899)
        return 8;                    //Sun-Neptune
    else if (id_1==399 && id_2==301)
        return 9;                    //Earth-Moon
    else if (id_1==699 && id_2==606)
        return 10;                   //Saturn-Titan
    else if (id_1==899 && id_2==801)
        return 11;                   //Neptune-Triton
    else
        return 0;
}

ScenarioBody* getBody1 (int System )
{
    ScenarioBody* first;
    if (System==1) //Sun-Mercury
        {StaBody* body1 = STA_SOLAR_SYSTEM->lookup(STA_SUN);
        first = new ScenarioBody(body1);
        first->setBody(body1);
        }
    else if (System==2) //Sun-Venus
        {StaBody* body1 = STA_SOLAR_SYSTEM->lookup(STA_SUN);
        first = new ScenarioBody(body1);
        first->setBody(body1);
        }
    else if (System==3) //Sun-Earth
        {StaBody* body1 = STA_SOLAR_SYSTEM->lookup(STA_SUN);
        first = new ScenarioBody(body1);
        first->setBody(body1);
        }
    else if (System==4) //Sun-Mars
        {StaBody* body1 = STA_SOLAR_SYSTEM->lookup(STA_SUN);
        first = new ScenarioBody(body1);
        first->setBody(body1);
        }
    else if (System==5) //Sun-Jupiter
        {StaBody* body1 = STA_SOLAR_SYSTEM->lookup(STA_SUN);
        first = new ScenarioBody(body1);
        first->setBody(body1);
        }
    else if (System==6) //Sun-Saturn
        {StaBody* body1 = STA_SOLAR_SYSTEM->lookup(STA_SUN);
        first = new ScenarioBody(body1);
        first->setBody(body1);
        }
    else if (System==7) //Sun-Neptune
        {StaBody* body1 = STA_SOLAR_SYSTEM->lookup(STA_SUN);
        first = new ScenarioBody(body1);
        first->setBody(body1);
        }
    else if (System==8) //Sun-Uranus
        {StaBody* body1 = STA_SOLAR_SYSTEM->lookup(STA_SUN);
        first = new ScenarioBody(body1);
        first->setBody(body1);
        }
    else if (System==9) //Earth-Moon
        {StaBody* body1 = STA_SOLAR_SYSTEM->lookup(STA_EARTH);
        first = new ScenarioBody(body1);
        first->setBody(body1);
        }
    else if (System==10) //Saturn-Titan
        {StaBody* body1 = STA_SOLAR_SYSTEM->lookup(STA_SATURN);
        first = new ScenarioBody(body1);
        first->setBody(body1);
        }
    else if (System==11) //Neptune-Triton
        {StaBody* body1 = STA_SOLAR_SYSTEM->lookup(STA_NEPTUNE);
        first = new ScenarioBody(body1);
        first->setBody(body1);
        }
    return first;
}

ScenarioBody* getBody2 (int System )
{
    ScenarioBody* second;
    if (System==1) //Sun-Mercury
        {StaBody* body2 = STA_SOLAR_SYSTEM->lookup(STA_MERCURY);
        second = new ScenarioBody(body2);
        second->setBody(body2);
        }
    else if (System==2) //Sun-Venus
        {StaBody* body2 = STA_SOLAR_SYSTEM->lookup(STA_VENUS);
        second = new ScenarioBody(body2);
        second->setBody(body2);
        }
    else if (System==3) //Sun-Earth
        {StaBody* body2 = STA_SOLAR_SYSTEM->lookup(STA_EARTH);
        second = new ScenarioBody(body2);
        second->setBody(body2);
        }
    else if (System==4) //Sun-Mars
        {StaBody* body2 = STA_SOLAR_SYSTEM->lookup(STA_MARS);
        second = new ScenarioBody(body2);
        second->setBody(body2);
        }
    else if (System==5) //Sun-Jupiter
        {StaBody* body2 = STA_SOLAR_SYSTEM->lookup(STA_JUPITER);
        second = new ScenarioBody(body2);
        second->setBody(body2);
        }
    else if (System==6) //Sun-Saturn
        {StaBody* body2 = STA_SOLAR_SYSTEM->lookup(STA_SATURN);
        second = new ScenarioBody(body2);
        second->setBody(body2);
        }
    else if (System==7) //Sun-Neptune
        {StaBody* body2 = STA_SOLAR_SYSTEM->lookup(STA_NEPTUNE);
        second = new ScenarioBody(body2);
        second->setBody(body2);
        }
    else if (System==8) //Sun-Uranus
        {StaBody* body2 = STA_SOLAR_SYSTEM->lookup(STA_URANUS);
        second = new ScenarioBody(body2);
        second->setBody(body2);
        }
    else if (System==9) //Earth-Moon
        {StaBody* body2 = STA_SOLAR_SYSTEM->lookup(STA_MOON);
        second = new ScenarioBody(body2);
        second->setBody(body2);
        }
    else if (System==10) //Saturn-Titan
        {StaBody* body2 = STA_SOLAR_SYSTEM->lookup(STA_TITAN);
        second = new ScenarioBody(body2);
        second->setBody(body2);
        }
    else if (System==11) //Neptune-Triton
        {StaBody* body2 = STA_SOLAR_SYSTEM->lookup(STA_TRITON);
        second = new ScenarioBody(body2);
        second->setBody(body2);
        }
    return second;
}

void radiusss (double mi, double x, double y, double z, double &R1, double &R2)

{
        R1=sqrt(pow(x+mi,2)+pow(y,2)+pow(z,2));		//(distance of mass 3 from mass 1)
        R2=sqrt(pow(x-1+mi,2)+pow(y,2)+pow(z,2));	//(distance of mass 3 from mass 2)

}

void Jacobi (double mi, double x, double y, double z, double vx, double vy, double vz, double &C)

{
        double r1,r2;
        radiusss (mi, x, y, z, r1,r2);
        C=pow(x,2)+pow(y,2)+2*(1-mi)/r1+2*mi/r2-(pow(vx,2)+pow(vy,2)+pow(vz,2));	//Jacobi's constant
}

