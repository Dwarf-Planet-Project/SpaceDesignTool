/* This is the header file to do the uniform crossover */

void unicross(population *new_pop_ptr,population *mate_pop_ptr);

void unicross(population *new_pop_ptr, population *mate_pop_ptr)
{
  int i,j,*gene,y,n,*par1,*par2,*chld1,*chld2;
  double rnd;
  for(i = 0,y = 0,n = 0;i < popsize;i++)
    {
      for(j = 0;j < chrom;j++)
	{

	  /*Select a bit for doing cross-over*/	
	  new_pop_ptr->ind_ptr = &(new_pop_ptr->ind[y]);
	  chld1 = &(new_pop_ptr->ind_ptr->genes[j]);

	  new_pop_ptr->ind_ptr = &(new_pop_ptr->ind[y+1]);
	  chld2 = &(new_pop_ptr->ind_ptr->genes[j]);
	
	  mate_pop_ptr->ind_ptr = &(mate_pop_ptr->ind[n]);
	  par1 = &(mate_pop_ptr->ind_ptr->genes[j]);

	  mate_pop_ptr->ind_ptr = &(mate_pop_ptr->ind[n+1]);
	  par2 = &(mate_pop_ptr->ind_ptr->genes[j]);

	  rnd = randomperc();

	  /*Checking whether to do cross-over or not*/
	  if(rnd <= pcross)
	    {
	      ncross++;
	      *chld1 = *par2;
	      *chld2 = *par2;
	    }

	  else
	    {
	      *chld1 = *par1;
	      *chld2 = *par2;
	    }
	}

      y = y+2;
      n = n+2;

    }

  for(i = 0;i < popsize;i++)
    {
      new_pop_ptr->ind_ptr = &(new_pop_ptr->ind[i]);
      gene = &(new_pop_ptr->ind_ptr->genes[0]);
      for(j = 0;j < chrom;j++)
	{
	  gene = &(new_pop_ptr->ind_ptr->genes[j]);
	}
    }
  return;
}


