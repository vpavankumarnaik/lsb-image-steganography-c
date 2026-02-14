# lsb-image-steganography-c

📌 Overview

This project implements LSB (Least Significant Bit) based image steganography in C to hide and extract secret messages inside BMP image files without noticeable visual distortion.

The system supports encoding a text file into a BMP image and decoding the hidden message back from the image.

🛠️ Technologies Used

C Programming

File Handling

Bitwise Operations

BMP File Format Handling

⚙️ Features

Encode secret message into a BMP image

Decode hidden message from encoded image

File validation checks

Error handling for invalid inputs

Modular design (encode / decode separation)

📂 Project Structure
.
├── main.c
├── encode.c
├── encode.h
├── decode.c
├── decode.h
├── common.h
├── types.h
└── secret.txt (sample input file)

🚀 How It Works

The project uses LSB substitution technique:

Each bit of the secret message is stored in the least significant bit of the image pixel data.

Since LSB changes minimally affect pixel value, visual distortion is negligible.

▶️ Compilation
gcc main.c encode.c decode.c -o steg

▶️ Usage
Encoding
./steg -e input.bmp secret.txt output.bmp

Decoding
./steg -d output.bmp
