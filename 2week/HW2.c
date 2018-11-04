#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

char *addrtype(int addrtype) {
        switch(addrtype) {
                case AF_INET:
                        return "AF_INET";
                case AF_INET6:
                        return "AF_INET6";
        }
        return "Unknown";
}

int main(int argc, char **argv) {
        struct hostent *tmp = 0;
        int i = 0;

        if (argc < 2) { // kiem tra tham so dau vao
                printf("Usage: %s <hostname>\n", argv[0]);
                return 0;
        }

        tmp = gethostbyname(argv[1]); // doc tham so
        if (!tmp) {
                printf("Not found information \n");
                return 0;
        }

        while(tmp->h_aliases[i] != NULL) { // in ra alias IP
        	    printf("ok");
                printf("h_aliases[i]: %s\n", tmp->h_aliases[i]);
                i++;
        }
        i = 0;
        while(tmp->h_addr_list[i] != NULL) { // dia chi official IP
                printf("h_addr_list[i]: %s\n", inet_ntoa( (struct in_addr) *((struct in_addr *) tmp->h_addr_list[i])));
                i++;
        }

        return 0;
}