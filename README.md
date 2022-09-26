# Quadtree
Quadtree for Compression and Decompression a PMP file as well as vertical and horizontal mirroring

to run the program ,run make and then the following command:

  ./quadtree [-c factor |  -m type] [input_file1] [output_file] where:
  
• -c factor indicates that the program will solve requirement 1 (factor = threshold imposed for compression);
• -m type indicates that the program will solve requirement 3 (type = type of mirroring and can take one
from the values: h - horizontal or v - vertical);
• input_file1 represents the name of the first input file;
• output_file represents the name of the output file, in which it will be written, depending on the command
received, the result of the program execution.
