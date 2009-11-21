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

------ Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ------
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
