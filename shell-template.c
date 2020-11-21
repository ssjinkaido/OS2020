#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include "readcmd.h"

void trace_cmd(struct cmdline *l) {

		int i, j;

		/* Display each command of the pipe */
		printf("trace_cmd\n");
		for (i=0; l->seq[i]!=0; i++) {
			char **cmd = l->seq[i];
			printf("seq[%d]: ", i);
			for (j=0; cmd[j]!=0; j++) {
				printf("(%s) ", cmd[j]);
			}
			printf("\n");
		}
}

void change_dir(char *param) {
	char *curr_dir;
	char buf[1024];
	char *path;
	if ((curr_dir=getcwd(NULL, 0)) == NULL) {
		perror("cd failed (getcwd)");
		return;
	}
	
	if (!param) path = getenv("HOME");
	else 
		if (strncmp(param,"/",1)) {
			if ((strlen(curr_dir)+strlen(param)+2)>1024) {
				perror("cd failed (path too large)");
				free(curr_dir);
				return;
			}
			path = strcpy(buf,curr_dir);
			path = strcat(path, "/");
			path = strcat(path, param);
		} else path = param;
		//printf("%s\n",path);
		if (chdir(path)) {
			perror ("cd failed (chdir)");
			chdir(curr_dir);
		}
		free(curr_dir);
}

int main()
{
	int spid, status;
	struct cmdline *l;
	char ***seq;
	char* s;
	while(1) {
		printf("shell> ");

		l = readcmd();
		trace_cmd(l);
		int i;
		int k;
		for (i=0; l->seq[i]!=0; i++) {
			char **cmd = l->seq[i];
			k=i;
		}
		
		seq = l->seq;
		if (! *seq) continue;

		if(!strcasecmp(**seq, "exit")) {
			return 0;
		}

		if(!strcasecmp(**seq, "cd")) {
			change_dir((*seq)[1]);
			continue;
		}
		
		int pipefd[2]; 
	    pid_t p1, p2,p3;
	  	int done1;
	    if (pipe(pipefd) < 0) { 
	        printf("\nPipe could not be initialized"); 
	        
	    } 
	   
		if (k==1){
		   	p1 = fork(); 
		    if (p1 < 0) { 
		        printf("\nCould not fork"); 
		        
		    } 
		  
		    if (p1 == 0) { 
		        // Child 1 executing.. 
		        // It only needs to write at the write end 
		        close(pipefd[0]); 
		        dup2(pipefd[1], STDOUT_FILENO); 
		        close(pipefd[1]); 
		  
		        if (execvp(**seq, *seq) < 0) { 
		            printf("\nCould not execute command 1.."); 
		            exit(0); 
		        } 
		    } else { 
		        // Parent executing 
		        p2 = fork(); 
		  
		        if (p2 < 0) { 
		            printf("\nCould not fork"); 
		            
		        } 
		  
		        // Child 2 executing.. 
		        // It only needs to read at the read end 
		        if (p2 == 0) { 
		        	close(pipefd[1]);
					dup2(pipefd[0], STDIN_FILENO);
		            close(pipefd[0]); 
		            if (execvp(seq[1][0],seq[1] ) < 0) { 
		                printf("\nCould not execute command 2.."); 
		                exit(0); 
		            } 
		        } else { 
		            // parent executing, waiting for two children 
		            waitpid(p1,&done1,0);
			        waitpid(p1,&done1,0); 
		        }
		   	}
	
		}
		else if(k==0){
		    p1 = fork(); 
			if (p1 < 0) { 
			    printf("\nCould not fork"); 
			        
			} 
			if (p1 == 0) { 
			if (execvp(**seq, *seq) < 0) { 
			    printf("\nCould not execute command 1.."); 
			    exit(0); 
			   	} 
			} else { 
			   	waitpid(p1,&done1,0);
			}
		}
		else{
			int p[2];
			pid_t pid;
			int fd_in=0;
			int j=0;
			while(j<k+1){
				pipe(p);
				if((pid=fork())==-1){
					exit(EXIT_FAILURE);
				}
				else if (pid==0){
					dup2(fd_in,0);
					if(j+1!=3){
						dup2(p[1],1);
					}
					close(p[0]);
					execvp(seq[j][0],seq[j]);
					exit(EXIT_FAILURE);
				}
				else{
					waitpid(pid,NULL,0);
					close(p[1]);
					fd_in=p[0];
					j++;
				}
			}
		}
	}
}
 
	
