/* This is the module used to formulate the mutation routine*/

void mutate(population *new_pop_ptr);             

void mutate(population *new_pop_ptr)
{
  int i,*ptr,j;
  double rand1;

  rand1=randomperc();
  new_pop_ptr->ind_ptr = &(new_pop_ptr->ind[0]);
  
  for(j = 0;j < popsize;j++)
    {
      ptr= &(new_pop_ptr->ind_ptr->genes[0]);
      new_pop_ptr->ind_ptr =&(new_pop_ptr->ind[j+1]);     
      
      /*Select bit */
      for (i = 0;i < chrom;i++)
	{
	  rand1 = randomperc();
	  
	  /*Check whether to do mutation or not*/
	  if(rand1 <= pmut_b)
	    {
	      if(*ptr == 0)
		*ptr =1;
	      else
		*ptr=0;
	      nmut++;
	    }
	  ptr++;
	}
    }
  return;
}
