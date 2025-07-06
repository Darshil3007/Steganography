#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"
int main(int argc, char *argv[])
{
    if(argc<3)
    {
    	printf("ERROR : Not enough arguments passed.\n");
    	printf("INFO : (For encoding)Provide Arguments like (./a.out -e Source_file_name Secret_file_name output_file_name(optional))\n\n");
    	printf("INFO : (For decoding)Provide Arguments like (./a.out -d Encoded_file_name output_file_name(optional))\n\n");
    	sleep(1);
    	return 0;
    }
    EncodeInfo encInfo;
    DecodeInfo decodeInfo;
    if(check_operation_type(argv) == e_encode)
    {
     	if(argc ==3)
	{
    		printf("ERROR : Not enough arguments passed.\n");
    		printf("INFO : (For encoding)Provide Arguments like (./a.out -e Source_file_name Secret_file_name output_file_name(optional))\n\n");
		sleep(1);
		return 0;
	}
    	printf("INFO : Selected encoding.\n\n");
    	if(argc >= 4)
	{
		if(read_and_validate_encode_args(argv,&encInfo) == e_failure)
		{
			printf("ERROR : Occured related to arguments passed.\n\n");
			sleep(1);
			return 0;
		}
		else
		{
			if(do_encoding(&encInfo) == e_failure)
			{
				printf("ERROR : Occured during Do encoding process.\n\n");
			}
			else
			{
				printf("---------------------------\n");
				printf("ENCODING DONE SUCCESSFULLY.\n");
				printf("---------------------------\n");
			}
		}
	}
    }
    else if(check_operation_type(argv) == e_decode)
    {
    	printf("INFO : Selected decoding\n\n");
	if(argc >= 3)
	{
		if(read_and_validate_decode_args(argv,&decodeInfo) == e_failure)
		{
			printf("ERROR : Occured related to arguments passed\n\n");
		}
		else
		{
			if(do_decoding(&decodeInfo) == e_failure)
			{
				printf("ERROR : Occured during Do decoding process.\n\n");
			}
			else
			{
				printf("---------------------------\n");
				printf("DECODING DONE SUCCESSFULLY.\n");
				printf("---------------------------\n");
			}
		}
	}
	else
	{
		printf("ERROR : Not enough arguments passed.\n");
		printf("INFO : (For decoding)Provide Arguments like (./a.out -d Encoded_file_name output_file_name(optional))\n\n");
    		sleep(1);
		return 0;
	}
    }
    else
    {
    	printf("ERROR : Unsupported Operation type.\n\n");
    }
    
    return 0;
}
