#ifndef DECODE_H
#define DECODE_H


typedef struct _DecodeInfo
{
	char *src_image_fname_decode;
	FILE *fptr_src_image_decode;

	char out_fname[30];
	FILE *fptr_out_fname;
	char extn_out_file[5];
	unsigned int out_extn_size;
	unsigned long out_file_size;

}DecodeInfo;

Status read_and_validate_decode_args(char *argv[],DecodeInfo *decodeInfo);

Status decode_magic_string(char *magic_string, DecodeInfo *decodeInfo);

Status decode_out_file_extn_size(DecodeInfo *decodeInfo);

Status decode_out_file_extn(DecodeInfo *decodeInfo);

Status decode_out_file_size(DecodeInfo *decodeInfo);

Status decode_out_file_data(DecodeInfo *decodeInfo);

Status do_decoding(DecodeInfo *decodeInfo);

Status open_files_decode(DecodeInfo *decodeInfo);

Status decode_data_from_image(char *data, int size, FILE *fptr_src_image_decode, FILE *fptr_out_fname);

uint decode_size(DecodeInfo *decodeInfo);

char decode_lsb_to_byte(char *image_buffer);

#endif
