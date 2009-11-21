/*This is the program to decode the chromosome to get real values*/

void decode(population *pop_ptr);

void decode(population *pop_ptr)
{
  double *real_ptr;
  int i,sum,b,k,c,d,*gene_ptr,m,x;

  pop_ptr->ind_ptr = &(pop_ptr->ind[0]);
  
  for(i = 0; i < popsize; i++)
    {
      real_ptr = &(pop_ptr->ind_ptr->xbin[0]);
      gene_ptr = &(pop_ptr->ind_ptr->genes[0]);
      for(m = 0; m < nchrom; m++)
	{
	  /*finding out the co-efficient 2 to the power of 
	    (l-1) where l is the no of bits assigned to this variable
	    
	    For More Info Study DEB's Book*/
	  sum = 0;
	  for(k = 0; k < vlen[m]; k++)
	    {
	      b = *gene_ptr;
	      d = vlen[m] - k - 1;
	      c = pow(2.0,d);
	      sum =sum + c * b;
	      gene_ptr++;
	    }
	  
	  x = vlen[m];
	  coef[m] = pow(2.0,x) - 1;
	  *real_ptr =lim_b[m][0] + (sum/coef[m])*(lim_b[m][1]-lim_b[m][0]);
	  real_ptr++;
	}
      pop_ptr->ind_ptr = &(pop_ptr->ind[i+1]);
    }
   return ;
}

