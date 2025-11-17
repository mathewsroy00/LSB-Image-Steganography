/*
 * Name        : Mathews Roy
 * Date        : 16-11-2025
 * File        : decode.c
 * Project     : LSB Image Steganography
 *
 * Description :
 * -------------
 * This file contains all functions required for the decoding
 * operation of the Steganography project. Decoding extracts the
 * hidden secret file from a previously encoded BMP image by 
 * reading the Least Significant Bits (LSBs).
 *
 * Major Responsibilities :
 * ------------------------
 * 1) Opening encoded BMP (stego) file
 * 2) Validating decoding arguments and preparing output filename
 * 3) Skipping BMP header (first 54 bytes)
 * 4) Decoding:
 *      → Magic string (#*) to confirm valid encoded file
 *      → Secret file extension size
 *      → Secret file extension (.txt / .c / .sh)
 *      → Secret file size
 *      → Extracting secret file data byte by byte
 * 5) Writing extracted data into the final output file
 *
 * Output :
 * --------
 * The fully reconstructed secret file identical to the original
 * file used in encoding.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "decode.h"
#include "types.h"

/* Open stego BMP image file
 * Input  : DecodeInfo pointer containing stego filename
 * Output : Opens fptr_stego_image
 * Return : e_success if file opens, else e_failure
 */
Status open_files_dec(DecodeInfo* decInfo)
{
    printf("INFO: Opening required files\n");

    // Open stego image file (encoded BMP)
    decInfo -> fptr_stego_image = fopen(decInfo -> stego_image_fname, "r");
    if(decInfo -> fptr_stego_image == NULL)
    {
        perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

        return e_failure;
    }

    printf("INFO: Opened %s\n", decInfo -> stego_image_fname);
    return e_success;
}

/* Read and validate decoding arguments
 * Input  : argv[], DecodeInfo pointer
 * Output : Stores stego image filename and output filename
 * Return : e_success if validation OK, else e_failure
 */
Status read_and_validate_decode_args(char* argv[], DecodeInfo *decInfo)
{
    printf("INFO: Validating Arguments\n");

     // Validate stego BMP filename
    char* sub = strstr(argv[2], ".bmp");
    if(sub != NULL && strcmp(sub, ".bmp") == 0)
    {
        decInfo -> stego_image_fname = argv[2];
    }
    else
    {
        printf("INFO: ## Error: Encoded file is not a .bmp file\n");
        return e_failure;
    }

    // Check if user provided output filename
    if(argv[3] != NULL)
    {
        strcpy(decInfo -> secret_output_fname, argv[3]); // Storing output name
        printf("INFO: Validation Successfull\n");
        return e_success;
    }
    else
    {
        // Default output file name
        printf("Output File not mentioned. Creating secret_output as default\n");
        strcpy(decInfo -> secret_output_fname, "secret_output");
        printf("INFO: Validation Successfull\n");
        return e_success;
    }
}

/* Perform decoding steps
 * Input  : DecodeInfo structure
 * Output : Extracts secret file from stego BMP
 * Return : e_success on complete decoding, else e_failure
 */
Status do_decoding(DecodeInfo* decInfo)
{
    if(open_files_dec(decInfo) == e_success)
    {
        printf("INFO: ## Decoding Procedure Started ##\n");

        if(skip_bmp_header(decInfo) == e_success)
        {
            if(decode_magic_string(decInfo) == e_success)
            {
                if(decode_secret_file_extn_size(decInfo) == e_success)
                {
                    if(decode_secret_file_extn(decInfo) == e_success)
                    {
                        if(decode_secret_file_size(decInfo) == e_success)
                        {
                            if(decode_secret_file_data(decInfo) == e_success)
                            {
                               fclose(decInfo->fptr_stego_image);
                               fclose(decInfo->fptr_secret_output);
                                return e_success;
                            }
                        }
                    }
                }

            }
        }
    }
    return e_failure;
}

/* Skip BMP header (54 bytes)
 * Input  : DecodeInfo pointer
 * Output : Moves file pointer to pixel data
 */
Status skip_bmp_header(DecodeInfo* decInfo)
{
    // skip the 54-byte BMP header
    fseek(decInfo -> fptr_stego_image, 54, SEEK_SET);
    return e_success;
}

/* Decode magic string "#*"
 * Input  : DecodeInfo structure
 * Output : Reads 2 encoded bytes and compares with MAGIC_STRING
 * Return : e_success if matches, else e_failure
 */
