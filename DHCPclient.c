#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <time.h>
#include <netinet/in.h>

#define SERVER "192.168.56.107" //IP address of server

#define BUFLEN 512  //Max length of buffer

void die(char *s)
{
    perror(s);
    exit(1);
}

int main(int argc, char *argv[])
{
    struct sockaddr_in si_other;
    int s, i, slen = sizeof(si_other), recv_len, portno;
    int transID, lifeTime;
    time_t t;
    char buf[BUFLEN], message[BUFLEN];
    char yiaddr[BUFLEN] = "0.0.0.0";

    system("clear");

    printf("...This is DHCP client...\n\n");
    if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

	memset((char *) &si_other, 0, sizeof(si_other));
    portno = atoi(argv[1]); //The port on which to listen for incoming data
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(portno);

    if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

 	printf("\nBegin DHCP 4-Handshake - Discover client arrives\n");

    //printf("yiaddr: %s\n", yiaddr);
    srand((unsigned) time(&t)); //Intialize random number generator
    transID = rand() % 900 + 100; //Generate a random 3-digit number for Transaction ID
    printf("Transaction ID: %d\n", transID);

    //Send to the server, print error if it doesn't work

    if(sendto(s, &yiaddr, sizeof(yiaddr), 0, (struct sockaddr *) &si_other, slen) == -1)
        die("sendto()");

    if(sendto(s, &transID, sizeof(transID), 0, (struct sockaddr *) &si_other, slen) == -1)
        die("sendto()");

    //Begin sequence for DHCP offer
    //Receive from server, print error if it doesn't work
    if((recv_len = recvfrom(s, &yiaddr, sizeof(yiaddr), 0, (struct sockaddr *) &si_other, &slen)) == -1)
            die("recvfrom()");

    if((recv_len = recvfrom(s, &transID, sizeof(transID), 0, (struct sockaddr *) &si_other, &slen)) == -1)
            die("recvfrom()");

    if((recv_len = recvfrom(s, &lifeTime, sizeof(lifeTime), 0, (struct sockaddr *) &si_other, &slen)) == -1)
            die("recvfrom()");

    printf("\n-From DHCP server-\n");
    printf("Assigned yiaddr is: %s", yiaddr); 
    printf("For Transaction ID: %d\n", transID);
    printf("Assigned Lifetime: %d secs\n\n", lifeTime);

    //Begin sequence for DHCP request
    printf("Confirm yiaddr is: %s", yiaddr);
    printf("For Transaction ID: %d\n", ++transID);
    printf("With Lifetime of: %d\n\n", lifeTime);

    //Send to DHCP server
    if(sendto(s, &yiaddr, sizeof(yiaddr), 0, (struct sockaddr *) &si_other, slen) == -1)
        die("sendto()");

    if(sendto(s, &transID, sizeof(transID), 0, (struct sockaddr *) &si_other, slen) == -1)
        die("sendto()");

    if(sendto(s, &lifeTime, sizeof(lifeTime), 0, (struct sockaddr *) &si_other, slen) == -1)
        die("sendto()");

    if((recv_len = recvfrom(s, &yiaddr, sizeof(yiaddr), 0, (struct sockaddr *) &si_other, &slen)) == -1)
        die("recvfrom()");

    if((recv_len = recvfrom(s, &transID, sizeof(transID), 0, (struct sockaddr *) &si_other, &slen)) == -1)
        die("recvfrom()");

    if((recv_len = recvfrom(s, &lifeTime, sizeof(lifeTime), 0, (struct sockaddr *) &si_other, &slen)) == -1)
   	    die("recvfrom()");

    printf("-ACK From DHCP server-\n");
    printf("yiaddr is: %s", yiaddr); 
    printf("Transaction ID: %d\n", transID);
    printf("Lifetime: %d secs\n\n", lifeTime);

    close(s);
    return 0;
}

