#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

char mysh[] = "mysh:~$ ";
char history[10][50];
char argv[10][50];
char input[200], hisinput[200];
int nhis;
int bg_num;
enum processes {CD, NORMAL, OUTDIR,INDIR,PIPE, BACKGROUND};
// int my_parse(char *input, char args[10][50]);
// int	my_execute(char args[10][50], int num);

int my_parse(char *input){
	char *p;
	int n = 0;
	p = strtok(input, " ");
	while (p != NULL){
		strcpy(argv[n++], p);
		p = strtok(NULL, " ");
	}
	argv[n - 1][strlen(argv[n-1]) - 1] = '\0';
    return n;
}

int my_execute(int num){
	int i, j, fp, fps[2], bg = 0;
    pid_t pid, pid2;
    char* in_dir, *out_dir;
    char* args[10];
    char* args2[10];
    enum processes type=NORMAL;

    // char ** exeargs = malloc ( num * sizeof (char *));

    if (num == 0){
        return 1;
    }

    if (strcmp(argv[0], "exit") ==0 ){
        exit(0);
    }

    // history try
    if (strcmp(argv[0],"!!") == 0){
        if (strlen(history[0]) == 0){
            printf("No commands in history.\n");
            return 1;
        }
        else{
            i = 0;
            while (i < 10){
                strcpy(argv[i], history[i]);
                if (i < nhis - 1){
                    printf("%s ", history[i]);
                }
                else if(i == nhis - 1) {
                    printf("%s\n", history[i]);
                }
                i ++;
            }
            num = nhis;
        }
    }


    for (i = 0; i <10; i++){
        args[i] = malloc (strlen(argv[i])+ 1);
        args2[i] = malloc (strlen(argv[i])+ 1);
    }
    for (i = 0; i < num; i ++){
        args[i] = argv[i];
    }
    args[i] = NULL;

    for (i = 0; i < num; i ++){
        strcpy (history[i], argv[i]);
    }
    nhis = num;

    if (strcmp(args[0], "cd") == 0){
        type = CD;
    }
    else {
        for (i = 0; i < num; i ++){
            if (strcmp(args[i], "<") == 0){
                if (args[i + 1] == NULL){
                    fprintf(stderr, "Error: Expect argument of \"<\"\n");
                    return 1; 
                }
                else {
                    in_dir = args[i + 1];
                    args[i] = NULL;
                    type = INDIR;
                    break;
                }
            }
            else if (strcmp(args[i], ">") == 0){
                if (args[i + 1] == NULL){
                    fprintf(stderr, "Error: Expect argument of \">\"\n");
                    return 1;
                }
                else {
                    out_dir = args[i + 1];
                    args[i] =  NULL;
                    type = OUTDIR;
                    break;
                }
            }
            else if (strcmp(args[i], "|") == 0){
                args[i] = NULL;
                
                for (j = i + 1; j < num; j++){
                    args2[j - 1 - i] = args[j];
                }
                args2[j - 1 - i] = NULL;
                type = PIPE;
                break;
            }
        }
    }
    


    if (strcmp(args[num - 1], "&")==0){
        bg = 1;
        args[num - 1] = NULL;
        // type = BACKGROUND;
    }

// cd
	if (type == CD){
            if (args[1] == NULL || strlen(args[1]) == 0){
               chdir(getenv("HOME"));
               return 1;
            }
            else if (*args[1]=='~'){ 
                memmove(args[1], args[1]+1, strlen(args[1]));
                strcpy(args2[0], getenv("HOME"));
                args[1] = strcat(args2[0], args[1]);
                if (chdir(args[1]) != 0){
                    perror("Error of directory");
                }
            }
            else if (chdir(args[1]) != 0){
            // else if (setenv(args[1]) != 0){
                perror("Error of directory");
            }
        
    }

// normal operation
    else {
    if (type == PIPE){
        pipe(fps);
    }

    pid = fork();
    if (type == PIPE && pid != 0){
        pid2 = fork();
    }
    
    if (pid < 0) {
      perror("lsh");
      exit(0);
    }
    else if (pid == 0 || (type == PIPE && pid2 == 0)) {
        switch (type){
            case NORMAL:
                if (bg) {
                    fp = open("/dev/null", 0);
                    dup2(fp, 0);
                    dup2(fp, 1);
                    dup2(fp, 2);
                    fflush(stdin);
                    fflush(stdout);
                    fflush(stderr);
                }
                if (execvp(args[0], args) != 0){
                    perror("Error command or parameters");
                }
                exit(0);
            break;

            case INDIR:
                fp = open(in_dir,O_RDONLY);
                if (fp == -1) {
                    perror("Wrong Directory");
                    exit(0);
                }
		        dup2(fp,STDIN_FILENO);
                close(fp);
                if (execvp(args[0], args) != 0){
                    perror("Error command or parameters");
                }
            break;

            case OUTDIR:
                fp = open(out_dir,O_WRONLY|O_CREAT|O_TRUNC, 0666);
		        dup2(fp,STDOUT_FILENO);
                if (execvp(args[0], args) != 0){
                    perror("Error command or parameters");
                }
            break;

            case PIPE:
                if (pid2 < 0){
                    perror("pipe error");
                    exit(0);
                }
                else if(pid2 == 0 && pid != 0) {
                    dup2(fps[0], STDIN_FILENO);
                    close(fps[1]);
		            execvp(args2[0],args2);
                    exit(0);
		            
                }
                if (pid < 0){
                    perror("pipe error");
                    exit(0);
                }
                else if (pid == 0) {
                    dup2(fps[1],STDOUT_FILENO);
                    close(fps[0]);
                    execvp(args[0], args);
                    exit(0);
                }
            break;
        }
    } 
    else {
            if (bg == 0) {
                if (type == PIPE){

                    close(fps[0]); 
                    close(fps[1]);
                }
                waitpid(pid,NULL, 0);

                if (type == PIPE){
                    waitpid(pid2, NULL, 0);
                }
            }
            else {
                fflush(stdout);
                printf ("[%i] %d\n", ++bg_num, pid);
            }
    }
        
    }
// remember history
    
	return 1;
}



int main () {
	char	args[10][50];
	int		do_exit	=	1, n, i = 0;
	
	do {
		printf("%s", mysh);
		fflush(stdout);
        fflush(stdin);

		fgets(input, sizeof input, stdin);
		
		n = my_parse(input);
		do_exit = my_execute(n);

         while (i < n){
             memset(argv[i], 0 , sizeof argv[i]);
             i++;
         }
	}
	while (do_exit);
    return 0;
}

