/*This is a routine to keep the fronts alive (caring the end problem)*/

void keepalive(population *pop1_ptr,population *pop2_ptr,population *pop3_ptr,int gen);

typedef struct
{
  int maxrank,   /*Max rank of the global population*/
    rankar[2*maxpop][2*maxpop], /*record of array of individual numbers at
				  a particular rank */
    rankno[2*maxpop];           /*record of no. of individuals at a particular rank*/

  int genes[2*maxpop][maxchrom],
    rank[2*maxpop],            /*rank of different individuals*/
    flag[2*maxpop];            /*Setting the flag */

  double fitness[2*maxpop][maxfun], /*Fitness function values for the different
				     individuals*/
    cub_len[2*maxpop],              /*Dummyfitness*/
    xreal[2*maxpop][maxvar],       /*value of the decoded variables for different individuals */
    xbin[2*maxpop][maxvar],   /* binray-coded variables */
    error[2*maxpop],               /*Error Values of the individuals*/
  constr[2*maxpop][maxcons];
}globpop;

/*Population structure for the pool having both the old as well as new
  population*/

globpop globalpop,*global_pop_ptr;

void grank(int gen);
/*Ranking the global pool*/

void grankc(int gen);
/*Ranking the global pool when the constraints are there*/

int indcmp1(double *ptr1,double *ptr2);
/*Comparison of the variables*/

void gsort(int rnk,int sel);
/*Sorting for the function values in ascending order*/

void gshare(int rnk);
/*Sharing the fitness*/

void sort(int rnk);

int left,Lastrank;
double fpara1[2*maxpop][2];

