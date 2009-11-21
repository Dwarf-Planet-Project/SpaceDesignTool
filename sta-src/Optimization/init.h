
/*This is the file which initializes the population*/
void init(population *pop_ptr);

void init(population *pop_ptr)
{
  int i,j;
  double d;
  pop_ptr->ind_ptr = &(pop_ptr->ind[0]);

  /*Loop Over the population size*/
  for (i = 0 ; i < popsize ; i++)
    {

      /*Loop over the chromosome length*/
      for (j = 0;j < chrom;j++)
	{
	  /*Generate a Random No. if it is less than 0.5 it
	    generates a 0 in the string otherwise 1*/
	  d = randomperc();
	  if(d >= 0.5)
	    {
	      pop_ptr->ind_ptr->genes[j] = 1;
	    }
	  else
	    {
	      pop_ptr->ind_ptr->genes[j] = 0;
	    }
	}
      pop_ptr->ind_ptr = &(pop_ptr->ind[i+1]);
    }
  pop_ptr->ind_ptr = &(pop_ptr->ind[0]);
 return;
}






























































