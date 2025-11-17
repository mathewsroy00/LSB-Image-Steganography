/*
 * Name        : Mathews Roy
 * Date        : 16-11-2025
 * File        : encode.c
 * Project     : LSB Image Steganography
 *
 * Description :
 * -------------
 * This file contains all the functions required for the encoding
 * operation of the Steganography project. Encoding hides a secret
 * file inside a 24-bit BMP image by modifying the Least Significant
 * Bits (LSBs) of image bytes.
 *
 * Major Responsibilities :
 * ------------------------
 * 1) Opening source image, secret file, and stego image files
 * 2) Validating file types and extracting required metadata
 * 3) Calculating image capacity to ensure encoding feasibility
 * 4) Copying BMP header safely to output image
 * 5) Encoding:
 *      → Magic string (#*)
 *      → Secret file extension size
 *      → Secret file extension (.txt / .c / .sh)
 *      → Secret file size (in bytes)
 *      → Entire secret file data byte by byte
 * 6) Writing leftover image data to keep BMP structure intact
 *
 * Output :
 * --------
 * A stego BMP image visually identical to the source image,
 * but internally containing the hidden secret file.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "encode.h"
#include "types.h"

/* Function Definitions */

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

    // Read the height (an int)j
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
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
    printf("INFO: Opening Required files\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo -> src_image_fname);

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo -> secret_fname);

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo -> stego_image_fname);
    printf("INFO: DONE\n");

    // No failure return e_success
    return e_success;
}

/* Read and validate encoding arguments
 * Input  : Command-line arguments and EncodeInfo pointer
 * Output : Stores src file, secret file and output file names
 * Return : e_success or e_failure based on validation
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    printf("INFO: Validating Arguments\n");

    // check if source is BMP image
    char* sub = strstr(argv[2], ".bmp");                
    if((sub != NULL) && strcmp(sub, ".bmp") == 0)
    { 
        encInfo -> src_image_fname = argv[2];            // store source filename
    }
    else
    {
        printf("INFO: ## Error: Source file is not a .bmp file\n");
        return e_failure;
    }

    // Check and validate secret file extension
    char* sub1 = strstr(argv[3], ".");
    if(sub1 != NULL && (strcmp( sub1, ".txt") == 0 || strcmp(sub1, ".c") == 0 || strcmp(sub1, ".sh") == 0))
    {
        encInfo -> secret_fname = argv[3];            // storing secret filename
        strcpy(encInfo -> extn_secret_file, sub1);    // storing secret file extension
    }
    else
    {
        printf("INFO: ## Error: Secret file is not a .txt/.c/.sh file\n");
        return e_failure;
    }

    // Optional: user can give output stego filename
    if(argv[4] != NULL)
    {
       char* sub2 = strstr(argv[4], ".bmp");
        if((sub2 != NULL) && strcmp(sub2, ".bmp") == 0)
        {
            encInfo -> stego_image_fname = argv[4]; // Filename given by user
            printf("INFO: Validation Successfull\n");
            return e_success;
        }
        else
        {
            printf("INFO: ## Error: Output file is not a .bmp file\n");
            return e_failure;
        }
    }
    else
    {
        printf("INFO: Output file not mentioned.Creating Stego.bmp as default\n");
        encInfo -> stego_image_fname = "stego.bmp";   // Default name if user is not given name
        printf("INFO: Validation Successfull\n");
        return e_success;
    }
}

/* Do encoding
 * Input  : EncodeInfo structure
 * Output : Creates stego image with encoded secret data
 * Description : Performs all encoding steps sequentially
 */
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_success)            
    {
        printf("INFO: ## Encoding Procedure Started ##\n");
        if(check_capacity(encInfo) == e_success)
        {
            if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
            {
                if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    if(encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo) == e_success)
                    {
                        if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
                        {
                            if(encode_secret_file_size(encInfo -> secret_file_size, encInfo) == e_success)
                            {
                                if(encode_secret_file_data(encInfo) == e_success)
                                {
                                    if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
                                    {
                                        fclose(encInfo->fptr_src_image);
                                        fclose(encInfo->fptr_secret);
                                        fclose(encInfo->fptr_stego_image);
                                        return e_success;
                                    }
                                }
                            }
                        }                       
                    }
                }
            }
        }
    }
    return e_failure;
}

/* Check image have enough capacity to encode secret file
 * Input  : EncodeInfo structure
 * Output : Calculates image capacity and required size
 * Return : e_success or e_failure
 */
Status check_capacity(EncodeInfo *encInfo)
{
    printf("INFO: Checking for %s size\n", encInfo -> secret_fname);

    // Get size of secret file
    encInfo -> secret_file_size = get_file_size(encInfo -> fptr_secret);
    if(encInfo -> secret_file_size == 0)
    {
        printf("INFO: Empty. No data to encode\n");
        return e_failure;
    }
    printf("INFO: Done.Not Empty\n");

    // Get BMP capacity
    printf("INFO: Checking for %s capacity to handle %s\n", encInfo -> src_image_fname, encInfo -> secret_fname);
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image); 

    uint magic_string_len = strlen(MAGIC_STRING);
    uint Extension_len = strlen(encInfo -> extn_secret_file);
    
    // Total bytes needed for encoding
    uint Encoding_things = 54 + (magic_string_len + sizeof(int) + Extension_len + sizeof(int) + encInfo -> secret_file_size) * 8;

    if(encInfo -> image_capacity > Encoding_things)
    {
        printf("INFO: Done. Capacity available\n");
        return e_success;
    }
    else
    {
        printf("INFO: ## Error: Capacity not available\n");
        return e_failure;
    }
}

/* Get file size
 * Input  : File pointer
 * Output : File size in bytes
 */
