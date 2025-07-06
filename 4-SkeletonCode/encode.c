#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/*Check operation type 
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
OperationType check_operation_type(char *argv[])
{
	if((strcmp(argv[1],"-e")) == 0)
	{
		return e_encode;
	}
	else if((strcmp(argv[1],"-d")) == 0)
	{
		return e_decode;
	}
	else
	{
		return e_unsupported;
	}
}

/*Read and validate Encode args from argv 
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	char arr[20];
	if(strstr(argv[2],".bmp") == NULL)
	{
	 	printf("ERROR : INPUT FILE IS NOT A BMP FILE.\n\n");
		return e_failure;
	}
	else
	{
		encInfo -> fptr_src_image= fopen(argv[2],"r");
	
		if(encInfo -> fptr_src_image == NULL)
		{
			printf("ERROR : Source file was not able to open.\n\n");
		}
		else
		{
			fread(arr,1,2,encInfo -> fptr_src_image);
		
			if(arr[0] == 0x42 && arr[1] == 0x4d)
			{
				printf("INFO : Source file is .bmp file.\n\n");
				encInfo -> src_image_fname = argv[2];
			}
			else
			{
				printf("INFO : Source file is not a .bmp file.\n\n");
				return e_failure;
			}
		}
	}
	encInfo -> secret_fname = argv[3];
	
	strcpy(arr,argv[3]);
	int i,j;
	for(i=0;arr[i] != '\0';i++)
	{
		if(arr[i] == '.')
		{
			for(j=0;arr[i+j] != '\0';j++)
			{
				encInfo -> extn_secret_file[j] = arr[i+j];
			}
			encInfo	-> secret_extn_size = j;
		}

	}
	printf("INFO : Secret file name is saved.\n\n");
	if(argv[4] != NULL)
	{
		encInfo -> fptr_stego_image= fopen(argv[4],"w");
	
		if(encInfo -> fptr_stego_image == NULL)
		{
			printf("ERROR : Output file was not able to open.\n\n");
			return e_failure;
		}
		else
		{
			if(strstr(argv[4],".bmp"))
			{
				printf("INFO : Output file is saved as per given file name:\n\n");
				encInfo -> stego_image_fname = argv[4]; 
			}
			else
			{
				printf("INFO : Output file was not as per given format(.bmp).\n\n");
				return e_failure;
			}
		}
	}
	else
	{
		printf("INFO : Output file is saved as default file name(stego.bmp).\n\n");
		encInfo -> stego_image_fname = "stego.bmp";	
	}
	return e_success;
}

/*Perform the encoding
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status do_encoding(EncodeInfo *encInfo)
{
	if(open_files(encInfo) == e_failure)
	{
		printf("Error occured during file opening.\n");
		return e_failure;
	}
	if(check_capacity(encInfo) == e_failure)
	{
		printf("Error: Source image cant store the secret text.\n");
		return e_failure;
	}
	
	copy_bmp_header(encInfo ->fptr_src_image,encInfo ->fptr_stego_image);
	
	encode_magic_string(MAGIC_STRING, encInfo);
	
	encode_secret_file_extn_size(encInfo);
	
	encode_secret_file_extn(encInfo ->extn_secret_file,encInfo);
	
	encode_secret_file_size(get_file_size(encInfo -> fptr_secret),encInfo);

	encode_secret_file_data(encInfo);

	copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}


/* check capacity
 * Input: structure
 * Output: encoding is possible or not
 * Description: it checks the whether secret file will be encoded in source image or not
 */
Status check_capacity(EncodeInfo *encInfo)
{
	if( ((strlen(MAGIC_STRING)*8) + 32 + (strlen(strstr(encInfo ->secret_fname,"."))*8) + 32 + (get_file_size(encInfo->fptr_secret) * 8))<=get_image_size_for_bmp(encInfo ->fptr_src_image) )
	{
		printf("INFO : Secret file can be encoded in source image.\n\n");
	}
	else
	{
		return e_failure;
	}
}
/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
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
    return width * height * 3;
}

