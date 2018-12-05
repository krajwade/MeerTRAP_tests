#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include<stdlib.h>


int main(int argc, char* argv[])
{

   int ngroups, nbeams, nchans, ntime, nsamples,nfreq;
   int i,j,k,l,m,n;
   FILE *fptr=NULL;

   nbeams = atoi(argv[1]);
   nchans = atoi(argv[2]);
   ntime = atoi(argv[3]);
   nsamples = atoi(argv[4]);
   nfreq= atoi(argv[5]);
   ngroups = atoi(argv[6]);
   srand(time(NULL));

   fptr = fopen("testdata.dat","wb");
   if (fptr == NULL)
   {
       printf("error opening file\n");
       exit(1);
   }

   for (n=0; n<ngroups; n++)
   {
 
   for (i=0;i<nbeams;i++)
   {
    
       for (j=0;j<nfreq;j++)
       {
          
	   for (k=0;k< nsamples; k++)
	   {
	       
	        for (l=0 ; l<ntime;l++)
		{

		    for (m=0;m<nchans;m++)
		    {
		       char idata = rand() % 10 + 1;
		       fwrite(&idata,sizeof(char),1,fptr);
		    } 

               }
           }

       }

   }

  }

   fclose(fptr);
   return(0);
}

      
