//--------------------------------------------------------------
// Author:     Zachary Langley
// Date:       11/18/2016
// Instructor: Thompson
// Assignment: Client / Server Ticket Application
// Purpose:    Create a Ticket Database Server that the client
//             can buy and return valid tickets
//--------------------------------------------------------------

//Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/un.h>

//Definitions
#define MIN 10000
#define MAX 99999            // Maximum range for Ticket
#define BUY_CMD "BUY"        // Purchase Ticket commmand
#define RTN_CMD "RETURN"     // Return Ticket command
#define CNCL_CMD "CANCEL"    // Cancel Connection to Server
#define TKT_MAX 25           // Maximum ticekts Generated

// for a strange reason, the compiler did not recognize boolean type
typedef enum { FALSE = 0, TRUE = 1 } bool;

struct Ticket{               // Struct that holds ticket information for future scalability
 int ticket_ID;              // Randomly assigned ticket_ID Number
 bool available;             // TRUE - Available | FALSE - Sold
};

//Function Prototypes
void error( const char *msg );                                   // Error Message
int search( struct Ticket *tkt_arr );                            // Search for available tickets
void updateSold( int tkt_num, struct Ticket *tkt_arr );          // Ticket Sold
void updateReturn( int tkt_num, struct Ticket *tkt_arr );        // Ticket False
bool valid(int tkt_num, struct Ticket *tkt_arr);                 // Check Ticket Number is Valid
bool isAvailable(int tkt_num, struct Ticket *tkt_arr);           // Check Ticket Number is Available
void printtickets(struct Ticket *tkt_arr);

int main(int argc, char *argv[]) {
 //Declarations
 int sockfd, newsockfd, portno, n;
 int newsockfd2; //second connection
 socklen_t clilen;
 char buffer[256];
 int temp1 = 0;
 int temp2 = 0;
 char temp_arr[25];
 fd_set fds;

 struct sockaddr_in serv_addr, cli_addr;
 struct Ticket ticket_arr[TKT_MAX];

 //Iterators
 int i = 0;
 int j = 0;

 srand(time(NULL));                             //initialize random seed
 for( i = 0; i < TKT_MAX; i++){
  ticket_arr[i].ticket_ID = rand() % MAX + MIN; // Assigns ticket numbers ranging from MIN to MAX
  ticket_arr[i].available = TRUE;               // Assigns ticket status to TRUE
 }

 if (argc < 2) {
  fprintf(stderr,"ERROR: No Port\n");
  exit(1);
 }
 sockfd = socket(AF_INET, SOCK_STREAM, 0);
 if (sockfd < 0)
  error("ERROR opening socket");

 //print available tickets
 printtickets(ticket_arr);
 bzero((char *) &serv_addr, sizeof(serv_addr));
 portno = atoi(argv[1]);
 serv_addr.sin_family = AF_INET;
 serv_addr.sin_addr.s_addr = INADDR_ANY;
 serv_addr.sin_port = htons(portno);

 if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
  error("ERROR on binding");

 listen(sockfd,5);
 clilen = sizeof(cli_addr);

 newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
 if (newsockfd < 0) error("ERROR on accept");
 else printf("Client # 1 Connected.\n");

 newsockfd2 = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
 if (newsockfd < 0) error("ERROR on accept");
 else printf("Client # 2 Connected.\n");

while(1){
 FD_ZERO(&fds);
 FD_SET(newsockfd,&fds);
 FD_SET(newsockfd2,&fds);

if( FD_ISSET(newsockfd, &fds)){

  //Read in value to buffer
  bzero(buffer,256);
  n = read(newsockfd,buffer,255);

  //Conditionals
  if( strcmp(buffer, BUY_CMD) == 0 ){
   printf("[CLIENT 1]: %s\n", buffer);
   temp1 = search(ticket_arr);       //assigns returned number as temp var
   if(isAvailable(temp1, ticket_arr) == TRUE){
    printf("[SERVER X]: Client 1 BUY %d\n", temp1);
    updateSold(temp1, ticket_arr);   //updates ticket number to FALSE
    sprintf(buffer, "%d", temp1);
    n = write(newsockfd, buffer, 255);
    if (n < 0) error("ERROR reading from socket");
   }else{
    printf("[SERVER X]: Database full\n");
    strcpy(buffer, "Database full");
    n = write(newsockfd, buffer, 255);
   }//else
  }//else

  else if( strncmp(buffer, RTN_CMD, 6) == 0 ){
   printf("[CLIENT 1]: %s\n", buffer);
   sprintf(temp_arr, "%s", buffer+7);
   temp2 = atoi(temp_arr);
   if(valid(temp2, ticket_arr) == TRUE && isAvailable(temp2, ticket_arr) == FALSE){
    printf("[SERVER X]: Client 1 RETURN %d\n", temp2);
    updateReturn(temp2, ticket_arr);
    strcpy(buffer, "RETURN ");
    strcat(buffer, temp_arr);
    n = write(newsockfd, buffer, 255);
    if (n < 0) error("ERROR reading from socket");
   }else{
     printf("[SERVER X]: Client 1 RETURN %d Failed\n", temp2);
     strcpy(buffer, "RETURN ");
     strcat(buffer, temp_arr);
     strcat(buffer, " Failed");
     n = write(newsockfd, buffer, 255);
     if (n < 0) error("ERROR reading from socket");
   }//else
  }//if
}//if 1st client
 if( FD_ISSET(newsockfd2, &fds)){
   //Read in value to buffer
   bzero(buffer,256);
   n = read(newsockfd2,buffer,255);

   //Conditionals
   if( strcmp(buffer, BUY_CMD) == 0 ){
    printf("[CLIENT 2]: %s\n", buffer);
    temp1 = search(ticket_arr);       //assigns returned number as temp var
    if(isAvailable(temp1, ticket_arr) == TRUE){
     printf("[SERVER X]: Client 2 BUY %d\n", temp1);
     updateSold(temp1, ticket_arr);   //updates ticket number to FALSE
     sprintf(buffer, "%d", temp1);
     n = write(newsockfd2, buffer, 255);
     if (n < 0) error("ERROR reading from socket");
    }else{
     printf("[SERVER X]: Database full\n");
     strcpy(buffer, "Database full");
     n = write(newsockfd2, buffer, 255);
    }//else
   }//else

   else if( strncmp(buffer, RTN_CMD, 6) == 0 ){
    printf("[CLIENT 2]: %s\n", buffer);
    sprintf(temp_arr, "%s", buffer+7);
    temp2 = atoi(temp_arr);
    if(valid(temp2, ticket_arr) == TRUE && isAvailable(temp2, ticket_arr) == FALSE){
     printf("[SERVER X]: Client 2 RETURN %d\n", temp2);
     updateReturn(temp2, ticket_arr);
     strcpy(buffer, "RETURN ");
     strcat(buffer, temp_arr);
     n = write(newsockfd2, buffer, 255);
     if (n < 0) error("ERROR reading from socket");
    }else{
      printf("[SERVER X]: Client 2 RETURN %d Failed\n", temp2);
      strcpy(buffer, "RETURN ");
      strcat(buffer, temp_arr);
      strcat(buffer, " Failed");
      n = write(newsockfd2, buffer, 255);
      if (n < 0) error("ERROR reading from socket");
    }//else
   }//if
  else break; //closes server when completed
 }//if 2nd client is connected
}//while
 close(newsockfd);
 close(newsockfd2);
 close(sockfd);
 printtickets(ticket_arr);
 return 0;
}

