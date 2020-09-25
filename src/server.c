#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <memory.h>
#include <errno.h>
#include <time.h>

#define SERVER_PORT 2000
char data_buffer[1024];
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"


static char*
set_current_date_time(char *option){
  time_t t;
  time(&t);
  char *temp;
  char *day;
  int day_number;
  char *month;
  int year;
  char *current_time;
  const char delimiter[2] = " ";
  temp = ctime(&t);
  day = strtok(temp, delimiter);
  month = strtok(NULL, delimiter);
  day_number = atoi(strtok(NULL, delimiter));
  current_time = strtok(NULL, delimiter);
  year = atoi(strtok(0, delimiter));
  char *output = calloc(1, 1024);
  if(strcmp(option, "date_time")){
    sprintf(output, "Last-Modified: %s, %d %s %d %s GMT\r\n", day, day_number, month, year, current_time);
    return output;
  }
  else if(strcmp(option, "date")){
    sprintf(output, "Date: %d %s %d\r\n", day_number, month, year);
    return output;
  }
  return NULL;
}

static void
HTTP_header_create(char *header){
  strcpy(header, "HTTP/1.1 200 OK\r\n");                           // Status Line
  strcat(header, set_current_date_time("date"));                         // General Headers
  strcat(header, "Connection: close\r\n");
  strcat(header, "Server: My Personal HTTP Server\r\n");           //Response Headers
  strcat(header, "Accept-Ranges: bytes\r\n");
  strcat(header, "Content-Type: text/html\r\n");                   //Entity Headers
  //strcat(header, "Content-Length: 242\r\n");
  strcat(header, set_current_date_time("date_time"));
  strcat(header, "\r\n");
}

static void
HTML_create(char *response, char *token){
  strcpy(response, "<html>"
                   "<h>names</h>"
                   "<p>");
  strcat(response, token);
  strcat(response, "</p>");
  strcat(response , "</html>");
}

static char*
process_GET_request(char * URL, unsigned int *response_length){
  printf("%s(%u) called with URL = %s\n", __FUNCTION__, __LINE__, URL);
  char delimiter[2] = "/";
  char *token;
  token = strtok(URL, delimiter);

  printf("%s\n", token);
  char *response = calloc(1, 1024);
  HTML_create(response, token);
  char *header = calloc(1, 248 + strlen(response));
  HTTP_header_create(header);
  strcat(header, response);
  *response_length = strlen(header);
  free(response);
  return header;
}

static char*
process_POST_request(char *URL, unsigned int *response_length){
  return NULL;
}


int
main(void){
  int master_sock_tcp_fd = 0,
      sent_recv_bytes = 0,
      addr_len = 0,
      opt = 1;

  int comm_socket_fd = 0; /* client specific communication socket file descriptor. Used only for data exchange between client and server */
  fd_set readfds; /* Set of file descriptor on which select() polls. Select() unblocks whenever data arrives on any fd present in this set*/
  struct sockaddr_in server_addr, client_addr;

  if((master_sock_tcp_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
    printf("master socket creation failed\n");
    exit(1);
  }
  if(setsockopt(master_sock_tcp_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0){
    printf("TCP socket creation failed for multiple connection\n");
    exit(EXIT_FAILURE);
  }
  printf("master socket created\n");

  server_addr.sin_family = AF_INET;           /*This socket will process only ipv4 network packets*/
  server_addr.sin_port = htons(SERVER_PORT);  /*Server will process any data arriving on port no 2000*/
  server_addr.sin_addr.s_addr = INADDR_ANY;
  addr_len = sizeof(struct sockaddr);

  if(bind(master_sock_tcp_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1){
    printf("socket bind failed\n");
    exit(1);
  }
  printf("socket bind successful\n");

  if(listen(master_sock_tcp_fd, 5) < 0){
    printf("listen failed\n");
    exit(1);
  }
  printf("start listening...\n");

  while(1){
    FD_ZERO(&readfds);
    FD_SET(master_sock_tcp_fd, &readfds);
    printf("blocked on select system call\n");
    select(master_sock_tcp_fd+1, &readfds, NULL, NULL, NULL);
    if(FD_ISSET(master_sock_tcp_fd, &readfds)){
      printf("New connection received\n");
      comm_socket_fd = accept(master_sock_tcp_fd, (struct sockaddr *)&client_addr, &addr_len);
      if(comm_socket_fd < 0){
        printf("The server could not accept the incomming request\n");
        exit(0);
      }
      char str[16];
      printf("connection accepted from the client : "ANSI_COLOR_RED"%s:%u\n"ANSI_COLOR_RESET, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
      while(1){
        printf("Server is ready to receive client message\n");
        memset(data_buffer, 0, sizeof(data_buffer));
        sent_recv_bytes = recvfrom(comm_socket_fd, (char*)data_buffer, sizeof(data_buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
        if(sent_recv_bytes == 0){
          close(comm_socket_fd);
          break;
        }
        else{
          printf("server has received %d bytes from the client "ANSI_COLOR_RED" %s%u\n"ANSI_COLOR_RESET, sent_recv_bytes, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        }
        /* Implement HTTP request processing functionality */
        printf("------------------------\n");
        printf("%s\n", data_buffer);
        printf("------------------------\n");
        char *request_line = NULL;
        char del[2] = "\n",
             *method = NULL,
             *URL = NULL;
        request_line = strtok(data_buffer, del);
        del[0] = ' ';
        method = strtok(request_line, del);
        URL = strtok(NULL, del);
        printf("Method = %s\n", method);
        printf("URL = %s\n", URL);
        char *response = NULL;
        unsigned int response_length = 0;
        if(strncmp(method, "GET", strlen("GET")) == 0){
          response = process_GET_request(URL, &response_length);
        }
        else if(strncmp(method, "POST", strlen("POST")) == 0){
          response = process_POST_request(URL, &response_length);
        }
        else{
          printf("unsupported URL method request\n");
          close(comm_socket_fd);
          break;
        }
        if(response){
          printf("response to be sent to the client :\n%s\n", response);
          sent_recv_bytes = sendto(comm_socket_fd, response, response_length, 0, (struct sockaddr *)&client_addr, sizeof(struct sockaddr));
          free(response);
          printf("server has sent %d bytes in reply to the client"ANSI_COLOR_RED " %s:%u\n" ANSI_COLOR_RESET, sent_recv_bytes, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
          close(comm_socket_fd);
          break;
        }
      }
    }
  }
  return 0;
}
