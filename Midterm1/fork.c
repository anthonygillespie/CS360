
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
main(){

	int pid = fork();
	std::cout << "PID id " << pid << "\n";
	if (pid == 0){ //Child
		std::cout << "Child about to exec\n";
		execl("/bin/ls","/bin/ls",(char *)0);
	} else{ //Parent
		int status;
		wait(&status);
		std::cout << "Parent done waiting\n";
	}
}
