/*PROJECT: DNS resolver RFC 1035 (resolver,message)
 *AUTHOR : AKSHAY RATHI,VIVEK SINGH,RAJANARASIMHA REDDY, SUBHAN SHAIK, RAVI
 *VERSION: 1.0
 *DATE   : 29'MAY 2022
 */
#include <iostream>
#include <stdlib.h>
#include<arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
/*DESCRIPTION: By using socket client and server communicating with client and the foreign server for IP address.
 * INPUT     : Hostname. example: google.com
 * OUTPUT    : Sucessfull
 */
using namespace std;

//List of DNS server registered on the system
char dns_servers[10][100];
int dns_server_count = 0;

//Type of DNS resources records
#define T_A 1
#define T_NS 2
#define T_CNAME 5
#define T_SOA 6
#define T_PTR 12
#define T_MX 15

//functions
void ngethostbyname(unsigned char*, int);
void ChangetoDnsNameFormat (unsigned char*,unsigned char*);
unsigned char* ReadName (unsigned char*,unsigned char*,int*);
void get_dns_servers();

//DNS header structure
struct DNS_HEADER
{
    unsigned short id;

    unsigned char rd :1;
    unsigned char tc :1;
    unsigned char aa :1;
    unsigned char opcode :4;
    unsigned char qr :1;

    unsigned char rcode :4;
    unsigned char cd :1;
    unsigned char ad :1;
    unsigned char z :1;
    unsigned char ra :1;

    unsigned short q_count;
    unsigned short ans_count;
    unsigned short auth_count;
    unsigned short add_count;
};

struct QUESTION
{
    unsigned short qtype;
    unsigned short qclass;
};

#pragma pack(push, 1)
struct R_DATA
{
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
#pragma pack(pop)

struct RES_RECORD
{
    unsigned char *name;
    struct R_DATA *resource;
    unsigned char *rdata;
};

typedef struct
{
    unsigned char *name;
    struct QUESTION *ques;
} QUERY;

void bzero(void *a, size_t n) {
    memset(a, 0, n);
}

void bcopy(const void *src, void *dest, size_t n) {
    memmove(dest, src, n);
}
  char *response;
  int client_fd1;

struct sockaddr_in* init_sockaddr_in(uint16_t port_number) {
    struct sockaddr_in *socket_address = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    memset(socket_address, 0, sizeof(*socket_address));
    socket_address -> sin_family = AF_INET;
    socket_address -> sin_addr.s_addr = htonl(INADDR_ANY);
    socket_address -> sin_port = htons(port_number);
    return socket_address;
}

char* process_operation(char *input) {
    size_t n = strlen(input) * sizeof(char);
    char *output = (char*)malloc(n);
    memcpy(output, input, n);
    return output;
}

int main( int argc, char *argv[] ) {

    const uint16_t port_number = 5001;
    int fd_size=256;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in *server_sockaddr = init_sockaddr_in(port_number);
    struct sockaddr_in *client_sockaddr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    socklen_t server_socklen = sizeof(*server_sockaddr);
    socklen_t client_socklen = sizeof(*client_sockaddr);


    if (bind(server_fd, (struct sockaddr *) server_sockaddr, server_socklen) < 0)
    {
        cout<<"\nError! Bind has failed";
        exit(0);
    }
    if (listen(server_fd, 3) < 0)
    {
        cout<<"\nError! Can't listen";
        exit(0);
    }


    const size_t buffer_len = 256;
    unsigned char *buffer = (unsigned char*)malloc(buffer_len * sizeof(char));
    char *response1 = NULL;
    time_t last_operation;
    __pid_t pid = -1;
    int count=0;
    while (1)
    {
        int client_fd = accept(server_fd, (struct sockaddr *) &client_sockaddr, &client_socklen);
        client_fd1=client_fd;
        pid = fork();

        if (pid == 0) {
            close(server_fd);

        if (client_fd == -1) {
            exit(0);
             }

            cout<<"\nConnection with "<<client_fd<<" has been established and delegated to the process "<<getpid()<<"\nWaiting for a query...";

            last_operation = clock();


            while (1) {
                recv(client_fd, buffer, buffer_len,0);

                if (sizeof(buffer) == 0) {
                    clock_t d = clock() - last_operation;
                    double dif = 1.0 * d / CLOCKS_PER_SEC;

                    if (dif > 5.0) {
                        cout<<"\nProcess : "<<getpid();
                        close(client_fd);
                        cout<<"\nConnection timed out after %.3lf seconds. "<<dif;
                        cout<<"\nClosing session with "<<client_fd<<" Bye!";
                        break;
                    }

                    continue;
                }

                cout<<"\nProcess : "<<getpid();
                cout<<"\nReceived "<<buffer<<". Processing... ";
		free(response1);
		if(count==0)
		{
		count=count+1;
		get_dns_servers();
		ngethostbyname(buffer , T_A);
		}
		else
		{
			exit(0);
		}
		bzero(buffer, buffer_len * sizeof(char));
                last_operation = clock();
            }
            exit(0);
        }
        else {
            close(client_fd);
        }
    }
}

/*
 * Description:perform a DNS query by sending a packet
 * Input: Hostname query
 * Ouput: Authoritive answers
 */
void ngethostbyname(unsigned char *host , int query_type)
{
    unsigned char buf[65536],*qname,*reader;
    int i , j , stop , s;

    struct sockaddr_in a;

    struct RES_RECORD answers[20],auth[20],addit[20];
    struct sockaddr_in dest;

    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;



    s = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP);

    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(dns_servers[0]);

