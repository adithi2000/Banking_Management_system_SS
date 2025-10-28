#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include "customer_client.h"
#include "employee_client.h"
#include "manager_client.h"
#include "admin_client.h"
#include "common.h"

#define BUFFER_SIZE 1024

int main() {
    printf("Welcome to Banking System, Please wait while connecting to Server\n");

    struct sockaddr_in serv;
    int sd;
    // Configure server address
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8083);
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");  // Replace with actual IP if needed

    

    while (1) {
        // Create socket
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
        // Connect to server
    if (connect(sd, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        perror("Connection failed");
        close(sd);
        exit(EXIT_FAILURE);
    }
        char cmd[BUFFER_SIZE];
        printf("\nEnter 1:Customer, 2:Employee, 3:Manager, 4:Admin, any other key to Exit\n");
        scanf("%s", cmd);
        write(sd, cmd, strlen(cmd) + 1);

        if (strcmp(cmd, "1") == 0) {
            int count = 3;
            while (count > 0) {
                int id, acc_no;
                char pass[13];
                char result[BUFFER_SIZE];

                printf("Enter Account Number:\n");
                scanf("%d", &acc_no);
                printf("Enter User ID:\n");
                scanf("%d", &id);
                printf("Enter Password:\n");
                scanf("%12s", pass);

                snprintf(result, sizeof(result), "%d %d %s", acc_no, id, pass);
                write(sd, result, strlen(result) + 1);

                memset(result, 0, sizeof(result));
                read(sd, result, sizeof(result));
                trim_whitespace(result);
                if ((strcmp(result, "Login Success") == 0)) {
                    customer(sd); // customer() was renamed in your includes
                    break;
                } 
                else if(strcmp(result,"System Error during login. Exiting.")==0){
                    printf("System Error encountered. Exiting login attempts.\n");
                    break;
                }
                else {
                    count--;
                    printf("%s \n",result);
                }
            }
        // --- FIX: Removed 1 extra '}' brace from here ---
        } else if (strcmp(cmd, "2") == 0) {
            int count = 3;
            while (count > 0) {
                int id;
                char pass[13];
                char result[BUFFER_SIZE];

                printf("Enter EMP ID:\n");
                scanf("%d", &id);
                printf("Enter Password:\n");
                scanf("%12s", pass);

                snprintf(result, sizeof(result), "%d %s", id, pass);
                write(sd, result, strlen(result) + 1);

                memset(result, 0, sizeof(result));
                read(sd, result, sizeof(result));
                
                trim_whitespace(result);
                
                if (strcmp(result, "Login Success") == 0) {
                    printf("Login Successful\n");
                    employee_client(sd);
                    break;
                }
                else if(strcmp(result,"System Error during login. Exiting.")==0){
                    printf("System Error encountered. Exiting login attempts.\n");
                    break;
                }
                 else {
                    printf("%s \n", result);
                    count--;
                }
            }
        // --- FIX: Removed 1 extra '}' brace from here ---
        } else if (strcmp(cmd, "3") == 0) {
            int count = 3;
            while (count > 0) {
                int empid, managerid;
                char pass[13];
                char result[BUFFER_SIZE];

                printf("Enter Manager ID:\n");
                scanf("%d", &managerid);
                printf("Enter Emp ID:\n");
                scanf("%d", &empid);
                printf("Enter Password:\n");
                
                scanf("%12s", pass);

                snprintf(result, sizeof(result), "%d %d %s", managerid, empid, pass);
                write(sd, result, strlen(result) + 1);

                memset(result, 0, sizeof(result));
                read(sd, result, sizeof(result));
                
                result[strcspn(result, "\r\n")] = '\0';
                
                if (strcmp(result, "Login Success") == 0) {
                    printf("Login Successful\n");
                    manager_client(sd);
                    break;
                }
                else if(strcmp(result,"System Error during login. Exiting.")==0){
                    printf("System Error encountered. Exiting login attempts.\n");
                    break;
                }
                 else {
                    printf("%s \n", result);
                    count--;
                }
            }

        } else if (strcmp(cmd, "4") == 0) {
            int count = 3;
            while (count > 0) {
                char pass[13];
                char result[BUFFER_SIZE];

                printf("Enter Password:\n");
                scanf("%12s", pass);
                
              

                snprintf(result, sizeof(result), "%s", pass);
                write(sd, result, strlen(result) + 1);

                memset(result, 0, sizeof(result));
                read(sd, result, sizeof(result));
                printf("%s \n", result);
                result[strcspn(result, "\r\n")] = '\0';
                if ((strcmp(result, "Login Success") == 0)) {
                    admin_client(sd);
                    break;
                } 
                else if(strcmp(result,"System Error during login. Exiting.")==0){
                    printf("System Error encountered. Exiting login attempts.\n");
                    break;
                }
                else {
                    count--;
                    printf("%s \n", result);
                }
            }
        } else {
            
            printf("Invalid choice, exiting.\n");
            break; 
        }
    } // End 

    close(sd);
    return 0;
}