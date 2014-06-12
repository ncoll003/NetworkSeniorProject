//Tyler Beveridge
//Nicholas Collier
//Christopher Jaress

// FORK X THREADS TO COMMUNICATE WITH THE CARDREADERS. 

// SET UP A SERVER WAITING FOR THE ANDRIOD APP
// CHECK IF IN DATABASE

// IF ACCEPTED SEND SEQUENCE MESSAGE TO CORRESPONDING DOOR

// WAIT FOR SEQUENCE FROM ANDRIOD APP

// SEND OPEN TO DOOR IF ACCEPTED

#include <stdio.h>		/*for printf() and fprintf()*/
#include <sys/socket.h>		/*for socket(), bind() and connect()*/
#include <arpa/inet.h>		/*for sockaddr_in() and inet_ntoa()*/
#include <stdlib.h>		/*for atoi()*/
#include <string.h>		/*for memset()*/
#include <unistd.h>		/*for close()*/
#include <pthread.h>
#include <time.h>
#include <vector>
#include <fstream>
#include <iostream>
#define RCVBUFFERSIZE 32

using namespace std;

struct accessInfo
{
	int ID;
	vector<int> doorAccess;
};

vector<accessInfo> initializeInfo(char * filename)
{
	vector<accessInfo> newAccessPeople;
	accessInfo newAccount;
	ifstream file(filename);
	int newID;
	string s;
	vector<int> newDoors;
	int doorNumber;
	while(file >> newID)
	{
		newAccount.ID = newID;
		if(file.eof())
		{
			cout << "Configuration File Invalid.\n";
			exit(1);
		}	

		while(file >> s && s.compare(".") != 0)
		{
			doorNumber = atoi(s.c_str());
			newDoors.push_back(doorNumber);
		}
		newAccount.doorAccess = newDoors;
		int size = newDoors.size();
		for(int i = 0; i < size; ++i)
			newDoors.pop_back();
	
		newAccessPeople.push_back(newAccount);
	}
	return newAccessPeople;
}	


vector<accessInfo> newList = initializeInfo("users.txt");

vector <int> connections;
int currentConnection = 0;


const int numClients = 4;
int numRecv = 0;
int numProc = 0;
int numDoors = 3;

bool sendSeq[numClients - 1];
bool accepted[numClients - 1];
bool rejected[numClients - 1];


//Keys used for RSA encryption
unsigned long long d = 139967;
unsigned long long n = 45293789;

char seq[10];
char print_seq[64];

char acceptMsg[32];
char rejectMsg[32];



pthread_mutex_t count_mutex;

void * door_process( void * ptr);

int generate_auth_msg(char * msg);

void decrypt( unsigned long & num );

void parse ( char * msg, int & type, unsigned long & num);

void * phone_process( void * ptr);

bool check_valid_id(char * id);

void generate_seq();

