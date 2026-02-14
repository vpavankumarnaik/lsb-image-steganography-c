#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status open_img_file(DecodeInfo *decInfo)				//opening source file
{
    	// Src Image file
    	decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    	// Do Error handling
    	if (decInfo->fptr_stego_image == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    		return e_failure;
    	}
	printf("INFO: Opened %s\n", decInfo->stego_image_fname);
    	return e_success;
}

Status do_decoding(DecodeInfo *decInfo)					//decoding starts from here
{
	printf("INFO: ## Decoding Procedure Started ##\n");

	printf("INFO: Opening required files\n");
	if(open_img_file(decInfo) != e_success)
	{
		printf("ERROR: Files Are Not Opened\n");
		return e_failure;
	}
	
	if(skip_bmp_header(decInfo->fptr_stego_image) != e_success)
	{
		printf("ERROR: .bmp Header File Are Not Skipped\n");
		fclose(decInfo->fptr_stego_image);
		return e_failure;
	}

	printf("INFO: Decoding Magic String Signature\n");
	if(decode_magic_string(decInfo) != e_success)
	{
		printf("ERROR: Not Decoded Magic String Signature\n");
		fclose(decInfo->fptr_stego_image);
		return e_failure;
	}
	printf("INFO: Done\n");
	
	if(decode_secret_file_extn_size(decInfo) != e_success)
	{
		printf("ERROR: Not Decoded Secret File Extn Size\n");
		fclose(decInfo->fptr_stego_image);
		return e_failure;
	}

	if(decode_secret_file_extn(decInfo) != e_success)
	{
		printf("ERROR: Not Decoded Secret File Extension\n");
		fclose(decInfo->fptr_stego_image);
		return e_failure;
	}

	printf("INFO: Decoding %s File Size\n",  decInfo->out_image_fname);
	if(decode_secret_file_size(decInfo) != e_success)
	{
		printf("ERROR: Not Decoded %s File Size\n",  decInfo->out_image_fname);
		fclose(decInfo->fptr_stego_image);
		fclose(decInfo->fptr_out_image);
		return e_failure;
	}
	printf("INFO: Done\n");

	printf("INFO: Decoding %s File Data\n",  decInfo->out_image_fname);
	if(decode_secret_file_data(decInfo) != e_success)
	{
		printf("ERROR: Not Decoded %s File Data\n",  decInfo->out_image_fname);
		fclose(decInfo->fptr_stego_image);
		fclose(decInfo->fptr_out_image);
		return e_failure;
	}
	printf("INFO: Done\n");

	//closing files
	fclose(decInfo->fptr_stego_image);
	fclose(decInfo->fptr_out_image);
	
	return e_success;
}
Status skip_bmp_header(FILE *src)					//skip bmp header
{
	fseek(src, 54, SEEK_SET);
	return e_success;
}
Status decode_magic_string(DecodeInfo *decInfo)				//decoding secret data
{
	char magic_string[strlen(MAGIC_STRING)+1];
	int i;
	for(i=0; i<strlen(MAGIC_STRING); i++)
	{
		char image_buffer[8];
		fread(image_buffer, 8, 1, decInfo->fptr_stego_image);
		magic_string[i] = decode_byte_from_lsb(image_buffer);
	}
	magic_string[i] = '\0';
	
	//compare if Magic string is matching with current data
	if(strcmp(MAGIC_STRING, magic_string) != 0)			//check if secret data is matched or not
	{
		printf("ERROR: Secret Data Not Matched\n");
		return e_failure;
	}
	return e_success;
}
char decode_byte_from_lsb(char *image_buffer)
{
	char ch = 0;
	for(int i=7; i>=0; i--)
	{
		ch = ch | ((image_buffer[7-i] & 1) << i);
	}
	return ch;
}
Status decode_secret_file_extn_size(DecodeInfo *decInfo)		//decode file extension size
{
	char image_buffer[32];
	fread(image_buffer, 32, 1, decInfo->fptr_stego_image);
	decInfo->size_of_extn = decode_int_from_lsb(image_buffer);

	return e_success;
}
int decode_int_from_lsb(char *image_buffer)				
{
	int size = 0;
	for(int i = 0; i<32; i++)
	{
		size = (size << 1) | (image_buffer[i] & 1);
	}
		
	return size;
}
Status decode_secret_file_extn(DecodeInfo *decInfo)			//decode secret file extension
{
	char extn[decInfo->size_of_extn + 1];
	int i;
	for(i=0; i < decInfo->size_of_extn; i++)
	{
		char image_buffer[8];
		fread(image_buffer, 8, 1, decInfo->fptr_stego_image);
		extn[i] = decode_byte_from_lsb(image_buffer);
	}
	extn[i] = '\0';
	
	strcat(decInfo->out_image_fname, extn);
    	
	// out Image file
    	decInfo->fptr_out_image = fopen(decInfo->out_image_fname, "w");//open the output file
   
    	if (decInfo->fptr_out_image == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->out_image_fname);

    		return e_failure;
    	}
	
	if(decInfo->out_ext == 1)
	{
		printf("INFO: Decoding Output File Extenstion\n");
		printf("INFO: Done\n");
		printf("INFO: Output File not mentioned. Creating %s as default\n", decInfo->out_image_fname);
	}
	printf("INFO: Opened %s\n", decInfo->out_image_fname);
	printf("INFO: Done. Opened all required files\n");
    	return e_success;
}
Status decode_secret_file_size(DecodeInfo *decInfo)			//decode secret file size
{
	char image_buffer[32];
	fread(image_buffer, 32, 1, decInfo->fptr_stego_image);
	decInfo->size_of_secret = decode_int_from_lsb(image_buffer);

	return e_success;
}
Status decode_secret_file_data(DecodeInfo *decInfo)			//decode secret file data
{
	char ch;
	for(int i=0; i < decInfo->size_of_secret; i++)
	{
		char buffer[8];
		fread(buffer, 8, 1, decInfo->fptr_stego_image);
		ch = decode_byte_from_lsb(buffer);
		fwrite(&ch, 1, 1, decInfo->fptr_out_image);
	}
	return e_success;
}
