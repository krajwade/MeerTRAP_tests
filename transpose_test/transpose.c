#include<stdio.h>
#include<math.h>
#include<time.h>
#include<stdlib.h>


int main(int argc, char* argv[])
{

    int i,j,k,l,m,n;
    int nbeams, nchans, ntime, nsamples,nfreq;
    FILE *fptr = NULL;
    int *idata=NULL,*odata=NULL;

    nbeams = atoi(argv[1]);
    nchans = atoi(argv[2]);
    ntime = atoi(argv[3]);
    nsamples = atoi(argv[4]);
    nfreq = atoi(argv[5]);

    fptr = fopen("testdata.dat","rb");

    idata = (int *)malloc(nbeams*nchans*ntime*nsamples*nfreq*sizeof(int));
    odata = (int *)malloc(nbeams*nchans*ntime*nsamples*nfreq*sizeof(int));

    fread(idata,sizeof(int),nbeams*nchans*ntime*nsamples*nfreq,fptr);

    fclose(fptr);
    clock_t begin=clock();

 for (n = 0;n < 20 ; n++)
 {
  int a = 0;
    for (i = 0; i < nbeams; i++)
    {

        for (j =0; j < nsamples; j++)
	{
	    
	     for (k = 0; k < ntime ; k++)
	     {

	        for (l = 0; l < nfreq ; l++)
		{

		   for (m=0;m<nchans;m++)
		   {
                      
		      odata[a] = idata[m + ntime*nchans*nsamples*l + nchans*(j*ntime + k) + nsamples*nchans*ntime*nfreq*i];
                      a++;
		   }

		}
            
	      }

	 }
 
     }
 }
     clock_t end = clock();

     double time_spent = (double)(end - begin) / (CLOCKS_PER_SEC);

     printf("Average time taken for transpose: %lf seconds \n",time_spent);

     free(idata);
     free(odata);

     return(0);

}