int main(int argc,char *argv[])
{


//Initialize List of Users
cout << "User List" << endl
<<"----------------------------------------------------" << endl;
	for(int i = 0; i< newList.size(); ++i)
	{
		cout << "--------------------------------" << endl
			<< "ID: " << newList[i].ID << endl <<
		"Doors: ";
		for(int j = 0; j < newList[i].doorAccess.size(); ++j)
		{
			cout << newList[i].doorAccess[j] << " ";
		}
		cout << endl;
	}


	for (int i = 0; i < numClients - 1; ++i)
	{
		sendSeq[i] = false;
		accepted[i] = false;
		rejected[i] = false;
	}

  pthread_t thread1, thread2;
  int  iret1, iret2, iret3, iret4;

  int clientSocket = -1;
  struct sockaddr_in servAddr;
int serverSocket;
  int port;

  sprintf(acceptMsg,"GREEN (Door is now unlocked)");
  sprintf(rejectMsg,"RED (Access Denied)");

	srand(time(NULL));		

  if (argc != 2)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }
   port=atoi(argv[1]);
   serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (serverSocket<0)
   {
      fprintf(stderr, "%s: error: cannot create socket\n", argv[0]);
      return 0;
   }

   servAddr.sin_family = AF_INET;
   servAddr.sin_addr.s_addr = INADDR_ANY;
   servAddr.sin_port = htons(port);
   if(bind(serverSocket, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
   {
      fprintf(stderr, "%s: error: cannot bind socket to port %d\n", argv[0], port);
	exit(1);
   }

    if (listen(serverSocket, 5) < 0)
	  {
	      fprintf(stderr, "%s: error: cannot listen on port\n", argv[0]);
        return 0;
    }
   
    printf("%s: ready and listening\n", argv[0]);

	
socklen_t clntLen = sizeof(servAddr);
    while (1)
    {

       if( (clientSocket = accept(serverSocket, (struct sockaddr *)&servAddr, &clntLen)) < 0)
	{
		perror("accept() failed: \n");
		exit(0);
	}

	if(currentConnection <3)
	{
	currentConnection++;
	connections.push_back(clientSocket);
		
                 iret1 = pthread_create(&thread1, NULL, door_process,NULL);

	}
	else
	{
	currentConnection++;
	connections.push_back(clientSocket);
//Here
             iret2 =  pthread_create(&thread2, NULL, phone_process,NULL);
        }
     }
  /* Wait till threads are complete before main continues. Unless we  */
  /* wait we run the risk of executing an exit which will terminate   */
  /* the process and all threads before the threads have completed.   */


     pthread_join( thread1, NULL);
     pthread_join( thread2, NULL); 
	return 0;
}

int generate_auth_msg() {
	//random number used to determine that actually connected to phone app
	int x = rand() % 100;
	return x;
}


void decrypt( unsigned long & num ){

}

void generate_seq() {

	sprintf(seq, "");
	sprintf(print_seq, "");
	int x;
	for(int i = 0; i < 5; ++i){
		x = rand() % 2;
		if(x == 0) {
			sprintf(print_seq, "%s red", print_seq);
			sprintf(seq, "%s0", seq);
		}
		else if(x == 1) {
			sprintf(print_seq, "%s green", print_seq);
			sprintf(seq, "%s1", seq);
		}
	}
}

//Fast exponentiation function for RSA encryption

long long fast_exp(long long x, long long y, long long N)
{
	if(y == 0)
		return 1;
	unsigned long z = 1;
	while(y> 1)
	{
		if(y%2)
		{
			z = (x*z)%N;
			z = (x*x)%N;
			y = (y-1)/2;
		}
		else
		{
			x = (x*x) %N;
			y = y/2;
		}
	}
	
	return z *x %N;
}


void parse ( char * msg, int & type, unsigned long & num) {
	// need to know a message protocol, now assuming that fields are 
	// seperated by a ;

	printf("within parser\n");
	char temp[1024];
  int pos = 0;
	
  for(; msg[pos] != '.'; ++pos){
     temp[pos] = msg[pos];
  }
  temp[pos] = '\0';
  type = atoi(temp);
  pos++;
  printf("type : %d\n", type);

  for(int i = 0; msg[pos] != '\0'; ++i){
		temp[i] = msg[pos]; 
		temp[i+1] = '\0';
    ++pos;
  }  

	num = atoll(temp);
	cout << "Number : " << num << endl;
}

void parse2(unsigned long & num, long & seconds)
{
	unsigned long id = num % 10000;
	unsigned long remaining = num/10000;
	seconds = remaining;
	num = id;
}

bool check_valid_id(unsigned long id)
{	
	for(int i = 0; i < newList.size(); ++i)
	{
		if(newList[i].ID = id)
			return true;
	}
	return false;
}

    
void * door_process( void * ptr){
  int i = numRecv - 1;

  int currentSocket = connections[currentConnection - 1];
  
printf("Connected to door %d\n", currentConnection);

  while(1) {
		if (sendSeq[i]) {
			if((send(currentSocket,print_seq,sizeof print_seq,0))!=sizeof (print_seq))
			{
				perror("send() failed");
				exit(1);
			}
			else
				sendSeq[i] = false;
		}
		else if (accepted[i]) {
			if((send(currentSocket,acceptMsg,sizeof acceptMsg,0))!=sizeof (acceptMsg))
			{
				perror("send() failed");
				exit(1);
			}
			else 
				accepted[i] = false;
		}
		else if (rejected[i]) {
			if((currentSocket,rejectMsg,sizeof rejectMsg,0) != sizeof (rejectMsg))
			{
				perror("send() failed");
				exit(1);
			}
			else 
				rejected[i] = false;
		
		}

	}
  
	close(currentSocket);
  pthread_exit(0);   
}

bool checkDoor(int doorNum, int checkID)
{
	for(int i =0; i < newList.size(); ++i)
	{
		if(newList[i].ID == checkID)
			for(int j = 0; j < newList[i].doorAccess.size(); ++j)
			{
				if(newList[i].doorAccess[j] == doorNum)
					return true;
			}
	}
	return false;
}

void * phone_process( void * ptr){


	int currentSocket = connections[currentConnection-1];	
	const int BUFFERSIZE = 1024;
	char buffer[BUFFERSIZE];
	unsigned long msg;

	int msg_type = -1;

	int door_number = -1;

	bool is_valid = false;
	bool authenticated = false;
	
	printf("Connection to new phone accepted\n");

	
	
	int x = generate_auth_msg();

	char auth_msg[BUFFERSIZE];
	sprintf(buffer, "127");
	printf("Authentication number: %s\n",buffer);
	int recvMsgSize;

	printf(buffer);
	printf("\n");
	

	time_t newTime;
	long currentTime;
	long timeConnect;
	int tempID = 0;
  
  while(1) {
		if((recvMsgSize = recv(currentSocket, buffer, BUFFERSIZE,0)) <  0){
			perror("receive() failed\n");
			exit(1);
		}

	printf("Received: %s\n", buffer);
    parse(buffer, msg_type, msg);
		switch(msg_type) {
			case 1:
				currentTime = time(&newTime);
				currentTime -= 7153;
				currentTime %= 1000;
				cout << "Time Check = " << currentTime << endl;
				msg = fast_exp(msg, d, n);
				parse2(msg, timeConnect);
				printf("Checking ID...\n");

				if(currentTime - timeConnect <=1 || timeConnect - currentTime <=1)
				{
				cout << "ID accepted!\n";
					authenticated = true;
					tempID = msg;
					is_valid = check_valid_id(tempID);
				}


				else if(!authenticated || !is_valid) {
					printf("Not Authenticated. Closing Connection...\n");
					numRecv--;
					close(currentSocket);
  				
 					pthread_exit(0);  
				}
				break;
	
			case 2:
				printf("Door Number Received.  Checking Access Permission...\n");
				if(!authenticated) {
					printf("No Access to Door. Closing connection...\n");
					numRecv--;
					close(currentSocket);
  				
 					pthread_exit(0);  
				}
				else {
					door_number = msg;
					bool doorAccess = checkDoor(door_number, tempID);
					if((door_number < 0 || door_number >= numDoors)) {
						printf("door number is out of range, closing connection\n");
						numRecv--;
						close(currentSocket);
  					
 						pthread_exit(0); 
					}
					else if(is_valid && doorAccess) {
						printf("ID Accepted. Sending message to door %d...\n", door_number);
						generate_seq();
						sendSeq[door_number] = true;
					}
					else{
						printf("ID does not have access to door. Closing connection...\n");
						rejected[door_number] = true;
					}
				} 
				break;
			case 3:
				printf("Sending sequence number to door...\n");
				if(!authenticated) {
					printf("ID is not authenticated! Closing connection...\n");
					numRecv--;
					close(currentSocket);
  				
 					pthread_exit(0);  
				}
				else {
					if(door_number < 0 || door_number >= numDoors) {
						printf("door number is out of range, closing connection\n");
						numRecv--;
						close(currentSocket);
  					
 						pthread_exit(0); 
					}
					if(msg == atoi(seq)) {
						accepted[door_number] = true;
						printf("Access Granted! Closing connection...\n");
						numRecv--;
						close(currentSocket);
  					
 						pthread_exit(0); 
					}
					else {
						rejected[door_number] = true;
						printf("Access Denied. Closing connection...\n");
						numRecv--;
						close(currentSocket);
  					
 						pthread_exit(0); 
					}
				}
				break;
			default:
				printf("Unrecognized Message.  Closing connection...\n");
				numRecv--;
				close(currentSocket);
  			
 				pthread_exit(0);   
				break;
		}
	}
	numRecv--;
	close(currentSocket);
 	pthread_exit(0);   
}
