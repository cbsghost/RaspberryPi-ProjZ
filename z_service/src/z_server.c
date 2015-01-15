#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <signal.h>
#include <ctype.h>

//#include <pthread.h>

// The PiWeather board i2c address
#define ADDRESS 0x04
 
// The I2C bus: This is for V2 pi's. For V1 Model B you need i2c-0
static const char *devName = "/dev/i2c-1";



int file;

void beforeExit(int param)
{
	int i;
	char cmd[16], tmp;
	cmd[0] = 'S', cmd[1] = 'S', cmd[2] = 'E', cmd[3] = '0', cmd[4] = '\0';
     write(file, cmd, strlen(cmd));
     for (i = 0; ; i++) {
				int rr;
				rr = read(file, &tmp, 1);
				if (rr != 1) {
					//printf("[error]\n");
					continue;
				}
				cmd[i] = tmp;
				
				if (tmp == '\0') {
					break;
				}
			}
	
	cmd[0] = 'S', cmd[1] = 'L', cmd[2] = 'P', cmd[3] = '2', cmd[4] = '5', cmd[5] = '5', cmd[6] = '\0';
     write(file, cmd, strlen(cmd));
     for (i = 0; ; i++) {
				int rr;
				rr = read(file, &tmp, 1);
				if (rr != 1) {
					//printf("[error]\n");
					continue;
				}
				cmd[i] = tmp;
				
				if (tmp == '\0') {
					break;
				}
			}
			
	cmd[0] = 'S', cmd[1] = 'L', cmd[2] = 'E', cmd[3] = '0', cmd[4] = '\0';
     write(file, cmd, strlen(cmd));
     for (i = 0; ; i++) {
				int rr;
				rr = read(file, &tmp, 1);
				if (rr != 1) {
					//printf("[error]\n");
					continue;
				}
				cmd[i] = tmp;
				
				if (tmp == '\0') {
					break;
				}
			}
			
			exit(0);
}

