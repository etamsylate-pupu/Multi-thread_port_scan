/**********************************************************\
*                                                         *\
*  program name:tcpclient.c                               *\
*  Author:etamsylate			                          *\
*  Date: 20-10-2020                                       *\
*                                                         *\
***********************************************************/

#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <pthread.h>

#define THREAD_NUM 100


typedef struct port_segment {
    struct in_addr dest;
    unsigned int min_port;
    unsigned int max_port;
} port_segment;

void print_usage(char * cmd)
{
	fprintf(stderr," %s usage:\n",cmd);
	fprintf(stderr,"%s start_port end_port IP_Addr\n",cmd);

}

int is_connect(int port, in_addr_t addr){
	int sockfd;
	struct sockaddr_in server;
	int ret;

	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if (-1==sockfd){
		perror("can not create socket\n");
		exit(-1);
	}

	memset(&server,0,sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = addr;
	server.sin_port = htons(port);
	
	ret=connect(sockfd,(struct sockaddr*)&server,sizeof(struct sockaddr));
	close(sockfd);
	if(ret<0){
	    return 0;
	}
    else{
		return 1;
	}
}
void *scan( void *arg )  //扫描端口打印信息函数
{
	struct servent *sptr;
	port_segment port;
	int scanport;
	memcpy( &port, arg, sizeof(struct port_segment) );
	
	for(scanport=port.min_port;scanport<=port.max_port;scanport++){
		if(is_connect(scanport,port.dest.s_addr)==1){
			sptr=getservbyport(htons(scanport),"tcp");
			if(sptr!=NULL){
				
				printf("port=%d\tname=%s\n",scanport,sptr->s_name);
			}
			else{
				printf("port=%d\tunknown\n",scanport);
			}
		}
	}
	
}


int main(int argc,char** argv)
{ 
	pthread_t * thread;
	int i,j,k;
	int MIN_PORT,MAX_PORT,SEG_LEN;
	if (argc<4)
	{
		print_usage(argv[0]);
		exit(1);

	}
	else{
		MIN_PORT=atoi(argv[1]);
		MAX_PORT=atoi(argv[2]);
		SEG_LEN=(MAX_PORT-MIN_PORT)/THREAD_NUM;
	}
	
	
	for ( j = 0; j < argc - 3; ++j ) { //对每个ip地址
		printf("\n扫描IP:%s\n",argv[j+3]);
		thread = ( pthread_t * )malloc( THREAD_NUM * sizeof(pthread_t) );  //每个ip开设100个线程
		port_segment port[THREAD_NUM];
        for ( i = 0; i < THREAD_NUM; ++i ) {
            
            port[i].dest.s_addr = inet_addr(argv[j+3]);
            port[i].min_port = i * SEG_LEN + MIN_PORT;

            /* the last segment */
            if ( i == (THREAD_NUM - 1) )
                port[i].max_port = MAX_PORT;
            else
                port[i].max_port = port[i].min_port + SEG_LEN - 1;
			
			if ( pthread_create(&thread[i], NULL, scan, (void *)&port[i]) != 0 )
            {
                perror( "pthread_create failed\n" );
                free(thread);
                exit(-2);
			}
				
		}
		for(k=0; k<THREAD_NUM; k++)
			pthread_join(thread[k], NULL);
   }
   if(thread != NULL)
	   free(thread);
    return 0;	
}