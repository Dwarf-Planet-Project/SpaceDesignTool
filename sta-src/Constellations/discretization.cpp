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
 ------ Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Steffen Peter, Claas Grohnfeldt --------------------------------------------------
 ------------------ E-mail: (claasgr@math.uni-bremen.de, spezisteffen@gmx.de) --------------------------------
 Modified by ...
 */

#include "Constellations/discretization.h"
#include "Astro-Core/stamath.h"
#include "celestia-src/celmath/mathlib.h"

using namespace Eigen;

DiscreteMesh::DiscreteMesh(const StaBody* body, int n)
{
    centralBody = body;
    numberRows = n;
    meshAsList = QList<DiscretizationPoint>();
    discreteMesh = new QList<DiscretizationPoint>[numberRows];
    generateMesh(numberRows);

}

/** Generates a mesh consisting of discrete points in such a way
  * that the points are approximately equally distrubuted over
  * the bodys surface
  * @param numberRows user defined number of rows occuring in the mesh
  */
void DiscreteMesh::generateMesh(int numberRows)
{
    int numberColumns[numberRows];
    float columnSpacing[numberRows];
    float latitudeValues[numberRows];
    double periphery[numberRows];
    float rowSpacing = 180.0f / (float)numberRows;
    float heightRow = std::sqrt(2.0) * centralBody->meanRadius() * std::sqrt(1.0-cos(sta::degToRad(rowSpacing)));
    for (int k = 0; k < numberRows; k++)
    {
        latitudeValues[k] = 90.0f - ((float)(k+1)-0.5f) * rowSpacing;
        periphery[k] = 2 * PI * centralBody->meanRadius() * cos(sta::degToRad(latitudeValues[k]));
        numberColumns[k] = (int)(periphery[k] / (double)heightRow);
        if (numberColumns[k] % 2 == 0)
        {
            numberColumns[k]++;
        }
        columnSpacing[k] = 360.0f / (float)numberColumns[k];
        for (int j = 0; j < numberColumns[k]; j++)
        {
            DiscretizationPoint p;
            p.latitude = latitudeValues[k];
            p.longitude = 180.0f - ((float)(j+1) - 0.5f) * columnSpacing[k];
            p.seen = new bool[36];
            p.rectangleWidth = columnSpacing[k];
            for(int jj=0; jj<36; jj++)
            {
                p.seen[jj]=false;
            }
            discreteMesh[k].push_front(p);
        }
    }
    // transform an array of QLists into a long QList
    for (int k=0; k<numberRows; k++)
    {
        for (int j=0; j<discreteMesh[k].count(); j++)
        {
            DiscretizationPoint point = discreteMesh[k].at(j);
            point.seen = new bool[36];
            for(int jj=0; jj<36; jj++)
            {
                point.seen[jj]=false;
            }
            meshAsList.append(point);
        }
    }



    /*
    // create an individual mesh for Australia:
    // Abercorn
    DiscretizationPoint pAbercorn;
    pAbercorn.latitude = 25.12f;
    pAbercorn.longitude = 151.05f;
    pAbercorn.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAbercorn.seen[jj]=false;
    }
    meshAsList.append(pAbercorn);
    // Aberdeen
    DiscretizationPoint pAberdeen;
    pAberdeen.latitude = 32.09f;
    pAberdeen.longitude = 150.56f;
    pAberdeen.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAberdeen.seen[jj]=false;
    }
    meshAsList.append(pAberdeen);
    // Abington Reef
    DiscretizationPoint pAbington;
    pAbington.latitude = 18.00f;
    pAbington.longitude = 149.35f;
    pAbington.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAbington.seen[jj]=false;
    }
    meshAsList.append(pAbington);
    // Abminga
    DiscretizationPoint pAbminga;
    pAbminga.latitude = 26.08f;
    pAbminga.longitude = 134.51f;
    pAbminga.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAbminga.seen[jj]=false;
    }
    meshAsList.append(pAbminga);
    // Acraman, L.
    DiscretizationPoint pAcraman;
    pAcraman.latitude = 32.02f;
    pAcraman.longitude = 135.23f;
    pAcraman.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAcraman.seen[jj]=false;
    }
    meshAsList.append(pAcraman);
    // Adaminaby
    DiscretizationPoint pAdaminaby;
    pAdaminaby.latitude = 36.00f;
    pAdaminaby.longitude = 148.45f;
    pAdaminaby.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAdaminaby.seen[jj]=false;
    }
    meshAsList.append(pAdaminaby);
    // Adavale
    DiscretizationPoint pAdavale;
    pAdavale.latitude = 25.52f;
    pAdavale.longitude = 144.32f;
    pAdavale.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAdavale.seen[jj]=false;
    }
    meshAsList.append(pAdavale);
    // Adelaide
    DiscretizationPoint pAdelaide;
    pAdelaide.latitude = 34.52f;
    pAdelaide.longitude = 138.30f;
    pAdelaide.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAdelaide.seen[jj]=false;
    }
    meshAsList.append(pAdelaide);
    // Adelaide River
    DiscretizationPoint pAdelaideRiver;
    pAdelaideRiver.latitude = 13.15f;
    pAdelaideRiver.longitude = 131.07f;
    pAdelaideRiver.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAdelaideRiver.seen[jj]=false;
    }
    meshAsList.append(pAdelaideRiver);
    // Adele I.
    DiscretizationPoint pAdele;
    pAdele.latitude = 15.32f;
    pAdele.longitude = 123.09f;
    pAdele.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAdele.seen[jj]=false;
    }
    meshAsList.append(pAdele);
    // Adieu, C.
    DiscretizationPoint pAdieuC;
    pAdieuC.latitude = 32.00f;
    pAdieuC.longitude = 132.10f;
    pAdieuC.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAdieuC.seen[jj]=false;
    }
    meshAsList.append(pAdieuC);
    // Adieu Pt.
    DiscretizationPoint pAdieuPt;
    pAdieuPt.latitude = 15.14f;
    pAdieuPt.longitude = 124.35f;
    pAdieuPt.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAdieuPt.seen[jj]=false;
    }
    meshAsList.append(pAdieuPt);
    // Admiralty G.
    DiscretizationPoint pAdmiralty;
    pAdmiralty.latitude = 14.20f;
    pAdmiralty.longitude = 125.55f;
    pAdmiralty.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAdmiralty.seen[jj]=false;
    }
    meshAsList.append(pAdmiralty);
    // Agnew
    DiscretizationPoint pAgnew;
    pAgnew.latitude = 28.01f;
    pAgnew.longitude = 120.31f;
    pAgnew.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAgnew.seen[jj]=false;
    }
    meshAsList.append(pAgnew);
    // Aileron
    DiscretizationPoint pAileron;
    pAileron.latitude = 22.39f;
    pAileron.longitude = 133.20f;
    pAileron.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAileron.seen[jj]=false;
    }
    meshAsList.append(pAileron);
    // Airlie Beach
    DiscretizationPoint pAirlieBeach;
    pAirlieBeach.latitude = 20.16f;
    pAirlieBeach.longitude = 148.43f;
    pAirlieBeach.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAirlieBeach.seen[jj]=false;
    }
    meshAsList.append(pAirlieBeach);
    // Alawoone
    DiscretizationPoint pAlawoone;
    pAlawoone.latitude = 34.45f;
    pAlawoone.longitude = 140.30f;
    pAlawoone.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAlawoone.seen[jj]=false;
    }
    meshAsList.append(pAlawoone);
    // Albacutya, L.
    DiscretizationPoint pAlbacutya;
    pAlbacutya.latitude = 35.45f;
    pAlbacutya.longitude = 141.58f;
    pAlbacutya.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAlbacutya.seen[jj]=false;
    }
    meshAsList.append(pAlbacutya);
    // Albany
    DiscretizationPoint pAlbany;
    pAlbany.latitude = 35.01f;
    pAlbany.longitude = 117.58f;
    pAlbany.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAlbany.seen[jj]=false;
    }
    meshAsList.append(pAlbany);
    // Albatross B.
    DiscretizationPoint pAlbatross;
    pAlbatross.latitude = 12.45f;
    pAlbatross.longitude = 141.30f;
    pAlbatross.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAlbatross.seen[jj]=false;
    }
    meshAsList.append(pAlbatross);
    // Albert, L.
    DiscretizationPoint pAlbert;
    pAlbert.latitude = 35.30f;
    pAlbert.longitude = 139.10f;
    pAlbert.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAlbert.seen[jj]=false;
    }
    meshAsList.append(pAlbert);
    // Albert Edward Ra.
    DiscretizationPoint pAlbertEdwardRa;
    pAlbertEdwardRa.latitude = 18.17f;
    pAlbertEdwardRa.longitude = 127.57f;
    pAlbertEdwardRa.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAlbertEdwardRa.seen[jj]=false;
    }
    meshAsList.append(pAlbertEdwardRa);
    // Albury-Wodonga
    DiscretizationPoint pAlburyWodonga;
    pAlburyWodonga.latitude = 36.03f;
    pAlburyWodonga.longitude = 146.56f;
    pAlburyWodonga.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAlburyWodonga.seen[jj]=false;
    }
    meshAsList.append(pAlburyWodonga);
    // Alexander, Mt.
    DiscretizationPoint pAlexander;
    pAlexander.latitude = 28.58f;
    pAlexander.longitude = 120.16f;
    pAlexander.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAlexander.seen[jj]=false;
    }
    meshAsList.append(pAlexander);
    // Alexandra
    DiscretizationPoint pAlexandra;
    pAlexandra.latitude = 37.08f;
    pAlexandra.longitude = 145.40f;
    pAlexandra.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAlexandra.seen[jj]=false;
    }
    meshAsList.append(pAlexandra);
    // Alexandrina, L.
    DiscretizationPoint pAlexandrina;
    pAlexandrina.latitude = 35.25f;
    pAlexandrina.longitude = 139.10;
    pAlexandrina.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAlexandrina.seen[jj]=false;
    }
    meshAsList.append(pAlexandrina);
    // Alice, Queens
    DiscretizationPoint pAliceQueens;
    pAliceQueens.latitude = 24.02f;
    pAliceQueens.longitude = 144.50f;
    pAliceQueens.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAliceQueens.seen[jj]=false;
    }
    meshAsList.append(pAliceQueens);
    // Alice Springs
    DiscretizationPoint pAliceSprings;
    pAliceSprings.latitude = 23.40f;
    pAliceSprings.longitude = 133.50f;
    pAliceSprings.seen = new bool[36];
    for(int jj=0; jj<36; jj++)
    {
        pAliceSprings.seen[jj]=false;
    }
    meshAsList.append(pAliceSprings);
    */
}
