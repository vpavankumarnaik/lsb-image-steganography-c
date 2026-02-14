#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)				//for get image size
{
    	uint width, height;
    	// Seek to 18th byte
    	fseek(fptr_image, 18, SEEK_SET);

    	// Read the width (an int)
    	fread(&width, sizeof(int), 1, fptr_image);
    	//printf("width = %u\n", width);

    	// Read the height (an int)
    	fread(&height, sizeof(int), 1, fptr_image);
    	//printf("height = %u\n", height);

    	// Return image capacity
	printf("INFO: Done. Found OK\n");
    	return width * height * 3;					//multiply width and height with 3[RGB]
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)					//open the all files
{
    	printf("INFO: Opening required files\n");
	// Src Image file
    	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    	// Do Error handling
    	if (encInfo->fptr_src_image == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    		return e_failure;
    	}
	printf("INFO: Opened %s\n", encInfo->src_image_fname);

    	// Secret file
    	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    	// Do Error handling
    	if (encInfo->fptr_secret == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    		return e_failure;
    	}
	printf("INFO: Opened %s\n", encInfo->secret_fname);

    	// Stego Image file
    	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    	// Do Error handling
    	if (encInfo->fptr_stego_image == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    		return e_failure;
    	}
	printf("INFO: Opened %s\n", encInfo->stego_image_fname);

    	// No failure return e_success
    	return e_success;
}
Status do_encoding(EncodeInfo *encInfo)					//encoding started from hear
{
	if(open_files(encInfo) != e_success)
	{
		printf("ERROR: Files are not opened\n");
		return e_failure;
	}
	printf("INFO: Done\n");
	
	printf("INFO: ## Encoding Procedure Started ##\n");
	if(check_capacity(encInfo) != e_success)
	{
		printf("ERROR: .bmp File Capacity is Not Sutable\n");
		fclose(encInfo->fptr_src_image);
		fclose(encInfo->fptr_secret);
		fclose( encInfo->fptr_stego_image);
		return e_failure;
	}
	
	printf("INFO: Copying Image Header\n");
	if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
	{
		printf("ERROR: .bmp File Header is Not Copied\n");
		fclose(encInfo->fptr_src_image);
		fclose(encInfo->fptr_secret);
		fclose( encInfo->fptr_stego_image);
		return e_failure;
	}
	
	printf("INFO: Encoding Magic String Signature\n");
	if(encode_magic_string(MAGIC_STRING, encInfo) != e_success)
	{
		printf("ERROR: Not Encoded Magic String Signature\n");
		fclose(encInfo->fptr_src_image);
		fclose(encInfo->fptr_secret);
		fclose( encInfo->fptr_stego_image);
		return e_failure;
	}
	printf("INFO: Done\n");
	
	if(encode_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) != e_success)
	{
		printf("ERROR: Not Encoded Magic String Size\n");
		fclose(encInfo->fptr_src_image);
		fclose(encInfo->fptr_secret);
		fclose( encInfo->fptr_stego_image);
		return e_failure;
	}

	printf("INFO: Encoding %s File Extenstion\n", encInfo->secret_fname);
	if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) != e_success)
	{
		printf("ERROR: Not Encoded %s File Extenstion\n", encInfo->secret_fname);
		fclose(encInfo->fptr_src_image);
		fclose(encInfo->fptr_secret);
		fclose( encInfo->fptr_stego_image);
		return e_failure;
	}
	printf("INFO: Done\n");

	printf("INFO: Encoding %s File Size\n", encInfo->secret_fname);
	if(encode_secret_file_size(encInfo->size_secret_file, encInfo) != e_success)
	{
		printf("ERROR: Not Encoded %s File Size\n", encInfo->secret_fname);
		fclose(encInfo->fptr_src_image);
		fclose(encInfo->fptr_secret);
		fclose( encInfo->fptr_stego_image);
		return e_failure;
	}
	printf("INFO: Done\n");

	printf("INFO: Encoding %s File Data\n", encInfo->secret_fname);
	if(encode_secret_file_data(encInfo) != e_success)
	{
		printf("ERROR: Not Encoded %s File Data\n", encInfo->secret_fname);
		fclose(encInfo->fptr_src_image);
		fclose(encInfo->fptr_secret);
		fclose( encInfo->fptr_stego_image);
		return e_failure;
	}
	printf("INFO: Done\n");

	printf("INFO: Copying Left Over Data\n");
	if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
	{
		printf("ERROR: Remaining Data Not Copied\n");
		fclose(encInfo->fptr_src_image);
		fclose(encInfo->fptr_secret);
		fclose( encInfo->fptr_stego_image);
		return e_failure;
	}
	printf("INFO: Done\n");

	//closing all files
	fclose(encInfo->fptr_src_image);
	fclose(encInfo->fptr_secret);
	fclose( encInfo->fptr_stego_image);

	return e_success;
}
Status check_capacity(EncodeInfo *encInfo)
{
	int magic_string = strlen(MAGIC_STRING);
	int extension = strlen(encInfo->extn_secret_file);

	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
	
	printf("INFO: Checking for beautiful.bmp capacity to handle secret.txt\n");
	int image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

	int encoding_things = (magic_string + sizeof(int) + extension + sizeof(int) + encInfo->size_secret_file) * 8;

	if(encoding_things <= image_capacity)
		return e_success;
	else
		return e_failure;
}
uint get_file_size(FILE *fptr)
{
	printf("INFO: Checking for secret.txt size\n");
	fseek(fptr, 0, SEEK_END);
	uint size = ftell(fptr);
	rewind(fptr);
	if(size != 0)
		printf("INFO: Done. Not Empty\n");
	return size;
}
Status copy_bmp_header(FILE *src, FILE *dest)				//copy header file
{
	fseek(src,0,SEEK_SET);
	char header[54];
	fread(header, 54, 1, src);
	fwrite(header, 54, 1, dest);
	printf("INFO: Done\n");
	return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)//encoding code
{
	if(encode_data_to_image(magic_string, strlen(MAGIC_STRING), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}
Status encode_data_to_image(const char *data, int size, FILE *src, FILE *dest)
{
	for(int i=0; i<size; i++)
	{
		char image_buffer[8];
		fread(image_buffer, 8, 1, src);
		encode_byte_to_lsb(data[i], image_buffer);
		fwrite(image_buffer, 8, 1, dest);
	}
	return e_success;
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
	for(int i=0; i<8; i++)
	{
		image_buffer[i] = ((data >> (7-i)) & 1) | (image_buffer[i] & ~1);
	}
	return e_success;
}
Status encode_file_extn_size(int size, EncodeInfo *encInfo)		//encoding file extension size
{
	char image_buffer[32];
	fread(image_buffer, 32, 1, encInfo->fptr_src_image);
	encode_int_to_lsb(size, image_buffer);
	fwrite(image_buffer, 32, 1, encInfo->fptr_stego_image);
	return e_success;
}
Status encode_int_to_lsb(int size, char *image_buffer)
{
	for(int i=0; i<32; i++)
	{
		image_buffer[i] = ((size >> (31-i)) & 1) | (image_buffer[i] & ~1);
	}
	return e_success;
}
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)//encoding secret file extension
{
	if(encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
		return e_success;
	else
		return e_failure;
}
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo) 	//encoding secret file size
{
    	char image_buffer[32];
    	fread(image_buffer, 32, 1, encInfo->fptr_src_image);
    	encode_int_to_lsb(file_size, image_buffer);
    	fwrite(image_buffer, 32, 1, encInfo->fptr_stego_image);
    	return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)			//encoding secret file data
{
	long n = encInfo->size_secret_file;
	for(int i=0; i<n; i++)
	{
		char buffer[8];
		char data;
		fread(buffer, 8, 1, encInfo->fptr_src_image);
		fread(&data, 1, 1, encInfo->fptr_secret);
		encode_byte_to_lsb(data, buffer);
		fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
	}
	return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)		//copy leftover data into output image
{
	int data;
	while((data = fgetc(fptr_src)) != EOF)
	{
		fputc(data, fptr_dest);
	}
	return e_success;
}