int main(int argc , char *argv[])
{
	
	int i = 0, j = 0;
 
  if ((file = open(devName, O_RDWR)) < 0) {
    fprintf(stderr, "I2C: Failed to access %d\n", (int)devName);
    exit(1);
  }
 
  printf("I2C: acquiring buss to 0x%x\n", ADDRESS);
 
  if (ioctl(file, I2C_SLAVE, ADDRESS) < 0) {
    fprintf(stderr, "I2C: Failed to acquire bus access/talk to slave 0x%x\n", ADDRESS);
    exit(1);
  }
	char cmd[16], tmp;
	
	//TCP
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[1024], ret_msg[16];
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8087 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
     cmd[0] = 'S', cmd[1] = 'S', cmd[2] = 'E', cmd[3] = '0', cmd[4] = '\0';
     write(file, cmd, strlen(cmd));
     for (i = 0; ; i++) {
				int rr;
				rr = read(file, &tmp, 1);
				if (rr != 1) {
					//printf("[error]\n");
					continue;
				}
				cmd[i] = tmp;
				
				if (tmp == '\0') {
					break;
				}
			}
     
     cmd[0] = 'S', cmd[1] = 'L', cmd[2] = 'P', cmd[3] = '2', cmd[4] = '5', cmd[5] = '5', cmd[6] = '\0';
     write(file, cmd, strlen(cmd));
     for (i = 0; ; i++) {
				int rr;
				rr = read(file, &tmp, 1);
				if (rr != 1) {
					//printf("[error]\n");
					continue;
				}
				cmd[i] = tmp;
				
				if (tmp == '\0') {
					break;
				}
			}
			
	cmd[0] = 'S', cmd[1] = 'L', cmd[2] = 'E', cmd[3] = '1', cmd[4] = '\0';
     write(file, cmd, strlen(cmd));
     for (i = 0; ; i++) {
				int rr;
				rr = read(file, &tmp, 1);
				if (rr != 1) {
					//printf("[error]\n");
					continue;
				}
				cmd[i] = tmp;
				
				if (tmp == '\0') {
					break;
				}
			}
     signal(SIGABRT, beforeExit);
     signal(SIGFPE, beforeExit);
     signal(SIGILL, beforeExit);
     signal(SIGINT, beforeExit);
     signal(SIGSEGV, beforeExit);
     signal(SIGTERM, beforeExit);
    //accept connection from an incoming client
    while(1) {
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");
     
     system("/home/pi/projects/mjpg-streamer/mjpg_streamer -i \"/home/pi/projects/mjpg-streamer/input_uvc.so -y -d /dev/video0 -f 30 -r 320x240 -q 75\" -o \"/home/pi/projects/mjpg-streamer/output_http.so -p 8081 -n\" &");
     
    cmd[0] = 'S', cmd[1] = 'L', cmd[2] = 'P', cmd[3] = '0', cmd[4] = '\0';
     write(file, cmd, strlen(cmd));
     for (i = 0; ; i++) {
				int rr;
				rr = read(file, &tmp, 1);
				if (rr != 1) {
					//printf("[error]\n");
					continue;
				}
				cmd[i] = tmp;
				
				if (tmp == '\0') {
					break;
				}
			}
			cmd[0] = 'S', cmd[1] = 'L', cmd[2] = 'E', cmd[3] = '1', cmd[4] = '\0';
     write(file, cmd, strlen(cmd));
     for (i = 0; ; i++) {
				int rr;
				rr = read(file, &tmp, 1);
				if (rr != 1) {
					//printf("[error]\n");
					continue;
				}
				cmd[i] = tmp;
				
				if (tmp == '\0') {
					break;
				}
			}
     cmd[0] = 'S', cmd[1] = 'S', cmd[2] = 'E', cmd[3] = '1', cmd[4] = '\0';
     write(file, cmd, strlen(cmd));
     for (i = 0; ; i++) {
				int rr;
				rr = read(file, &tmp, 1);
				if (rr != 1) {
					//printf("[error]\n");
					continue;
				}
				cmd[i] = tmp;
				
				if (tmp == '\0') {
					break;
				}
			}
    //Receive a message from client
    while(1) {
   
    int process_cmd = 0;
    while( (read_size = recv(client_sock , client_message , 1024 , 0))) {
		
		if(read_size == 0)
		{
			puts("Client disconnected");
			break;
		}
		else if(read_size == -1)
		{
			perror("recv failed");
			continue;
		}
		
		for (i = 0 ; i< read_size; i++) {
		//Send the message back to client
		//write(client_sock , client_message , strlen(client_message));
		//printf("%c",client_message[i]);
			if (i + j > 15) {
				j = 0;
				break;
			}
			cmd[i + j] = client_message[i];
			//printf("%c", cmd[i+j]);
			//("%c\n", client_message[i]);
			if (client_message[i] == '\0') {
				process_cmd = 1;
				j = 0;
				break;
			}
		}
		if (i == read_size) {
			j = i;
		}
		if (process_cmd == 1) {
				break;
			}
		
		}
		
		if(read_size == 0)
		{
			puts("Client disconnected\n");
			system("/usr/bin/killall mjpg_streamer");
			cmd[0] = 'S', cmd[1] = 'S', cmd[2] = 'E', cmd[3] = '0', cmd[4] = '\0';
     write(file, cmd, strlen(cmd));
     for (i = 0; ; i++) {
				int rr;
				rr = read(file, &tmp, 1);
				if (rr != 1) {
					//printf("[error]\n");
					continue;
				}
				cmd[i] = tmp;
				
				if (tmp == '\0') {
					break;
				}
			}
     cmd[0] = 'S', cmd[1] = 'L', cmd[2] = 'P', cmd[3] = '2', cmd[4] = '5', cmd[5] = '5',cmd[6] = '\0';
     write(file, cmd, strlen(cmd));
     for (i = 0; ; i++) {
				int rr;
				rr = read(file, &tmp, 1);
				if (rr != 1) {
					//printf("[error]\n");
					continue;
				}
				cmd[i] = tmp;
				
				if (tmp == '\0') {
					break;
				}
			}
			
			cmd[0] = 'S', cmd[1] = 'L', cmd[2] = 'E', cmd[3] = '1', cmd[4] = '\0';
     write(file, cmd, strlen(cmd));
     for (i = 0; ; i++) {
				int rr;
				rr = read(file, &tmp, 1);
				if (rr != 1) {
					//printf("[error]\n");
					continue;
				}
				cmd[i] = tmp;
				
				if (tmp == '\0') {
					break;
				}
			}
			break;
		}
		
			
		 //printf("here, %d\n",process_cmd);
		 if (strlen(cmd) < 3) {
			 process_cmd = 0;
			 printf("%s\n",cmd);
		 }
		 
		 char *cmd_ptr;
		 //printf("hereee, %d\n",process_cmd);
		 if (process_cmd == 1) {
			 int flag;
			 flag = 0;
			//("process\n");
			 if (toupper(cmd[0]) == 'G') {
				 flag = 1;
				
			 }
			 write(file, cmd, strlen(cmd));
			//write(file, cmd, 1);
			 usleep(10000);
			 
			 for (i = 0; ; i++) {
				int rr;
				rr = read(file, &tmp, 1);
				if (rr != 1) {
					//printf("[error]\n");
					continue;
				}
				cmd[i] = tmp;
				
				if (tmp == '\0') {
					break;
				}
			}
			
			if (flag == 1) {
				
				//cmd_ptr = cmd + 3;
				printf("ee:%s\n", cmd);
				int len = strlen(cmd);
				cmd[len] = '\n';
				write(client_sock , cmd + 3 , len - 2);
			} else {
				printf("aa:%s\n", cmd);
				cmd[1] = '\n';
				write(client_sock , cmd , 2);
			}
			
			process_cmd = 0;
		 }
		
	 }
	 
		
	}
    return 0;
}
