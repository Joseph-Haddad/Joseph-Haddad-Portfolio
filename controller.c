#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdbool.h>
#include <ctype.h>

// ============================================================================================================== ||
// ============================================== Global Variables ============================================== ||
// ============================================================================================================== ||

char *dataToSend = '\0';
char *sendSpace = " ";

// ============================================================================================================== ||
// ================================================= Methods ==================================================== ||
// ============================================================================================================== ||

bool num_Check(char number[])
{
    int i = 0;
    //  Checks for negative numbers.
    if (number[0] == '-')
        i = 1;
    for (; number[i] != 0; i++)
    {
        if (!isdigit(number[i]))
            return false;
    }
    return true;
}

void error_message()
{
    fprintf(stdout, "Usage: controller <address> <port> {[-o out_file] [-log log_file]\n");
    fprintf(stdout, "[-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
    fflush(stdout);
    exit(1);
}

// ============================================================================================================== ||
// ============================================== Main Program Loop ============================================= ||
// ============================================================================================================== ||

int main(int argc, char *argv[])
{
    // ============================================================================================================== ||
    // ========================================== Checking Invalid Inputs =========================================== ||
    // ============================================================================================================== ||

    if ((argc == 1) | (argc == 2) | (argc == 3))
    {
        error_message();
    }

    if (strcmp(argv[1], "--help") == 0) // checks for the help argument.
    {
        error_message();
    }

    if (num_Check(argv[2]) == false) // checks if the port number is valid.
    {
        error_message();
    }

    if ((strcmp(argv[3], "-log") == 0) & (strcmp(argv[5], "-o") == 0))
    {
        error_message();
    }

    // ============================================================================================================== ||
    // ============================================== Socket Creation =============================================== ||
    // ============================================================================================================== ||
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);   // Socket creation
    struct hostent *he;

    // Checks for valid socket
    if (fd == -1)
    {
        perror("socket()");
        return 1;
    }

    if ((he = gethostbyname(argv[1])) == NULL)  // Gets the host info.
    {
        fprintf(stderr, "Could not connect to overseer at %s %d\n",argv[1],atoi(argv[2]));
        fflush(stderr);
        exit(1); 
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr = *((struct in_addr *)he->h_addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        fprintf(stderr, "Could not connect to overseer at %s %d\n",argv[1],atoi(argv[2]));
        fflush(stderr);
        exit(1); 
    }
    
    // This is the stuff that will be sent.
    for (int i = 1; i < argc; i++)
    {
        dataToSend = argv[i];
        send(fd, dataToSend, strlen(dataToSend), 0);    // Sends the data.
        send(fd, sendSpace, strlen(sendSpace),0);   // Sends a space to seperate the arguments.
    }
    
    if (shutdown(fd, SHUT_RDWR) == -1)
    {
        perror("shutdown()");
        return 1;
    }

    close(fd);  // closes the socket
    exit(1);
}

// ============================================================================================================== ||
// =============================================== End Program ================================================== ||
// ============================================================================================================== ||