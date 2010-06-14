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

//=========================== Allocation and operations on arrays ===========================//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// #include <malloc.h>

double** dmatrix_allocation(int nrow,int ncol){
/* Function allocating memory for a matrix of (nrow x ncol) double type numbers
   Inputs:
   nrow,ncol = number of rows and columns
   Outputs:
   mat = matrix, defined as an array of pointers to double pointers
*/
        int i;
        double **mat;
        mat = (double **)malloc(nrow*sizeof(double*));

        if(mat == NULL)	{
                                        printf("\nError: Not enough memory1");
                                        exit(1);
                                        }
        for(i=0;i<nrow;i++)	{
                mat[i] = (double*)malloc((size_t)(ncol*sizeof(double)));

                if(mat[i] == NULL)	{
                                                        printf("\nError: Not enough memory2");
                                                        exit(1);
                                                        }
                                                }
        return mat;
}

void dmatrix_free(double **mat, int nrow, int ncol)
/* Function DE-allocating memory for a matrix of (nrow x ncol) double type numbers allocated with dmatrix_allocation
   Inputs:
   nrow,ncol = number of rows and columns
   mat = matrix, defined as an array of pointers to double pointers
*/
{
        int i;
        for(i=0;i<nrow;i++)
                {
                if(mat[i]==NULL) printf("W");
                free(mat[i]);
                }
        free(mat);
}

double*** d3darray_allocation(int npage,int nrow,int ncol)
/* Function allocating memory for a 3D array of (npage x nrow x ncol) double type numbers
   Inputs:
   npage,nrow,ncol = number of pages, rows and columns
   Outputs:
   array = 3D array, defined as an array of pointers to an array of pointers to double pointers
   pppd: pointer to pointer to pointer to double
*/
{
        int i;
        double ***arrays;

        arrays = (double***)malloc(npage*sizeof(double**));
        if(arrays == NULL)
                                        {
                                        printf("\nError: Not enough memory");
                                        exit(1);
                                        }

        for(i=0;i<npage;i++)
                arrays[i]=dmatrix_allocation(nrow,ncol);			//allocating memory for a matrix for each page of the 3D array

        return arrays;
}


int** imatrix_allocation(int nrow,int ncol)
{
/* Function allocating memory for a matrix of (nrow x ncol) int type numbers
   Inputs:
   nrow,ncol = number of rows and columns
   Outputs:
   mat = matrix, defined as an array of pointers to int pointers */

        int i;
        int **mat;
        mat = (int **)malloc(nrow*sizeof(int*));

        if(mat == NULL)	{
                                        printf("\nError: Not enough memory");
                                        exit(1);
                                        }
        for(i=0;i<nrow;i++)	{
                mat[i] = (int*)malloc((size_t)(ncol*sizeof(int)));

                if(mat[i] == NULL)	{
                                                        printf("\nError: Not enough memory");
                                                        exit(1);
                                                        }
                                                }
        return mat;
}


int*** i3darray_allocation(int npage,int nrow,int ncol)
{
/* Function allocating memory for a 3D array of (npage x nrow x ncol) double type numbers
   Inputs:
   npage,nrow,ncol = number of pages, rows and columns
   Outputs:
   array = 3D array, defined as an array of pointers to an array of pointers to double pointers
   pppd: pointer to pointer to pointer to double*/

        int i;
        int ***array;

        array = (int***)malloc(npage*sizeof(int**));
        if(array == NULL)
                                        {
                                        printf("\nError: Not enough memory");
                                        exit(1);
                                        }

        for(i=0;i<npage;i++)
                array[i]=imatrix_allocation(nrow,ncol);			//allocating memory for a matrix for each page of the 3D array

        return array;
}


