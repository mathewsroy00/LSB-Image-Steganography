/*
 * Name        : Mathews Roy
 * Date        : 16-11-2025
 * Project     : LSB Image Steganography (Encoding & Decoding)
 *
 * Description :
 * -------------
 * This project implements Steganography — the technique of hiding a secret
 * message or file inside an image, without visibly changing the image.  
 * 
 * This implementation uses the LSB (Least Significant Bit) technique on
 * BMP images. Only the lowest bit of each pixel byte is modified,
 * making visual changes undetectable to the human eye.
 *
 * The project has two major operations:
 *
 * 1) Encoding  (-e)
 *    Embeds a secret text file (.txt / .c / .sh) into a BMP image.
 *
 * 2) Decoding  (-d)
 *    Extracts the previously hidden secret data from an encoded stego BMP file.
 */


#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char* argv[])
{
    /* Check for basic argument count and unsupported operations */
    if(argc < 3 || argc > 5 || check_operation_type(argv) == e_unsupported) 
    {
        printf("## ERROR : Entered arguments is Unsupported ##\n");
        printf("Usage : \n");
        printf("\tEncode : %s -e < Source.bmp file > < Secret_message file > < Output file (optional) >\n", argv[0]);
        printf("\tDecode : %s -d < Encoded.bmp file > < Output file (optional) >\n", argv[0]);
        return e_failure; 
    }

    /* Encoding Operation */
    else if(check_operation_type(argv) == e_encode) // checking operation type
    {
        EncodeInfo encInfo; // Sturcture variable for encoding

        /* Validate argument count and encoding arguments */
        if((argc == 4 || argc == 5) && read_and_validate_encode_args(argv, &encInfo) == e_success) // validating arguments
        {
            if(do_encoding(&encInfo) == e_success)
            {
                printf("INFO: ## Encoding Done Succesfully ##\n");
                return e_success;
            }
            else
            {
                printf("INFO: ## Encoding Failed ##\n");
                return e_failure;
            }
        }
        else
        {
            printf("INFO: ## ERROR: Invalid Encode Arguments ##\n");
            printf("Usage: %s -e <src.bmp> <secret_file> <output(optional)>\n", argv[0]);
            return e_failure;
        }
    }

    /* Decoding Operation */
    else if(check_operation_type(argv) == e_decode)
    {
        DecodeInfo decInfo; // Strucuture variable for decoding

        /* Validate argument count and decoding arguments */
        if((argc == 3 || argc == 4) && read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            if(do_decoding(&decInfo) == e_success)
            {
                printf("INFO: ## Decoding Done Successfully ##\n");
                return e_success;
            }
            else
            {
                printf("INFO: ## Decoding failed ##\n");
                return e_failure;
            }
        }
        else
        {
            printf("INFO: ## ERROR: Invalid Decode Arguments ##\n");
            printf("Usage: %s -d <Encoded.bmp> <Output(optional)>\n", argv[0]);
            return e_failure;
        }
    }
    return e_failure;
}

/* Check operation type
 * Input  : Command-line arguments
 * Output : Returns e_encode, e_decode or e_unsupported
 * Description : Checks if user requested encoding (-e) or decoding (-d)
 */
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
    {
        return e_encode;          // Encoding operation
    }
    else if(strcmp(argv[1], "-d") == 0)
    {
        return e_decode;          // Decoding operation 
    }
    else
    {
        return e_unsupported;      // Invalid argument
    }
}