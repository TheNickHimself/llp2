#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/time.h>

#define full // for advanced features

#define PORT 1234
#define MAX_DATA_SIZE 40

int main(int argc, char *argv[])
{
    char *argument1 = argv[1];
    FILE *file;

    if (argc >= 2)
    {
        char filepath[255]; // Adjust the size as needed
        strcpy(filepath, "/home/student/Documents/GitHub/llp2/sysHome/log/");
        strcat(filepath, argument1);
        file = fopen(filepath, "a+"); // Open file in append mode
    }
    else
    {
        file = fopen("/home/student/Documents/GitHub/llp2/sysHome/log/log.txt", "a+");
    }

    if (file == NULL)
    {
        fprintf(stderr, "ERROR: Log File!\n");
        return 1;
    }
    char buffer[30];

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

    printf("\nEnter Request (GETTIME): ");
    fgets(buffer, 29, stdin);
    for (int i = 0; i < strlen(buffer); i++)
    {
        buffer[i] = toupper(buffer[i]);
    }

    if (send(sockfd, buffer, strlen(buffer), 0) == -1)
    {
        printf("send() failed\n");
        return -3;
    }
    else
    {
        timeArr[0] = time(NULL); // client send time stamp
        buffer[strcspn(buffer, "\n")] = 0;
        fprintf(file, "<--OUT: %s:  %s\n", buffer, asctime(timeInfo));
    }

    printf("\nSENT:  %s\n\n", buffer);

    numbytes = recv(sockfd, buffer, MAX_DATA_SIZE, 0);
    if (numbytes == -1)
    {
        printf("recv() failed\n");
        return -4;
    }
    fprintf(file, "-->IN: %s\n", buffer);
    printf("Received:  %s\n\n", buffer);

    if (!strstr(buffer, "ERROR"))
    {

        /*Some magical wizard s**t is making it occasioanly receive the data
        and sometimes not.
        If it dont work idk rebuild it like 1-3 times for some reason it helps*/

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
        printf("Delay: %d\n\n", roundTrip);
        fprintf(file, "-->IN: Offset: %d\n", offset);
        fprintf(file, "-->IN: Delay: %d\n", roundTrip);
    }

#ifdef full
    struct timeval tv;
    tv.tv_sec = timeArr[3];
    tv.tv_usec = 0;

    if (settimeofday(&tv, NULL) == 0)
    {
        printf("System time updated successfully.\n");
    }
    else
    {
        perror("Error updating system time");
        return -1;
    }
#endif

    fprintf(file, "----------------------END OF LOG---------------------\n\n");

    fclose(file);
    return 0;
}