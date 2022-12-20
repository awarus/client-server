#include "constraints.h"
#include "ptpool.h"

int global_cnt;
pthread_mutex_t mlc = PTHREAD_MUTEX_INITIALIZER;
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

	ptpool_t *pool = ptpool_create(1, N);

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

	while(j < N) {
		fd[j] = accept(servfd, (struct sockaddr *) adr,
			&adrlen);
		ptpool_add_work(pool, worker_t, (void*)(&fd[j++]));
	}

	printf("All job is scheduled\n");
	ptpool_wait(pool);
	ptpool_destroy(pool);

	close(servfd);
	free(fd);
	free(adr);

	printf("End of work\n");

	return 0;
}

void *worker_t(void *work_fd) {
	int fd = *(int*)work_fd;
	int loop_c = 0;
	char buf[MSG_SIZE];
	char tmp[3];
	char str[21];
	ssize_t nread;
	int fptr;
	size_t fsize;
	int i = 0;

	pthread_mutex_lock(&mlc);
	global_cnt++;
	sprintf(tmp, "%d", global_cnt);
	pthread_mutex_unlock(&mlc);

	printf("name is %s\n", tmp);

	nread = read(fd, buf, msg_size);
        if (nread == -1) {
		perror("read failed");
        }

        if(nread > 0) {
		printf("reading...\n");
                memcpy(str, SERVER_DIR, strlen(SERVER_DIR));
                memcpy(str + strlen(SERVER_DIR), tmp, strlen(tmp));

                printf("filename is %s\n", str);
                fptr = open(str, O_RDWR | O_CREAT, 0666);

		fsize  = msg_size * map_size;
		ftruncate(fptr, fsize);

		char *ptr = mmap(NULL, fsize,
			PROT_WRITE, MAP_SHARED,
			fptr, 0);

		while(i < nmessage - 1) {
			if(loop_c % map_size == 0 && loop_c >= map_size) {
				//printf("remap:i = %d,  loop_c = %d. Prev addr = %p \n", i, loop_c, ptr);
				munmap(ptr, msg_size * loop_c);
				ftruncate(fptr, msg_size * i + map_size);
				ptr = mmap(NULL, fsize,
					   PROT_WRITE, MAP_SHARED,
					   fptr, 0);

				loop_c = 0;
			}
                        memcpy(ptr + (size_t)(msg_size * loop_c), buf, nread);
			i++;
			//msync(ptr + nread * loop_c, msg_size, MS_ASYNC);
			loop_c++;
			nread = read(fd, buf, msg_size);
		}

		munmap(ptr, msg_size * loop_c);
                close(fptr);
                write(fd, ack, 10);

                printf("end of write...\n");
		close(fd);
	}
	return NULL;
}
