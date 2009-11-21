/*This is the file used for crossover for Real Coded GA*/

void realcross(population *new_pop_ptr,population *mate_pop_ptr);

void realcross(population *new_pop_ptr,population *mate_pop_ptr)
{
  int i,j,y,n;
  double rnd,par1,par2,chld1,chld2,betaq,beta,alpha;
  double y1,y2,yu,yl,expp;

  y=0; n=0;
  for(i = 0; i < popsize/2; i++)
    {
      rnd = randomperc();
      
      /*Check Whether the cross-over to be performed*/
      if(rnd <= pcross)
	{
	  
	  /*Loop over no of variables*/
	  for(j = 0;j < nvar;j++)
	    { 
	      /*Selected Two Parents*/ 
	      par1 = mate_pop_ptr->ind[y].xreal[j];
	      par2 = mate_pop_ptr->ind[y+1].xreal[j]; 
	      
	      yl = lim_r[j][0];
	      yu = lim_r[j][1];
	      
	      rnd = randomperc();
	      
	      /* Check whether variable is selected or not*/
	      if(rnd <= 0.5)
		{
		  /*Variable selected*/
		  ncross++;
		  
		  if(fabs(par1 - par2) > 0.000001) // changed by Deb (31/10/01)
		    {
		      if(par2 > par1)
			{
			  y2 = par2;
			  y1 = par1;
			}
		      else
			{
			  y2 = par1;
			  y1 = par2;
			}
		      
		      /*Find beta value*/
		      if((y1 - yl) > (yu - y2))
			{
			  beta = 1 + (2*(yu - y2)/(y2 - y1));
			  //printf("beta = %f\n",beta);
			}
		      else
			{
			  beta = 1 + (2*(y1-yl)/(y2-y1));
			  //printf("beta = %f\n",beta);
			}
		      
		      /*Find alpha*/
		      expp = di + 1.0;
		      
		      beta = 1.0/beta;
		      
		      alpha = 2.0 - pow(beta,expp);
		      
		      if (alpha < 0.0) 
			{
			  printf("ERRRROR %f %d %d %f %f\n",alpha,y,n,par1,par2);
			  exit(-1);
			}

		      rnd = randomperc(); 
		      
		      if (rnd <= 1.0/alpha)
			{
			  alpha = alpha*rnd;
			  expp = 1.0/(di+1.0);
			  betaq = pow(alpha,expp);
			}
		      else
			{
			  alpha = alpha*rnd;
			  alpha = 1.0/(2.0-alpha);
			  expp = 1.0/(di+1.0);
			  if (alpha < 0.0) 
			    {
			      printf("ERRRORRR \n");
			      exit(-1);
			    }
			  betaq = pow(alpha,expp);
			}
		      
		      /*Generating two children*/
		      chld1 = 0.5*((y1+y2) - betaq*(y2-y1));
		      chld2 = 0.5*((y1+y2) + betaq*(y2-y1));
		      
		    }
		  else
		    {
		      
		      betaq = 1.0;
		      y1 = par1; y2 = par2;
		      
		      /*Generation two children*/
		      chld1 = 0.5*((y1+y2) - betaq*(y2-y1));
		      chld2 =  0.5*((y1+y2) + betaq*(y2-y1));
		      
		    }
		  // added by deb (31/10/01)
		  if (chld1 < yl) chld1 = yl;
		  if (chld1 > yu) chld1 = yu;
		  if (chld2 < yl) chld2 = yl;
		  if (chld2 > yu) chld2 = yu;
		}
	      else
		{
		  
		  /*Copying the children to parents*/
		  chld1 = par1;
		  chld2 = par2;
		}
	      new_pop_ptr->ind[n].xreal[j] = chld1;
	      new_pop_ptr->ind[n+1].xreal[j] = chld2;
	    }
	}
      else
	{
	  for(j = 0;j < nvar;j++)
	    {
	      par1 = mate_pop_ptr->ind[y].xreal[j];
	      par2 = mate_pop_ptr->ind[y+1].xreal[j]; 
	      chld1 = par1;
	      chld2 = par2;
	      new_pop_ptr->ind[n].xreal[j] = chld1;
	      new_pop_ptr->ind[n+1].xreal[j] = chld2;
	    }
	}
      n = n+2; y=y+2;
    }
  return;
}

