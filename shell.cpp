#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include<readline/readline.h> 
#include<readline/history.h>


#define RED   		"\001\033[0;31m\002"
#define YELLOW 		"\001\033[0;33m\002"
#define CYAN 		"\001\033[0;36m\002"
#define GREEN 		"\001\033[0;32m\002"
#define BLUE 		"\001\033[0;34m\002"
#define INVERT		"\001\033[0;7m\002"
#define RESET  		"\001\e[0m\002" 
#define BOLD		"\001\e[1m\002"
#define ITALICS		"\001\e[3m\002"
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"
#define BUILT_NUM 3

using namespace std;

void shell_cd(char **args);
void shell_help(char **args);
void shell_exit(char **args);

const char *builtins_str[]={"cd","help","exit"};

void (*builtin_func[]) (char **) = {
  &shell_cd,
  &shell_help,
  &shell_exit
};

void shell_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "shell: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("shell");
    }
  }
  return;
}

void shell_help(char **args)
{
  int i;
  cout<<"Type program names and arguments, and hit enter.\n";
  cout<<"The following commands are built in:\n";

  for (i = 0; i < BUILT_NUM; i++) {
    cout<<builtins_str[i]<<"\n";
  }

  cout<<"Use the man command for information on other programs.\n";
  return ;
}

void shell_exit(char **args)
{
  free(args);
  exit(0);
}

void shell_launch(char **args){
	pid_t pid, wpid;
	int status;
	pid = fork();
	if(pid == 0){
		//child
		if(execvp(args[0],args)==-1){
			perror("shell");
		}
		exit(1);
	}
	else if(pid == -1){
		perror("shell");
	}
	else{
		//parent
		do {
      		wpid = waitpid(pid, &status, WUNTRACED);
    	   } while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return;
}

void shell_execute(char **args){
	if(args[0]==NULL){
		return;
	}
	for(int i=0; i<BUILT_NUM; i++){
		if(strcmp(args[0], builtins_str[i])==0){
			(*builtin_func[i])(args);
			return;
		}
	}
	shell_launch(args);
	return;
}

char *shell_readline(char* path){
	char* buf; 
  	char *line = (char*)malloc(1000*sizeof(char));
    char str[200];
	strcpy(str, RED);
	strcat(str, BOLD);
	strcat(str, getenv("USER"));
	strcat(str, RESET);
	strcat(str, ":");
	strcat(str, CYAN);
	strcat(str, BOLD);
	strcat(str, path);
	strcat(str, RESET);
	strcat(str, "> ");
	buf = readline(str); 
    if (strlen(buf) != 0) { 
        add_history(buf); 
        strcpy(line, buf); 
        return line; 
    } else { 
        shell_readline(path);
    } 
}

char **parse_line(char *line){
	int bufsize = TOK_BUFSIZE, position = 0;
	char **args = (char**)malloc(bufsize*sizeof(char*));
	char *token = strtok(line, TOK_DELIM);
	while(token != NULL){
		args[position]=token;
		position++;
		if(position>=bufsize){
			bufsize+=TOK_BUFSIZE;
			args = (char**)realloc(args, bufsize * sizeof(char*));
		}
		token = strtok(NULL, TOK_DELIM);
	}
	if(!strcmp(args[0],"ls")){
		args[position]=(char*)"--color";
		position++;
		args[position]=0;
	}
	else{
		args[position]=0;
	}
	return args;
}

void shell_loop(){
	char *line;
	char **args;
	char path[FILENAME_MAX];
	while(1){
		
		getcwd(path,FILENAME_MAX);
		line = shell_readline(path);
		args = parse_line(line);
		shell_execute(args);
		free(line);
		free(args);
		}	

}

int main(){
	
	shell_loop();
	return 1;
	
}