/**********************************************************************************
 * read_lime_data.h
 *
 * Read binary data from lime record files written by QIO from SCIDAC
 * This can be used for files written by QDP++ and Chroma.
 * Serial version
 *
 * A. M. Abdel-Rehim 
 *
 * Baded on the example lime_extract_record.c given in the lime library 
 * by Balint Joo. Parts for checking the endianness of the local machine
 * and swapping bytes if needed is based on the openQCD code by Martin Luscher
 *
 *
 *
 * Date: January 26, 2013
 *
 * Send comments or bugs to: amabdelrehim@gmail.com
 *********************************************************************************/

#ifndef _READ_LIME_DATA_H
#define _READ_LIME_DATA_H

#include <lime_config.h>
#include <stdio.h>
#include <stdlib.h>
#include <lime.h>
#include <lime_fixed_types.h>
/*#define MAXBUF 1048576*/
#define MAXBUF 8

#include <limits.h>
#include <float.h>


/* 
 * Uitilities for byte swapping and checking endiness of the local machine. 
 */

/*
 * Test if the machine is compliant with the IEEE-754 standard
 * These utilites are copied from M. Luscher's openQCD code
 */

#if ((DBL_MANT_DIG!=53)||(DBL_MIN_EXP!=-1021)||(DBL_MAX_EXP!=1024))
#error : Machine is not compliant with the IEEE-754 standard
#endif

#if (SHRT_MAX==0x7fffffff)
typedef short int stdint_t;
typedef unsigned short int stduint_t;
#elif (INT_MAX==0x7fffffff)
typedef int stdint_t;
typedef unsigned int stduint_t;
#elif (LONG_MAX==0x7fffffff)
typedef long int stdint_t;
typedef unsigned long int stduint_t;
#else
#error : There is no four-byte integer type on this machine 
#endif


#undef UNKNOWN_ENDIAN
#undef LITTLE_ENDIAN
#undef BIG_ENDIAN

#define UNKNOWN_ENDIAN 0
#define LITTLE_ENDIAN 1
#define BIG_ENDIAN 2


/* return minimum of two integers */
n_uint64_t mino(n_uint64_t i, n_uint64_t j);

/* Return the endiness of the local machine 
 * 0 unknown, 1 little endian, 2 big endian
*/
int endianness(void);

/* Swap bytes for an array of n elements of 4-byte size
 * works for int or float
 */
void bswap_int(int n,void *a);

/* Swap bytes of an array of n elements of 8-byte size*/
void bswap_double(int n,void *a);



/*
 * Read scidac binary data from an input file as a raw data. The meaning of the data 
 * and its layout as bytes will be interpreted by the calling function. This just reads
 * the data as a series of bytes in the same order it was written by qdp++ (or chroma).
 *
 * buff_out  is the output buffer. It is expected that it occupies buff_size bytes in 
 * memory. It is the responsibility of the user to allocate enough memory for this 
 * buffer when calling this function. Byte swapping will be done afterwards.
 * fin the name of the file from which data will be read.
 * return is EXIT_SUCCESS if the reading was successful or EXIT_FAILURE if not.
 * These are defined in lime
 */

int  read_qdppp_scidac_binary_data(char *buff_out, long int buff_size, char *fin);







/* Read a lattice fermion field written by Chroma or QDP++
 *
 * lattice fermion is given by a colored dirac spinor at each site.
 * lattice sites are counted lexiographically with time index runs
 * slowest, then z, then y, then x. Counting the directions as 0,1,2,3
 * we have 0=x, 1=y, 2=z, 3=t.
 * At each site the spin index runs slowest then the color index.
 * Real part is written first then the imaginary part.
 * Data written by Chroma and qdp++ are stored as big endian. So, when 
 * reading the data on a machine that is little endian one has to swap the 
 * bytes.
 * 
 * latdims[4] is the array which has the size of the lattice in the 0,1,2,3 directions
 * LatticeFermion is a one dimensional array of doubles where the spinor elements are
 * given in the order above. It is expected that this array has properly allocated memory
 * fin is the name of the input file
 */


int read_qdppp_lattice_fermion_float(int latdims[], float  *LatticeFermion, char *fin);

int read_qdppp_lattice_fermion_double(int latdims[], double *LatticeFermion, char *fin);


/*
 *utility function that will return the spinor field at a site
 *site[4]={x,y,z,t} is the site given by global coordinates
 *lat_dims[] size of the lattice in the 4 directions 
 *LatticeFermion is a buffer containing the whole lattice fermion field as read by read_qdppp_lattice_fermion_float
 *spinor[12][2] is the spinor filed with 2 are the real and imaginary parts and 12 refers
 *to color and spin in the order 
 *(spin=0, color=0) 
 *(spin=0, color=1)
 *(spin=0, color=2)
 *(spin=1,color=0)
 *(spin=1, color=1)
 *etc
 *.....
 *(spin=2, color=2)
 */
void get_spinor_float(int site[], int lat_dims[], float *LatticeFermion, float spinor[12][2]);


/*sililarly, this gets a spinor at a site for the double case*/
void get_spinor_double(int site[], int lat_dims[], double *LatticeFermion, double spinor[12][2]);

#endif

    
  
