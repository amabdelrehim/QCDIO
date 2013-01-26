/*********************************************************
 * example.c
 *
 * test program for using the utility reading functions
 *
 * A. M. Abdel-Rehim
 * January 26, 2013
 * amabdelrehim@gmail.com
 *********************************************************/
#include"read_lime_data.h"




int main(int argc, char *argv[]){ 

    int i,j;

    int lat_size[]={32,32,32,64}; /* lattice size */

    if(argc != 3)
    {
        fprintf(stderr,"Usage: exec_name input_file_name output_file_name\n");

        return 0;
    }

    long int num_bytes=lat_size[0]*lat_size[1]*lat_size[2]*lat_size[3]*4*3*2*4; /*single precision lattice fermion */

    int num_elems=num_bytes/4;

    float *lattice_fermion= (float *) calloc(num_elems,sizeof(float));

    i=read_qdppp_lattice_fermion_float(lat_size,lattice_fermion,argv[1]);
    
    fprintf(stdout,"reading a single precision fermion field, status = %d\n",i);

    int ix,iy,iz,it,is,ic;
    int count=0;

    FILE *fout=fopen(argv[2],"w");

    if(fout==NULL)
    {
        fprintf(stderr,"Error: couldn't open output file\n");
        return 1;
    }

    float myspinor[12][2]; /*spinor at a single site*/
    int site[4];


    it=9; /*just read a single time slice and write it out to a file*/
    for(iz=0; iz<lat_size[2]; iz++)
       for(iy=0; iy<lat_size[1]; iy++)
         for(ix=0; ix<lat_size[0]; ix++)
         {
            site[0]=ix; site[1]=iy; site[2]=iz; site[3]=it;
            get_spinor_float(site,lat_size,lattice_fermion,myspinor);
            for(i=0; i<12; i++)
                fprintf(fout,"%f   %f\n",myspinor[i][0],myspinor[i][1]);
               
         }
    return 0;

}



