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

#define PORT 4277
#define MAXAVGSIZE 100
 
int main (int argc, char *argv[]) {
     if (argc < 2)     /* argc should be 2 for correct execution */
     {
      cout << "usage: <executable> <argument>\n\n" <<  argv[0];
      exit(-1); 
      }

	int sock1;
	double f;
	double *arr;

	arr = (double*) malloc(MAXAVGSIZE*sizeof(double));

	sock1 =  socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in serv;

	serv.sin_port = htons(PORT);
        cout << "PORT" << PORT << "   " << htons(PORT) << endl;
	//printf("%x %x\n",PORT,htons(PORT));
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr("127.0.0.1");
        cout << "CLIENT CONNECTING" << endl;
	//printf("client connecting\n");
	if (connect(sock1, (struct sockaddr *)&serv,sizeof(serv)) < 0){
		cout << "fail to connect" << endl;
		return 1;
	}

	char buf[50];
	int n = 0;
	int size = argc - 1;
	write(sock1,&size,sizeof(size));

	for(int i = 0; i < argc - 1; i++){
		strcpy(buf, argv[i+1]);
		printf("value   = %s\n", buf);
        write(sock1,buf,sizeof(buf));
	}
	cout << "Array [";
	while(1) {
		if (read(sock1,buf,50) > 0) {
			if(strcmp(buf,"cmsc312")==0) {
				break;	
			}
			else{
				printf("%s ", buf);
			}
		}
	}
	
	cout << "]" << endl;
}
