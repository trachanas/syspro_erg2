#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>


/*
     *  This program send to the named pipe all the necessary information (filename,length,size,file)
     *  The whole communication ends when reader receives the string "stop" = "stop communication"
 * */


int main(int argc, char *argv[]) {

	char *FIFO = strdup(argv[0]);
	char *iDir = strdup(argv[1]);
	int bsize = atoi(argv[2]);
	
	char bufferRead[bsize];
    char buffer[bsize];
    char path[512];

    char *stop = strdup("stop communication");

	int fd;
	int len;
	struct dirent *entry;
	struct stat buf;
	FILE *file = NULL;
	int fi;
	int filesize;
	char bufferFile[bsize];

	mkfifo(FIFO,0666);

	DIR *input_dir = opendir(iDir);
	
	int sum = 0;
    int k = 0;

	while ((entry = readdir(input_dir)) != NULL) {

	    if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;

        // send the filenames length
        len = (int)strlen(entry->d_name);
        
        char length[10];
        
        sprintf(length, "%d", len);
		
	    fd = open(FIFO, O_WRONLY,0666);
        write(fd, length, (size_t)bsize);
        close(fd);

        fd = open(FIFO, O_RDONLY,0666);
        read(fd,buffer,(size_t)(bsize));
        close(fd);

        // send the filename

        fd = open(FIFO,O_WRONLY,0666);
        sprintf(bufferRead, "%s", entry->d_name);
        write(fd, bufferRead, (size_t)bsize);
        memset(bufferRead, '\0', bsize);
        close(fd);

        fd = open(FIFO, O_RDONLY,0666);
        read(fd,buffer,(size_t)bsize);
        close(fd);

        // send the files size
        
        sprintf(path, "%s/%s", iDir, entry->d_name);
        
        file = fopen(path, "r");

        if (file == NULL) {
            perror("Error with open file");
        }

        //Use stat to count file size
        fi = fileno(file);
        fstat(fi, &buf);
        filesize = (int) buf.st_size;

        sprintf(bufferRead, "%d", filesize);
        
        fd = open(FIFO,O_WRONLY,0666);
        write(fd, bufferRead, bsize);
        close(fd);

        fd = open(FIFO, O_RDONLY,0666);
        read(fd,buffer,bsize);
        close(fd);

        memset(bufferRead, '\0', bsize);

        // send the whole file
        
        int s = 0;

        while (s = (int)fread(bufferFile, 1, (size_t) bsize, file)) {

        	if (s == 0) break;

            fd = open(FIFO,O_WRONLY,0666);
            write(fd, bufferFile, s);
            close(fd);

            fd = open(FIFO, O_RDONLY,0666);
            read(fd,buffer,bsize);
            close(fd);

            sum += s;
            memset(bufferFile, '\0', bsize);
        }

        // send termination message "00"
        
        memset(bufferRead,'\0',bsize);
        strcpy(bufferRead,"00");

        fd = open(FIFO,O_WRONLY,0666);
        write(fd,bufferRead,bsize);
        close(fd);

        printf("Sent file\n");
        
        fclose(file);
    }
    
    printf("Writer worker about to exit\n");

    fd = open(FIFO,O_WRONLY,0666);

    //Send "stop" string
    write(fd, stop, (size_t)bsize);
    
    close(fd);
}