void keepalive(population *pop1_ptr,population *pop2_ptr,population *pop3_ptr,int gen)
{
  int i,j,jj,k,m,l,rec;

  int st,pool,poolf,sel;

  int *gene1_ptr, *gene2_ptr;

  double *gene3_ptr,*gene4_ptr,*xbin1_ptr,*xbin2_ptr;

  /*Forming the global mating pool*/

  for(i = 0;i < popsize;i++)
    {
      if(nchrom > 0)
	{
	  /*Binary Coded GA genes are copied*/
	  for(k = 0;k < chrom;k++)
	    {
	      globalpop.genes[i][k]=pop1_ptr->ind[i].genes[k];
	      globalpop.genes[i+popsize][k] = pop2_ptr->ind[i].genes[k];
	    }
	  for (k=0; k < nchrom; k++)
	    {
	      globalpop.xbin[i][k] = pop1_ptr->ind[i].xbin[k];
	      globalpop.xbin[i+popsize][k] = pop2_ptr->ind[i].xbin[k];
	    }
	 }
      if (nvar > 0)
	 {
	   /*For Real Coded GA x values are copied */
	   for(k = 0;k < nvar;k++)
	     {
	       globalpop.xreal[i][k] = pop1_ptr->ind[i].xreal[k];
	       globalpop.xreal[i+popsize][k] = pop2_ptr->ind[i].xreal[k];
	     }
	 }


      /*Fitness is copied to the global pool */
       for(l = 0;l < nfunc;l++)
	 {
	   globalpop.fitness[i][l] = pop1_ptr->ind[i].fitness[l];
	   globalpop.fitness[i+popsize][l] = pop2_ptr->ind[i].fitness[l];
	 }

       /*Initializing the dummyfitness to zero */
       globalpop.cub_len[i] = 0;
       globalpop.cub_len[i+popsize] = 0;
       globalpop.error[i] = pop1_ptr->ind[i].error;
       globalpop.error[i+popsize] = pop2_ptr->ind[i].error;
       for (jj=0; jj<ncons; jj++)
	{
	  globalpop.constr[i][jj] = pop1_ptr->ind[i].constr[jj];
	  globalpop.constr[i+popsize][jj] = pop2_ptr->ind[i].constr[jj];
	}
  }


  global_pop_ptr = &(globalpop);

  /*Finding the global ranks */
  if(ncons == 0)
    grank(gen);
   else
     grankc(gen);

  m = globalpop.maxrank;

  /* Sharing the fitness to get the dummy fitness */
  for(i = 0;i < m;i++)
    {
      gshare(i+1);
    }

  poolf = popsize;
  pool = 0;


  /*Initializing the flags of population to zero */
  for(i = 0;i < 2*popsize;i++)
    {
      globalpop.flag[i] = 0;
    }
  // decide which all solutions belong to the pop3
  rec = 0;
  st = 0;
  for(i = 0 ;i < m ; i++)
    {
      /*    Elitism Applied Here     */
      st = pool;
      pool += globalpop.rankno[i];

      if(pool <= popsize)
	{
	  for(k = 0;k < 2*popsize ;k++)
	    {
	      if(globalpop.rank[k] == i+1)
		globalpop.flag[k] = 1;
	    }
	  pop3_ptr->rankno[i] = globalpop.rankno[i];
	}
      else
	{
	  sel = popsize - st;
	  Lastrank = i+1;
	  pop3_ptr->rankno[i] = sel;
	  gsort(i+1,sel);
	  break;
	}
    }

  k = 0;
  for(i = 0,k = 0;i < 2*popsize && k < popsize; i++)
    {
      if(nchrom > 0)
	{
	  if(globalpop.flag[i] == 1)
	    {
	      gene1_ptr = &(globalpop.genes[i][0]);
	      xbin1_ptr = &(globalpop.xbin[i][0]);
	      pop3_ptr->ind_ptr = &(pop3_ptr->ind[k]);
	      gene2_ptr = &(pop3_ptr->ind_ptr->genes[0]);
	      xbin2_ptr = &(pop3_ptr->ind_ptr->xbin[0]);

	      for(j = 0 ; j < chrom; j++)
		{
		  *gene2_ptr++ = *gene1_ptr++;
		}
	      for (j=0; j < nchrom; j++)
		*xbin2_ptr++ = *xbin1_ptr++;
	    }
	}
      if (nvar > 0)
	{
	  if(globalpop.flag[i] == 1)
	    {
	      gene3_ptr = &(globalpop.xreal[i][0]);
	      pop3_ptr->ind_ptr = &(pop3_ptr->ind[k]);
	      gene4_ptr = &(pop3_ptr->ind_ptr->xreal[0]);

	      for(j = 0 ;j < nvar;j++)
		{
		  *gene4_ptr++ = *gene3_ptr++;
		}
	    }
	}
      if(globalpop.flag[i] == 1)
	{
	  for(j = 0;j < nfunc;j++)
	    pop3_ptr->ind[k].fitness[j] = globalpop.fitness[i][j];
	  pop3_ptr->ind[k].cub_len = globalpop.cub_len[i];
	  if(ncons != 0)
	    pop3_ptr->ind[k].error = globalpop.error[i];
	  for (jj=0; jj<ncons; jj++)
	    pop3_ptr->ind[k].constr[jj] = globalpop.constr[i][jj];
	  pop3_ptr->ind[k].rank = globalpop.rank[i];
	  k++;  // increment the pop3 counter
	}
    }

  pop3_ptr->maxrank = Lastrank;

  return;
}

void grank(int gen)
{
  int i,j,k,rnk,val,nondom,popsize1,gflg[2*maxpop],q;
  double *ptr1,*ptr2;
//  FILE *gr;
//  gr = fopen("g_rank_record.out","a");
//  fprintf(gr,"Genration no. = %d\n",gen);
  /*----------------------------* RANKING *---------------------------------*/
  rnk = 0;
  nondom = 0;
  popsize1 = 2*popsize;

  for(i = 0;i < popsize1;i++)
    {
      gflg[i] = 2;
    }


  for(k = 0;k < popsize1;k++)
    {
      q =  0;
      for(j = 0;j < popsize1;j++)
	{
	  if (gflg[j] != 1) break;
	}
      if(j == popsize1) break;
      rnk = rnk +1;
      for( j = 0 ;j < popsize1; j++)
	{
	  if(gflg[j] == 0) gflg[j] = 2;
	}
      for(i = 0;i < popsize1 ; i++)
	{
	  if(gflg[i] != 1 && gflg[i] != 0)
	    {
	      ptr1 = &(global_pop_ptr->fitness[i][0]);
	      for(j = 0;j < popsize1 ; j++)
		{
		  if( i!= j)
		    {
		      if(gflg[j] != 1)
			{
			  ptr2 = &(global_pop_ptr->fitness[j][0]);
			  val = indcmp1(ptr1,ptr2);
			  if( val == 2)
			    {
			      gflg[i] = 0;/* individual 1 is dominated */
			      break;
			    }
			  if(val == 1)
			    {
			      gflg[j] = 0;/* individual 2 is dominated */
			    }
			  if(val == 3)
			    {
			      nondom++;/* individual 1 & 2 are non dominated */
			      if(gflg[j] != 0)gflg[j] = 3;
			    }
			}
		    }
		}
	      if( j == popsize1)
		{
		  global_pop_ptr->rank[i] = rnk;
		  gflg[i] = 1;
		  global_pop_ptr->rankar[rnk-1][q] =  i;
		  q++;
		}
	    }
	}
      global_pop_ptr->rankno[rnk-1] = q;
    }
  global_pop_ptr->maxrank = rnk;

//  fprintf(gr,"   RANK     No Of Individuals\n");
//  for(i = 0;i < rnk;i++)
//    fprintf(gr,"\t%d\t%d\n",i+1,globalpop.rankno[i]);

//  fclose(gr);
  return;
}