uint get_file_size(FILE* fptr)
{
    fseek(fptr, 0, SEEK_END);         // Moving file pointer to end
    uint pos = ftell(fptr);            // Storing the position that is file size
    rewind(fptr);                     // Resetting to the beginning
    return pos;
}

/* Copy BMP header
 * Input  : Source and destination file pointers
 * Output : Writes first 54 bytes (header) from source to destination
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("INFO: Copying Image Header\n");
   char header[54];
    rewind(fptr_src_image);                      // moving the fiepointer to the start
     fread(header, sizeof(char), 54, fptr_src_image); // Read header
    fwrite(header, sizeof(char), 54, fptr_dest_image); // Write header
    printf("INFO: Done\n");
    return e_success;
}

/* Encode magic string
 * Input  : MAGIC_STRING constant and EncodeInfo structure
 * Output : Encodes magic string into image
 */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("INFO: Encoding Magic String Signature\n");
    if(encode_data_to_image(magic_string, strlen(magic_string), encInfo) == e_success)
    {
        printf("INFO: Done\n");
        return e_success;
    }
    return e_failure;
}

/* Encode data to image
 * Input  : Data, size, and EncodeInfo structure
 * Output : Writes encoded data into fptr_stego_image
 */
Status encode_data_to_image(const char *data, int size, EncodeInfo* encInfo)
{
    char buffer[8];
    for(int i = 0; i < size; i++)
    {
        fread(buffer, sizeof(char), 8, encInfo -> fptr_src_image);    // Reading 8 bytes
        encode_byte_to_lsb(data[i], buffer);                          // Encoding 1 byte of data
        fwrite(buffer, sizeof(char), 8, encInfo -> fptr_stego_image); // Write modified bytes
    }
    return e_success;
}

/* Encodes a single byte into 8 LSBs of a buffer
 * Input  : One byte of data and 8-byte image buffer
 * Output : Modified image buffer with encoded bits
 */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i = 0; i < 8; i++)
    {
       // Extract bit at position (7-i) to encode MSB first
        uint bit = data & (1 << (7 - i));

        // Clear LSB of current image byte using mask 11111110
        image_buffer[i] = image_buffer[i] & (~1);

        // If extracted bit is set, set the LSB to 1
        if(bit)
        {
            image_buffer[i] = image_buffer[i] | 1;
        }
    }
    return e_success;
}


/* Encode 32-bit integer into 32 bytes LSBs
 * Input  : Integer data and 32 -byte image buffer
 * Output : Modified buffer with encoded integer
 */
Status encode_int_to_lsb(int data, char* image_buffer)
{
    for(int i = 0; i < 32; i++)
    {
        int bit = data & (1 << (31 - i));    // Get MSB first

        image_buffer[i] = image_buffer[i] & (~1);   // Clear LSB

        if(bit)
        {
            image_buffer[i] = image_buffer[i] | 1;  // Set LSB if bit = 1
        }
    }
    return e_success;
}


/* Encode secret file extension size (integer)
 * Input  : Size of file extension and EncodeInfo structure
 * Output : Encodes extension size (integer) into image
 */
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo) 
{
    printf("INFO: Encoding %s File Extension Size\n", encInfo -> secret_fname);
    char buffer[32];        

    fread(buffer, sizeof(char), 32, encInfo -> fptr_src_image);      // Reading 32 Bytes from source image
    encode_int_to_lsb(size, buffer);                                 // Encoding extension size into LSBs
    fwrite(buffer, sizeof(char), 32, encInfo -> fptr_stego_image);   // Write encoded bytes to stego image
    
    printf("INFO: Done\n");
    return e_success;
}


/* Encode file extension (e.g., .txt)
 * Input  : File extension string & EncodeInfo
 * Output : Encodes extension characters using encode_data_to_image()
 */

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File extension\n", encInfo -> secret_fname);
    // Encoding files extension (.txt, .c, .sh)
    if(encode_data_to_image(file_extn, strlen(file_extn), encInfo) == e_success)
    {
        printf("INFO: Done\n");
        return e_success;
    }
    return e_failure;
}

/* Encode secret file size (in bytes)
 * Input  : Size of secret file
 * Output : Write file size into 32 pixels
 */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Size\n", encInfo -> secret_fname);
    char buffer[32];        

    fread(buffer, sizeof(char), 32, encInfo -> fptr_src_image);    // Read 32 bytes from image
    encode_int_to_lsb(file_size, buffer);                          // Encoding file size into LSBs
    fwrite(buffer, sizeof(char), 32, encInfo -> fptr_stego_image); // writing modified bytes to stego image
   
    printf("INFO: Done\n");
    return e_success;
}

/* Encode secret file data (raw contents)
 * Input  : EncodeInfo structure
 * Output : Writes encoded bytes to stego image
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Data\n", encInfo -> secret_fname);
    char secret_file_data[encInfo -> secret_file_size];

    // Reads entire secret file
    fread(secret_file_data, sizeof(char), encInfo -> secret_file_size, encInfo -> fptr_secret);

    // Encode data bytes into image pixels
    if(encode_data_to_image(secret_file_data, encInfo -> secret_file_size, encInfo) == e_success)
    {
        printf("INFO: Done\n");
        return e_success;
    }
    return e_failure; //If encoding failed returns failure
}

/* Copy remaining image data
 * Input  : Source and destination file pointers
 * Output : Copies unmodified remaining bytes of image
 */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("INFO: Copying Left Over Data\n");
    int ch;
    while((ch = fgetc(fptr_src)) != EOF)  // Reading each byte until EOF
    {
        fputc(ch, fptr_dest);             // Writing to destination image
    }
    printf("INFO: Done\n");
    return e_success;
}