//Prototype Definitions
void error(const char *msg) { // Error Message
 perror(msg);
 exit(1);
} // function

int search(struct Ticket *tkt_arr){
 int i = 0;
 for( i = 0; i < TKT_MAX; i++)
  if(tkt_arr[i].available == TRUE) { return tkt_arr[i].ticket_ID; }
 return -1; //no tickets are available
} // function

void updateSold( int tkt_num, struct Ticket *tkt_arr){ // Update SOLD Ticket
 int i = 0;
 for( i = 0; i < TKT_MAX; i++ )
  if( tkt_arr[i].ticket_ID == tkt_num ){ tkt_arr[i].available = FALSE; break; } //ticket was sold
} // function

void updateReturn( int tkt_num, struct Ticket *tkt_arr){ // Update RETURN Ticket
 int i = 0;
 for( i = 0; i < TKT_MAX; i++ )
  if( tkt_arr[i].ticket_ID == tkt_num ){ tkt_arr[i].available = TRUE; break; } //ticket was sold
} // function

bool valid(int tkt_num, struct Ticket *tkt_arr){ // Check if Ticket number is Valid Is
 int i = 0;
 for( i = 0; i < TKT_MAX; i++ )
  if(tkt_arr[i].ticket_ID == tkt_num ){ return TRUE; }
 return FALSE;
} //function

bool isAvailable(int tkt_num, struct Ticket *tkt_arr){
 int i = 0;
 for( i = 0; i < TKT_MAX; i++ )
  if( tkt_arr[i].ticket_ID == tkt_num && tkt_arr[i].available == TRUE ){ return TRUE; }
 return FALSE;
} // function

void printtickets(struct Ticket *tkt_arr){
 int i = 0;
 printf("[SERVER]: Database Table:\n");
 printf("-------------------------------------\n");
 for (i = 0; i < TKT_MAX; i++){
  printf("[Tkt#\t%d]\t%d\t:\t", i+1, tkt_arr[i].ticket_ID);
  if( tkt_arr[i].available == TRUE ) { printf("AVAIL\n"); }
  else if( tkt_arr[i].available == FALSE ) { printf("SOLD\n"); }
 } // for
 printf("-------------------------------------\n");
} // function
