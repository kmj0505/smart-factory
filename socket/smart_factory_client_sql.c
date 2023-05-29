/* base : KSH */
/* modified by JUB, KMJ */
/* This file contains SQLClient system */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <mysql/mysql.h>

#define BUF_SIZE 100
#define NAME_SIZE 20
#define ARR_CNT 4

void* send_msg(void* arg);
void* recv_msg(void* arg);
void error_handling(char* msg);

char name[NAME_SIZE] = "[Default]";
char msg[BUF_SIZE];

void finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);        
}

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread, mysql_thread;
    void* thread_return;

    if (argc != 4) {
        printf("Usage : %s <IP> <port> <name>\n", argv[0]);
        exit(1);
    }

    sprintf(name, "%s", argv[3]);

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    sprintf(msg, "[%s:PASSWD]", name);
    write(sock, msg, strlen(msg));
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);


    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);

    close(sock);
    return 0;
}


void* send_msg(void* arg)
{
    int* sock = (int*)arg;
    int str_len;
    int ret;
    fd_set initset, newset;
    struct timeval tv;
    char name_msg[NAME_SIZE + BUF_SIZE + 2];

    FD_ZERO(&initset);
    FD_SET(STDIN_FILENO, &initset);

    fputs("Input a message! [ID]msg (Default ID:ALLMSG)\n", stdout);
    while (1) {
        memset(msg, 0, sizeof(msg));
        name_msg[0] = '\0';
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        newset = initset;
        ret = select(STDIN_FILENO + 1, &newset, NULL, NULL, &tv);
        if (FD_ISSET(STDIN_FILENO, &newset))
        {
            fgets(msg, BUF_SIZE, stdin);
            if (!strncmp(msg, "quit\n", 5)) {
                *sock = -1;
                return NULL;
            }
            else if (msg[0] != '[')
            {
                strcat(name_msg, "[ALLMSG]");
                strcat(name_msg, msg);
            }
            else
                strcpy(name_msg, msg);
            if (write(*sock, name_msg, strlen(name_msg)) <= 0)
            {
                *sock = -1;
                return NULL;
            }
        }
        if (ret == 0)
        {
            if (*sock == -1)
                return NULL;
        }
    }
}

void* recv_msg(void* arg)
{
    MYSQL* conn;
    MYSQL_RES* res_ptr;
    MYSQL_ROW sqlrow;
    int res;
    char sql_cmd[200] = { 0 };
    char* host = "localhost";
    char* user = "iot03";
    char* pass = "raspberry";
    char* dbname = "Smart_Factory";

    int* sock = (int*)arg;
    int i;
    char* pToken;
    char* pArray[ARR_CNT] = { 0 };

    char name_msg[NAME_SIZE + BUF_SIZE + 1];
    int str_len;

    char Product_Name[10];
    int Count=0;
    int Goal=0;
    float Rate;
    conn = mysql_init(NULL);

    puts("MYSQL startup");
    if (!(mysql_real_connect(conn, host, user, pass, dbname, 0, NULL, 0)))
    {
        fprintf(stderr, "ERROR : %s[%d]\n", mysql_error(conn), mysql_errno(conn));
        exit(1);
    }
    else
        printf("Connection Successful!\n\n");

    while (1) {
        memset(name_msg, 0x0, sizeof(name_msg));
        str_len = read(*sock, name_msg, NAME_SIZE + BUF_SIZE);
        if (str_len <= 0)
        {
            *sock = -1;
            return NULL;
        }
        name_msg[str_len] = 0;
        fputs(name_msg, stdout);

        pToken = strtok(name_msg, "[:@]");
        i = 0;
        while (pToken != NULL)
        {
            pArray[i] = pToken;
            if ( ++i >= ARR_CNT)
                break;
            pToken = strtok(NULL, "[:@]");

        }
        if(!strcmp(pArray[2],"GET"))
        {
            char count_msg[50]={0};
            sprintf(sql_cmd, "SELECT * FROM Inventory");
            
            if (mysql_query(conn, sql_cmd)) 
            {
                finish_with_error(conn);
            }
            MYSQL_RES *result = mysql_store_result(conn);
            if (result == NULL) 
            {
                finish_with_error(conn);
            }
            MYSQL_ROW row;
            int product_count[4];
            int i = 0;
            while(row = mysql_fetch_row(result))
            {
                product_count[i++] = atoi(row[2]);
            }
            sprintf(count_msg, "[AND]DEV@CNT@%d@%d@%d@%d\n", product_count[0],
                            product_count[1],product_count[2],product_count[3]);

            printf("%s",count_msg);
            write(*sock, count_msg, strlen(count_msg));
        }
        else if(!strcmp(pArray[1],"DEV") && (i == 4)){
        
//            printf("DEV : %s\n",pArray[2]);
            strcpy(Product_Name, pArray[2]);
            sprintf(sql_cmd, "SELECT * FROM Inventory where Product_Name=\'%s\'",
                                                                    Product_Name);

            if (mysql_query(conn, sql_cmd)) 
            {
                finish_with_error(conn);
            }
            MYSQL_RES *result = mysql_store_result(conn);
            if (result == NULL) 
            {
                finish_with_error(conn);
            }
            int num_fields = mysql_num_fields(result);
            MYSQL_ROW row;
            row = mysql_fetch_row(result); 
            Count = atoi(row[2])+atoi(pArray[3]);
            Goal = atoi(row[3]);
            
              sprintf(sql_cmd,
                    "update Inventory set Count=%d, Rate_per=%.1lf where Product_Name=\'%s\'",
                                                    Count,(float)(Count*100/Goal), Product_Name);

//            printf("%s",sql_cmd);

            res = mysql_query(conn, sql_cmd);
            if (!res)
                printf("inserted %lu rows\n", (unsigned long)mysql_affected_rows(conn));
            else
                fprintf(stderr, "ERROR: %s[%d]\n", mysql_error(conn), mysql_errno(conn));
        }
        else{ 
            continue;
        }
    }
    mysql_free_result(res_ptr);
    mysql_close(conn);

}

void error_handling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