/* Get file size
 * Input: file pointer of secret file
 * Output: size of secret file
 * Description: It calculates the size of secret file
 */
uint get_file_size(FILE *fptr)
{
	int size;
	fseek(fptr,0,SEEK_END);
	size = ftell(fptr);
	return size;
}

/* Copy bmp image_header
 * Input: file pointer of source image, file pointer for destination image
 * Output: data will be copied to destination file
 * Description: copy bmp header to output file.
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	rewind(fptr_src_image);
	rewind(fptr_dest_image);
	char arr[54];
	fread(arr,1,54,fptr_src_image);
	fwrite(arr,1,54,fptr_dest_image);
	
	printf("INFO : Successfully copied bmp header to output file.\n\n");
}

/* Store Magic string
 * Input: magic string, structure
 * Output: data will be encoded 
 * Description: It encodes the magic_string data to source image
 */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
 {
 	encode_data_to_image(MAGIC_STRING,2,encInfo ->fptr_src_image, encInfo ->fptr_stego_image);
 	
	printf("INFO : Successfully Encoded magic string.\n\n");
 }

/* Encodes secret file extension
 * Input: file extension, structure
 * Output: data will be encoded 
 * Description: It encodes the secret file extension data to source image
 */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
	encode_data_to_image ( file_extn , encInfo->secret_extn_size , encInfo -> fptr_src_image , encInfo -> fptr_stego_image );
	
	printf("INFO : Successfully Encoded secret file extension.\n\n");
}

/* Encode secret file size
 * Input: size of file extension, structure
 * Output: data will be encoded 
 * Description: It encodes the secret file size to source image
 */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
	encInfo -> secret_extn_size = file_size;
	encode_secret_file_extn_size(encInfo);
	
	printf("INFO : Successfully Encoded secret file size.\n\n");
}

/* Encode secret file data
 * Input: structure 
 * Output: data will be encoded 
 * Description: It encodes the secret file data to source image
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char arr[encInfo -> secret_extn_size];
	rewind(encInfo -> fptr_secret);
	for(int i=0;i<encInfo -> secret_extn_size;i++)
	{
		arr[i] = fgetc(encInfo -> fptr_secret);
	}
	encode_data_to_image(arr,encInfo ->secret_extn_size,encInfo ->fptr_src_image,encInfo -> fptr_stego_image);
	
	printf("INFO : Successfully Encoded secret data into image file.\n\n");
}


/* Encode function, which does the real encoding
 * Input: data, size of data, file pointer of source image, file pointer for output image
 * Output: data will be encoded 
 * Description: It encodes the data to source image
 */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char arr[8];
	for(int i=0;i<size;i++)
	{	
		fread(arr,1,8,fptr_src_image);
		encode_byte_to_lsb(data[i],arr);
		fwrite(arr,1,8,fptr_stego_image);
	}
}

/* Encode a byte into lsb of image data array
 * Input: one byte of data, array of 8 bytes of source data
 * Output: updates the array with encoded data
 * Description: It performs the logical part for encoding 1 byte of data
 */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
	for(int i=0;i<=7;i++)
	{
		image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> (7-i)) & 1);
	}
}

/* Copy remaining image bytes from src to stego image after encoding
 * Input: file pointer of source image, file pointer for output image
 * Output: tranfer of remaining data 
 * Description: It encodes remaining data to source image
 */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	long int initial = ftell(fptr_src);
	long int end = get_file_size(fptr_src);
	fseek(fptr_src,initial,SEEK_SET);
	char arr[end-initial];
	fread(arr,1,end-initial,fptr_src);
	fwrite(arr,1,end-initial,fptr_dest);
	
	printf("INFO : Successfully Copied remaining data.\n\n");
}

Status encode_secret_file_extn_size(EncodeInfo *encInfo)
{
	char arr[32];
	fread(arr,1,32,encInfo -> fptr_src_image);
	for(int i=0;i<32;i++)
	{
		arr[i] = (arr[i] & 0xFE) | (((encInfo ->secret_extn_size) >> (31-i)) & 1);
	}	
	fwrite(arr,1,32,encInfo ->fptr_stego_image);
}
