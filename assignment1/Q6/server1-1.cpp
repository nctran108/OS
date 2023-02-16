#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>  
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

using namespace std;

 
#define PORT 2080
 
main()
{
	int sock1,sock2, clength;
	sock1 =  socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in serv,cli;
 
	serv.sin_family = AF_INET;
	serv.sin_port = htons(PORT);
	serv.sin_addr.s_addr = inet_addr("127.0.0.1");
	bind(sock1,(struct sockaddr *)&serv, sizeof(serv));
	listen(sock1,5);
	clength = sizeof(cli);
	int i=0;
	char buf[50];
        while(1){
	sock2 = accept(sock1,(struct sockaddr *)&cli, (socklen_t *)&clength);
        cout << "\n CLIENT CONNECTED" << endl;
	//printf("\n Client Connected\n");
        read(sock2,buf,50);
        cout << "\n DISPLAY : " << buf << endl;
        //printf("Display: %s",buf);
	FILE* fp = fopen(buf,"r");
	while(!feof(fp)){
		for(i=0;i<50;i++)
                  buf[i]='\0';
		fread(buf,sizeof(char),49,fp);
		write(sock2,buf,50);
	}
	write(sock2,"cmsc312",50);
	fclose(fp);
        cout << "\n testing" << endl;
       //printf ("testing");
       }
	return 0;
}
 
