#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>  
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <signal.h>
#include <string.h>

using namespace std;

 
#define PORT 4277
#define MAXAVGSIZE 100

void sigpipe_handler(int signum) {
    cout << "Caught SIGPIPE signal " << signum << endl;
}

void swap(double* a, double* b){
  double t = *a;
  *a = *b;
  *b = t;
}

int partition(double *arr, int low, int high){
  int pivot = arr[high];
  int i = low - 1;

  for(int j = low; j <= high - 1; j++) {
    if (arr[j] < pivot) {
      i++;
      swap(&arr[i], &arr[j]);
    }
  }
  swap(&arr[i + 1], &arr[high]);
  return (i + 1);
}

void quickSort(double *arr, int low, int high){
  if (low < high) {
    int p = partition(arr, low, high);
    quickSort(arr, low, p - 1);
    quickSort(arr, p + 1, high);
  }
}
 
int main() {
	int sock1,sock2, clength;
	sock1 =  socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in serv,cli;
 
	serv.sin_family = AF_INET;
	serv.sin_port = htons(PORT);
	serv.sin_addr.s_addr = inet_addr("127.0.0.1");
	bind(sock1,(struct sockaddr *)&serv, sizeof(serv));
	listen(sock1,5);
	clength = sizeof(cli);
	int n=0, i = 0, size = 0;
	char buf[50], *temp;
	double *arr;
	double f;
	char *endptr;

	arr = (double*) malloc(MAXAVGSIZE*sizeof(double));
	temp = (char*) malloc(50*sizeof(char));
	while(1){
		sock2 = accept(sock1,(struct sockaddr *)&cli, (socklen_t *)&clength);
		cout << "\n CLIENT CONNECTED" << endl;
		read(sock2,&size,sizeof(size));
		while(i < size){	
			read(sock2,buf,sizeof(buf));
			f = strtod( buf, &endptr);
			arr[i] = f;
			i++;
		}

		quickSort(arr, 0, size-1);

		i = 0;
		while(i < size) {
			sprintf(buf, "%lf", arr[i]);
			write(sock2,buf,50);
			i++;
		}
		strcpy(buf,"cmsc312");
		write(sock2,buf,50);

		i = 0;
	}
	return 0;
}
 
