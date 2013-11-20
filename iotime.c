/*I/O timing - write vs. fwrite*/

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#define 128_MEG 134217728

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

	/*structs to hold time data for each run of write/frwite*/
	struct timeval * before_time;
	struct timeval * after_time;

	/*hold the current size we're using to write to disk*/
	int write_size;

	/*hold number of things of the size to write to the disk*/
	int num_write;

	/*number of times to call write*/
	int write_loop;

	/*String to hold file names! reusable!*/
	char * filename;

	/*String to get a relative path!*/
	char * relpath = "./";

	/*
 	 * Buffer for write - doesn't have any content,
 	 * just to make sure we stay in our space.
 	 */
	void * write_buffer;

	/*holds the number of times we've done the writes.*/
	int count;

	/*return values for functions*/
	int fscanf_return;
	int gettimeofday_return;
	int close_return;
	int fclose_return;
	ssize_t write_return;

	/*loop counter*/
	int i;

	/*We need to specify an input and output file!*/
	if(argc != 2)
	{
		printf("Please specify an input file and an output file.\n");
		exit(EXIT_FAILURE);
	}

	/*Try to open up the input file*/
	filename = malloc(strlen(argv[0]) + strlen(relpath) + 1);
	filename = strncat(filename, relpath, strlen(relpath));
	filename = strncat(filename, argv[0], strlen(argv[0]));
	input = fopen(filename, "r");
	if(input == NULL)
	{
		perror("Error on opening input file - ");
		exit(EXIT_FAILURE);
	}
	free(filename);

	/*Open up the output file*/
	filename = malloc(strlen(argv[1]) + strlen(relpath) + 1);
	filename = strncat(filename, argv[1], strlen(argv[1]));
	filename = strncat(filename, relpath, strlen(relpath));
	output = fopen(argv[1], "w");
	if(output == NULL)
	{
		perror("Error on opening/creating output file - ");
		exit(EXIT_FAILURE);
	}
	free(filename);

	/*initialize our time structs*/
	before_time = malloc(sizeof(struct timeval));
	after_time = malloc(sizeof(struct timeval));

	/*make a big huge buffer to write from*/
	write_buffer = malloc(128_MEG);

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
	while(fscanf_return >= 0 && fscanf_return != EOF)
	{

		/*Read in the size to write!*/
		fscanf_return = fscanf(input, "%i", &write_size);
		fprintf(output, "Doing 10 writes of size %i", write_size);
		/*calculate the number of times to do the write*/
		num_write = 128_MEG/write_size;
		for(count = 0; count < 10; count++)
		{
			/*Here we use write*/
			gettimeofday_return = gettimeofday(before_time, NULL);
			if(gettimeofday_return == -1)
			{
				perror("Error on gettimeofday - ");
				exit(EXIT_FAILURE);
			}
			/*open the file*/
			dump_fd = open(tempnam("/tmp/", "dump"));
			if(dump_fd < 0)
			{
				perror("Error on open - ");
				exit(EXIT_FAILURE);

			}
			/*write to the file with a size of the buffer write_loop times!)*/
			for(i = 0; i < num_write, i++)
			{
				write_return = write(dump_fd, write_buffer, write_size);
				if(write_return < 0)
				{
					perror("Error on write - ");
					exit(EXIT_FAILURE);
				}
			}
			/*close the file*/
			close_return = close(dump_fd);
			if(close_return < 0)
			{
				perror("Error on close - ");
				exit(EXIT_		write_buffer = realloc(write_buffer, write_size);
FAILURE);
			}
			gettimeofday_return = gettimeofday(after_time, NULL);
			if(gettimeofday_return == -1)
			{
				perror("Error on gettimeofday - ");
				exit(EXIT_FAILURE);
			}
			fprintf(output, "write %d: From %d seconds %d microseconds to %d seconds %d microseconds\n", count, (int)before_time->tv_sec, (int)before_time->tv_usec, (int)after_time->tv_sec, (int)after_time->tv_usec);
			/*And here we use fwrite*/
			gettimeofday_return = gettimeofday(before_time, NULL);
			if(gettimeofday_return == -1)
			{
				perror("Error on gettimeofday - ");
				exit(EXIT_FAILURE);
			}
			/*open the file*/
			dump = fopen(tempnam("/tmp/", "dump"));
			if(dump == NULL)
			{
				perror("Error on fopen - ");
				exit(EXIT_FAILURE);
			}
			/*write to the file*/
			fwrite(write_buffer, write_size, num_write, dump);
			if(ferror(dump))
			{
				perror("Error on fwrite - ");
				exit(EXIT_FAILURE);
			}
			/*close the file*/
			fclose_return = fclose(dump);
			if(fclose_return < 0)
			{
				perror("Error on fclose - ");
				exit(EXIT_FAILURE);
			}
			gettimeofday_return = gettimeofday(after_time, NULL);
			if(gettimeofday_return == -1)
			{
				perror("Error on gettimeofday - ");
				exit(EXIT_FAILURE);
			}
			fprintf(output, "fwrite %d: From %d seconds %d microseconds to %d seconds %d microseconds\n", count, (int)before_time->tv_sec, (int)before_time->tv_usec, (int)after_time->tv_sec, (int)after_time->tv_usec);

		}
	}
	/*clean up our toys when we're done*/
	free(write_buffer);
	free(after_time);
	free(before_time);
	return return_value;
}
