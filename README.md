# Quadtree for Compression and Decompression a PMP file as well as vertical and horizontal mirroring

This project explores the concept of image compression using quadtree data structures. The goal is to represent images using a minimal number of bits while maintaining a reasonable level of image quality.

## Overview

In our image-centric world, the consumption and interpretation of images have become integral to our daily lives. However, we often lack the necessary skills to effectively read and understand images. This project aims to address this by introducing a method of image compression using quadtree structures.

## Quadtree Structure

A quadtree is a hierarchical data structure that organizes information for multidimensional data. In the context of image compression, it represents a square region of an image. Each node in the quadtree corresponds to a subdivision of the region, and each non-terminal node has four children representing smaller sub-regions. This recursive subdivision continues until the nodes represent uniformly sized squares.

## Compression Algorithm

The compression algorithm works as follows:

    Given a square image of power-of-two dimensions, the image is represented using a quadtree.
    Each level of the quadtree corresponds to a division of the image into quadrants.
    If a quadrant is uniform (composed of pixels of the same color), it becomes a terminal node in the quadtree.
    If a quadrant is not uniform, it is further divided into four equal parts, and each part becomes a child node.
    The process continues until all nodes in the quadtree represent uniformly sized squares or single pixels.

## Decompression Process

To reconstruct the original image from its compressed representation, the following steps are followed:

    Select the terminal nodes of the quadtree that correspond to the object's pixels.
    Use the depth information to determine the size of each square region.
    Combine the pixels from the terminal nodes to recreate the original image.

## File Format

The project uses the PPM file format for image representation. The PPM file consists of a text header followed by binary pixel data. The header contains information about the file type (P6 necessary) , image dimensions, and color depth (256 max).


## Usage

To use the program, follow these steps:

- Prepare the input image in PPM format
- run make
- run the following command:
  
      ./quadtree [-c factor |  -m type] [input_file1] [output_file] where:
  
 -c factor indicates that the program will compress the image (factor = threshold imposed for compression)

  - if a higher threshold is provided for image compression, the compression tree is constructed in such a way that each leaf node represents a block of pixels and contains the average color of that block. Instead of preserving the detailed information of individual pixels, the algorithm calculates the average color within each block. This approach allows for a greater reduction in the number of bits required to represent the image, but it may result in a higher loss of quality in the reconstructed image.
 
 -d factor indicates that the program will reconstruct the original image
 
 -m type indicates that the program will solve mirror the image (type = type of mirroring and can take one from the values: h - horizontal or v - vertical);

 input_file1 represents the name of the first input file;

 output_file represents the name of the output file, in which it will be written, depending on the command
received, the result of the program execution.
