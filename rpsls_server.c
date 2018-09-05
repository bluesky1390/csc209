#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define MAX_BACKLOG 5
#define MAX_CONNECTIONS 2
#define BUF_SIZE 128
#define PORT 60000


static int client1;
static int client2;

struct sockname {
	int sock_fd;
	char *username;
};

/*
 *This function is to ckeck of input from users are correct or not
 * it compre the buffer with r-p-s-l-S or e
 *if correct returns 1 and if not returns 0
 *
 * */

int check_buf(char * buf){
	if(strlen(buf) !=0){
		char r[] = "r";
		char p[] = "p";
		char s[] = "s";
		char l[] = "l";
		char S[] = "S";
		char e[] = "e";


		if((strcmp(buf,r)==0) || (strcmp(buf,p)==0) || (strcmp(buf,s)==0) || (strcmp(buf,l)==0) || (strcmp(buf,S)==0)|| (strcmp(buf,e)==0))

			return 1;}



	return 0;
}
/*
 *for each letter that client chooses I have a number for that
 *this function check which number is related to each input
 *
 * */

int find_number(char *c){
	char r[] = "r";
	char p[] = "p";
	char s[] = "s";
	char l[] = "l";
	char S[] = "S";
	char e[] = "e" ;




	if(strcmp(c,r)==0) 
		return 0;
	else if(strcmp(c,l)==0)
		return 1;
	else if(strcmp(c,S)==0) 
		return 2;
	else if(strcmp(c,s)==0)
		return 3;
	else if(strcmp(c,p)==0)
		return 4;
	else if(strcmp(c,e)==0)
		return 5;

	else 
		return -1;
}
/*
 *this function uses two numbers related to each input and calculate
 *if this user won or lose. for example each r-p-s-l-S if we make
 *a pentagon where each will crush the nex one we see that each one 
 *crush next and third next, but second and fourth one after current
 * can beat the current one.
 * */


int * check_score(int n1 , int n2){

	static int res[4];

	if((n2 == (n1+1)%5) || (n2==(n1+3)%5)){
		client1 ++ ;
		res[0]=1 ;
		res[1]=0;
		res[2]= client1;
	}
	else  if((n2 == (n1+2)%5) || (n2==(n1+4)%5) ){
		client2 ++ ;
		res[1]= 1 ;
		res[0]=0;
		res[3]= client2;}
	else if(n1==n2){    //when both numbers are equal
		res[1]= 1 ;
		res[0]=1;
		res[2]= client1;
		res[3]= client2;}

	return res;

}






/* Accept a connection. Note that a new file descriptor is created for
 * communication with the client. The initial socket descriptor is used
 * to accept connections, but the new socket is used to communicate.
 * Return the new client's file descriptor or -1 on error.
 */
int accept_connection(int fd, struct sockname *connections) {
	int user_index = 0;
	while (user_index < MAX_CONNECTIONS && connections[user_index].sock_fd != -1) {
		user_index++;
	}

	if (user_index == MAX_CONNECTIONS) {
		fprintf(stderr, "server: max concurrent connections\n");
		return -1;
	}

	int client_fd = accept(fd, NULL, NULL);
	if (client_fd < 0) {
		perror("server: accept");
		close(fd);
		exit(1);
	}

	connections[user_index].sock_fd = client_fd;
	connections[user_index].username = NULL;
	return client_fd;
}


/* Read a message from client_index and calculates the score and send back the result to them.
 * Return the fd if it has been closed or 0 otherwise.
 */
