/* @Author Gregory Mayer (gmayer1@umbc.edu)
 * part 5, In my code when all of the p1 are not closed no noticable effects happen to the program. the effects are the the file descriptors are not released properly. potentially causing the program to hang because it can not access a file descriptor because it is in use.
 */
#define _POSIX_SOURCE
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
int pipe1[4][3]; //list of p1
int pipe2[4][3]; //list of p2
pid_t childpid[4]; //list of the children

//shows how many votes a candidate has for all candidates
void showVotes(char** argv){
	for(int i = 0; i < 4; i++){
		kill(childpid[i],SIGUSR2);
		unsigned long value = 0;
		read(pipe2[i][0], &value, 4);
		printf(" %d. %s - %lu \n", i , argv[i + 1], value);
	}
}

//allows the user to vote for a candidate
void castBallot(char** argv){

	for(int i = 0; i < 4; i++){
		printf(" %d. %s \n", i, argv[i + 1]);
	}

	int user_choice = -1;
	while(user_choice < 0 || user_choice > 3){
		scanf("%d", &user_choice);
	}

	kill(childpid[user_choice], SIGUSR1);

}

//allows for the user to commit voter fraud
void selectVoterFraud(char** argv){
	printf("Select the candidate which you would like to change votes: \n");	
	unsigned long fraudVotes = 0;
	for(int i = 0; i < 4; i++){
		printf(" %d. %s \n", i, argv[i + 1]);
	}
	//valid choice
	int user_choice = -1;
	while(user_choice < 0 || user_choice > 3){
		scanf("%d", &user_choice);
	}
	printf("Enter how many votes: ");
	scanf("%lu", &fraudVotes);
	//ensures bounds are kept
	if(fraudVotes >= 0 && fraudVotes < 4294967296){
		write(pipe1[user_choice][1], &fraudVotes, 4);
		kill(childpid[user_choice], SIGALRM);
	}
	else{
		printf("Enter a valid number");
	}
}

//display the options for the user to select
void menu(char** argv){
	int user_choice = -1;
	while(user_choice != 0){
		printf("Main Menu:\n  0. End program\n  1. Cast ballot\n  2. Show vote counts\n  3. set vote count \n");
		scanf("%d", &user_choice);
		if(user_choice == 0){
			return;
		}
		else if(user_choice == 1){
			castBallot(argv);
		}
		else if(user_choice == 2){
			showVotes(argv);
		}
		else if(user_choice == 3){
			selectVoterFraud(argv);
		}
	}
}


int main(int argc, char **argv){
	if(argc != 5){
		printf("You have the wrong number of candidates for this election, you must have 4 \n");
		return 0;
	}
	//creates 8 pipes 
	for(int i = 0; i < 4; i++){
		pipe(pipe1[i]);
		pipe(pipe2[i]);
	}
	//creates four forks
	for(int j = 0; j < 4; j++){
		//catches errors
                if((childpid[j] = fork()) == -1)
                {
			printf("Failed");
                        perror("fork");
                        return -1;
                }
		//this is the child process
                if(childpid[j] == 0)
                {
                        /* Child process switching fd closes up uneeded fd */
			dup2(pipe1[j][0], 0);
			close(pipe1[j][0]);
			dup2(pipe2[j][1], 2);
			close(pipe2[j][1]);
			for(int k = 0; k < 4; k++ ){
				if(k != j){
				close(pipe1[k][0]);
				//close(pipe1[k][1]);
				//close(pipe2[k][0]);
				close(pipe2[k][1]);
				}
			}
			execlp("./candidate", "./candidate", argv[j + 1],(char*) NULL);
                }
                else
                {
			//parent
                }
	}
	//remove the pipes not being used
	for(int k = 0; k < 4; k++ ){
		close(pipe1[k][0]);
		close(pipe2[k][1]);
	}
	menu(argv);
	return 0; //exits
}
