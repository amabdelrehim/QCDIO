/***************************************************************************
 * read_lime_data.c
 * read data from lime files
 * A.M. Abdel-Rehim 2013
 * Based on examples given in the lime libarary by Balint Joo and C. DeTar
 ***************************************************************************/

#include"read_lime_data.h"


 n_uint64_t mino(n_uint64_t i, n_uint64_t j){
  return i < j ? i : j;
 }
 

int endianness(void)
{
   stduint_t i;
   unsigned char *b;

   i=0x04030201;
   b=(unsigned char*)(&i);

   if ((b[0]==1u)&&(b[1]==2u)&&(b[2]==3u)&&(b[3]==4u))
      return LITTLE_ENDIAN;
   else if ((b[0]==4u)&&(b[1]==3u)&&(b[2]==2u)&&(b[3]==1u))
      return BIG_ENDIAN;
   else return UNKNOWN_ENDIAN;
}


void bswap_int(int n,void *a)
{
   unsigned char *ba,*bam,bas;

   ba=(unsigned char*)(a);
   bam=ba+4*n;

   for (;ba<bam;ba+=4)
   {
      bas=ba[3];
      ba[3]=ba[0];
      ba[0]=bas;

      bas=ba[2];
      ba[2]=ba[1];
      ba[1]=bas;
   }
}


void bswap_double(int n,void *a)
{
   unsigned char *ba,*bam,bas;

   ba=(unsigned char*)(a);
   bam=ba+8*n;

   for (;ba<bam;ba+=8)
   {
      bas=ba[7];
      ba[7]=ba[0];
      ba[0]=bas;

      bas=ba[6];
      ba[6]=ba[1];
      ba[1]=bas;

      bas=ba[5];
      ba[5]=ba[2];
      ba[2]=bas;

      bas=ba[4];
      ba[4]=ba[3];
      ba[3]=bas;
   }
}
                                                     


int read_qdppp_scidac_binary_data(char *buff_out, long int buff_size, char *fin)
{

  char buf[MAXBUF];  /* auxilary buffer for using a buffer copy. Bytes first copied to buf then from buf to buff_out*/
  LimeReader *reader;
  int status;
  n_uint64_t nbytes, bytes_left, bytes_to_copy, read_bytes;
  int rec_seek,msg_seek;
  int rec, msg;
  char *lime_type;
  size_t bytes_pad;
  int MB_flag, ME_flag;

  int i;
  /* Open file */
  FILE *fp = fopen(fin, "r");
  if(fp == NULL) { 
    fprintf(stderr,"Unable to open file %s for reading\n", fin);
    return EXIT_FAILURE;
  }



  /* Open LIME reader */
  reader = limeCreateReader(fp);
  if( reader == (LimeReader *)NULL ) { 
    fprintf(stderr, "Unable to open LimeReader\n");
    return EXIT_FAILURE;
  }


  /* Loop over records */
  msg=0;
  rec=0; 
  while( (status = limeReaderNextRecord(reader)) != LIME_EOF ){
    
    if( status != LIME_SUCCESS ) { 
      fprintf(stderr, "limeReaderNextRecord returned status = %d\n", 
	      status);
      return EXIT_FAILURE;
    }

    nbytes    = limeReaderBytes(reader);
    lime_type = limeReaderType(reader);
    bytes_pad = limeReaderPadBytes(reader);
    MB_flag   = limeReaderMBFlag(reader);
    ME_flag   = limeReaderMEFlag(reader);

    /* Update message and record numbers */
    if(MB_flag == 1){
      msg++;
      rec = 0;
    }

    rec++;

#if 1
    printf("\n\n");
    printf("Type:           %s\n",   lime_type);
    printf("Data Length:    %ld\n",  nbytes);
    printf("Padding Length: %zd\n",   bytes_pad);
    printf("MB flag:        %d\n",   MB_flag);
    printf("ME flag:        %d\n",   ME_flag);
#endif


    /* Skip to next record until target record is reached */
    /*if (msg != msg_seek || rec != rec_seek) continue;*/
    if (strcmp(lime_type,"scidac-binary-data") != 0) continue;
    
    /*check that the data record has the expected number of byutes*/
    if(nbytes != buff_size)
    {
      fprintf(stderr,"Error, expecting %ld bytes but data has %ld bytes\n",buff_size,nbytes);
      return EXIT_FAILURE;
    }


    /* Buffered copy */
    
    bytes_left = nbytes;
    int bcopied=0;
    while(bytes_left > (n_uint64_t)0){
      bytes_to_copy = mino((n_uint64_t)MAXBUF,bytes_left);
      read_bytes = bytes_to_copy;
      status = limeReaderReadData((void *)buf, &read_bytes, reader);
    
      if( status < 0 && status != LIME_EOR ) { 
	fprintf(stderr, "LIME read error occurred: status= %d\n", status);
	exit(EXIT_FAILURE);
      }
      if (read_bytes != bytes_to_copy) {
	fprintf(stderr, "Read error %lld bytes wanted,%lld read\n", 
		(unsigned long long)nbytes, (unsigned long long)read_bytes);
	return EXIT_FAILURE;
      }
    
      /* copy to the output buffer */
      for(i=0; i<read_bytes; i++)
        buff_out[bcopied+i]=buf[i];

      bcopied += read_bytes;
      bytes_left -= bytes_to_copy;
    }

    /* Quit at this record */
    break;
  }

  limeDestroyReader(reader);
  DCAP(fclose)(fp);

  return EXIT_SUCCESS;
}   
    