int read_from(int client_index, struct sockname *connections) {
	int fd = connections[client_index].sock_fd;
	char buf[BUF_SIZE + 1],count[50];
	static int inputs[100], client_ind[2];
	static int index_arr[100] ={-1},ask_name=0, fds[2],fd_arr[100], j, k,i, game_counter,total1, total2;
	char name1[20]; 
	char name2[20];
	int *result=NULL;

	fd_arr[j]= fd+1;
	j++;
	if(ask_name<2 && fd_arr[j-1] !=fd_arr[j-2]){
		while(write(fd,"Please type your name:",21) < 0 ); // if both users connect it will ask them for their name and this will happen just one time
		ask_name ++;
	}
	bzero(buf,sizeof(buf));
	int num_read = read(fd, &buf, BUF_SIZE);
	buf[num_read] = '\0';

	

	if(strlen(buf) !=0 && k <2){
		if(k==0){
			strncpy(name1, buf, strlen(buf));
			name1[strlen(buf)+1]='\0';
		} // saving their names
		else if(k==1){
			strncpy(name2, buf, strlen(buf));
			name2[strlen(buf)+1]='\0';
			
		}
		fds[k]= fd;

		client_ind[k] = client_index; // also saving index received and we will order the names according the corresponding index later
		k++;
	}

	if(k==2){
		sleep(4);

		for(int h=0; h<2; h++){
			while(write(fds[h],"/ok/",4)<0); // send ok to the users to start entering inputs
			k++;
				
			printf("fd is %d \n",fds[h]);

		}	}


//	each time we will check if both users enter
//      or one user enter twice we ignore that. but if we finish one round nom
	if(((i>0 && index_arr[i-1] != client_index) || (i%2 ==0)) &&  check_buf(buf)==1  ){ 	                                                         
		int num = find_number(buf);
		if(num!=5){
			index_arr[i] = client_index;
			i++;
			inputs[i-1]= num;
			if(i %2==0){
				if(index_arr[i-2] < index_arr[i-1]){ //check which one is the first client
					result=check_score(inputs[i-2], inputs[i-1]); // we put them in correct order to know which score is related to which client
					total1 = result[2];
					total2=  result[3];
					sleep(2);

					if(result[0]> result[1]){
						write(connections[index_arr[i-2]].sock_fd, "/win/", 5);
						write(connections[index_arr[i-1]].sock_fd, "/lose/", 6);
						game_counter++;
					}
					else if(result[0]< result[1]){
						write(connections[index_arr[i-1]].sock_fd,"/win/",4);
						write(connections[index_arr[i-2]].sock_fd, "/lose/", 6);
						game_counter++;
					}
					else   {
						for(int h=0; h<2;h++){
							write(fds[h], "/equal/",7);}
						game_counter++;}

				}
				else {
					result=check_score(inputs[i-1], inputs[i-2]); //order the clients , first client at the left side and its result will be displyed as res[0]
					total1 = result[2];
					total2=  result[3];
					sleep(2);

					if(result[0]> result[1]){
						write(connections[index_arr[i-1]].sock_fd, "/win/", 5);
						write(connections[index_arr[i-2]].sock_fd, "/lose/", 6);
						game_counter++;
					}
					else if(result[0]< result[1]){
						write(connections[index_arr[i-2]].sock_fd,"/win/",4);
						write(connections[index_arr[i-1]].sock_fd, "/lose/", 6);
						game_counter++;
					}
					else   {
						for(int h=0; h<2;h++){
							write(fds[h], "/equal/",7);}
						game_counter++;}
				}		         			   
			}      
		}

		// this part we received e from user so we concatenate all result and send them to the users
		else if(num==5){
			char total_client1[20], total_client2[20];
			char final[50]= "Total round was:" ;
			sleep(3);

			for(int d=0; d<2; d++){
				while(write( fds[d],"/Exit/",6) < 0 );
				sprintf(count, "%d", game_counter);
				sprintf(total_client1, "%d", total1);
				sprintf(total_client2, "%d", total2);
			}
			strncat(final,count,strlen(count));
			strncat(final," ",1);
			if(client_ind[0]  >  client_ind[1] ){// it matters what was the client name who has bigger index
				strncat(final,name2,strlen(name2)+1);
				strncat(final,": ",2);
				strncat(final,total_client1,strlen(total_client1)+1);
				strncat(final," ",1);
				strncat(final,name1,strlen(name2)+1);
				strncat(final,": ",2);
				strncat(final,total_client2,strlen(total_client2)+1);

			} else {
				strncat(final,name1,strlen(name1)+1);
				strncat(final,": ",2);
				strncat(final,total_client1,strlen(total_client1)+1);
				strncat(final," ",1);
				strncat(final,name2,strlen(name2)+1);
				strncat(final,": ",2);
				strncat(final,total_client2,strlen(total_client2)+1);

			}
			sleep(9);	
			while(write(fds[0],final,strlen(final)) < 0 );
			while(write(fds[1],final,strlen(final)) < 0 );	


		}
	}


	else if (num_read == 0 || write(fd, buf, strlen(buf)) != strlen(buf)) {
		connections[client_index].sock_fd = -1;
		return fd;
	}

	return 0;
}


int main(int argc, char* argv[]) {
	int n=0;
	if(argc ==2){
		n = atoi(argv[1]); }
	struct sockname connections[MAX_CONNECTIONS];
	for (int index = 0; index < MAX_CONNECTIONS; index++) {
		connections[index].sock_fd = -1;
		connections[index].username = NULL;
	}



	// Set information about the port (and IP) we want to be connected to.
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;

	memset(&server.sin_zero, 0, 8);


	// Create the socket FD.

	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0) {
		perror("server: socket");
		exit(1);
	}

	else
		printf("Socket successfully created..\n");

	server.sin_port = htons(PORT+n);




	while (bind(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		close(sock_fd);
		int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (sock_fd < 0) {
			perror("server: socket");
			exit(1);}
		server.sin_port = htons(PORT+1+n);

	}

	printf("Successfuly binded");
	printf("server port = %d\n", htons(server.sin_port));


	// Announce willingness to accept connections on this socket.
	if (listen(sock_fd, MAX_BACKLOG) < 0) {
		perror("server: listen");
		close(sock_fd);
		exit(1);
	}

	else
		printf("Server listening..\n");

	// The client accept - message accept loop. First, we prepare to listen 
	// to multiple file descriptors by initializing a set of file descriptors.
	int max_fd = sock_fd;
	fd_set all_fds;
	FD_ZERO(&all_fds);
	FD_SET(sock_fd, &all_fds);

	while (1) {
		// select updates the fd_set it receives, so we always use a copy and retain the original.
		fd_set listen_fds = all_fds;
		int nready = select(max_fd + 1, &listen_fds, NULL, NULL, NULL);
		if (nready == -1) {
			perror("server: select");
			exit(1);
		}

		// Is it the original socket? Create a new connection ...
		if (FD_ISSET(sock_fd, &listen_fds)) {
			int client_fd = accept_connection(sock_fd, connections);
			if (client_fd > max_fd) {
				max_fd = client_fd;
			}
			FD_SET(client_fd, &all_fds);
			printf("Accepted connection\n");
		}

		// Next, check the clients.
		// NOTE: We could do some tricks with nready to terminate this loop early.
		for (int index = 0; index < MAX_CONNECTIONS; index++) {
			if (connections[index].sock_fd > -1 
					&& FD_ISSET(connections[index].sock_fd, &listen_fds)) {

				int client_closed = read_from(index, connections);
				if (client_closed > 0) {
					FD_CLR(client_closed, &all_fds);
					printf("Client %d disconnected\n", client_closed);
				} 			}
		}
	}

	return 1;
}
