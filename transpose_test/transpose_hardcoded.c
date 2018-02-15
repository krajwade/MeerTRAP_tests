#include<stdio.h>
#include<math.h>
#include<time.h>
#include<stdlib.h>

#define NBEAMS  6
#define NCHANS  128
#define NTIME   64
#define NFREQ   32
#define NSAMPLES 1024




int main()
{

    int i,j,k,l,m;
    FILE *fptr = NULL;
    int *idata=NULL,*odata=NULL;


    fptr = fopen("testdata.dat","rb");

    idata = (int *)malloc(NBEAMS*NCHANS*NTIME*NFREQ*NSAMPLES*sizeof(int));
    odata = (int *)malloc(NBEAMS*NCHANS*NTIME*NFREQ*NSAMPLES*sizeof(int));

    fread(idata,sizeof(int),NBEAMS*NCHANS*NTIME*NFREQ*NSAMPLES,fptr);

    fclose(fptr);
    int a = 0;
    clock_t begin=clock();

    for (i = 0; i < NBEAMS; i++)
    {

        for (j =0; j < NSAMPLES; j++)
	{
	    
	     for (k = 0; k < NTIME ; k++)
	     {

	        for (l = 0; l < NFREQ ; l++)
		{

		   for (m=0;m<NCHANS;m++)
		   {
                      
		      odata[a] = idata[m + NTIME*NCHANS*NSAMPLES*l + NCHANS*(j*NTIME + k) + NSAMPLES*NCHANS*NTIME*NFREQ*i];
                      a++;

		   }

		}
            
	      }

	 }
 
     }


     clock_t end = clock();

     double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

     printf("Time taken for transpose: %lf seconds \n",time_spent);

     free(idata);
     free(odata);

     return(0);

}