int read_qdppp_lattice_fermion_float(int latdims[], float  *LatticeFermion, char *fin)
{

    long int buff_size=latdims[0]*latdims[1]*latdims[2]*latdims[3]*4*3*2*4; /*size in bytes of a single precision lattice fermion*/
    int num_elem=buff_size/4;
    int check_reading;

    check_reading=read_qdppp_scidac_binary_data((void *)LatticeFermion, buff_size,fin);

    if(check_reading == EXIT_FAILURE)
    {
        fprintf(stderr,"Error reading the SCIDAC binary data\n");
        exit(EXIT_FAILURE);
    }

    /*test the machine endianness*/
    int check_end=endianness();

    if(check_end == UNKNOWN_ENDIAN)
    {
       fprintf(stderr,"Error: machine endianess is unknown.\n");
       exit(EXIT_FAILURE);
    }

    if(check_end == BIG_ENDIAN)
      return;   /*no swapping is needed*/

    if(check_end == LITTLE_ENDIAN)
      bswap_int(num_elem,LatticeFermion);

    return EXIT_SUCCESS;   
}



 
int read_qdppp_lattice_fermion_double(int latdims[], double  *LatticeFermion, char *fin)
{

    long int buff_size=latdims[0]*latdims[1]*latdims[2]*latdims[3]*4*3*2*8; /*size in bytes of a single precision lattice fermion*/
    int num_elem=buff_size/8;
    int check_reading;

    check_reading=read_qdppp_scidac_binary_data((void *)LatticeFermion, buff_size,fin);

    if(check_reading == EXIT_FAILURE)
    {
        fprintf(stderr,"Error reading SCIDAC binary data\n");
        exit(EXIT_FAILURE);
    }

    /*test the machine endianness*/
    int check_end=endianness();

    if(check_end == UNKNOWN_ENDIAN)
    {
       fprintf(stderr,"Error: machine endianess is unknown.\n");
       exit(EXIT_FAILURE);
    }

    if(check_end == BIG_ENDIAN)
      return;   /*no swapping is needed*/

    if(check_end == LITTLE_ENDIAN)
      bswap_double(num_elem,LatticeFermion);

    return;   
}


void get_spinor_float(int site[], int lat_dims[], float *LatticeFermion, float spinor[12][2])
{
    int linear_index;

    linear_index=site[0]+site[1]*lat_dims[0]+site[2]*lat_dims[0]*lat_dims[1]+site[3]*lat_dims[0]*lat_dims[1]*lat_dims[2];

    int loc=linear_index*24; /*first element in the LatticeFermion array for this spinor*/


    int is,ic,j,nc=3,ns=4;
    int count=loc;
    for(is=0; is<4; is++)
      for(ic=0; ic<3; ic++)
      {
          j=ic+is*nc; /*linear index for color-spin*/
          spinor[j][0]=LatticeFermion[count];
          spinor[j][1]=LatticeFermion[count+1];
          count +=2;
      }
   
    return;
}


void get_spinor_double(int site[], int lat_dims[], double *LatticeFermion, double spinor[12][2])
{
    int linear_index;

    linear_index=site[0]+site[1]*lat_dims[0]+site[2]*lat_dims[0]*lat_dims[1]+site[3]*lat_dims[0]*lat_dims[1]*lat_dims[2];

    int loc=linear_index*24; /*first element in the LatticeFermion array for this spinor*/


    int is,ic,j,nc=3,ns=4;
    int count=loc;
    for(is=0; is<4; is++)
      for(ic=0; ic<3; ic++)
      {
          j=ic+is*nc; /*linear idex for color-soin*/
          spinor[j][0]=LatticeFermion[count];
          spinor[j][1]=LatticeFermion[count+1];
          count +=2;
      }
   
    return;
}


