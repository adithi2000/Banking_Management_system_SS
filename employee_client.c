#define BUFFER_SIZE 1024
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "common.h"
#include "bank_struct.h"
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
                 int records_found=0;
                read(sd,&records_found,sizeof(records_found));
                printf("Total Loan Records: %d\n", records_found);
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
                 int records_found=0;
                read(sd,&records_found,sizeof(records_found));
                printf("Total Customer Records: %d\n", records_found);
                size_t total_array_size = records_found * sizeof(struct Transaction);
                struct Transaction *trans_array = malloc(total_array_size);
                read(sd,trans_array,records_found * sizeof(struct Transaction));
                for(int i=0;i<records_found;i++){
                //     
                printf("Transaction Time : %s | %s : Rs. %lf \n",trans_array[i].timestamp,trans_array[i].operation,trans_array[i].amt);
                }
                free(trans_array);
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
