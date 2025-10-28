#define BUFFER_SIZE 1024
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "common.h"
#include <sys/select.h>
#include <sys/time.h>

void manager_client(int sd){
    while(1){
        char cmd[BUFFER_SIZE];
        printf("Enter your choice : \n 1. View Loan Applications \n 2. Activate Customer Acc \n 3. Deactivate Customer Acc \n 4. Assign Loan Applications to Employee \n 5.Remove Customer \n 6.View Employee Details \n 7. Change Your Password \n 8. View Feedback \n 9. Logout \n 10. Exit \n");
        scanf("%s", cmd);
       if(write(sd, cmd, strlen(cmd) + 1) < 0){
            continue;
        }
        int choice;
        sscanf(cmd, "%d", &choice);
        char BUFF[BUFFER_SIZE];

        switch(choice){
            case 1:
            // view pending loan applications
            {
                 fd_set read_fds;
                struct timeval timeout;
                int retval;
                 while(1){
                    FD_ZERO(&read_fds);
                    FD_SET(sd, &read_fds);
                    timeout.tv_sec = 10;  // 10 seconds timeout
                    timeout.tv_usec = 0;
                    retval = select(sd + 1, &read_fds, NULL, NULL, &timeout);
                    if (retval == -1) {
                        perror("select()");
                        break;
                    } else if (retval == 0) {
                        printf("All Records Done.\n");
                        break;
                    }
                    memset(BUFF, 0, sizeof(BUFF));
                    int bytes_read = read(sd, BUFF, sizeof(BUFF) - 1);
                    if (bytes_read <= 0) {
                    if (bytes_read == 0) printf("Server disconnected.\n");
                    else perror("read error");
                    break; // Exit the inner reading loop
                }
                    BUFF[bytes_read] = '\0';
                    trim_whitespace(BUFF);
                    BUFF[strcspn(BUFF, "\r\n")] = '\0';
                    if(strcmp(BUFF, "END") == 0){
                        printf("No more loan applications.\n");
                        break;
                    }
                    printf("%s\n", BUFF);
                    memset(BUFF, 0, sizeof(BUFF));
                }
            }
                break;

            case 2: {
                // Activate Customer Account
                char buf[BUFFER_SIZE];
                int acc_no;
                int id;
                printf("Enter Account Number to be activated : \n");
                scanf("%d", &acc_no);
                printf("Enter your Customer ID : \n");
                scanf("%d", &id);
                snprintf(buf, sizeof(buf), "%d %d", acc_no, id);
                write(sd, buf, strlen(buf) + 1);
                memset(buf, 0, sizeof(buf));
                read(sd, buf, sizeof(buf));
                printf("%s\n", buf);
                break;
            }

            case 3: {
                // Deactivate Customer Account
                char buf[BUFFER_SIZE];
                int acc_no;
                int id;
                printf("Enter Account Number to be deactivated : \n");
                scanf("%d", &acc_no);
                printf("Enter your Customer ID : \n");
                scanf("%d", &id);
                snprintf(buf, sizeof(buf), "%d %d", acc_no, id);
                write(sd, buf, strlen(buf) + 1);
                memset(buf, 0, sizeof(buf));
                read(sd, buf, sizeof(buf));
                printf("%s\n", buf);
                break;
            }

            case 4: {
                // Assign Loan Application to Employee
                char buf[BUFFER_SIZE];
                int loan_id;
                int empid;
                printf("Enter Loan ID to be assigned : \n");
                scanf("%d", &loan_id);
                printf("Enter Employee ID to whom the loan is assigned : \n");
                scanf("%d", &empid);
                snprintf(buf, sizeof(buf), "%d %d", loan_id, empid);
                write(sd, buf, strlen(buf) + 1);
                memset(buf, 0, sizeof(buf));
                read(sd, buf, sizeof(buf));
                printf("%s\n", buf);
                break;
            }
            // Remove Customer
            case 5:{
                char buf[BUFFER_SIZE];
                        int acc_no;
                        int id;
                        printf("Enter Account Number : \n");
                        scanf("%d",&acc_no);
                        printf("Enter ID : \n");
                        scanf("%d",&id);
                        snprintf(buf,sizeof(buf),"%d %d",acc_no,id);
                        write(sd,buf,strlen(buf) + 1);
                        memset(buf,0,sizeof(buf));
                        read(sd,buf,sizeof(buf));
                        printf("%s\n",buf);
                    break;
            }
            // View Employee Details
            case 6:
            {
                fd_set read_fds;
                struct timeval timeout;
                int retval;
                 while(1){
                    FD_ZERO(&read_fds);
                    FD_SET(sd, &read_fds);
                    timeout.tv_sec = 10;  // 10 seconds timeout
                    timeout.tv_usec = 0;
                    retval = select(sd + 1, &read_fds, NULL, NULL, &timeout);
                    if (retval == -1) {
                        perror("select()");
                        break;
                    } else if (retval == 0) {
                        printf("All Records Done.\n");
                        break;
                    }
                    memset(BUFF, 0, sizeof(BUFF));
                    int bytes_read = read(sd, BUFF, sizeof(BUFF) - 1);
                    if (bytes_read <= 0) {
                    if (bytes_read == 0) printf("Server disconnected.\n");
                    else perror("read error");
                    break; 
                }
                    BUFF[bytes_read] = '\0';
                    BUFF[strcspn(BUFF, "\r\n")] = '\0';
                    trim_whitespace(BUFF);
                    if(strcmp(BUFF, "END") == 0){
                        break;
                    }
                    printf("%s\n", BUFF);
                }
                break;
            }
            case 7: {
                // Change Password
                char buf[BUFFER_SIZE];
                char newpass[13];
                printf("Enter New Password : \n");
                scanf("%12s", newpass);
                snprintf(buf, sizeof(buf), "%12s",newpass);
                write(sd, buf, strlen(buf) + 1);
                memset(buf, 0, sizeof(buf));
                read(sd, buf, sizeof(buf));
                printf("%s\n", buf);
                break;
            }

            case 8:
                // View Feedback
                int bytes_read;
                 fd_set read_fds;
                struct timeval timeout;
                int retval;
                 while(1){
                    FD_ZERO(&read_fds);
                    FD_SET(sd, &read_fds);
                    timeout.tv_sec = 10;  // 10 seconds timeout
                    timeout.tv_usec = 0;
                    retval = select(sd + 1, &read_fds, NULL, NULL, &timeout);
                    if (retval == -1) {
                        perror("select()");
                        break;
                    } else if (retval == 0) {
                        printf("All Records Done.\n");
                        break;
                    }
                    memset(BUFF, 0, sizeof(BUFF));
                    int bytes_read = read(sd, BUFF, sizeof(BUFF) - 1);
                    if (bytes_read <= 0) {
                    if (bytes_read == 0) printf("Server disconnected.\n");
                    else perror("read error");
                    break; // Exit the inner reading loop
                }
                    BUFF[bytes_read] = '\0';
                    BUFF[strcspn(BUFF, "\r\n")] = '\0';
                    trim_whitespace(BUFF);
                    if(strcmp(BUFF, "END") == 0){
                        break;
                    }
                    printf("%s\n", BUFF);
                    memset(BUFF, 0, sizeof(BUFF));
                }
                break;

            case 9: {
                // Logout
                read(sd, BUFF, sizeof(BUFF));
                printf("%s\n", BUFF);
                return;
            }

            default:
                // Exit
                close(sd);
                printf("Exiting...\n");
                return;
                break;
        }
    }
}
