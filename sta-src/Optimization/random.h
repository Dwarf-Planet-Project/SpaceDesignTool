#include <math.h> 
 
/* variables are declared static so that they cannot conflict 
with names of   */ 
/* other global variables in other files.  See K&R, p 80, for 
scope of static */ 
static double oldrand[55];                      /* Array of 55 
random numbers */ 
static int jrand;                                    /* 
current random number */ 
static double rndx2;                       /* used with random 
normal deviate */ 
static int rndcalcflag;                    /* used with random 
normal deviate */ 
 
void advance_random() 
/* Create next batch of 55 random numbers */ 
{ 
    int j1; 
    double new_random; 
 
    for(j1 = 0; j1 < 24; j1++) 
    { 
        new_random = oldrand[j1] - oldrand[j1+31]; 
        if(new_random < 0.0) new_random = new_random + 1.0; 
        oldrand[j1] = new_random; 
    } 
    for(j1 = 24; j1 < 55; j1++) 
    { 
        new_random = oldrand [j1] - oldrand [j1-24]; 
        if(new_random < 0.0) new_random = new_random + 1.0; 
        oldrand[j1] = new_random; 
    } 
} 
 
 
int flip(double prob) 
/* Flip a biased coin - true if heads */ 
{ 
    double randomperc(); 
 
    if(randomperc() <= prob) 
        return(1); 
    else 
        return(0); 
} 
 
 
void initrandomnormaldeviate() 
/* initialization routine for randomnormaldeviate */ 
{ 
    rndcalcflag = 1; 
} 
 
 
double noise(double mu ,double sigma) 
/* normal noise with specified mean & std dev: mu & sigma */ 
{ 
    double randomnormaldeviate(); 
 
    return((randomnormaldeviate()*sigma) + mu); 
} 
 
 
//randomize(double randomseed) 
  /*Get seed number for random and start it up */ 
/*{ 
    int j1; 
 
    for(j1=0; j1<=54; j1++) oldrand[j1] = 0.0; 
    jrand=0; 
 
    if(numfiles == 0) 
    { 
        do 
        { 
            fprintf(outfp," Enter random number seed, 0.0 to 1.0 -> "); 
            fscanf(infp,"%f", &randomseed); 
        } 
        while((randomseed < 0.0) || (randomseed > 1.0)); 
    } 
    else 
    { 
        fscanf(infp,"%f", &randomseed); 
    } 
 
 
    warmup_random(randomseed); 
    } */
 
 
double randomnormaldeviate() 
/* random normal deviate after ACM algorithm 267 / Box-Muller Method */ 
{ 
//    double sqrt(), log(), sin(), cos(); 
    double randomperc();  
    double t, rndx1; 
 
    if(rndcalcflag) 
    { 
        rndx1 = sqrt(- 2.0*log((double) randomperc())); 
        t = 6.2831853072 * (double) randomperc(); 
        rndx2 = sin(t); 
        rndcalcflag = 0; 
        return(rndx1 * cos(t)); 
    } 
    else 
    { 
        rndcalcflag = 1; 
        return(rndx2); 
    } 
} 
 
 
double randomperc() 
/* Fetch a single random number between 0.0 and 1.0 - Subtractive Method */ 
/* See Knuth, D. (1969), v. 2 for details */ 
/* name changed from random() to avoid library conflicts on some machines*/ 
{ 
    jrand++; 
    if(jrand >= 55) 
    { 
        jrand = 1; 
        advance_random(); 
    } 
    return((double) oldrand[jrand]); 
} 
 
 
int rnd(int low, int high) 
/* Pick a random integer between low and high */ 
{ 
    int i; 
    double randomperc(); 
 
    if(low >= high) 
        i = low; 
    else 
    { 
        i = (randomperc() * (high - low + 1)) + low; 
        if(i > high) i = high; 
    } 
    return(i); 
} 
 
 
double rndreal(double lo ,double hi) 
/* real random number between specified limits */
{ 
    return((randomperc() * (hi - lo)) + lo); 
} 
 
 
void warmup_random(double random_seed) 
/* Get random off and running */  
{ 
    int j1, ii; 
    double new_random, prev_random; 
 
    oldrand[54] = random_seed; 
    new_random = 0.000000001; 
    prev_random = random_seed; 
    for(j1 = 1 ; j1 <= 54; j1++) 
    { 
        ii = (21*j1)%54; 
        oldrand[ii] = new_random; 
        new_random = prev_random-new_random; 
        if(new_random<0.0) new_random = new_random + 1.0; 
        prev_random = oldrand[ii]; 
    } 
 
    advance_random(); 
    advance_random(); 
    advance_random(); 
 
    jrand = 0; 
} 
/*-------------------------------------------------------------*/
