
/*This is the file which initializes the population*/
void realinit(population *pop_ptr);

void realinit(population *pop_ptr)
{
  int i,j;
  double d,d1;


  for (i = 0 ; i < popsize ; i++)
	{
	 for (j = 0; j < nvar; j++)
		{
		  d = randomperc();
		  d1 = 2*d - 1;
		  /*if limits are not specified then generates any number between
		    zero and infinity*/
		  if(ans != 1)
		    {
		      pop_ptr->ind[i].xreal[j] = 1/d1 ;
			}
		  /*if limits are specified it generates the value in
		    range of minimum and maximum value of the variable*/
		 else
			  pop_ptr->ind[i].xreal[j] = d*(lim_r[j][1] - lim_r[j][0])+lim_r[j][0];
		}
	}

 return;
}























































