#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "improvc/improvc.h"
#include <sys/wait.h>
#include <unistd.h>

#define PROGRAM "program"	


int execute(char* router){

	char *const parmList[] = {router, NULL};

	 pid_t  pid;

     if ((pid = fork()) < 0) { 
		printf("*** ERROR: forking child process failed\n");
		exit(1);
     }
     else if (pid == 0) { 
		system("xterm -e \"./program 1\"");
     }

     return pid;
}


int main(){

	char* router_config = file_read("inputs/roteador.config");
	list_t* lines = string_split(router_config, "\n");
	list_t* programs = new_list(sizeof(int));
	node_t* element = lines->head;
	while(element != NULL){ 

		char* data = ((char*)element->data);
		list_t* columns = string_split(data, "\t");
		char* id = (char*)list_get_element(columns, 0)->data;

	    int pid = execute(id);
	    list_append(programs, &pid);
    	element = element->next;
	}


	node_t* program = programs->head;
	while(program != NULL){
		int status;
		int pid = *(int*)program->data;
		while(waitpid(pid, &status, WNOHANG) != pid);
		program = program->next;
	}
	
    return 0;
}