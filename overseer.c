#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>

#define NUM_HANDLER_THREADS 5   // 5 threads in the thread pool.
#define size_array 2048         // this is the size of the buffers.

// ============================================================================================================== ||
// ============================================== Global Variables ============================================== ||
// ============================================================================================================== ||

char buf[size_array];
int bytes_recieved = 0;
int clientfd = -1;
struct sockaddr clientaddr;
socklen_t clientaddr_len;
int count = 0;
int fd = -1;
int position = 1;
int num_requests = 0; // number of pending requests.
int quit = 0;
int second_count = 0;
int n = 0;
int wait_count = 0;

char *argument_list[size_array] = { NULL };
char *arguments_to_pass[size_array] = { NULL };
char delim[] = " ";
char *ptr;
char *random_arg;

bool dash_o_output = false;
bool dash_log_output = false;
bool dash_o_output2 = false;
bool dash_log_output2 = false;
bool temp_key = false;
char *recieved_items;
char *ip_address;
char *filename;
char *output_type;
char *output_type2;
char *filepath;
char *filepath2;
char *termination_time;
int pid_to_kill;
pid_t pid;
pid_t pid_before;
pid_t pid_after;

pthread_mutex_t request_mutex;
pthread_mutex_t quit_mutex;
pthread_cond_t got_request;

// ============================================================================================================== ||
// ================================================= Methods ==================================================== ||
// ============================================================================================================== ||

// single request format.
struct request
{
    void (*func)(void *);
    void *data;
    struct request *next; // pointer to next request, if none = NULL.
};

struct request *requests = NULL;     // head of linked list request.
struct request *last_request = NULL; // pointer to last request.

void add_request(void (*func)(void *),
                 void *data,
                 pthread_mutex_t *p_mutex,
                 pthread_cond_t *p_cond_var)
{
    struct request *a_request; // pointer to newly added request
    a_request = (struct request *)malloc(sizeof(struct request));   // create structure with new request
    if (!a_request)
    { // malloc failed.
        fprintf(stderr, "add_request: out of memory\n");
        fflush(stderr);
        exit(1);
    }
    a_request->func = func;
    a_request->data = data;
    a_request->next = NULL;

    pthread_mutex_lock(p_mutex); // lock the mutex for exclusive access.

    if (num_requests == 0)      // add new request to the end of the list, updating list.
    {
        requests = a_request;
        last_request = a_request;
    }
    else
    {
        last_request->next = a_request;
        last_request = a_request;
    }
    num_requests++;

    // unlocking the mutex
    pthread_mutex_unlock(p_mutex);
    pthread_cond_signal(p_cond_var);
}

struct request *get_request()
{
    struct request *a_request; // pointer to request.

    if (num_requests > 0)
    {
        a_request = requests;
        requests = a_request->next;
        if (requests == NULL)
        {
            last_request = NULL;
        }
        num_requests--;
    }
    else
    {
        a_request = NULL;
    }
    return a_request;
}

void handle_request(struct request *a_request, int thread_id)
{
    a_request->func(a_request->data);
    fflush(stdout);
}

void *handle_requests_loop(void *data)
{
    struct request *a_request;      // pointer to a request.
    int thread_id = *((int *)data); // thread identifying number.

    // lock the mutex, to allow exclusive access.
    pthread_mutex_lock(&request_mutex);

    int running = 1;
    while (running)
    {
        if (num_requests > 0)
        { /* a request is pending */
            a_request = get_request();
            if (a_request)
            { /* got a request - handle it and free it */
                /* unlock mutex - so other threads would be able to handle */
                /* other reqeusts waiting in the queue paralelly.          */
                pthread_mutex_unlock(&request_mutex);
                handle_request(a_request, thread_id);
                free(a_request);
                /* and lock the mutex again. */
                pthread_mutex_lock(&request_mutex);
            }
        }
        else
        {
            /* wait for a request to arrive. note the mutex will be */
            /* unlocked here, thus allowing other threads access to */
            /* requests list.                                       */

            pthread_cond_wait(&got_request, &request_mutex);
            /* and after we return from pthread_cond_wait, the mutex  */
            /* is locked again, so we don't need to lock it ourselves */
        }
        pthread_mutex_lock(&quit_mutex);
        if (quit) running = 0;
        pthread_mutex_unlock(&quit_mutex);
    }
    pthread_mutex_unlock(&request_mutex);
    return NULL;
}

int file_desc;

void output_to_file(void *arg)
{
    file_desc = -1;
    file_desc = open(filepath, O_WRONLY | O_CREAT | S_IRUSR);  // Opens as write only and creates newfile if needed.
    if (file_desc == -1)
    {
        printf("Could not print to file.");
        fflush(stdout);
    }
    dup2(file_desc, STDOUT_FILENO);
    close(file_desc);
}

