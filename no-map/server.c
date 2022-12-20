#include <arpa/inet.h>
#include <unistd.h>

#include "constraints.h"
#include "ptpool.h"

int global_cnt;
pthread_mutex_t mlk = PTHREAD_MUTEX_INITIALIZER;
char ack[10] = "good";

void* worker_t(void *work_fd);

int init_sockaddr(struct sockaddr_in * saddr_in, char* ip, int port) {
	saddr_in->sin_family = AF_INET;
	saddr_in->sin_port = htons(port);
	return 0;
}

int main() {
	struct sockaddr_in *adr;
	socklen_t adrlen;
	int servfd = 0;
	int *fd;
	int j = 0;
	struct stat st = {0};

	adr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));

	ptpool_t *pool = ptpool_create(num_workers, N);

	fd = malloc(N * sizeof(int));

	if (stat(SERVER_DIR, &st) == -1) {
		mkdir(SERVER_DIR, 0700);
	}

	memset(adr, 0, sizeof(struct sockaddr_in));

	init_sockaddr(adr, home_ip, 55555);
	if((servfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket creation failed\n");
		exit(EXIT_FAILURE);
	}


	bind(servfd, (struct sockaddr *)adr, sizeof(struct sockaddr_in));

	listen(servfd, num_workers);
	printf("Starting server with servfd = %d\n", servfd);

	while(j < 64) {
		fd[j] = accept(servfd, (struct sockaddr *) adr,
			&adrlen);
		ptpool_add_work(pool, worker_t, (void*)(&fd[j++]));
	}

	printf("All job is scheduled\n");
	ptpool_wait(pool);
	ptpool_destroy(pool);
	sleep(1);
	close(servfd);
	free(fd);
	free(adr);
	printf("End of work\n");

	return 0;
}

void *worker_t(void *work_fd) {
	int fd = *(int*)work_fd;
	int loop_c = 0;
	char buf[8000];
	char tmp[3];
	char str[21];
	ssize_t nread;
	FILE *fptr;

	pthread_mutex_lock(&mlk);
	global_cnt++;
	sprintf(tmp, "%d", global_cnt);
	pthread_mutex_unlock(&mlk);

	printf("name is %s\n", tmp);

	nread = read(fd, buf, 8000);
        if (nread == -1) {
		perror("read failed");
        }

        if(nread > 0) {
		printf("reading...\n");
                memcpy(str, SERVER_DIR, strlen(SERVER_DIR));
                memcpy(str + strlen(SERVER_DIR), tmp, strlen(tmp));=

                printf("filename is %s\n", str);
                fptr = fopen(str, "wb");
                while(loop_c < nmessage - 1) {
                        fwrite(buf, sizeof(char), nread, fptr);
                        loop_c++;
                        nread = read(fd, buf, 8000);
                }

                fclose(fptr);
                write(fd, ack, 10);

                printf("end of write...\n");
		close(fd);
	}
	return NULL;
}
