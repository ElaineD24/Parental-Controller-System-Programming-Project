#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include<string.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>
#include <stdbool.h>

#define BUFFER_LENGTH 256
#define IP_LENGTH 16
static char data[BUFFER_LENGTH];
static char ip[IP_LENGTH];

int hostname_to_ip(char * hostname , char* ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ( (he = gethostbyname( hostname ) ) == NULL)
    {
        // get the host info
        herror("hostname_to_ip");
        return 1;
    }

    addr_list = (struct in_addr **) he->h_addr_list;

    for(i = 0; addr_list[i] != NULL; i++)
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        printf("Converted to IP: %s\n\n", ip);
        return 0;
    }

    return 1;
}

char* ReadFile(char *filename){
    FILE *fptr;

    // char filename[100];
    char c;

    // printf("Enter the filename to open \n");
    // scanf("%s", filename);

    // Open file
    fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        printf("Cannot open file \n");
        exit(0);
    }

    // Read contents from file
    c = fgetc(fptr);
    while (c != EOF)
    {
        printf ("%c", c);
        c = fgetc(fptr);
    }

    fclose(fptr);
}

int main(int argc, char* argv[]){
  int fd, ret;
  bool isAdd = false;
  bool isRemove = false;

  // if (argc < 2 || argc > 3){
  //   fprintf(stderr, "usage: ./userspace -option context\n");
  // }

  fd = open("/dev/hook_module", O_RDWR);
  if (fd < 0){
    perror("Open device failed");
    return errno;
  }
  ReadFile("help.txt");
  printf("> ");
  scanf("%[^\n]s\n", data);
  while ((getchar()) != '\n');
  while(strcmp(data, "-q") != 0){

    isAdd = false;
    isRemove = false;

    if(strcmp(data, "-a")== 0){
      printf("Incorrect Command! Please enter again: \n");
    }

    char * pch;
    pch = strtok (data, "- ");
    while (pch != NULL)
    {
      if(strcmp(pch, "a")== 0){
        isAdd = true;

      }
      else if(strcmp(pch, "a")!=0 && isAdd == true){
        printf("Adding...");
        printf ("%s\n",pch);
        hostname_to_ip(pch, ip);
        ret = write(fd, &ip, IP_LENGTH);
      }
      else if(strcmp(pch, "r") == 0){
        printf("Removing...\n");
        isRemove = true;
        ret = write(fd, "r", 1);

      }

      else if(strcmp(pch, "h")== 0){
        printf("Openning help.txt...\n");
        ReadFile("help.txt");
      }
      else{
        printf("Incorrect Command! Please enter again: \n");
      }
      pch = strtok (NULL, "- ");
    }
    printf("> ");
    scanf("%[^\n]s\n", data);
    while ((getchar()) != '\n');
  }
  if(strcmp(data, "-q") == 0){
    printf("Bye!\n");
  }


}