void connecting_message(void *arg)  // Connection message.
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(stdout, "%d-%02d-%02d %02d:%02d:%02d - connection received from %s\n",
                        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                        ip_address);
    fflush(stdout);
}

void filepath_connection(void *arg)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    if (dash_log_output == true){freopen(filepath, "a+", stdout);}
    if (dash_log_output2 == true){freopen(filepath2, "a+", stdout);}
    fprintf(stdout, "%d-%02d-%02d %02d:%02d:%02d - attempting to execute %s\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
            filename);
    if (dash_log_output == true){freopen ("/dev/tty", "a", stdout);}
    if (dash_log_output2 == true){freopen ("/dev/tty", "a", stdout);}
    fflush(stdout);
    while(1)
    {
        if (access(filename, F_OK) != -1)   // Checks if valid file to execute.
        {
            // file exists
            pid = fork();
            if (pid < 0)
            {
                perror("fork failed\n");
                exit(1);
            }
            if (pid == 0)
            {
                // Child Process
                if (dash_o_output == true){freopen(filepath, "a+", stdout);}
                if (dash_o_output2 == true){freopen(filepath, "a+", stdout);}
                execvp(filename, arguments_to_pass);
                if (dash_o_output == true){freopen ("/dev/tty", "a", stdout);}
                if (dash_o_output2 == true){freopen ("/dev/tty", "a", stdout);}
                fflush(stdout);
                exit(0);
                break;
            }
            if (pid > 0)
            {
                // Parent Process
                wait(NULL);
                time_t t = time(NULL);
                struct tm tm = *localtime(&t);
                if (dash_log_output == true){freopen(filepath, "a+", stdout);}
                if (dash_log_output2 == true){freopen(filepath2, "a+", stdout);}
                fprintf(stdout, "%d-%02d-%02d %02d:%02d:%02d - %s has been executed with pid %d\n",
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                    filename, pid);
                fprintf(stdout, "%d-%02d-%02d %02d:%02d:%02d - %d has terminated with status code %d\n",
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                    pid, WEXITSTATUS(pid));
                if (dash_log_output == true){freopen ("/dev/tty", "a", stdout);}
                if (dash_log_output2 == true){freopen ("/dev/tty", "a", stdout);}
                fflush(stdout);
                break;
            }
        }
        else
        {
            if (dash_log_output == true){freopen(filepath, "a+", stdout);}
            if (dash_log_output2 == true){freopen(filepath2, "a+", stdout);}
            fprintf(stdout, "%d-%02d-%02d %02d:%02d:%02d - could not execute %s\n",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                filename);
            if (dash_log_output == true){freopen ("/dev/tty", "a", stdout);}
            if (dash_log_output2 == true){freopen ("/dev/tty", "a", stdout);}
            fflush(stdout);
            break;
        }
        break;
        
    }
    
}

void termination_signal(void *arg)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    /*
    if (dash_log_output == true){freopen(filepath, "a+", stdout);}
    if (dash_log_output2 == true){freopen(filepath2, "a+", stdout);}
    fprintf(stdout, "%d-%02d-%02d %02d:%02d:%02d - %d has terminated with status code %d\n",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
        pid, WEXITSTATUS(pid));
    if (dash_log_output == true){freopen ("/dev/tty", "a", stdout);}
    if (dash_log_output2 == true){freopen ("/dev/tty", "a", stdout);}
    */
    fflush(stdout);
    kill(pid, SIGTERM);
    if (kill(pid, 0))
    {
        // means the process is not dead - (SIGKILL will force the process to die in 5 seconds.)
        sleep(5);
        if (dash_log_output == true){freopen(filepath, "a+", stdout);}
        if (dash_log_output2 == true){freopen(filepath2, "a+", stdout);}
        fprintf(stdout, "%d-%02d-%02d %02d:%02d:%02d - sent SIGKILL to pid %d\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
            pid);
        if (dash_log_output == true){freopen ("/dev/tty", "a", stdout);}
        if (dash_log_output2 == true){freopen ("/dev/tty", "a", stdout);}
        fflush(stdout);
        kill(pid, SIGKILL);
    }
}

void sig_handler(int signum)
{
    termination_signal(random_arg);
}

