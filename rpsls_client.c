#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>    /* Internet domain header */
#include <arpa/inet.h>     /* only needed on my mac */
#include <netdb.h>         /* gethostname */

#define PORT_NUM 60000
#define BUF_SIZE 128

int split(char * buff);
/*
 * this function gets input from other function and print stdrr or stdout
 * for the user to know the result in each round
 * */

int handle_print(char *buff){

	if(split(buff)==1){
		printf("Congrats!\n");
		fprintf(stderr, "Win!\n");
	}

	else if(split(buff)==2){
		printf("Sorry!\n");
		fprintf(stderr, "Lose!\n");
	}

	else if(split(buff)==3){
		printf("tie!\n");
		fprintf(stderr, "Equal!\n");
	}
	else if(split(buff)==4){
		return 1;
	}

	return 0;

}
/*
 *this function split the buffer to see this user
 *won or lose or they should exit
 *
 * */
int split (char * buf)
{
	int i = 0;
	char *p = strtok (buf, "/");
	char *array[4];

	while (p != NULL)
	{
		array[i++] = p;
		p = strtok (NULL, "/");
	}
	for (i = 0; i < 4; i++) {
		if(array[i] !=NULL){


			if(strcmp(array[i],"win")==0)
				return 1;
			else if(strcmp(array[i],"lose")==0)
				return 2;
			else if(strcmp(array[i],"equal")==0)
				return 3;
			else if(strcmp(array[i],"Exit")==0)
				return 4;
			else if(strcmp(array[i],"ok")==0)
				return 5;
		}}

	return 0;


}
/*
 *each input should be allowed to be sent
 *to the server so this function check if the input
 *that user enters is correct or not
 * */

int check_input(char * buf){
	if(strlen(buf) !=0){
		char r[] = "r";
		char p[] = "p";
		char s[] = "s";
		char l[] = "l";
		char S[] = "S";
		char e[] = "e";

		if((strcmp(buf,r)==0) || (strcmp(buf,p)==0) || (strcmp(buf,s)==0) || (strcmp(buf,l)==0) || (strcmp(buf,S)==0) || (strcmp(buf,e)==0))

			return 1;}



	return 0;
}

/*
 *This function handle client input
 *first it receive a message from server to type his name
 *and then each turn the user should enter a letter as input
 *if user enter e it will get the result and then exit
 * */

int main(int argc, char **argv) {

	if (argc < 2) {
		fprintf(stderr, "Usage: simple_client hostname\n");
		exit(1);
	}
	char *hostname = argv[1];
	int n=0;
	if( argc ==3){
		n = atoi(argv[2]); }



	struct sockaddr_in addr;

	// Allow sockets across machines.
	addr.sin_family = AF_INET;

	// The port the server will be listening on.
	addr.sin_port = htons(PORT_NUM+n);

	// Clear this field; sin_zero is used for padding for the struct.
	memset(&(addr.sin_zero), 0, 8);

	// Lookup host IP address.
	struct hostent *hp = gethostbyname(hostname);
	if (hp == NULL) {
		fprintf(stderr, "unknown host %s\n", hostname);
		exit(1);
	}

	addr.sin_addr = *((struct in_addr *) hp->h_addr);

	// Request connection to server.
	int soc = socket(AF_INET, SOCK_STREAM, 0);
	if (soc < 0) {
		perror("socket");
		exit(1);
	}

	while (connect(soc, (struct sockaddr *)&addr, sizeof(addr)) == -1) {	
		close(soc);
		addr.sin_port = htons(PORT_NUM+1+n);
		soc = socket(AF_INET, SOCK_STREAM, 0);
		if (soc < 0) {
			perror("socket");
			exit(1);}
	}




	char buff[BUF_SIZE+1];
	static int count=1;
	int i, j;
	int num_read, pass,go=0;
	for(;;)
	{
		if(count==1){
			bzero(buff,sizeof(buff));
			write(soc,buff,sizeof(buff));
			count ++;
			bzero(buff,sizeof(buff));
			num_read=read(soc,&buff,BUF_SIZE);
			buff[num_read] = '\0';
			printf("From Server : %s\n",buff);
			bzero(buff,sizeof(buff));
			j=0;
			i = 0;
			while((j = getchar()) != '\n' && j != EOF ) { // in this part we get the buffer that server sent which is asking for users name
				buff[i++] = j;
			}
			buff[i] = '\0';
			write(soc,buff,strlen(buff)); // we write the name to be sent to server
			buff[0]='\0';
		}

		bzero(buff,sizeof(buff));

		num_read=read(soc,&buff,BUF_SIZE); 
		buff[num_read] = '\0';
		


		// we check if the server sent "ok" we will show a sentence to ask user to enter the letter
		// also we want to repeat this untill one user enter e so I added "go" here to continue the loop
		while((strlen(buff) != 0) && (split(buff)==5 || go==1)){ 
			buff[0]='\0';
			printf("Enter r,p,s,l,S or e for exit(no change is accepted) : ");
			j=0;
			i = 0;
			while((j = getchar()) != '\n' && j != EOF ) {
				buff[i++] = j;
			}
			buff[i] = '\0';
			pass = check_input(buff);

			// here we checked if the input was correct letter or not.
			// if it is correct go to read the result server will send
			if(pass==1){
				write(soc,buff,strlen(buff));
				bzero(buff,sizeof(buff));
				sleep(5);
				go=1;
				num_read=read(soc,&buff,BUF_SIZE);
				if(num_read>0){
					if(handle_print(buff)==0){ // it means user received win or lose but not exit so continue
						go =1;
					}
					else 
						go =2;}   // but if it is exit so change go to 2 and jump to outside of the loop
			}
			// here the letter that user enter was wronge so 
			// we continue to ask him to enter again
			else if(pass==0){
				while(pass==0){
					bzero(buff,sizeof(buff));
					printf("Please enter a correct letter r,p,s,l,S or e for exit((no change is accepted): ");      
					j=0;
					i = 0;
					while((j = getchar()) != '\n' && j != EOF ) {
						buff[i++] = j;
					}
					buff[i] = '\0';
					pass = check_input(buff);
					if(pass==1){
						write(soc,buff,strlen(buff));
						bzero(buff,sizeof(buff));
						sleep(3);
						go=1;
						num_read=read(soc,&buff,BUF_SIZE);
						if(num_read>0){
							if(handle_print(buff)==0){ // like before if user got win or lose continue
								go =1;
							}
							else 
								go =2;}  // but if it is exit  so go to other step
					}
				}
			}
		}

		if(go==2){   // user received exit here he will see the final result

			bzero(buff,sizeof(buff));
			num_read=read(soc,&buff,BUF_SIZE);
			buff[num_read]='\0';
			printf("%s\n",buff);	
			exit(1);

		}
	}

	// Clean up.
	close(soc);
	return 0;
}
