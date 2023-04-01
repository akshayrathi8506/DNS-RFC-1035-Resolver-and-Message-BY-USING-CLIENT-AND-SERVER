/*PROJECT: DNS resolver RFC 1035 (resolver,message)
 *AUTHOR : AKSHAY RATHI,VIVEK SINGH,RAJANARASIMHA REDDY, SUBHAN SHAIK, RAVI
 *VERSION: 1.0
 *DATE   : 29'MAY 2022
 */

#include "../IncludeHeaderFile/DNS.h"         //header file
#include <iostream>      // for input and output
using namespace std;
/*DESCRIPTION: By using socket client and server communicating with nameserver and the foreign server for IP address.
 * INPUT     : Hostname. example: google.com
 * OUTPUT    : Sucessfull
 */

/*
 * Description:perform a DNS query by sending a packet
 * Input: Hostname query
 * Ouput: Authoritive answers 
 * -----------------------------------RESOLVER PART ---------------------------------*/
int DNS::ngethostbyname(unsigned char *host , int query_type)
{
    unsigned char buf[65536],*qname;
    int i , s;
    struct sockaddr_in dest;

    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;

    cout<<"\nResolving "<<host;
    string str(reinterpret_cast< char const* >(host));  //converting unsigned char into a string
    size=str.size();
    //conneting socket with the server by using PORT NO : 53
    s = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP);
    if(s==-1)
        {
          perror("socket");
          exit(1);
        }
    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(HostID[0]);

    dns = (struct DNS_HEADER *)&buf;

    dns->id = (unsigned short) htons(getpid());
    dns->qr = 0;      //This is a query
    dns->opcode = 0;  //this is a standard query
    dns->aa = 0;      //not authoritative
    dns->tc = 0;      //this message is not truncated
    dns->rd = 1;      //recursion desired
    dns->ra = 0;      //recursion not available
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1);  //Only one question
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;
    //point to query portion
    qname =(unsigned char*)&buf[sizeof(struct DNS_HEADER)];

    ChangetoDnsNameFormat(qname , host);  //calling this function to change the format of the query
    qinfo =(struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)];
    qinfo->qtype = htons( query_type );
    qinfo->qclass = htons(1);
    //sending query here
    sendto(s,(char*)buf,sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION),0,(struct sockaddr*)&dest,sizeof(dest));
    i=sizeof(dest);
    //receive the answer
    recvfrom(s,(char*)buf , 65536 , 0 , (struct sockaddr*)&dest , (socklen_t*)&i );
    dns = (struct DNS_HEADER*) buf;
    cout<<"\nThe response contains : ";
    cout<<"\nQuestions. "<<ntohs(dns->q_count);
    cout<<"\nAnswers. "<<ntohs(dns->ans_count);
    cout<<"\nAuthoritative Servers. "<<ntohs(dns->auth_count);
    cout<<"\nAdditional records. "<<ntohs(dns->add_count)<<"\n\n";
    value=(ntohs(dns->ans_count));       // storing the value of authoritive answer entries in values
    close(s);
return 0;
}
/*
 * Description:Get the DNS servers from /etc/resolv.conf file on linux
 * Input: 0
 * Output: successfull
 */
int DNS::get_dns_servers()
{
    FILE *fp;
    char line[200],*p;
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

    strcpy(HostID[0] , "208.67.222.222");
    strcpy(HostID[1] , "208.67.220.220");
    return 0;
}
/*
 *Description: This will covert google.com to 6google3com
Input:Hostname
Output:query type name
 */

int DNS::ChangetoDnsNameFormat(unsigned char* dns,unsigned char* host)
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
    return 0;
}
/* 
 * Description:connecting with the nameserver to take IP as output
 * Input: Hostname
 * Output: Successfull IP address
 */
int DNS::IPfromnameserver()
{
/*---------------------------------CONNECTING WITH THE NAMESERVER--------------------------------*/
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    string message="";
    char buffer[256];  // it will take input from user
    portno = 5001;

    //creating socket to communicat with the nameserver and get file descriptor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd==-1)
        {
          perror("socket");
          exit(1);
        }
    server = gethostbyname("127.0.0.1");  // host ID

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));    //this function erase the data in the n bytes
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);  //this function copies n bytes from src to dest
    serv_addr.sin_port = htons(portno);


    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR while connecting");
        exit(1);
    }
    cout<<"Enter Query for nameserver: ";  //taking input from user again and same as the above one
	bzero(buffer,256);           //this function erase the data in the n bytes
	cin>>buffer;                 //taking input from user
    message=buffer;
    size1=message.size();
    if(size==size1)
    {  
	ofstream file1;                        //using file management system to store the data of each URL
	file1.open("../IncludeHeaderFile/data.csv", ios::out | ios::app);       //opening file to enter the data
    try{
        if(file1.fail())
            throw "err";
            }
    catch(...)
        {
            cout<<"\nFile is failed to open"<<endl;
            exit(1);
        }
	if (file1.is_open()) {
        n = send(sockfd,buffer,sizeof(buffer),0);   //sending query to server and will wait for answer

        if (n < 0){
            perror("socket");
            exit(1);
        }
	cout<<"\nserver replied: \n";  //server will send the IP address of teh query
	for(int i=0;i<value;i++)
	{
        bzero(buffer,256);
        n = recv(sockfd, buffer, 255,0);    //receive the answer and output will be a query

        if (n < 0){
            perror("socket");
            exit(1);
        }
        cout<<"\nAvailable answers are ans["<<i+1<<"]\n";
	    cout<<"IP : "<<buffer<<"\n";         //printing the message here which is IP address
        file1<<message<<","<<buffer<<"\n";        //storing the IP into file with the query
        sleep(1);
	}
    file1.close();      //closing output file
    }
    char y;      //taking input from user to display stored data
    Start:
    cout<<"\nWant to check stored IPs. Pleases select (Y/N)?";
    cin>>y;
    if(y=='y' || y=='Y')  //condition to check wheather we want to check the data or not
    {
        Displaydata();
    }
    else if(y=='n' || y=='N')
    {
	    cout<<"\nExit successfully\n";
    }
    else
    {
        cout<<"\nAlert : Select correct input\n";
        goto Start;
    }
    }
    else
    {
        cout<<"\nError: Please enter correct URL same as previous\n";
    }
    close(sockfd);
    return 0;
}

int DNS::Displaydata()
{
                ifstream file;    //this is used to read from the file
                vector<vector<string>> content;     //stl container to store data of file
                vector<string> row;
                string line1, word;
                file.open("../IncludeHeaderFile/data.csv", ios::in);    //opening the file
                    try{
                        if(file.fail())
                            throw "err";
                            }
                    catch(...)
                        {
                            cout<<"\nFile is failed to open"<<endl;
                            exit(1);
                        }
                while (getline(file, line1)) {
                    row.clear();
                    stringstream str(line1);
                    while (getline(str, word, ','))
                        row.push_back(word);
                    content.push_back(row);  //pushing the data in the container
                }
                for (int i = 0; i < content.size(); ++i) {
                    for (int j = 0; j < content[i].size(); ++j) {
		                	cout<<content[i][j]<<"\t\t";   //printing data which is stored in the container
                    }
                    cout<<endl;
                }
		file.close();   //closing input file
    return 0;
}
