/*I/O timing - write vs. fwrite*/

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define ONETWOEIGHTMEG 134217728

int main(int argc, char** argv)
{
	int return_value = 0;

	/*input and output files*/
	FILE * input = NULL;
	FILE * output = NULL;
	
	/*dump file for using the write/fwrite stuff.*/
	FILE * dump = NULL;

	/*file descriptor for dump file*/
	int dump_fd;

	/*structs to hold time data for each run of write/frwite, and the elapsed time*/
	struct timeval before_time;
	struct timeval after_time;
	struct timeval elapsed_time;

	/*hold the current size we're using to write to disk*/
	int write_size;

	/*hold number of things of the size to write to the disk*/
	int num_write;

	/*Buffer for write - doesn't have any content, just to make sure we stay in our space.*/
	void * write_buffer = NULL;

	/*holds the number of times we've done the writes.*/
	int count;

	/*holds the temporary filename so we can unlink it when we're done!*/
	char * tempfilename;

	/*return values for functions*/
	int fscanf_return;
	int gettimeofday_return;
	int close_return;
	int fclose_return;
	ssize_t write_return;

	/*loop counter*/
	int i;

	/*We need to specify an input and output file!*/
	if(argc != 3)
	{
		printf("Please specify an input file and an output file.\n");
		exit(EXIT_FAILURE);
	}

	/*Try to open up the input file*/
	input = fopen(argv[1], "r");
	if(input == NULL)
	{
		perror("Error on opening input file - ");
		exit(EXIT_FAILURE);
	}

	/*Open up the output file*/
	output = fopen(argv[2], "w");
	if(output == NULL)
	{
		perror("Error on opening/creating output file - ");
		exit(EXIT_FAILURE);
	}

	/*
	 * Here's the meat of the program.
	 * First, we read in a value from the input file
	 * Then, we write to the disk with write,
	 * and dump the time data to our output file.
	 * After, we use fwrite.
	 * We repeat the write/fwrite process 10 times.
	 * Then we start again.
	 *
	 */

	/*Read in the size to write!*/
	fscanf_return = fscanf(input, "%i", &write_size);
	while(fscanf_return >= 0 && fscanf_return != EOF)
	{
		/*Create the buffer to read from*/
		write_buffer = realloc(write_buffer, write_size);
		if(write_buffer == NULL)
		{
			perror("Error on write_buffer realloc: ");
			exit(EXIT_FAILURE);
		}
		printf("Doing 10 writes of size %i\n", write_size);
		/*calculate the number of times to do the write*/
		num_write = ONETWOEIGHTMEG/write_size;

		for(count = 0; count < 10; count++)
		{
			/*Here we use write*/
			printf("write %d of size %d\n", count, write_size);
			/*open the file and make sure to make it a clean file!*/
			tempfilename = tempnam("/tmp/", "dump");
			dump_fd = open(tempfilename, O_WRONLY | O_CREAT | O_TRUNC | O_EXCL, 0660);
			if(dump_fd < 0)
			{
				perror("Error on open - ");
				exit(EXIT_FAILURE);

			}
			gettimeofday_return = gettimeofday(&before_time, NULL);
			if(gettimeofday_return == -1)
			{
				perror("Error on gettimeofday - ");
				exit(EXIT_FAILURE);
			}
			/*write to the file with a size of the buffer write_loop times!)*/
			for(i = 0; i < num_write; i++)
			{
				write_return = write(dump_fd, write_buffer, write_size);
				if(write_return < 0)
				{
					perror("Error on write - ");
					exit(EXIT_FAILURE);
				}
			}
			gettimeofday_return = gettimeofday(&after_time, NULL);
			if(gettimeofday_return == -1)
			{
				perror("Error on gettimeofday - ");
				exit(EXIT_FAILURE);
			}
			/*close the file*/
			close_return = close(dump_fd);
			if(close_return < 0)
			{
				perror("Error on close - ");
				exit(EXIT_FAILURE);
			}
			/*delete the file to maybe prevent weird slowdown.*/
			unlink(tempfilename);
			timersub(&after_time, &before_time, &elapsed_time);
			fprintf(output, "write %d: %d seconds %d microseconds with %d calls to write\n", count, (int)elapsed_time.tv_sec, (int)elapsed_time.tv_usec, num_write);
			fflush(output);
		}
		for(count = 0; count < 10; count++)
		{
			/*And here we use fwrite*/
			printf("fwrite %d of size %d\n", count, write_size);
			/*open the file*/
			tempfilename = tempnam("/tmp/", "dump");
			dump = fopen(tempfilename, "w");
			if(dump == NULL)
			{
				perror("Error on fopen - ");
				exit(EXIT_FAILURE);
			}
			gettimeofday_return = gettimeofday(&before_time, NULL);
			if(gettimeofday_return == -1)
			{
				perror("Error on gettimeofday - ");
				exit(EXIT_FAILURE);
			}
			/*write to the file*/
			for(i = 0; i < num_write; i++){
				fwrite(write_buffer, 1, write_size, dump);
			}
			if(ferror(dump))
			{
				perror("Error on fwrite - ");
				exit(EXIT_FAILURE);
			}
			gettimeofday_return = gettimeofday(&after_time, NULL);
			if(gettimeofday_return == -1)
			{
				perror("Error on gettimeofday - ");
				exit(EXIT_FAILURE);
			}
			/*close the file*/
			fclose_return = fclose(dump);
			if(fclose_return < 0)
			{
				perror("Error on fclose - ");
				exit(EXIT_FAILURE);
			}
			/*delete file to prevent weird errors.*/
			unlink(tempfilename);
			/*find the elapsed time! yay macros*/
			timersub(&after_time, &before_time, &elapsed_time);
			fprintf(output, "fwrite %d: %d seconds %d microseconds with %d call to fwrite\n", count, (int)elapsed_time.tv_sec, (int)elapsed_time.tv_usec, num_write);

			/*write to our output file after each run just to be safe/lazy*/
			fflush(output);
		}
		fscanf_return = fscanf(input, "%i", &write_size);
	}
	/*clean up our toys when we're done*/
	fclose(input);
	fclose(output);
	free(write_buffer);
	return return_value;
}
