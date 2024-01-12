#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define PORT 1234 /* the port client will be connecting to */
#define MSG "Hi i am a client\n"
#define MAX_DATA_SIZE 40 /* max number of bytes we can get at once */

int main(int argc, char *argv[])
{
    FILE *file;
    file = fopen("/home/student/Documents/GitHub/llp2/sysHome/log/log.txt", "a+"); // Open file in append mode

    if (file == NULL)
    {
        fprintf(stderr, "ERROR: Log File!\n");
        return 1;
    }
    fprintf(file, "\n----------------------START OF LOG---------------------\n");

    int offset;
    int roundTrip;

    // time_t t0, t1, t2, t3;
    time_t timeArr[4], timeLog;
    struct tm *timeInfo;
    timeInfo = localtime(&timeLog);

    int sockfd, numbytes;

    struct sockaddr_in srv_addr = {0};

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("socket() failed\n");
        return -1;
    }

    srv_addr.sin_family = AF_INET; /* host byte order */
    // srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    srv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    srv_addr.sin_port = htons(PORT); /* short, network byte order */

    if (connect(sockfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) == -1)
    {
        printf("connect() failed\n");
        return -2;
    }

    char userInput[30];
    printf("Enter Request (or Enter for GETTIME): ");
    fgets(userInput, 29, stdin);

    if (send(sockfd, userInput, strlen(userInput), 0) == -1)
    {
        printf("send() failed\n");
        return -3;
    }
    else
    {
        timeArr[0] = time(NULL); // client send time stamp
        userInput[strcspn(userInput, "\n")] = 0;
        fprintf(file, "<--OUT: %s:  %s\n", userInput, asctime(timeInfo));
    }

    printf("SENT:  %s\n\n", userInput);
    
    /*
    time_t nowEp;

    numbytes = recv(sockfd, (void *)&nowEp, MAX_DATA_SIZE, 0);
    if (numbytes == -1)
    {
        printf("recv() failed\n");
        return -4;
    }


    char arr[MAX_DATA_SIZE + 1];
    numbytes = recv(sockfd, (void *)&arr, MAX_DATA_SIZE, 0);
    if (numbytes == -1)
    {
        printf("recv() failed\n");
        return -4;
    }
    
    arr[strcspn(arr, "\n")] = 0;
    fprintf(file, "-->IN: %s:  %s", arr, asctime(timeInfo));
    printf("Replying to: %s\n", arr);
    
    fprintf(file, "-->IN: %ld:  %s", nowEp, asctime(timeInfo));
    printf("Received Epoch: %ld\n", nowEp);
    */

    numbytes = recv(sockfd, (void *)&timeArr[1], MAX_DATA_SIZE, 0);
    if (numbytes == -1)
    {
        printf("recv() failed\n");
        return -4;
    }
    numbytes = recv(sockfd, (void *)&timeArr[2], MAX_DATA_SIZE, 0);
    if (numbytes == -1)
    {
        printf("recv() failed\n");
        return -4;
    }
    timeArr[3] = time(NULL) + (10 * 68);

    printf("Received: TIME t1:%ld  t2:%ld\n\n", timeArr[1], timeArr[2]);

    for (size_t i = 0; i < 4; i++)
    {
        timeInfo = localtime(&timeArr[i]);
        printf("t%ld: %ld %s\n", i, timeArr[i], asctime(timeInfo));
        fprintf(file, "-->IN: t%ld: %ld %s\n", i, timeArr[i], asctime(timeInfo));
    }

    close(sockfd);

    offset = ((timeArr[1] - timeArr[0]) + (timeArr[2] - timeArr[3])) / 2;
    roundTrip = (timeArr[3] - timeArr[0]) - (timeArr[2] - timeArr[1]);

    printf("Offset: %d\n", offset);
    printf("Delay: %d\n", roundTrip);
    fprintf(file, "-->IN: Offset: %d\n", offset);
    fprintf(file, "-->IN: Delay: %d\n", roundTrip);

    fprintf(file, "----------------------END OF LOG---------------------\n\n");
    fclose(file);
    return 0;
}

/*
int logger(char (*msgPtr)[1024])
{
    time_t timeLog = time(NULL);
    struct tm *timeLogInf;

    timeLogInf = localtime(&timeLog);

    FILE *file;

    file = fopen("/home/student/Documents/GitHub/llp2/sysHome/log/log.txt", "a+"); // Open file in append mode


    if (file == NULL)
    {
        fprintf(stderr, "Error opening file!\n");
        return 1;
    }

    //fprintf(file, "%ld : %s", timeLog, asctime(timeLogInf));
    fprintf(file, "%s:  %s", *msgPtr, asctime(timeLogInf));

    fclose(file);

    return 0;
}
*/