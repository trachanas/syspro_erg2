#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include "listID.h"

/*
     *  This program reads from the named pipe all the necessary information (filename,length,size,file)
     *  After this, the program copies the received file to the mirror directory.
     *  Also, it writes to the logfile.
     *  The whole communication ends when reader receives the string "stop" = "stop communication"
 * */


int main(int argc,char *argv[]){

	char *FIFO = strdup(argv[0]);
	int bsize = atoi(argv[1]);
	int id = atoi(argv[3]);
	char *mirrorDir = strdup(argv[4]);
    
    	char *stop = strdup("stop communication");
	char bufferRead[bsize];
	int fd;
	int sumOfBytes = 0;
	int currBytes = 0;
	char *filename;
	int sum = 0;

	ListID *lid = NULL;

	FILE *toTransfer = NULL;
	FILE *logfile = fopen(argv[2],"a");
	
	if(logfile == NULL){
	    perror("Error with logfile opening!\n");
	}

	mkfifo(FIFO, 0777);

	//this loop ends with "stop" received
	while(1) {

        fd = open(FIFO, O_RDONLY, 0666);

        // read the filename length
        currBytes =	read(fd, bufferRead, bsize);

        //Check for "stop"
        int check = strncmp(bufferRead, stop, strlen(stop));

        close(fd);
        
        if (!check) {

        	printf("Finished all files ... \n");
        	
        	break;
        }
        
        memset(bufferRead, '\0', bsize);
        fd = open(FIFO, O_WRONLY, 0666);
        write(fd, "OK", 3);
        close(fd);


        // read the filename
        fd = open(FIFO, O_RDONLY, 0666);
        currBytes = read(fd, bufferRead, bsize);
	
        printf("The file to write is %s\n", bufferRead);
        filename = strdup(bufferRead);
        close(fd);

        memset(bufferRead, '\0', bsize);
        fd = open(FIFO, O_WRONLY, 0666);
        write(fd, "OK", 3);
        close(fd);

        // read the file size
        fd = open(FIFO, O_RDONLY, 0666);
        read(fd, bufferRead, bsize);
        sumOfBytes = atoi(bufferRead);
        //printf("%d\n",atoi(bufferRead));
        //printf("%d\n",sumOfBytes);

        close(fd);

     //   appendString(filename,sumOfBytes,id,&lid);

        //Write to the logfile
        if(search(id,lid)){
            fprintf(logfile, "%d\t%s\t%d\t%d\t%d\t%d\t\n", id ,filename,sumOfBytes,sumOfBytes,1,0);
        }else{
            fprintf(logfile, "%d\t%s\t%d\t%d\t%d\t%d\t\n", id ,filename,sumOfBytes,sumOfBytes,1,1);
        }



        memset(bufferRead, '\0', bsize);
        fd = open(FIFO, O_WRONLY, 0666);
        write(fd, "OK", 3);
        close(fd);

        char path[100];
        char subDir[100];

        // Creating the new file for the mirroring
        sprintf(subDir, "%s/%d", mirrorDir, id);
        mkdir(subDir, 0777);

        sprintf(path,"%s/%s", subDir, filename);

        printf("The path of the filename %s\n", path);

        toTransfer = fopen(path, "w+");
        
        if (toTransfer == NULL) {
            
            perror("error with open file");
            
            return 0;
        }

        int j = 0;
        sum = 0;

        printf("Start reading the file from pipe\n");

        // Read the whole file
        while(1) {
        	
            fd = open(FIFO, O_RDONLY, 0666);

            currBytes = (int)read(fd, bufferRead, bsize);
            
            close(fd);

            // Each data tranfer ends when "00" is received
            if (!strcmp(bufferRead, "00")) {
                
                fclose(toTransfer);
                
                break;
            }
            
            j = (int)fwrite(bufferRead, 1, (size_t) currBytes, toTransfer);

            sum += j;

            memset(bufferRead, '\0', bsize);

            fd = open(FIFO, O_WRONLY, 0666);
            write(fd, "OK", 3);
            close(fd);
        }

        printf("End of writing file %s\n", filename);
    }

    	printf("Reader worker about to exit\n");

	fclose(logfile);

	return 0;
}