void for_loop_threaded(void *arg)
{
    if (bytes_recieved == -1)
    {
        perror("recv()");
        exit(1);
    }
    else if (bytes_recieved == 0)
    {
        exit(1);    // connection closed by peer.
    }
    else
    {
        buf[bytes_recieved] = '\0';
        ptr = strtok(buf, delim);
        
        for(int i = 1; i < (sizeof(argument_list)/sizeof(argument_list[0])); i++)
        {
            if (ptr != NULL)
            {
                argument_list[i] = ptr;
                ptr = strtok(NULL, delim);
                count++;
                position++;
            }
        }
        argument_list[position + 1] = NULL; // Appends null to the end of the list.
        if ((strcmp(argument_list[3], "-o") == 0) | (strcmp(argument_list[3], "-log") == 0) | (strcmp(argument_list[3], "-t") == 0))
        {
            if ((((strcmp(argument_list[3], "-o") == 0) & (strcmp(argument_list[5], "-log") == 0))) | ((strcmp(argument_list[3], "-log") == 0) & (strcmp(argument_list[5], "-t") == 0)))
            {
                if ((strcmp(argument_list[3], "-o") == 0) & (strcmp(argument_list[5], "-log") == 0))
                {
                    if ((strcmp(argument_list[7], "-t") == 0))
                    {
                        dash_log_output2 = true;
                        dash_o_output2 = true;
                        ip_address = argument_list[1];
                        output_type = argument_list[3];
                        filepath = argument_list[4];
                        output_type2 = argument_list[5];
                        filepath2 = argument_list[6];
                        termination_time = argument_list[8];
                        filename = argument_list[9];
                        connecting_message(arg);
                        signal(SIGALRM, sig_handler);
                        time_t t = time(NULL);
                        struct tm tm = *localtime(&t);
                        if (dash_log_output == true){freopen(filepath, "a+", stdout);}
                        if (dash_log_output2 == true){freopen(filepath2, "a+", stdout);}
                        fprintf(stdout, "%d-%02d-%02d %02d:%02d:%02d - sent SIGTERM to pid %d\n",
                            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                            getpid());
                        if (dash_log_output == true){freopen ("/dev/tty", "a", stdout);}
                        if (dash_log_output2 == true){freopen ("/dev/tty", "a", stdout);}
                        fflush(stdout);
                        int time_to_term = atoi(termination_time);
                        alarm(time_to_term);
                        while (n <= count)
                        {
                            arguments_to_pass[n] = argument_list[9 + n];
                            n++;
                        }
                        filepath_connection(arg);
                    }
                    else
                    {
                        dash_log_output2 = true;
                        dash_o_output2 = true;
                        ip_address = argument_list[1];
                        output_type = argument_list[3];
                        filepath = argument_list[4];
                        output_type2 = argument_list[5];
                        filepath2 = argument_list[6];
                        filename = argument_list[7];
                        connecting_message(arg);
                        signal(SIGALRM, sig_handler);
                        int time_to_term = 10;
                        alarm(time_to_term);
                        while (n <= count)
                        {
                            arguments_to_pass[n] = argument_list[7 + n];
                            n++;
                        }
                        filepath_connection(arg);
                        alarm(0);   // stops alarm
                    }
                }
                if ((strcmp(argument_list[3], "-log") == 0) & (strcmp(argument_list[5], "-t") == 0))
                {
                    dash_log_output2 = true;
                    ip_address = argument_list[1];
                    output_type = argument_list[3];
                    filepath2 = argument_list[4];
                    termination_time = argument_list[6];
                    filename = argument_list[7];
                    connecting_message(arg);
                    signal(SIGALRM, sig_handler);
                    time_t t = time(NULL);
                    struct tm tm = *localtime(&t);
                    if (dash_log_output == true){freopen(filepath, "a+", stdout);}
                    if (dash_log_output2 == true){freopen(filepath2, "a+", stdout);}
                    fprintf(stdout, "%d-%02d-%02d %02d:%02d:%02d - sent SIGTERM to pid %d\n",
                        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                        getpid());
                    if (dash_log_output == true){freopen ("/dev/tty", "a", stdout);}
                    if (dash_log_output2 == true){freopen ("/dev/tty", "a", stdout);}
                    fflush(stdout);
                    int time_to_term = atoi(termination_time);
                    alarm(time_to_term);
                    while (n <= count)
                    {
                        arguments_to_pass[n] = argument_list[7 + n];
                        n++;
                    }
                    filepath_connection(arg);
                }   
            }
            else
            {
                if (strcmp(argument_list[3], "-log") == 0)
                {
                    dash_log_output = true;
                    ip_address = argument_list[1];
                    output_type = argument_list[3];
                    filepath = argument_list[4];
                    filename = argument_list[5];
                    connecting_message(arg);
                    signal(SIGALRM, sig_handler);
                    int time_to_term = 10;
                    alarm(time_to_term);
                    while (n <= count)
                    {
                        arguments_to_pass[n] = argument_list[5 + n];
                        n++;
                    }
                    filepath_connection(arg);
                    alarm(0);   // stops alarm
                }
                if (strcmp(argument_list[3], "-o") == 0)
                {
                    dash_o_output = true;
                    ip_address = argument_list[1];
                    output_type = argument_list[3];
                    filepath = argument_list[4];
                    filename = argument_list[5];
                    connecting_message(arg);
                    signal(SIGALRM, sig_handler);
                    int time_to_term = 10;
                    alarm(time_to_term);
                    while (n <= count)
                    {
                        arguments_to_pass[n] = argument_list[5 + n];
                        n++;
                    }
                    filepath_connection(arg);
                    alarm(0);   // stops alarm
                }
                if (strcmp(argument_list[3], "-t") == 0)
                {
                    ip_address = argument_list[1];
                    output_type = argument_list[3];
                    termination_time = argument_list[4];
                    filename = argument_list[5];
                    connecting_message(arg);
                    signal(SIGALRM, sig_handler);
                    time_t t = time(NULL);
                    struct tm tm = *localtime(&t);
                    if (dash_log_output == true){freopen(filepath, "a+", stdout);}
                    if (dash_log_output2 == true){freopen(filepath2, "a+", stdout);}
                    fprintf(stdout, "%d-%02d-%02d %02d:%02d:%02d - sent SIGTERM to pid %d\n",
                        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                        getpid());
                    if (dash_log_output == true){freopen ("/dev/tty", "a", stdout);}
                    if (dash_log_output2 == true){freopen ("/dev/tty", "a", stdout);}
                    fflush(stdout);
                    int time_to_term = atoi(termination_time);
                    alarm(time_to_term);
                    while (n <= count)
                    {
                        arguments_to_pass[n] = argument_list[5 + n];
                        n++;
                    }
                    filepath_connection(arg);
                }
            }
        }
        else
        {
            ip_address = argument_list[1];
            filename = argument_list[3];
            connecting_message(arg);
            signal(SIGALRM, sig_handler);
            int time_to_term = 10;
            alarm(time_to_term);
            while (n <= count)
            {
                arguments_to_pass[n] = argument_list[3 + n];
                n++;
            }
            filepath_connection(arg);
            alarm(0);   // stops alarm
        }
        dash_log_output = false;
        dash_log_output2 = false;
        dash_o_output2 = false;
        dash_o_output = false;
        temp_key = false;
    }
    // end of for(;;)
}

