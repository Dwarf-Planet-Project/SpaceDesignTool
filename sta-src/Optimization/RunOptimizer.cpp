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
*/

//------------------ Author: Francesco Castellini -------------------------------------------//
//------- Affiliation: Dipartimento di Ingegneria Aerospaziale, Politecnico di Milano -------//
//------------------------- E-mail: (castellfr@gmail.com) -----------------------------------//

//======================= Function calling the optimization algorithm =======================//*/

#include <stdlib.h>
#include "Optimization/GlobalOptimizers.h"

void GlobalOptimizers::RunOptimizer(int optimizer)
        {
        if (optimizer==0)					//optimizer 0 --> PSO_1D
                {
                pso1D();
                }

        else if (optimizer==1)					//optimizer 1 --> NSGA-II
                {
                nsga2();
                }
        else if (optimizer==2)					//optimizer 2 --> DG-MOPSO
                {
                dgmopso();
                }

        }
