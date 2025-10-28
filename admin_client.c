#include <sys/socket.h>
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

#define BUFFER_SIZE 1024

void admin_client(int sd){
    char cmd[BUFFER_SIZE];
    char BUFF[BUFFER_SIZE];
    while(1){
        // --- MENU ---
        printf("\n--- Admin Menu ---\n");
        printf(" 1. Change Admin Password\n");
        printf(" 2. Create Customer\n");
        printf(" 3. Create Employee\n");
        printf(" 4. Create Manager\n");
        printf(" 5. Modify Customer Name\n");
        printf(" 6. Modify Customer Branch\n");
        printf(" 7. Delete Customer\n");
        printf(" 8. Modify Employee Name\n");
        printf(" 9. Modify Employee Branch\n");
        printf("10. Modify Employee Managerid\n");
        printf("11. Delete Employee\n");
        printf("12. Delete Manager\n");
        printf("13. View All Customers\n");
        printf("14. View All Employees\n");
        printf("15. View All Managers\n");

        printf("16. Logout\n");
        printf("Enter any other key to Exit.\n");
        printf("Enter your choice: ");

        scanf("%s", cmd);
        if(write(sd, cmd, strlen(cmd) + 1) < 0){
            continue;
        }

        switch(atoi(cmd)){
            case 1:
                // Change admin Password
                {
                    char buf[BUFFER_SIZE];
                    char newpass[13];
                    printf("Enter New Password : \n");
                    scanf("%12s",newpass);
                    newpass[12]='\0';
                    // snprintf(buf,sizeof(buf),"%s",newpass);
                    write(sd,newpass,strlen(newpass)+1);
                    memset(buf,0,sizeof(buf));
                    read(sd,buf,sizeof(buf));
                    printf("%s\n",buf);
                }
                break;
            case 2:
            {
                // Create Customer
                    char buf[BUFFER_SIZE];
                    int id;
                    char name[20];
                    char branch[20];
                    char password[13];
                    printf("Enter customer_id : \n");
                    scanf("%d",&id);
                    printf("Enter Name : \n");
                    scanf("%s",name);
                    printf("Enter Branch : \n");
                    scanf("%3s",branch);
                    printf("Enter Password : \n");
                    scanf("%12s",password);
                    snprintf(buf,sizeof(buf),"%d %s %s %s",id,name,branch,password);
                   if( write(sd,buf,strlen(buf)+1)){
              printf("data written to server \n");
          }
                    if(read(sd,buf,sizeof(buf)) > 0){
                printf("Message sent by server \n");
                    printf("%48s\n",buf);
                break;
        }
            case 3:
                // Create Employee
                {
                    char buf[BUFFER_SIZE];
                    int empid;
                    int managerid;
                    char password[13];
                    char name[20];
                    char branch[20];
                    printf("Enter Name : \n");
                    scanf("%s",name);
                    printf("Enter Branch ex : BEN : \n");
                    scanf("%3s",branch);
                    printf("Enter Manager ID if employee created is manager enter manager id as 1: \n");
                    scanf("%d",&managerid);
                    printf("Enter Password : \n");
                    scanf("%s",password);
                    snprintf(buf,sizeof(buf),"%d %s %s %s",managerid,password,name,branch);
                    write(sd,buf,strlen(buf)+1);
                    memset(buf,0,sizeof(buf));
                    read(sd,buf,sizeof(buf));
                    printf("%s\n",buf);
                }
                break;
            case 4:
            {
                // Create Manager

                    char buf[BUFFER_SIZE];
                    int empid;
                    char password[13];
                   printf("Enter Manager empid ID : \n");
                    scanf("%d",&empid);
                    printf("Enter Password : \n");
                    scanf("%s",password);
                    snprintf(buf,sizeof(buf),"%d %s",empid,password);
                    write(sd,buf,strlen(buf)+1);
                    memset(buf,0,sizeof(buf));
                    read(sd,buf,sizeof(buf));
                    printf("%s\n",buf);
                    break;
        }
            case 5:
            // modify Customer name
                    {
                        char buf[BUFFER_SIZE];
                        int acc_no;
                        int id;
                        char name[20];
                        printf("Enter Account Number : \n");
                        scanf("%d",&acc_no);
                        printf("Enter ID : \n");
                        scanf("%d",&id);
                        printf("Enter New Name : \n");
                        scanf("%s",name);
                        snprintf(buf,sizeof(buf),"%d %d %s",acc_no,id,name);
                        write(sd,buf,strlen(buf)+1);
                        memset(buf,0,sizeof(buf));
                        read(sd,buf,sizeof(buf));
                        printf("%s\n",buf);
                    }
                    break;
            case 6:
            // modify Customer branch
                    {
                        char buf[BUFFER_SIZE];
                        int acc_no;
                        int id;
                        char branch[20];
                        printf("Enter Account Number : \n");
                        scanf("%d",&acc_no);
                        printf("Enter ID : \n");
                        scanf("%d",&id);
                        printf("Enter New Branch : \n");
                        scanf("%s",branch);
                        snprintf(buf,sizeof(buf),"%d %d %s",acc_no,id,branch);
                        write(sd,buf,strlen(buf)+1);
                        memset(buf,0,sizeof(buf));
                        read(sd,buf,sizeof(buf));
                        printf("%s\n",buf);
                    }
                    break;
            case 7:
            // Delete customer
                    {
                        char buf[BUFFER_SIZE];
                        int acc_no;
                        int id;
                        printf("Enter Account Number : \n");
                        scanf("%d",&acc_no);
                        printf("Enter ID : \n");
                        scanf("%d",&id);
                        snprintf(buf,sizeof(buf),"%d %d",acc_no,id);
                        write(sd,buf,strlen(buf)+1);
                        memset(buf,0,sizeof(buf));
                        read(sd,buf,sizeof(buf));
                        printf("%s\n",buf);
                    }
                    break;
                case 8:
                // Modify Employee name
                    {
                        char buf[BUFFER_SIZE];
                        int empid;
                        char name[20];
                        printf("Enter Employee ID : \n");
                        scanf("%d",&empid);
                        printf("Enter New Name : \n");
                        scanf("%s",name);
                        snprintf(buf,sizeof(buf),"%d %s",empid,name);
                        write(sd,buf,strlen(buf)+1);
                        memset(buf,0,sizeof(buf));
                        read(sd,buf,sizeof(buf));
                        printf("%s\n",buf);
                    }
                    break;
                case 9:
                // Modify employee branch
                    {
                        char buf[BUFFER_SIZE];
                        int empid;
                        char branch[20];
                        printf("Enter Employee ID : \n");
                        scanf("%d",&empid);
                        printf("Enter New Branch : \n");
                        scanf("%3s",branch);
                        snprintf(buf,sizeof(buf),"%d %3s",empid,branch);
                        write(sd,buf,strlen(buf)+1);
                        memset(buf,0,sizeof(buf));
                        read(sd,buf,sizeof(buf));
                        printf("%s\n",buf);
                    }
                    break;
                    case 10:
                // Modify employee managerid
                    {
                        char buf[BUFFER_SIZE];
                        int empid;
                        int managerid;
                        printf("Enter Employee ID : \n");
                        scanf("%d",&empid);
                        printf("Enter manager id: \n");
                        scanf("%d",&managerid);
                        snprintf(buf,sizeof(buf),"%d %d",empid,managerid);
                        write(sd,buf,strlen(buf)+1);
                        memset(buf,0,sizeof(buf));
                        read(sd,buf,sizeof(buf));
                        printf("%s\n",buf);
                    }
                    break;
                case 11:
                // Delete Employee
                    {
                        char buf[BUFFER_SIZE];
                        int empid;
                        printf("Enter Employee ID to be deleted: Make sure that you delete manager before his employee records if the employee is a manager \n");
                        scanf("%d",&empid);
                        snprintf(buf,sizeof(buf),"%d",empid);
                        write(sd,buf,sizeof(buf));
                        memset(buf,0,strlen(buf)+1);
                        read(sd,buf,sizeof(buf));
                        printf("%s\n",buf);
                    }
                    break;
                case 12:
                // Delete Manager
                    {
                        char buf[BUFFER_SIZE];
                        int empid;
                        int managerid;
                        printf("Enter Manager ID : \n");
                        scanf("%d",&managerid);
                        printf("Enter Employee ID : \n");
                        scanf("%d",&empid);
                        snprintf(buf,sizeof(buf),"%d %d",managerid,empid);
                        write(sd,buf,strlen(buf)+1);
                        memset(buf,0,sizeof(buf));
                        read(sd,buf,sizeof(buf));
                        printf("%s\n",buf);
                    }
                    break;
                 case 13: {
                // View Customers
                
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

                    memset(BUFF,0,sizeof(BUFF));
                    int bytes_read = read(sd, BUFF, sizeof(BUFF) - 1);
                    if (bytes_read <= 0) {
                        if (bytes_read == 0) printf("Server disconnected.\n");
                        else perror("read error");
                        break; // Exit the reading loop
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
                break;
            }
             case 14: {
                // View Employees
                
                fflush(stdout);
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

                    memset(BUFF,0,sizeof(BUFF));
                    int bytes_read = read(sd, BUFF, sizeof(BUFF) - 1);
                    if (bytes_read <= 0) {
                        if (bytes_read == 0) printf("Server disconnected.\n");
                        else perror("read error");
                        break; // Exit the reading loop
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
             case 15: 
                // View Manager
                {
                fflush(stdout);
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

                    memset(BUFF,0,sizeof(BUFF));
                    int bytes_read = read(sd, BUFF, sizeof(BUFF) - 1);
                    if (bytes_read <= 0) {
                        if (bytes_read == 0) printf("Server disconnected.\n");
                        else perror("read error");
                        break; // Exit the reading loop
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
                break;
            }
    
                case 16:
                // logout
                    {
                        char buf[BUFFER_SIZE];
                        memset(buf,0,sizeof(buf));
                        read(sd,buf,sizeof(buf));
                        printf("%s\n",buf);
                        return;
                    }
                    break;
                default:
                close(sd);
                    return;
                    break;
                }
            }
        }

}