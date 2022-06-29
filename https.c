#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#define PORT 8080
#define CALLOC_WITH_CHK(var, count, type, msg) if(NULL == (var = (type *)calloc(count,sizeof(type)))) { \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }

typedef struct t_s {
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    int* stack_ar;
    volatile int idx;
} t_stack;

typedef struct t_i {
    pthread_cond_t  cond;
    pthread_mutex_t mutex;
    int             id;
    volatile int    fd;
} t_info;

pthread_t *threads = NULL;
t_info *infos = NULL;
t_stack stack = {.lock = PTHREAD_MUTEX_INITIALIZER, .cond = PTHREAD_COND_INITIALIZER, .stack_ar = NULL, .idx = -1};
struct timeval tv = {.tv_sec=30L, .tv_usec=0};



void assign_worker(int fd) {
    int tidx = -1;
    t_info *info;
    pthread_mutex_lock(&stack.lock);
    while(stack.idx == -1) {
        printf("waiting for condition in assign worker\n");
        pthread_cond_wait(&stack.cond, &stack.lock);
    }

    info = &infos[stack.stack_ar[stack.idx --]];
    printf("got %d\n", info->id);
    pthread_mutex_unlock(&stack.lock);

    pthread_mutex_lock(&info->mutex);
    info->fd = fd;
    printf("assiging fd %d to  %d\n", info->fd, info->id);
    pthread_cond_signal(&info->cond);
    pthread_mutex_unlock(&info->mutex);
}

void free_up_worker(t_info *info) {
    pthread_mutex_lock(&stack.lock);
    stack.stack_ar[++stack.idx] = info->id;
    pthread_cond_signal(&stack.cond);
    pthread_mutex_unlock(&stack.lock);
    pthread_mutex_unlock(&info->mutex);
}

void *handle_request(void *arg) {

    t_info *info = (t_info *)arg;
    int recvb = 0;
    char method[2048];
    char *msg = "HTTP/1.1 200 OK\r\nContent-Length: 24\r\nContent-Type: application/json\r\n\r\n{\"message\":\"it works!!\"}";
    char *html = "HTTP/1.1 200 OK\r\nContent-Length: 68\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><body><p>This text is normal.</p></body></html>";

    while(1) {
        pthread_mutex_lock(&info->mutex);
        while(info->fd == -1) {
            printf("waiting on fd update %d\n", info->fd);
            pthread_cond_wait(&info->cond, &info->mutex);
        }
        printf("got new fd %d\n", info->fd);

        do {
            recvb = recv(info->fd, method, 2048, 0);

            if (recvb > 0 ) {
                switch (method[0]) {
                case 'C' :
                    printf("Recived a CONNECT method");
                    break;
                case 'D' :
                    printf("Recived a DELETE method");
                    break;
                case 'G' :
                    send(info->fd, html, strlen(html), 0);
                    // write(info->fd, msg, strlen(msg));
                    printf("Recived a GET method\n");
                    // printf("%s", method);
                    break;
                case 'H' :
                    printf("Recived a HEAD method");
                    break;
                case 'O' :
                    printf("Recived a OPTIONS method");
                    break;
                case 'P' :
                    switch(method[1]) {
                    case 'A':
                    printf("Recived a PATCH method");
                    break;
                    case 'O':
                    printf("Recived a POST method");
                    break;
                    case 'U':
                    printf("Recived a PUT method");
                    break;
                    }
                
                default:
                    break;
                }
            }
        }while(recvb > 0 || !(errno == EWOULDBLOCK || errno == EAGAIN));
        
        close(info->fd);
        info->fd = -1;
        free_up_worker(info);
    }
}

void create_workers(int num_workers) {
    int i = 0;
    
    CALLOC_WITH_CHK(threads, num_workers, pthread_t, "create tpool")
    CALLOC_WITH_CHK(infos, num_workers, t_info, "allocating thread args")
    CALLOC_WITH_CHK(stack.stack_ar, num_workers, int, "allocating stack")
    stack.idx = num_workers - 1;

    for ( i = 0 ; i < num_workers ; i++ ) {
        infos[i].fd = -1;
        infos[i].id = i;
        pthread_cond_init(&infos[i].cond, NULL);
        pthread_mutex_init(&infos[i].mutex, NULL);
        pthread_create(&threads[i], NULL, handle_request, &infos[i]);
        stack.stack_ar[i] = i;
    }
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    int optval = 1;
    int optlen = sizeof(optval);

    
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    optval = 10;
    if (setsockopt(server_fd, SOL_TCP, TCP_KEEPIDLE, (void *)&optval, sizeof(optval))) { perror("ERROR: setsocketopt(), SO_KEEPIDLE"); exit(0); };

    optval = 5;
    if (setsockopt(server_fd, SOL_TCP, TCP_KEEPCNT, (void *)&optval, sizeof(optval))) { perror("ERROR: setsocketopt(), SO_KEEPCNT"); exit(0); };

    optval = 5;
    if (setsockopt(server_fd, SOL_TCP, TCP_KEEPINTVL, (void *)&optval, sizeof(optval))) { perror("ERROR: setsocketopt(), SO_KEEPINTVL"); exit(0); };


    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, SOMAXCONN) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    create_workers(10);
    optval = 1;
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        if(setsockopt(new_socket, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
            perror("setsockopt(keepalive)");
            close(new_socket);
            exit(EXIT_FAILURE);
        }
        if(setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0) {
            perror("setsockopt(timeout)");
            close(new_socket);
            exit(EXIT_FAILURE);
        }
        printf("accepted connection with fd = %d\n", new_socket);
        assign_worker(new_socket);
        
    }
    return 0;
}