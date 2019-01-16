#include <stdio.h>          
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

#define	QSIZE	   8		/* size of input queue */
#define	MAXDG	4096		/* max datagram size */

int i;
struct hostent *host;
struct in_addr **addr_list;
typedef struct {
  void		*dg_data;		/* ptr to actual datagram */
  size_t	dg_len;			/* length of datagram */
  struct sockaddr  *dg_sa;	/* ptr to sockaddr{} w/client's address */
  socklen_t	dg_salen;		/* length of sockaddr{} */
} DG;
static DG	dg[QSIZE];			/* queue of datagrams to process */

static int	iget;		/* next one for main loop to process */
static int	iput;		/* next one for signal handler to read into */
static int	nqueue;		/* # on queue for main loop to process */
static socklen_t clilen;/* max length of sockaddr{} */
static int		sockfd;

static void	sig_io(int);

// check parameter from user is Ip or domain
// Input : a string
// Output : 0 ~ ip else domain  
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
    // printf("Official name: %s\n", host->h_name);
    char s[100];
    strcpy(s,host->h_name);
    strcpy(argv,"Official name: ");
    strcat(argv,s);
}

// print ip from domainname 
void domaintoip(char *argv){ // input : domain name -> output : IPs of this domain 
    host = gethostbyname(argv);
    // check exist of the host name
    if (gethostbyname(argv) == NULL){
        strcpy(argv,"Not found information\n");
        return;
    }
    // print IPs of this host
    addr_list = (struct in_addr **)host->h_addr_list;
    char s[100]; strcpy(s,"");
    if (addr_list[0] != NULL ) {
    	// printf("Official IP: %s\n", inet_ntoa(*addr_list[0]));
    	strcpy(argv,"Official IP: ");
    	strcpy(s,inet_ntoa(*addr_list[0]));
        strcat(argv,s);
    }
    if (addr_list[1] != NULL){ 
    	// printf("Alias IP: \n");
        strcpy(s,"\nAlias IP: \n");
        strcat(argv,s);
    }
    for(i = 1; addr_list[i] != NULL; i++) {
        strcpy(s,inet_ntoa(*addr_list[i]));
        strcat(s,"\n");
        strcat(argv,s);       
        // printf("%s\n", inet_ntoa(*addr_list[i]));
    }    
}

int main(int argc, char *argv[])
{
	if (argc !=2){
		printf("Usage %s <port_number> \n",argv[0]);
	}
	int			i;
	const int	on = 1;
	sigset_t	zeromask, newmask, oldmask;
	
	struct sockaddr_in	servaddr, cliaddr;
	
	if ((sockfd=socket(AF_INET, SOCK_DGRAM, 0)) == -1 ){  /* calls socket() */
		perror("socket() error\n");
		return 0;
	}
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));
	
	if(bind(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr))==-1){ /* calls bind() */
		perror("bind() error\n");
		return 0;
	}
		
	clilen = sizeof(cliaddr);
	
	for (i = 0; i < QSIZE; i++) {	/* init queue of buffers */
		dg[i].dg_data = malloc(MAXDG);
		dg[i].dg_sa = malloc(clilen);
		dg[i].dg_salen = clilen;
	}
	iget = iput = nqueue = 0;
	
	/* Signal handlers are established for SIGIO. The socket owner is
	 * set using fcntl and the signal-driven and non-blocking I/O flags are set using ioctl
	 */
	signal(SIGIO, sig_io);
	fcntl(sockfd, F_SETOWN, getpid());
	ioctl(sockfd, FIOASYNC, &on);
	ioctl(sockfd, FIONBIO, &on);
	
	/* Three signal sets are initialized: zeromask (which never changes),
	 * oldmask (which contains the old signal mask when we block SIGIO), and newmask.
	 */
	sigemptyset(&zeromask);		
	sigemptyset(&oldmask);
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGIO);	/* signal we want to block */
	
	/* Stores the current signal mask of the process in oldmask and then
	 * logically ORs newmask into the current signal mask. This blocks SIGIO
	 * and returns the current signal mask. We need SIGIO blocked when we test
	 * nqueue at the top of the loop
	 */
	sigprocmask(SIG_BLOCK, &newmask, &oldmask);
	
	for ( ; ; ) {
		while (nqueue == 0)
			sigsuspend(&zeromask);	/* wait for datagram to process */

		/* unblock SIGIO by calling sigprocmask to set the signal mask of
		 * the process to the value that was saved earlier (oldmask).
		 * The reply is then sent by sendto.
		 */
		sigprocmask(SIG_SETMASK, &oldmask, NULL);
        // proccessing data
        char s[MAXDG];
        strcpy(s,dg[iget].dg_data);   
        s[strlen(s)-1] = '\0'; 
        // if (checkIporDomain(s)) 
        if (!checkIporDomain(s)) iptodomain(s); else domaintoip(s);

	    // 
		sendto(sockfd, s, strlen(s), 0,
			   dg[iget].dg_sa, dg[iget].dg_salen);

		if (++iget >= QSIZE)
			iget = 0;

		/* SIGIO is blocked and the value of nqueue is decremented.
		 * We must block the signal while modifying this variable since
		 * it is shared between the main loop and the signal handler.
		 */
		sigprocmask(SIG_BLOCK, &newmask, &oldmask);
		nqueue--;
	}
}

static void sig_io(int signo)
{
	ssize_t		len;
	DG			*ptr;

	for (; ; ) {
		if (nqueue >= QSIZE){
			perror("receive overflow");
			break;
		}

		ptr = &dg[iput];
		ptr->dg_salen = clilen;
		len = recvfrom(sockfd, ptr->dg_data, MAXDG, 0,
					   ptr->dg_sa, &ptr->dg_salen);
		if (len < 0) {
			if (errno == EWOULDBLOCK)
				break;		/* all done; no more queued to read */
			else{
				perror("recvfrom error");
				break;
			}
		}
		ptr->dg_len = len;

		nqueue++;
		if (++iput >= QSIZE)
			iput = 0;

	}	
}
