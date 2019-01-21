#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int i;
struct hostent *host;
struct in_addr **addr_list;

// check parameter from user is Ip or domain
int checkIporDomain(char *argv){ // out : 0 ~ IP , 1~domain
    char a[3];
    int j,count=0,cs=0;
    for (j=0; j<strlen(argv);j++){
        if (argv[j]!='.') {
            if (argv[j]<'0' || argv[j]>'9') return 1;
            a[cs] = argv[j];
            cs++;
        }
            else {
                count++;
                if (count >=4) return 1;
                a[cs] = '\0';
                if (atoi(a)<0 || atoi(a)>255) return 1;
                cs = 0;
                a[cs]='\0';
            }
    }
    if (count!=3) return 3;
    a[3]= '\0';
    if (atoi(a)<0 || atoi(a)>255) return 1;
    return 0;
}

// print domainname from ip
void iptodomain(char *argv){
    struct in_addr ipv4addr;
        inet_pton(AF_INET,argv, &ipv4addr);
        host = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
        printf("Official name: %s\n", host->h_name);
}
// print ip from domainname 
void domaintoip(char *argv){ // input : domain name -> output : IPs of this domain 
    host = gethostbyname(argv);
    // check exist of the host name
    if (gethostbyname(argv) == NULL){
        printf("Not found information\n");
        return;
    }
    // print IPs of this host
    addr_list = (struct in_addr **)host->h_addr_list;
    if (addr_list[0] != NULL ) printf("Official IP: %s\n", inet_ntoa(*addr_list[0]));
    if (addr_list[1] != NULL) printf("Alias IP: \n");
    for(i = 1; addr_list[i] != NULL; i++) {       
        printf("%s\n", inet_ntoa(*addr_list[i]));
    }    
}

int main(int argc, char *argv[])
{
    // check command from user
    if (argc != 2) {
        printf("Usage: %s <hostname> or %s <IP>\n", argv[0],argv[0]);
        return 1;
    }
    if(checkIporDomain(argv[1])) domaintoip(argv[1]);
      else iptodomain(argv[1]);
    return 0;
}