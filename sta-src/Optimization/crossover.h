/*This is the file for formulating the crossover process*/

void crossover(population *new_pop_ptr,population *mate_pop_ptr) ;

void crossover(population *new_pop_ptr,population *mate_pop_ptr)
{
  int i,k,n,y,mating_site,*par1,*par2,*chld1,*chld2,c;
  double rnd;
  rnd=randomperc();  

  new_pop_ptr->ind_ptr=&(new_pop_ptr->ind[0]);
  
  mate_pop_ptr->ind_ptr=&(mate_pop_ptr->ind[0]); 

  for (i = 0,y = 0,n = 0;i < popsize/2;i++)
    {
      new_pop_ptr->ind_ptr = &(new_pop_ptr->ind[n]);
      chld1=&(new_pop_ptr->ind_ptr->genes[0]);
      n = n+1;
      
      new_pop_ptr->ind_ptr = &(new_pop_ptr->ind[n]);
      chld2=&(new_pop_ptr->ind_ptr->genes[0]);
      n = n+1;
      
      mate_pop_ptr->ind_ptr = &(mate_pop_ptr->ind[y]);
      par1 = &(mate_pop_ptr->ind_ptr->genes[0]);
      y = y+1;
      
      mate_pop_ptr->ind_ptr = &(mate_pop_ptr->ind[y]);
      par2 = &(mate_pop_ptr->ind_ptr->genes[0]); 
      y = y+1;
      
      rnd = randomperc();
      if (rnd < pcross)
	{
	  ncross++;
	  rnd = randomperc();
	  c = floor(rnd*(chrom+10));
	  mating_site = c;
	  if(mating_site >= chrom)
	    {
	      mating_site = mating_site/2;
	    }
	  
	  for(k = 0;k < chrom;k++)
	    {
	      if(k > mating_site-1)
		{
		  *chld1++ = *par2++;
		  *chld2++ = *par1++;
		}
	      else
		{
		  *chld1++ = *par1++;
		  *chld2++ = *par2++;
		}
	    }
	}
    else 
      {
	for (k = 0;k < chrom;k++)
	  {
	    *chld1++ = *par1++;
	    *chld2++ = *par2++;
	  }
      }
    }
  return;
}

