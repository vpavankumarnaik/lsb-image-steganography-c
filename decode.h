#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4
#define MAX_FILE_NAME 100

typedef struct _DecodeInfo
{
    	/* Stego Image Info */
    	char *stego_image_fname;
    	FILE *fptr_stego_image;
    	int size_of_extn;
	int size_of_secret;

    	/*Output File Info*/
    	char out_image_fname[MAX_FILE_NAME];
    	FILE *fptr_out_image;
	int out_ext;

} DecodeInfo;


/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointer for i/p file */
Status open_img_file(DecodeInfo *decInfo);

/* Skip bmp image header */
Status skip_bmp_header(FILE *src);

/* Store Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
char decode_byte_from_lsb(char *image_buffer);

int decode_int_from_lsb(char *image_buffer);

#endif