void handler(int num)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(stdout, "\n%d-%02d-%02d %02d:%02d:%02d - received SIGINT\n",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fprintf(stdout, "%d-%02d-%02d %02d:%02d:%02d - Cleaning up and terminating...\n",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fflush(stdout);
    fflush(stderr);
    close(clientfd);
    close(fd);
    kill(pid_before, SIGKILL);
    kill(pid, SIGKILL);
    exit(0);
}

// ============================================================================================================== ||
// ============================================ Main Program Loop =============================================== ||
// ============================================================================================================== ||

int main(int argc, char *argv[])
{
    // handling SIGINT ^c
    signal(SIGINT, handler);

    int i;
    int thr_id[NUM_HANDLER_THREADS];
    pthread_t p_threads[NUM_HANDLER_THREADS];

    pthread_mutex_init(&request_mutex, NULL);
    pthread_mutex_init(&quit_mutex, NULL);
    pthread_cond_init(&got_request, NULL);

    // initialise the server bit.
    fd = socket(AF_INET, SOCK_STREAM, 0);   // socket creation

    if (fd == -1)
    {
        perror("socket()");
        return 1;
    }

    int opt_enable = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt_enable, sizeof(opt_enable));
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt_enable, sizeof(opt_enable));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1])); 

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind()");
        return 1;
    }

    if (listen(fd, 10) == -1)
    {
        perror("listen()");
        return 1; 
    }

    for (i = 0; i < NUM_HANDLER_THREADS; i++)
    {
        thr_id[i] = i;
        pthread_create(&p_threads[i], NULL, handle_requests_loop, (void *)&thr_id[i]);
    }
    // Ensure memory is all clean first.
    memset(buf, 0, sizeof buf);
    memset(arguments_to_pass, 0, sizeof(arguments_to_pass));
    memset(argument_list, 0, sizeof(argument_list));

    for(;;)
    {
        clientfd = accept(fd, &clientaddr, &clientaddr_len);

        if (clientfd == -1)
        {
            perror("accept()");
            exit(1);
        }

        bytes_recieved = recv(clientfd, buf, 2047, 0);
        add_request(for_loop_threaded, NULL, &request_mutex, &got_request);
        if (shutdown(clientfd, SHUT_RDWR) == -1)
        {
            perror("shutdown()");
            exit(1);
        }
        close(clientfd);
    }
    pthread_mutex_lock(&quit_mutex);
    quit = 1;
    pthread_mutex_unlock(&quit_mutex);
    pthread_cond_broadcast(&got_request);

    for (i = 0; i < NUM_HANDLER_THREADS; i++)
    {
        pthread_join(p_threads[i], NULL);
    }
    close(fd);  // closes the socket
    return 0;
}
// ============================================================================================================== ||
// =============================================== End Program ================================================== ||
// ============================================================================================================== ||