    dns = (struct DNS_HEADER *)&buf;

    dns->id = (unsigned short) htons(getpid());
    dns->qr = 0;
    dns->opcode = 0;
    dns->aa = 0;
    dns->tc = 0;
    dns->rd = 1;
    dns->ra = 0;
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1);
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;

    qname =(unsigned char*)&buf[sizeof(struct DNS_HEADER)];

    ChangetoDnsNameFormat(qname , host);
    qinfo =(struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)];
    qinfo->qtype = htons( query_type );
    qinfo->qclass = htons(1);

    if( sendto(s,(char*)buf,sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION),0,(struct sockaddr*)&dest,sizeof(dest)) < 0)
    {
        perror("sendto failed");
    }

    i = sizeof dest;
    if(recvfrom (s,(char*)buf , 65536 , 0 , (struct sockaddr*)&dest , (socklen_t*)&i ) < 0)
    {
        perror("recvfrom failed");
    }
    dns = (struct DNS_HEADER*) buf;

    reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION)];
    stop=0;

    for(i=0;i<ntohs(dns->ans_count);i++)
    {
        answers[i].name=ReadName(reader,buf,&stop);
        reader = reader + stop;

        answers[i].resource = (struct R_DATA*)(reader);
        reader = reader + sizeof(struct R_DATA);

        if(ntohs(answers[i].resource->type) == 1) //if its an ipv4 address
        {
            answers[i].rdata = (unsigned char*)malloc(ntohs(answers[i].resource->data_len));

            for(j=0 ; j<ntohs(answers[i].resource->data_len) ; j++)
            {
                answers[i].rdata[j]=reader[j];
            }

            answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';

            reader = reader + ntohs(answers[i].resource->data_len);
        }

        else
        {
            answers[i].rdata = ReadName(reader,buf,&stop);
            reader = reader + stop;
        }
    }

    for(i=0;i<ntohs(dns->auth_count);i++)
    {
        auth[i].name=ReadName(reader,buf,&stop);
        reader+=stop;

        auth[i].resource=(struct R_DATA*)(reader);
        reader+=sizeof(struct R_DATA);

        auth[i].rdata=ReadName(reader,buf,&stop);
        reader+=stop;
    }

    for(i=0;i<ntohs(dns->add_count);i++)
    {
        addit[i].name=ReadName(reader,buf,&stop);
        reader+=stop;

        addit[i].resource=(struct R_DATA*)(reader);
        reader+=sizeof(struct R_DATA);

        if(ntohs(addit[i].resource->type)==1)
        {
            addit[i].rdata = (unsigned char*)malloc(ntohs(addit[i].resource->data_len));
            for(j=0;j<ntohs(addit[i].resource->data_len);j++)
                addit[i].rdata[j]=reader[j];

            addit[i].rdata[ntohs(addit[i].resource->data_len)]='\0';
            reader+=ntohs(addit[i].resource->data_len);
        }
        else
        {
            addit[i].rdata=ReadName(reader,buf,&stop);
            reader+=stop;
            free(addit[i].rdata);
        }
    }
    cout<<"\nAnswer Records : "<<ntohs(dns->ans_count);
    for(i=0 ; i < ntohs(dns->ans_count) ; i++)
    {
        cout<<"\nName :  "<<answers[i].name;
        free(answers[i].name);

        if( ntohs(answers[i].resource->type) == T_A) //IPv4 address
        {
            long *p;
            p=(long*)answers[i].rdata;
            a.sin_addr.s_addr=(*p); //working without ntohl
	        response=inet_ntoa(a.sin_addr);
            cout<<" has IPv4 address : "<<inet_ntoa(a.sin_addr);
	        send(client_fd1, response, strlen(response), 0);
            cout<<"\nType: "<<answers[i].resource->type;
            cout<<"\nClass: "<<answers[i].resource->_class;
            cout<<"\nTTL: "<<answers[i].resource->ttl;
            cout<<"\nData length: "<<answers[i].resource->data_len;
            sleep(1);
            free(p);
        }
        if(ntohs(answers[i].resource->type)==5)
        {
            cout<<"has alias name : "<<answers[i].rdata;
            free(answers[i].rdata);
            free(answers[i].resource);
        }

        printf("\n");
    }
    for( i=0 ; i < ntohs(dns->auth_count) ; i++)
    {
        cout<<"Name :  "<<auth[i].name;
        if(ntohs(auth[i].resource->type)==2)
        {
            cout<<" has nameserver : "<<auth[i].rdata;
        }
        cout<<"\n";
    }
    for(i=0; i < ntohs(dns->add_count) ; i++)
    {
        cout<<"\nName: "<<addit[i].name<<" ";
        if(ntohs(addit[i].resource->type)==1)
        {
            long *p;
            p=(long*)addit[i].rdata;
            a.sin_addr.s_addr=(*p);
            cout<<"has IPv4 address : "<<inet_ntoa(a.sin_addr);
        }
        cout<<"\n";
    }

    return;
}
/*
 * Description: this function is reading the name /TTL / type/ class and IP
 * input: no
 * output:successfull
 */

