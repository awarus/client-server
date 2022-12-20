#include "constraints.h"
#include <netinet/in.h>

pthread_t pt[64];

void *client_t(void* data);

int main() {
	printf("Starting client\n");

	for(int i = 0; i < N; i++) {
		if(pthread_create(&pt[i], NULL, client_t, NULL) != 0) {
			perror("thread %d didn't created\n");
		}
	}


	for(int i = 0; i < N; i++) {
		pthread_join(pt[i], NULL);
	}

	printf("Threads have been created\n");
	return 0;
}

void *client_t(void* data) {
	printf("new thread %ld \n", pthread_self());
	int res = 0;
	struct sockaddr_in saddr = {0};
	char send_buf[8000];
	int fd;
	ssize_t nread;
	char ack[10];

	memset(&send_buf, 1, sizeof(char)*8000);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(55555);
	fd = socket(AF_INET, SOCK_STREAM, 0);

	connect(fd, (struct sockaddr *) &saddr, sizeof(saddr));
	res = inet_pton(AF_INET, home_ip, &saddr.sin_addr);

	if(res == 0)
		perror("inet_pton: src address is not correct\n");

	for(int i = 0; i < nmessage; i++) {
		write(fd, send_buf, 8000);
	}

	nread = 0;
	while(nread <= 0) {
		nread = read(fd, ack, 10);
		if(nread == -1) {
			perror("read failed");
			exit(EXIT_FAILURE);
		}
	}

	printf("Ack recieved\n");
	pthread_exit(NULL);
}

