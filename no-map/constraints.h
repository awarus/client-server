#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>                                                         
#include <sys/types.h>                                                          
#include <sys/stat.h>
#include <sys/mman.h>

#include <stdio.h>                                                              
#include <stdlib.h>                                                             
#include <string.h>
#include <unistd.h> 
#include <pthread.h>
#include <fcntl.h>                                                              

#define home_ip "127.0.0.1"

#define SERVER_DIR "/var/tmp/server_data/"
#define CLIENT_DIR "/var/tmp/client_dir/"
char *serv_dir = "/var/tmp/server_data/";
#define SERV_DIR(name) strcpy(serv_dir, name) 
#define MSG_SIZE 8000

const int nmessage = 50000;
const size_t msg_size = MSG_SIZE;
const int N = 64;
const int map_size = 10000;
const int num_workers = 64/3;
