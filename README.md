LSB Image Steganography using C

This project implements Image Steganography using the Least Significant Bit (LSB) technique.
It allows you to hide a secret file inside a BMP image and later decode it back without any visual difference in the image.

📘 What is LSB Steganography?

LSB (Least Significant Bit) steganography works by modifying the lowest bit of each image byte.
Changing this bit does not affect the visible appearance of the image, making the hidden data invisible to the human eye.

📂 Project Features

Hide any text-based secret file inside a BMP image

Extract the hidden file from the encoded image

No visual difference in the output image

Easy command-line interface

Pure C implementation (no OpenCV, no external libs)

Safe encoding of:

Magic string

File extension

File size

Actual file data

🔧 How It Works (Internal Flow)
Encoding

Read BMP header (54 bytes)

Write the header to output file

Encode the following into the image bytes:

Magic string (#*)

Secret file extension size

Secret file extension (e.g., .txt)

Secret file size

Secret file contents (byte by byte)

Copy remaining image bytes unchanged

Decoding

Skip the 54-byte BMP header

Read and verify the magic string

Decode extension size

Decode extension

Decode file size

Decode file contents and save the output file

🧪 Usage Instructions

Encoding (Hide Data)
./a.out -e <source.bmp> <secret_file> <output_stego.bmp>

Decoding (Extract Data)
./a.out -d <stego.bmp> <output_filename>

📁 Project Structure
├── encode.c            # Encoding logic
├── encode.h
├── decode.c            # Decoding logic
├── decode.h
├── main.c              # Argument handling and flow control
├── types.h             # Custom data types and enums
├── common.h            # Magic string
├── beautiful.bmp       # Sample input BMP
└── secret.txt          # Sample secret file

🧠 Why BMP Image?

BMP is used because:

It stores pixel data uncompressed

Easy to modify specific bytes

Predictable and stable format

Perfect for LSB steganography

📌 Limitations

Works only with BMP images

Secret file must fit inside image capacity

Not suitable for compressed formats like JPG

👨‍💻 Author

Mathews Roy