Status decode_magic_string(DecodeInfo* decInfo)
{
    printf("INFO: Decoding Magic String Signature\n");
    char magic_string[3];
    char image_buffer[8];

    // Reading encoded magic string (2 bytes)
    for(int i = 0; i < 2; i++)
    {
        fread(image_buffer, sizeof(char), 8, decInfo -> fptr_stego_image);
        magic_string[i] = decode_bytes_from_lsb(image_buffer);
    }
    magic_string[2] = '\0'; // Adding NUll terminator

    if(strcmp(MAGIC_STRING, magic_string) == 0) // Comparing magic string with decoded magic string
    {
        printf("INFO: Done\n");
        return e_success;
    }
    return e_failure;
}

/* Decode 1 byte from 8 LSBs of buffer
 * Input  : 8-byte image buffer
 * Output : Decoded character
 */
char decode_bytes_from_lsb(char* image_buffer)
{
    char ch = 0;
    for(int i = 0; i < 8; i++)
    {
        uint bit = image_buffer[i] & 1;  // Extract LSB

        if(bit)
        {
            ch = ch | 1 << (7 - i); // Form character bit by bit (MSB first)
        }
    }
    return ch;
}

/* Decode integer (4 bytes) from 32 LSBs
 * Input  : 32-byte buffer
 * Output : Decoded integer
 */
uint decode_int_from_lsb(char* image_buffer)
{ 
    uint num = 0;

    // Extracting 32 bits from 32 bytes
    for(int i = 0; i < 32; i++)
    {
        uint bit = image_buffer[i] & 1;
        if(bit)
        {
            num = num | (1 << (31 - i)); 
        } 
    }
    return num;
}

/* Decode secret file extension size
 * Input  : DecodeInfo pointer
 * Output : Reads 32 LSBs → integer extn_size
 */
Status decode_secret_file_extn_size(DecodeInfo* decInfo)
{
    printf("INFO: Decoding Output File Extension Size\n");
    char image_buffer[32];

    fread(image_buffer, sizeof(char), 32, decInfo -> fptr_stego_image);
    decInfo -> extn_size = decode_int_from_lsb(image_buffer); // Decoding 32 bit integer

    printf("INFO: Done\n");
    return e_success; 
}

/* Decode secret file extension (.txt / .c / .sh)
 * Input  : DecodeInfo pointer
 * Output : Builds output file name and opens output file
 */
Status decode_secret_file_extn(DecodeInfo* decInfo)
{
    printf("INFO: Decoding Output File Extension\n");
    char extn[decInfo -> extn_size + 1];
    char image_buffer[8];

    // Decode each character of extension
    for(uint i = 0; i < decInfo -> extn_size; i++)
    {
        fread(image_buffer, sizeof(char), 8, decInfo -> fptr_stego_image);
        extn[i] = decode_bytes_from_lsb(image_buffer);
    }
    extn[decInfo -> extn_size] = '\0'; // Adding null terminator

    strcat(decInfo -> secret_output_fname, extn); // Appends extension to output filename

    printf("INFO: Opening %s\n", decInfo -> secret_output_fname);

    // Opening the final output file
    decInfo -> fptr_secret_output = fopen(decInfo -> secret_output_fname, "w");
    if(decInfo -> fptr_secret_output == NULL)
    {
        perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_output_fname);
        return e_failure;
    }
    printf("INFO: Done.Opened %s\n", decInfo -> secret_output_fname);

    printf("INFO: Done\n");
    return e_success; 
}

/* Decode secret file size
 * Input  : DecodeInfo pointer
 * Output : Extracts file size (integer)
 */
Status decode_secret_file_size(DecodeInfo* decInfo)
{
    printf("INFO: Decoding %s File Size\n", decInfo -> secret_output_fname);
    char image_buffer[32];

    fread(image_buffer, sizeof(char), 32, decInfo -> fptr_stego_image);
    decInfo -> secret_file_size = decode_int_from_lsb(image_buffer);

    printf("INFO: Done\n");
    return e_success; 
}

/* Decode secret file data (raw contents)
 * Input  : DecodeInfo pointer
 * Output : Writes decoded characters into output file
 */
Status decode_secret_file_data(DecodeInfo* decInfo)
{
    printf("INFO: Decoding %s File Data\n", decInfo -> secret_output_fname);
    char image_buffer[8];

    // Decode each byte of secret file
    for(uint i = 0; i < decInfo -> secret_file_size; i++)
    {
        fread(image_buffer, sizeof(char), 8, decInfo -> fptr_stego_image);
        char ch = decode_bytes_from_lsb(image_buffer); // convert LSBs to character
        fputc(ch, decInfo -> fptr_secret_output);     // Writing to output file
    }
    printf("INFO: Done\n");
    return e_success;
}