void grankc(int gen)
{
  int i,j,k,rnk,val,nondom,popsize1,gflg[2*maxpop],q;
  double *ptr1,*ptr2;
  double *err_ptr1,*err_ptr2;

  /*----------------------------* RANKING *---------------------------------*/
  rnk = 0;
  nondom = 0;
  popsize1 = 2*popsize;
  min_fit = popsize1;
  delta_fit = 0.1 *popsize1;
  for(i=0;i<popsize1;i++)
    {
      gflg[i] = 2;
    }
  for(k = 0;k < popsize1;k++)
    {
      q =  0;
      for(j = 0;j < popsize1;j++)
	{
	  if (gflg[j] != 1) break;
	}
      if(j == popsize1) break;
      rnk = rnk +1;
      for( j = 0 ;j < popsize1; j++)
	{
	  if(gflg[j] == 0) gflg[j] = 2;
	}
      for(i = 0;i< popsize1 ; i++)
	{
	  if(gflg[i] != 1 && gflg[i] != 0)
	    {
	      ptr1 = &(global_pop_ptr->fitness[i][0]);
	      err_ptr1 = &(global_pop_ptr->error[i]);
	      for(j = 0;j < popsize1 ; j++)
		{
		  if( i!= j)
		    {
		      if(gflg[j] != 1)
			{
			  ptr2 = &(global_pop_ptr->fitness[j][0]);
			  err_ptr2 = &(global_pop_ptr->error[j]);

			  if(*err_ptr1 < 1.0e-6 && *err_ptr2 > 1.0e-6)
			    {/* first feasible second individaul is infeasible*/
			      gflg[j] = 0;
			    }
			  else
			    {
			      if(*err_ptr1 >1.0e-6 && *err_ptr2 < 1.0e-6)
				{/*first individual is infeasible and second is feasible*/
				  gflg[i] = 0;
				  break;
				}
			      else
				{/*both feasible or both infeasible*/
				  if(*err_ptr1 > *err_ptr2)
				    {
				      gflg[i] = 0;
				      /*first individual is more infeasible*/
				      break;
				    }
				  else
				    {
				      if(*err_ptr1 < *err_ptr2)
					gflg[j] = 0;
					/*second individual is more infeasible*/

				      else
					{
					  val = indcmp1(ptr1,ptr2);
					  if( val == 2)
					    {
					      gflg[i] = 0;
					      /* individual 1 is dominated */
					      break;
					    }
					  if(val == 1)
					    {
					      gflg[j] = 0;
					      /* individual 2 is dominated */
					    }
					  if(val == 3)
					    {
					      nondom++;/* individual 1 & 2 are non dominated */
					      if(gflg[j] != 0) gflg[j] = 3;
					    }
					}
				    }
				}
			    }
			}
		    }
		}
	      if( j == popsize1)
		{
		  global_pop_ptr->rank[i] = rnk;
		  gflg[i] = 1;
		  global_pop_ptr->rankar[rnk-1][q] =  i;
		  q++;
		}
	    }
	}
      global_pop_ptr->rankno[rnk-1] = q;
    }
  global_pop_ptr->maxrank = rnk;
//  fprintf(gr,"   RANK     No Of Individuals\n");
//  for(i = 0;i < rnk;i++)
//    {
//      fprintf(gr,"\t%d\t%d\n",i+1,globalpop.rankno[i]);
//    }
//  fclose(gr);

  return;
}



