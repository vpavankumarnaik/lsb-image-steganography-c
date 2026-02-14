#if 0
Name        : V Pavan Kumar Naik
ID          : 25021_097
Description : LSB Image Stegnography
#endif

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
	EncodeInfo encInfo;
	DecodeInfo decInfo;
    	uint img_size;

    	if(argc < 2)
    	{	
		printf("Encoding : ./lsb_steg -e <.bmp file> <.txt file> [output file]\n");
                printf("Decoding : ./lsb_steg -d <.bmp file> [output file]\n");
		return 0;
	}

	if(check_operation_type(argv) == e_encode)				//for encoding
    	{
		if(argc < 4 || argc > 5)
		{
			printf("Encoding : ./lsb_steg -e <.bmp file> <.txt file> [output file]\n");
			return 0;
		}
		if(read_and_validate_encode_args(argv,&encInfo) == e_success)	//if all arguments passed do encoding
	    	{
			if(do_encoding(&encInfo) == e_success)
			{
				printf("INFO: ## Encoding Done Successfully ##\n");
			}
			else
			{
				printf("ERROR: ## Encoding failed Successfully ##\n");
			}
	    	}
	    	else
	    	{
			printf("ERROR: ## Encoding Argument Validation Failed Successfully\n");
	    	}

    	}
    	else if(check_operation_type(argv) == e_decode)				//for decoding
    	{
		if(argc < 3 || argc > 4)
		{
                	printf("Decoding : ./lsb_steg -d <.bmp file> [output file]\n");
			return 0;
		}
		if(read_and_validate_decode_args(argv,&decInfo) == e_success)	//if all arguments passed do decoding
	    	{
			if(do_decoding(&decInfo) == e_success)
			{
				printf("INFO: ## Decoding Done Successfully ##\n");
			}
			else
			{
				printf("ERROR: ## Decoding failed Successfully ##\n");
			}

	    	}
	    	else
	    	{
			printf("ERROR: ## Decoding Argument Validation Failed Successfully ##\n");
		}
    	}
    	else									//if mode not matched print usage message
    	{	
		printf("Encoding : ./lsb_steg -e <.bmp file> <.txt file> [output file]\n");
                printf("Decoding : ./lsb_steg -d <.bmp file> [output file]\n");
	}
    
    	return 0;
}
OperationType check_operation_type(char *argv[])				//check operation type
{
	if(strcmp(argv[1], "-e") == 0)
	{
		return e_encode;
	}
	else if(strcmp(argv[1], "-d") == 0)
	{
		return e_decode;
	}
	else
	{
		return e_unsupported;
	}
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)		//read and validate encoding
{
	if(strstr(argv[2], ".bmp") == NULL)
	{
		printf("ERROR: Source File is not a .bmp file\n");
	       	return e_failure;
	}
	encInfo->src_image_fname = argv[2];
	
	if((strstr(argv[3], ".txt") == NULL) && (strstr(argv[3], ".c") == NULL) && (strstr(argv[3], ".sh") == NULL))
	{
		printf("ERROR: Secret file is not a .txt or .c or .sh file\n");
		return e_failure;
	}
	encInfo->secret_fname = argv[3];
	
	char *ret = strchr(argv[3], '.');
	if(ret != NULL)
	{
		strcpy(encInfo->extn_secret_file, ret);
	}

	if(argv[4] == NULL)							//if output file not given create defauly name
	{
		encInfo->stego_image_fname = "stego.bmp";
		printf("INFO: Output File not mentioned. Creating steged_img.bmp as default\n");
	}
	else									//if output file given 
	{
		if(strstr(argv[4], ".bmp") == NULL)
		{
			printf("ERROR: Output file is not a .bmp file\n");
			return e_failure;
		}
		encInfo->stego_image_fname = argv[4];
	}

	return e_success;
}
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)		//read and validate decoding arguments
{
	if(strstr(argv[2], ".bmp") == NULL)
	{
		printf("ERROR: Stego file is not a .bmp file\n");
	       	return e_failure;
	}
	decInfo->stego_image_fname = argv[2];
	
	if(argv[3] == NULL)							//if output file not given create default name
	{
		strcpy(decInfo->out_image_fname, "output");
		decInfo->out_ext = 1;
	}
	else									//if output file given store it 
	{
		decInfo->out_ext = 0;
		char *ext = strchr(argv[3], '.');				//if output file given with extension remove it
		if(ext != NULL)
		{
			*ext = '\0';	
		}
		strcpy(decInfo->out_image_fname, argv[3]);
	}

	return e_success;
}
