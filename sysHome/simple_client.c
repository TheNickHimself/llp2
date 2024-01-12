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
    int offset;
    int roundTrip;

    //time_t t0, t1, t2, t3;
    time_t timeArr[4];
    struct tm *timeInfo;
    // struct tm *local_time = localtime(&epoch_time);

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
    }

    printf("Data sent: %s", userInput);

    char arr[2][MAX_DATA_SIZE];
    time_t nowEp;

    for (size_t i = 0; i < 3; i++)
    {
        if (i == 2)
        {
            numbytes = recv(sockfd, (void *)&nowEp, MAX_DATA_SIZE, 0);
            if (numbytes == -1)
            {
                printf("recv() failed\n");
                return -4;
            }
        }

        if (i != 2)
        {
            numbytes = recv(sockfd, (void *)&arr[i], MAX_DATA_SIZE, 0);
            if (numbytes == -1)
            {
                printf("recv() failed\n");
                return -4;
            }
        }
    }
    printf("Received 0: %s\n", arr[0]);
    printf("Received 1: %s\n", arr[1]);
    printf("Received Ep: %ld\n", nowEp);

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
    timeArr[3] = time(NULL) + (10 * 65);


    for (size_t i = 0; i < 4; i++)
    {
        timeInfo = localtime(&timeArr[i]);
        printf("t%ld: %ld %s\n", i, timeArr[i], asctime(timeInfo));
    }

    close(sockfd);

    offset = ((timeArr[1] - timeArr[0]) + (timeArr[2] - timeArr[3])) / 2;
    roundTrip = (timeArr[3] - timeArr[0]) - (timeArr[2] - timeArr[1]);

    printf("Offset: %d\n", offset);
    printf("Delay: %d\n", roundTrip);

    return 0;
}
