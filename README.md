# LSB Image Steganography using C

This project implements Image Steganography using the Least Significant Bit (LSB) technique.
It allows you to hide a secret file inside a BMP image and later decode it back without any visual difference in the image.


## 📘 What is LSB Steganography?

LSB (Least Significant Bit) steganography works by modifying the lowest bit of each image byte.
Changing this bit does not affect the visible appearance of the image, making the hidden data invisible to the human eye.


## 📂 Project Features

- Hide any text-based secret file inside a BMP image

- Extract the hidden file from the encoded image

- No visual difference in the output image

- Easy command-line interface

- Pure C implementation (no OpenCV, no external libs)

- Safe encoding of:

- Magic string

- File extension

- File size

- Actual file data


# 🧪 Usage Instructions

**Encoding (Hide Data)**
./a.out -e <source.bmp> <secret_file> <output_stego.bmp>

**Decoding (Extract Data)**
./a.out -d <stego.bmp> <output_filename>


# 🧠 Why BMP Image?

BMP is used because:

It stores pixel data uncompressed

Easy to modify specific bytes

Predictable and stable format

Perfect for LSB steganography


# 📌 Limitations

Works only with BMP images

Secret file must fit inside image capacity

Not suitable for compressed formats like JPG


# 👨‍💻 Author

Mathews Roy
