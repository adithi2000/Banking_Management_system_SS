#define BUFFER_SIZE 1024
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "common.h"
#include "bank_struct.h"
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
            // view  loan applications
            {
                int records_found=0;
                read(sd,&records_found,sizeof(records_found));
                printf("Total Loan Records: %d\n", records_found);
                 if(records_found == 0) {
                    printf("No loans available.\n");
                    break;
                }
                size_t total_array_size = records_found * sizeof(struct Loan);
                struct Loan *trans_array = malloc(total_array_size);
                read(sd,trans_array,records_found * sizeof(struct Loan));
                for(int i=0;i<records_found;i++){
                //     
                printf("Loan ID : %d | Account No : %d | Amount : Rs. %lf | Assigned Employee ID : %d | Status : %d \n",trans_array[i].loan_id,trans_array[i].acc_no,trans_array[i].amt,trans_array[i].empid,trans_array[i].status);
                }
                free(trans_array);
                break;
            }

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
             // View Employees
                 struct Emp{
            int empid;
            int managerid;
            char name[20];
            char branch[20];
                 };
                //uint32_t records_found=0;
                int records_found=0;
                read(sd,&records_found,sizeof(records_found));
                //int records_found_host = ntohl(records_found);
                printf("Total Employee Records: %d\n", records_found);
                 if(records_found == 0) {
                    printf("No employees available.\n");
                    break;
                }
                size_t total_array_size = records_found * sizeof(struct Emp);
                struct Emp *emp_array = malloc(total_array_size);
                read(sd,emp_array,records_found * sizeof(struct Emp));
                for(int i=0;i<records_found;i++){
                    emp_array[i].name[sizeof(emp_array[i].name) - 1] = '\0';
                    emp_array[i].branch[sizeof(emp_array[i].branch) - 1] = '\0';
                    printf("empid: %d |managerid: %d| Name: %s | Branch: %3s \n",emp_array[i].empid,emp_array[i].managerid,emp_array[i].name,emp_array[i].branch);
                }
                free(emp_array);
                fflush(stdout);
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
            {
                // View Feedback
                int records_found=0;
                read(sd,&records_found,sizeof(records_found));
                printf("Total Customer Records: %d\n", records_found);
                if(records_found == 0) {
                    printf("No feedback available.\n");
                    break;
                }
                    
                size_t total_array_size = records_found * sizeof(struct feedback);
                struct feedback *trans_array = malloc(total_array_size);
                read(sd,trans_array,records_found * sizeof(struct feedback));
                for(int i=0;i<records_found;i++){
                //     
                printf("Acc_no: %d |feedback:  %s \n",trans_array[i].acc_no,trans_array[i].feedback);
                }
                free(trans_array);
                break;
            }
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
