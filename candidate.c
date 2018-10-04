/* @Author Gregory Mayer (gmayer1@umbc.edu)
 * creates a candidate and keeps track of the number of votes the candidate has received
 */
#define _POSIX_SOURCE
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

//stores the number of votes a candidate has received
unsigned int votes = 0;
bool voterFraudBool = false;
//this displays the candidate that was created and their pid.
void createCandidate(char* candidate){
	printf("I am %s at %d\n", candidate, getpid());
	
}

//shows the number of votes that a candidate has received
void showVotes(int signo){
	//fprintf(stderr, "%4X", votes);
	write(2, &votes, 4);
}

//increaments the number of votes that a candidate has
void voteForCandidate(int signo){
	votes = votes + 0x01;
}

//allows the number of votes for a candidate to be set
void voterFraud(int signo){
	voterFraudBool = true;
}



// sets up the program taking in a candidate
// as strings
int main(int argc, char **argv){
	if(argc < 2){
	  printf("no candidate given");
	  return 0;
	}
	if(argc > 2){
		printf("too many candidates were given");
		return 0;
	}
	for(int i = 1; i < argc; i++){
		createCandidate(argv[i]);
	}
	sigset_t mask;
 	sigemptyset(&mask);
 	struct sigaction sa = {
 		.sa_handler = voteForCandidate,
 		.sa_mask = mask,
 		.sa_flags = 0
 	};
 	sigaction(SIGUSR1, &sa, NULL);
	sigset_t mask2;
 	sigemptyset(&mask2);
 	struct sigaction sa2 = {
 		.sa_handler = showVotes,
 		.sa_mask = mask2,
 		.sa_flags = 0
 	};
	sigset_t mask3;
 	sigemptyset(&mask3);
 	sigaction(SIGUSR2, &sa2, NULL);
 	struct sigaction sa3 = {
 		.sa_handler = voterFraud,
 		.sa_mask = mask3,
 		.sa_flags = 0
 	};
 	sigaction(SIGALRM, &sa3, NULL);
    	
	//terminates the process when ctrl-d is presssed
	unsigned int buf;
	while(true) {
		ssize_t valid = read(0, &buf, sizeof(buf));
		if(valid == 0){
			break;
		}
		else if(valid > 0 && voterFraudBool){
			votes = buf;
			voterFraudBool = false;
		}
	}
	printf("terminating pid %d \n", getpid());
}
