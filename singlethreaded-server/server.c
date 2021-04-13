#include <arpa/inet.h>
#include <unistd.h>

#include "constraints.h"

void* worker(void *data);

int main() {
	struct sockaddr_in adr = {0};
	socklen_t adrlen;
	int servfd = 0;
	int res = 0;
	int fd = 1;
        ssize_t nread;
	char buf[8000];
	char ack[10] = "good";
        int client_c = 1;	    // count for clients                                  
        int loop_c = 0;		    // count for messages                                                         
        int num_workers = N/3;      // quantity of server threads                                  
        pthread_t wpt[num_workers]; // server threads                                             
  
  	char tmp[3];                                                            
        char str[21];  
	FILE *fptr;

	struct stat st = {0};
	
	if (stat(SERVER_DIR, &st) == -1) {
		mkdir(SERVER_DIR, 0700);
	}

	memset(&adr, 0, sizeof(struct sockaddr));
	adr.sin_family = AF_INET;
	adr.sin_port = htons(55555);
	adrlen = sizeof(adr);

	if((servfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket creation failed\n");
		exit(EXIT_FAILURE);
	}
	

	bind(servfd, (struct sockaddr *) &adr, sizeof(adr));

	listen(servfd, N);
	printf("Starting server with servfd = %d\n", servfd);

	if(res < 0 || servfd < 0)
		goto fail;
	
	while(1) {
		fd = accept(servfd, (struct sockaddr *) &adr,
			&adrlen);
		
		if(!fd)
			break;

		nread = read(fd, buf, 8000);
		if (nread == -1) {
			perror("read failed");
			goto fail;
		}
		
		if(nread > 0) {
			//printf("reading...\n");
			sprintf(tmp, "%d", client_c); 
			memcpy(str, SERVER_DIR, strlen(SERVER_DIR));
			memcpy(str + strlen(SERVER_DIR), tmp, strlen(tmp));
			
			//printf("filename is %s\n", str);
			//printf("read %ld bytes\n", nread); 	
			
			fptr = fopen(str, "wb");
			
			while(loop_c < 5000 - 1) {
				fwrite(buf, sizeof(char), nread,
					    fptr);			
				loop_c++;
				nread = read(fd, buf, 8000);
			}
			
			fclose(fptr);
			write(fd, ack, 10);	

			memset(str, 0, strlen(str)*sizeof(char));
			client_c++;
			loop_c = 0;
			printf("end of write...\n");
			//sleep(1);
			//close(fd);
		} 
	} 

	sleep(1);
	close(fd);
	close(servfd);
	printf("End of work\n");
	return 0;
fail:
	perror("something went wrong\n");
	exit(EXIT_FAILURE);
}

void *worker(void *data) {
	pthread_detach(pthread_self());

	pthread_exit(NULL);
}