int indcmp1(double *ptr1,double *ptr2)
{
  double fit1[maxfun],fit2[maxfun];
  int i,value,m,n;
  for(i = 0;i < nfunc ;i++)
    {
      fit1[i] = *ptr1++;
      fit2[i] = *ptr2++;
    }
  m = 0;n=0;
  while(m < nfunc && fit1[m] <= fit2[m])
    {
      if((fit2[m] - fit1[m]) < 1e-7) n++;
      m++;
    }
  if(m == nfunc)
    {
      if(n == nfunc) value = 3;
      else value = 1;                    /*value = 1 for dominating*/
    }
  else
    {
      m = 0;n = 0;
      while(m < nfunc && fit1[m] >= fit2[m])
	{
	  if((fit1[m] - fit2[m]) < 1e-7) n++;
	  m++;
	}
      if(m == nfunc)
	{
	  if(n != nfunc)
	    value = 2;                       /*value =  2 for dominated */
	  else value = 3;
	}
      else value = 3;                   /*value = 3 for incomparable*/
    }
  return value;
}


/* This is the file used to sort the dummyfitness arrays */
void gsort(int rnk,int sel)
{
  int i,j,a,q;
  double array[2*maxpop][2],temp,temp1;

  q = globalpop.rankno[rnk-1];

  for(i = 0 ;i < q ;i++)
    {
      array[i][0] = globalpop.rankar[rnk-1][i];
      a = globalpop.rankar[rnk-1][i];
      array[i][1] = globalpop.cub_len[a];
    }
  for(i = 0;i < q ;i++)
    {
      for(j = i+1;j < q;j++)
	{
	  if(array[i][1] < array[j][1])
	    {
	      temp = array[i][1];
	      temp1 = array[i][0];
	      array[i][1] = array[j][1];
	      array[i][0] = array[j][0];

	      array[j][1] = temp;
	      array[j][0] = temp1;
	    }
	}
    }

  for(i = 0;i < sel;i++)
    {
      a = array[i][0];
      globalpop.flag[a] = 1;
    }
  return;
}

/*========================================================================*/

void gshare(int rnk)
{
  double length[2*maxpop][2],max;
  int i,j,m1,a ;

  m1 = globalpop.rankno[rnk-1];

  for(j = 0;j < nfunc;j++)
    {
      for(i = 0;i < m1;i++)
	{
	  fpara1[i][0] = 0;
	  fpara1[i][1] = 0;
	}

      for(i = 0;i < m1;i++)
	{
	  a = globalpop.rankar[rnk-1][i];
	  fpara1[i][0] = (double)a ;
	  fpara1[i][1] = globalpop.fitness[a][j];
	}

      sort(m1); /*Sort the arrays in ascending order of the fitness*/

      max = fpara1[m1-1][1];
      for(i = 0;i < m1;i++)
	{
	  if(i == 0 ||i == (m1-1))
	    {
	      length[i][0] = fpara1[i][0];
	      length[i][1] = 100*max;
	    }
	  else
	    {
	      length[i][0] = fpara1[i][0];
	      length[i][1] = fabs(fpara1[i+1][1]- fpara1[i-1][1]);
	    }
	}
      for(i = 0;i < m1;i++)
	{
	  a = length[i][0];
	  globalpop.cub_len[a] += length[i][1];
	}
    }

  return;
}


void sort(int m1)
{
  double temp,temp1;
  int i1,k1;
  for(k1 = 0;k1 < m1-1;k1++)
    {
      for(i1 = k1+1;i1 < m1;i1++)
	{
	  if(fpara1[k1][1] > fpara1[i1][1])
	    {
	      temp = fpara1[k1][1];
	      temp1 = fpara1[k1][0];
	      fpara1[k1][1] = fpara1[i1][1];
	      fpara1[k1][0] = fpara1[i1][0];
	      fpara1[i1][1] = temp;
	      fpara1[i1][0] = temp1;
	    }
	}
    }
  return;
}
