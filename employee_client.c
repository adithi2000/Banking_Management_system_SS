#define BUFFER_SIZE 1024
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "common.h"
#include <sys/select.h>
#include <sys/time.h>

void employee_client(int sd){
    while(1){
        char cmd[BUFFER_SIZE];
        printf("Enter your choice : \n 1. Create Customer \n 2. Modify Customer Name \n 3. Modify Customer Branch \n 4. View Loan Applications \n 5. Approve Loan Applications \n 6.Change Your Password \n 7.See Customer Transactions \n 8. Logout \n 9. Exit \n");
        scanf("%s", cmd);
        if(write(sd, cmd, strlen(cmd) + 1) < 0){
            continue;
        }
        int choice;
        sscanf(cmd, "%d", &choice);
        char BUFF[BUFFER_SIZE];

        switch(choice){
            case 1: {
                // Create Customer
                int id;
                char pass[13];
                char name[20];
                char branch[20];
                printf("Enter Customer ID:\n");
                scanf("%d", &id);
                printf("Enter Password:\n");
                scanf("%12s", pass);
                printf("Enter Name:\n");
                scanf("%s", name);
                printf("Enter Branch:\n");
                scanf("%3s", branch);
                snprintf(BUFF, sizeof(BUFF), "%d %s %s %s", id, pass, name, branch);
                write(sd, BUFF, strlen(BUFF) + 1);
                read(sd, BUFF, sizeof(BUFF));
                printf("%s", BUFF);
                break;
            }

            case 2: {
                // Modify Customer Name
                int acc_no;
                int id;
                char name[20];
                printf("Enter Customer Account Number:\n");
                scanf("%d", &acc_no);
                printf("Enter Customer ID:\n");
                scanf("%d", &id);
                printf("Enter New Name:\n");
                scanf("%s", name);
                snprintf(BUFF, sizeof(BUFF), "%d %d %s", acc_no, id, name);
                write(sd, BUFF, strlen(BUFF) + 1);
                read(sd, BUFF, sizeof(BUFF));
                printf("%s", BUFF);
                break;
            }

            case 3: {
                // Modify Customer Branch
                int acc_no;
                int id;
                char branch[20];
                printf("Enter Customer Account Number:\n");
                scanf("%d", &acc_no);
                printf("Enter Customer ID:\n");
                scanf("%d", &id);
                printf("Enter New Branch:\n");
                scanf("%3s", branch);
                snprintf(BUFF, sizeof(BUFF), "%d %d %s", acc_no, id, branch);
                write(sd, BUFF, strlen(BUFF) + 1);
                read(sd, BUFF, sizeof(BUFF));
                printf("%s", BUFF);
                break;
            }

            case 4: {
                // View Loan Applications
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
                        break;
                    }
                    printf("%s\n", BUFF);
                    fflush(stdout);
                }
                break;
            }

            case 5: {
                // Approve Loan
                int acc_no;
                int loan_id;
                printf("Enter Customer Account Number:\n");
                scanf("%d", &acc_no);
                printf("Enter Loan ID:\n");
                scanf("%d", &loan_id);
                snprintf(BUFF, sizeof(BUFF), "%d %d", acc_no, loan_id);
                write(sd, BUFF, strlen(BUFF) + 1);
                read(sd, BUFF, sizeof(BUFF));
                printf("%s\n", BUFF);
                break;
            }

            case 6: {
                // Change Password
                char pass[13];
                printf("Enter new password : \n");
                scanf("%12s", pass);
                write(sd, pass, sizeof(pass));
                read(sd, BUFF, sizeof(BUFF));
                printf("%s\n", BUFF);
                break;
            }

            case 7:{
                // See Customer transactions
                int acc_no;
                printf("Enter Customer Account Number:\n");
                scanf("%d", &acc_no);
                snprintf(BUFF, sizeof(BUFF), "%d", acc_no);
                write(sd, BUFF, strlen(BUFF) + 1);
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
                   // printf("Bytes read: %d\n", bytes_read); // Debugging line
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
                    fflush(stdout);
                }
                fflush(stdout);
                break;
              
            }

            case 8: {
                // Logout
                fflush(stdout);
                read(sd, BUFF, sizeof(BUFF));
                printf("%s\n", BUFF);
                return;
            }

            default:
                close(sd);
                return;
        }
    }
}
