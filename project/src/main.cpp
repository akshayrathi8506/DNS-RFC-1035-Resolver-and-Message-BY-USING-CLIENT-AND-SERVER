/*PROJECT: DNS resolver RFC 1035 (resolver,message)
 *AUTHOR : AKSHAY RATHI,VIVEK SINGH,RAJANARASIMHA REDDY, SUBHAN SHAIK, RAVI
 *VERSION: 1.0
 *DATE   : 29'MAY 2022
 */

#include <iostream>      // for input and output
#include "../IncludeHeaderFile/DNS.h"          //DNS header file
using namespace std;
/*DESCRIPTION: By using socket client and server communicating with nameserver and the foreign server for IP address.
 * INPUT     : Hostname. example: google.com
 * OUTPUT    : Sucessfull
 */

/*
 * Main Function
 */
int main() {
	DNS dns;                   //creating class object
	unsigned char hostname[100];   //to take input from user as a query
	cout<<"Enter Host name : ";
	cin>>hostname;
	dns.get_dns_servers();     //calling function to connect with the foreign server
    dns.ngethostbyname(hostname , T_A);   //sending query by calling this function
    dns.IPfromnameserver();   //calling this function to communicate with the nameserver and for message passing
    return 0;
}
