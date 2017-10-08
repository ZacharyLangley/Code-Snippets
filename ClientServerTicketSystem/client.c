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
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>

//Definitions
#define MIN 10000            // Minimum range for Ticket
#define MAX 99999            // Maximum range for Ticket
#define BUY_CMD "BUY"        // Purchase Ticket commmand
#define RTN_CMD "RETURN"     // Return Ticket command
#define CNCL_CMD "CANCEL"    // Cancel Connection to Server
#define TKT_MAX 25           // Maximum tickets generated

// for a strange reason, the compiler did not recognize boolean type
typedef enum { FALSE = 0, TRUE = 1 } bool;

//Prototypes
void printmenu();                    // prints menu with available commands
void error(const char *msg);         // error message

int main(int argc, char *argv[])
{
 int sockfd, portno, n;
 struct sockaddr_in serv_addr;
 struct hostent *server;

 char buffer[256];
 char inventory[TKT_MAX][256] = { 0 };
 int i = 0, j = 0;
 printmenu();         // prints available commands

 if (argc < 3) {
  fprintf(stderr,"usage %s hostname port\n", argv[0]);
  exit(0);
 }

 portno = atoi(argv[2]);
 sockfd = socket(AF_INET, SOCK_STREAM, 0);

 if (sockfd < 0) error("ERROR: opening socket");
 server = gethostbyname(argv[1]);

 if (server == NULL) {
  fprintf(stderr,"ERROR: No Host\n");
  exit(0);
 }

 bzero((char *) &serv_addr, sizeof(serv_addr));
 serv_addr.sin_family = AF_INET;
 bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
 serv_addr.sin_port = htons(portno);
// for(i = 0;i < 15;i++){
 if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
  error("ERROR connecting");

// 15 BUY COMMAND
 for(i = 0; i < 15; i++){
  //gets input
  bzero(buffer,256);
  strcpy(buffer, BUY_CMD);
  printf("[CLIENT]: %s\n", buffer);
  n = write(sockfd,buffer,strlen(buffer));
  if(n < 0) error("ERROR: Writing to socket");
  bzero(buffer, 256);
  n = read(sockfd, buffer, 255);
  if (n < 0) error("ERROR: Reading from socket");
  for(j=0; j < strlen(buffer); j++)
   inventory[i][j] = buffer[j];
  if(strcmp(inventory[i], "Database full") == 0){
    for(j = 0; j < strlen(buffer); j++)
     inventory[i][j] = 0;
  }//if
  printf("[SERVER]: %s\n", buffer);
 }//for BUY

// RETURN COMMAND
 for(i = 0; i < TKT_MAX-20; i++){
  bzero(buffer,256);
  //build command
  strcpy(buffer, RTN_CMD);
  strcat(buffer, " ");
  strcat(buffer, inventory[i]);

  printf("[CLIENT]: %s\n", buffer);
  n = write(sockfd,buffer,strlen(buffer));
   if(n < 0) error("ERROR: Writing to socket");
  bzero(buffer, 256);
  n = read(sockfd, buffer, 255);
   if (n < 0) error("ERROR: Reading from socket");
  printf("[SERVER]: %s\n", buffer);
 }//for
 strcpy(buffer, "CANCEL");
 n = write(sockfd,buffer,strlen(buffer));
 if(n < 0) error("ERROR: Writing to socket");
 close(sockfd);
 //print tickets when done
 printf("[Client]: Database Table\n");
 printf("-------------------------------\n");
 for(i=0;i<TKT_MAX-5;i++){
  printf("[Tkt#\t%d]:\t%s\n", i+1, inventory[i]);
 }//for
 printf("-------------------------------\n");
 return 0;
}

//Definitions
void error(const char *msg)
{
    perror(msg);
    exit(0);
}//function

void printmenu(){
 printf(" +-------------------------------------------------------------------------------+\n");
 printf(" | Menu:                                                                         |\n");
 printf(" | \"BUY\"               - Purchase an available ticket                            |\n");
 printf(" | \"RETURN [TICKET #]\" - Return Valid Tickets                                    |\n");
 printf(" | \"CANCEL\"            - Exit Ticket Server                                      |\n");
 printf(" +-------------------------------------------------------------------------------+\n");
}//function
