537proj4
========

Project 4 for cs 537 - I/O timing!
With this program, timing info is generated for various size writes to make a 128MB file using write and fwrite.

Compiling:
Type make. Yeah, that's it.

Usage:
iotime "input" "output"
where "input" is a file containing integers of the write sizes to be tested, and "output" is a file where the timing info is stored.

Important:
I'm not sure what will happen if a write size is specified that isn't a factor of 128MB. The program won't write the correct size file, probably.

Tested on Mumble-20 in the Mumble labs.