u_char* ReadName(unsigned char* reader,unsigned char* buffer,int* count)
{
    unsigned char *name;
    unsigned int p=0,jumped=0,offset;
    int i , j;

    *count = 1;
    name = (unsigned char*)malloc(256);

    name[0]='\0';

    while(*reader!=0)
    {
        if(*reader>=192)
        {
            offset = (*reader)*256 + *(reader+1) - 49152;
            reader = buffer + offset - 1;
            jumped = 1;
        }
        else
        {
            name[p++]=*reader;
        }

        reader = reader+1;

        if(jumped==0)
        {
            *count = *count + 1;
        }
    }

    name[p]='\0';
    if(jumped==1)
    {
        *count = *count + 1;
    }

    for(i=0;i<(int)strlen((const char*)name);i++)
    {
        p=name[i];
        for(j=0;j<(int)p;j++)
        {
            name[i]=name[i+1];
            i=i+1;
        }
        name[i]='.';
    }
    name[i-1]='\0';
    return name;
    free(name);
    free(count);
    free(buffer);
    free(reader);
}
/*
 * Description:Get the DNS servers from /etc/resolv.conf file on linux
 * Input: 0
 * Output: successfull
 */

void get_dns_servers()
{
    FILE *fp;
    char line[200] , *p;
    if((fp = fopen("/etc/resolv.conf" , "r")) == NULL)
    {
        cout<<"\nFailed opening /etc/resolv.conf file ";
    }

    while(fgets(line , 200 , fp))
    {
        if(line[0] == '#')
        {
            continue;
        }
        if(strncmp(line , "nameserver" , 10) == 0)
        {
            p = strtok(line , " ");
            p = strtok(NULL , " ");


        }
    }

    strcpy(dns_servers[0] , "208.67.222.222");
    strcpy(dns_servers[1] , "208.67.220.220");
}
/*
 *Description: This will covert google.com to 6google3com
Input:Hostname
Output:query type name
 */
void ChangetoDnsNameFormat(unsigned char* dns,unsigned char* host)
{
    int lock = 0 , i;
    strcat((char*)host,".");

    for(i = 0 ; i < strlen((char*)host) ; i++)
    {
        if(host[i]=='.')
        {
            *dns++ = i-lock;
            for(;lock<i;lock++)
            {
                *dns++=host[lock];
            }
            lock++;
        }
    }
    *dns++='\0';
}
