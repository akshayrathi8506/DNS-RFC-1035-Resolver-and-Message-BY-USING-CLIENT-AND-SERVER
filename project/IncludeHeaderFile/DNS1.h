/*AUTHOR : AKSHAY RATHI,VIVEK SINGH,RAJANARASIMHA REDDY, SUBHAN SHAIK, RAVI
 *VERSION: 1.0
 *DATE   : 29'MAY 2022
 */
#ifndef DNS_H
#define DNS_H


#include <unistd.h>      // to get getpid()
#include <iostream>      // for input and output
#include <stdlib.h>      // this isthe standard lib.
#include <sys/socket.h>  // for socket use
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>      // for strlen (string manipulation)
#include<arpa/inet.h>    // to use inet_addr,inet_ntoa,ntohs etc
#include <fstream>       //for input output file
#include <vector>        //stl container to store data of .csv file
#include <string>        //used for String types and set of converting functions
#include <sstream>       //part of input output library
//Type of DNS resources records
#define T_A 1      //IPV4 address
class DNS
{
//List of DNS server registered on the system
char HostID[10][100];   //to store the DNS address
int HostID_count = 0;
    public:
//DNS header structure
    struct DNS_HEADER
{
    unsigned short id;         //Identification number
    unsigned char rd :1;       //recursive desired
    unsigned char tc :1;       //truncated message
    unsigned char aa :1;       //authoritive answer
    unsigned char opcode :4;   //purpose of message
    unsigned char qr :1;       //query

    unsigned char rcode :4;    //response code
    unsigned char cd :1;       //checking disabled
    unsigned char ad :1;       //authenticated data
    unsigned char z :1;        //its z! reserved
    unsigned char ra :1;       //recursive available

    unsigned short q_count;     // number of question entries
    unsigned short ans_count;   // number of answer entries
    unsigned short auth_count;  // number of authority entries
    unsigned short add_count;   // number of resource entries
};
//constant sized fields of query structure
struct QUESTION
{
    unsigned short qtype;
    unsigned short qclass;
};
//constant sized field of resource record structure
struct RES_RECORD
{
    unsigned char *name;
    struct R_DATA *resource;
    unsigned char *rdata;
};
int size,size1;
unsigned short value;//instance variable to store answers entries
int ngethostbyname(unsigned char *host , int query_type);   //Perform a DNS query by sending a packet
int get_dns_servers();   //Get the DNS servers from /etc/resolv.conf file on Linux
int ChangetoDnsNameFormat(unsigned char* dns,unsigned char* host);    //This will convert google.com to 6google3com
int IPfromnameserver();   //This is used to communicate with the nameserver and for message send and receive
int Displaydata();
};
#endif //DNS_H
