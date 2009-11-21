/*This is the file to get the different individuals selected*/

void nselect(population *old_pop_ptr,population *pop2_ptr);

void nselect(population *old_pop_ptr,population *pop2_ptr)
{
  int *fit_ptr1,*fit_ptr2;

  double rnd2,*f1_ptr,*f2_ptr;
  
  int *s1_ptr,*s2_ptr,*select_ptr;
  double *select_ptr_r, *s1_ptr_r, *s2_ptr_r;
  
  individual *j,*j1;
  
  int i,rnd,rnd1,k,n,j2,r,s;
  
  old_pop_ptr->ind_ptr = &(old_pop_ptr->ind[0]);
  
  pop2_ptr->ind_ptr= &(pop2_ptr->ind[0]); 
  
  j =  &(old_pop_ptr->ind[popsize-1]);
  
  old_pop_ptr->ind_ptr = &(old_pop_ptr->ind[0]); 
  j2 = 0;
  r = popsize;
  s = chrom;
  
  for(n = 0,k = 0;n < popsize;n++,k++)
    {
      pop2_ptr->ind_ptr = &(pop2_ptr->ind[k]);
      select_ptr = &(pop2_ptr->ind_ptr->genes[0]);
      select_ptr_r = &(pop2_ptr->ind_ptr->xreal[0]);

      rnd2 = randomperc(); 

      rnd2 = popsize* rnd2; 

      rnd = floor(rnd2);

      if(rnd == 0)
	rnd = popsize - k;

      if(rnd == popsize)
	rnd = (popsize-2)/2;
      
      /*Select first parent randomly*/	
      j = &(old_pop_ptr->ind[rnd-1]);
      
      rnd2 = randomperc(); 
      
      rnd2 = popsize * rnd2; 
      
      rnd1 = floor(rnd2);
      
      if (rnd1 == 0)
	rnd1 = popsize - n;

      if(rnd1 == popsize)
	rnd1 = (popsize - 4)/2;
      
      
      /*Select second parent randomly*/
      j1 = &(old_pop_ptr->ind[rnd1-1]);
      
      old_pop_ptr->ind_ptr = j;
      
      s1_ptr = &(old_pop_ptr->ind_ptr->genes[0]);
      s1_ptr_r = &(old_pop_ptr->ind_ptr->xreal[0]);
      fit_ptr1 = &(old_pop_ptr->ind_ptr->rank);
      f1_ptr = &(old_pop_ptr->ind_ptr->cub_len);
      
      old_pop_ptr->ind_ptr = j1;
      s2_ptr = &(old_pop_ptr->ind_ptr->genes[0]);
      s2_ptr_r = &(old_pop_ptr->ind_ptr->xreal[0]);
      fit_ptr2 = &(old_pop_ptr->ind_ptr->rank);
      f2_ptr = &(old_pop_ptr->ind_ptr->cub_len);
/*--------------------------------------------------------------------------*/
      
/*------------------SELECTION PROCEDURE------------------------------------*/
      
      /*Comparing the fitnesses*/
      
      if(*fit_ptr1 > *fit_ptr2)
	{
	  for(i = 0;i < chrom;i++)
	    *select_ptr++=*s2_ptr++;
	  for(i = 0;i < nvar;i++)
	    *select_ptr_r++=*s2_ptr_r++;
	}
      else
	{
	  if(*fit_ptr1 < *fit_ptr2)
	    {
	      for(i = 0;i < chrom;i++)
		*select_ptr++=*s1_ptr++;
	      for(i = 0;i < nvar;i++)
		*select_ptr_r++=*s1_ptr_r++;
	    }
	  else
	    {
	      if(*f1_ptr < *f2_ptr)
		{
		  for(i = 0;i < chrom;i++)
		    *select_ptr++=*s2_ptr++;
		  for(i = 0;i < nvar;i++)
		    *select_ptr_r++=*s2_ptr_r++;
		}
	      else
		{
		  for(i = 0;i < chrom;i++)
		    *select_ptr++=*s1_ptr++;
		  for(i = 0;i < nvar;i++)
		    *select_ptr_r++=*s1_ptr_r++;
		}
	    }
	}
    }
  return;
}







