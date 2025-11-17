#ifndef DECODE_H
#define DECODE_H

#include "types.h"

/* 
 * Structure to store information required for 
 * decoding a secret file from a stego BMP image.
 * Contains stego image info, output file info,
 * and intermediate decoded values.
 */

typedef struct _DecodeInfo
{
    /* Stego image Info */
    char* stego_image_fname; // Encoded image file name
    FILE* fptr_stego_image;  // Encoded image file pointer

    /* Output Secret File Info */
    char secret_output_fname[50]; // Storing filename of output file(without extension)
    FILE* fptr_secret_output; // Output file pointer

    uint extn_size;           // Stores secret file extension size
    uint secret_file_size;  // stores secret file size

}DecodeInfo;

/* Decoding function prototype */

/* Opens stego image file */
Status open_files_dec(DecodeInfo* decInfo);

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char* argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo* decInfo);

/* Skip BMP header (54 bytes) */
Status skip_bmp_header(DecodeInfo* decInfo);

/* Decode magic string from image (#*) */
Status decode_magic_string(DecodeInfo* decInfo);

/* Decode a byte from 8 LSBs */
char decode_bytes_from_lsb(char* image_buffer);

/* Decde an integer (32 bits) from 32 LSBs */
uint decode_int_from_lsb(char* image_buffer);

/* Decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo* decInfo);

/* Decode secret file extension( .txt / .c / .sh) */
Status decode_secret_file_extn(DecodeInfo* decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo* decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo* decInfo);

#endif