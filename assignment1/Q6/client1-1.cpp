#include <stdlib.h>
#include <cstdlib>
//#include <glut.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>  
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
 
using namespace std;

#define PORT 2080
 
int main (int argc, char *argv[])
{
     if (argc != 2)     /* argc should be 2 for correct execution */
     {
      cout << "usage: <executable> <filename>\n\n" <<  argv[0];
      exit(-1); 
      }

	int sock1;
	sock1 =  socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in serv;
 
	serv.sin_port = htons(PORT);
        cout << "PORT" << PORT << "   " << htons(PORT) << endl;
	//printf("%x %x\n",PORT,htons(PORT));
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr("127.0.0.1");
        cout << "CLIENT CONNECTING" << endl;
	//printf("client connecting\n");
	connect(sock1, (struct sockaddr *)&serv,sizeof(serv));
       	char buf[50];
        strcpy (buf, argv[1]);
        write (sock1,buf,50);
	FILE* fp = fopen(buf,"w");
	while(1){
		//bzero(buf,sizeof(buf));
		read(sock1,buf,50);
		if(strcmp(buf,"cmsc312")==0)
		{
			break;	
		}
		fprintf(fp,"%s",buf);
	}
	fclose(fp);
}
