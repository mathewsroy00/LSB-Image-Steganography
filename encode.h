#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;        // store the Src_Image_fname
    FILE *fptr_src_image;         // File pointer for src_image
    uint image_capacity;          // Store the src_img_filesize

    /* Secret File Info */
    char *secret_fname;          // store the Secret_fname
    FILE *fptr_secret;           // File pointer for secret_file
    uint secret_file_size;       // Storing the secret file size
    char extn_secret_file[5];    // Storing the secret file extension

    /* Stego Image Info */
    char *stego_image_fname;     // Store the ouptut_img_fname
    FILE *fptr_stego_image;      // File pointer for output_img

} EncodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/* Encode secret file extension size */
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo); 

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding #*/
Status encode_data_to_image(const char *data, int size, EncodeInfo* encInfo);

/* Encode a byte into LSB of image data array #*/
Status encode_byte_to_lsb(char data, char *image_buffer); 

// Encoding an into to LSB of image data array
Status encode_int_to_lsb(int data, char* buffer );

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif
 