#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include "listID.h"


int isDirectoryExists(char *path){
	struct stat stats;

	stat(path,&stats);

	if(S_ISDIR(stats.st_mode)){
		return 1;	
	}

	return 0;
}


// rmdir -rf actually
void remove_directory(char *path) {

   	DIR *d = opendir(path);
   	size_t path_len = strlen(path);

	if(d == NULL)return;

  	struct dirent *p;

  	while (p = readdir(d)) {

		char *buf;
		size_t len;

		if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) continue;

		len = path_len + strlen(p->d_name) + 2; 
		buf = malloc(len);
			
		struct stat statbuf;

		snprintf(buf, len, "%s/%s", path, p->d_name);

		if (!stat(buf, &statbuf)) {
			
			if (S_ISDIR(statbuf.st_mode)) remove_directory(buf);
			
			else unlink(buf);
		}

		free(buf);
	}

	closedir(d);

  
 	rmdir(path);
	
	 return;
}

int main(int argc, char *argv[]) {
    
    int res,  bsize, check = 0, pid = -1, status;
    pid_t pid1, pid2, w;
    char ident[10];

    char *cDir , * iDir , * mDir , *logFile, *token;
    FILE *file,*log_file;
    DIR *common_dir = NULL, *input_dir = NULL, *mirror_dir = NULL;
    ListID *lid = NULL;
    int id = -1;


    char filename[20];



    struct dirent *entry;

	void handler() {

		destroyList(&lid);
		
		free(iDir);
		free(cDir);
		free(logFile);

	    fclose(log_file);
	    fclose(file);

	    closedir(common_dir);
	   	closedir(input_dir);
	 	closedir(mirror_dir);

		remove_directory(mDir);
		free(mDir);
		exit(0);
	}

	signal(SIGINT, handler);

    if (argc != 13) {
        
        printf("Wrong arguments!\n");
        
        return 1;
    }

    //arguments processing
    for (int i = 0; i < argc; i++) {

        if (!strcmp(argv[i], "-n")) id = atoi(argv[i + 1]);

        else if (!strcmp(argv[i], "-c")) {
            //creating common directory

            mkdir(argv[i + 1], 0777);

            cDir = strdup(argv[i + 1]);

            common_dir = opendir(argv[i + 1]);
        } else if (!strcmp(argv[i], "-i")) {
            //creating input directory

            //mkdir(argv[i + 1], 0777)

	       iDir = strdup(argv[i + 1]);

            if(!isDirectoryExists(iDir)){
		        printf("<Input directory does not exist! Return!>\n");
		        exit(1);	
	       }

        } else if (!strcmp(argv[i], "-m")) {
            //creating mirror directory

            mDir = strdup(argv[i + 1]);

            int check;

            check = mkdir(argv[i + 1], 0777);

            if(!check){
                mirror_dir = opendir(argv[i + 1]);
            }
            else{
                printf("<Mirror directory has already been created! Return!>\n");
                exit(1);
            }
        } else if (!strcmp(argv[i], "-b")) {
            //assign bsize

            bsize = atoi(argv[i + 1]);

        }
        else if (!strcmp(argv[i], "-l")) {
            //creating logfile

            log_file = fopen(argv[i+1], "a");
            
            logFile = strdup(argv[i+1]);
        }
    }

    //create the file with the format .id
    sprintf(filename, "%s/%d.id", cDir, id);
    
    file = fopen(filename, "a");
    
    if (file == NULL) {
        
        printf("Error with file opening!\n");
        
        return 1;
    }

    //write current id to the file .id
    fprintf(file, "%d", getpid());

    char bufferSize[100];
    sprintf(bufferSize,"%d",bsize);

    //Check every 2s for new entries
    while(1) {

        while ((entry = readdir(common_dir)) != NULL) {

            if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;

            token = strtok(entry->d_name, ".id");

            pid = atoi(token);

            //fprintf(log_file,"%d\t",pid);

            if (search(pid,lid) || pid == id) continue;

            append(pid, &lid);
            
            sprintf(ident, "%d", pid);

            char pipeName[100];
            
            memset(pipeName, '\0', 100 );

            //Creating the named pipe with the format id1_to_id2.fifo
            sprintf(pipeName, "%s/%d_to_%d.fifo", cDir, id, pid);

            pid_t writer_pid = fork();

            //execute writers  program
            if (writer_pid == 0) execlp("./writer", pipeName, iDir, bufferSize, NULL);

            //Creating the named pipe with the format id1_to_id2.fifo
            sprintf(pipeName, "%s/%d_to_%d.fifo", cDir, pid, id);

            pid_t reader_pid = fork();

            //execute writers  program
            if (reader_pid == 0) execlp("./reader", pipeName, bufferSize, logFile, ident, mDir, NULL);

            for (int j = 0; j < 3; j++) {

                w = waitpid(reader_pid, &status, WUNTRACED | WCONTINUED);

                if (WIFEXITED(status)) {
                	
                	printf("Exited, status=%d\n", WEXITSTATUS(status));

                	break;
                }

                else if (WIFSIGNALED(status)) printf("Killed by signal %d\n", WTERMSIG(status));

                else printf("Unknown exit\n");
            }
        }

        sleep(2);

        rewinddir(common_dir);
    }
}
