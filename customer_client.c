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

void customer(int sd){
    while(1){
        char cmd[BUFFER_SIZE];
           double money;
        printf("Enter your choice : \n 1. Check Balance \n 2. Deposit Money \n 3. Withdraw Amount \n4. Fund Transfer to Beneficiary \n 5. Loan Application \n 6.change password \n 7.send feedback \n 8.View passbook \n 10.Logout \n 11.Exit \n");
        scanf("%s", cmd);
        if(write(sd, cmd, strlen(cmd) + 1) < 0){
            continue;
        }
        
        int choice;
        sscanf(cmd, "%d", &choice);
        char BUFF[BUFFER_SIZE];
        switch(choice){
            case 1:
                // Check Balance
                {
                double bal;
                read(sd, &bal, sizeof(bal));
                printf("Your Balance is %lf \n", bal);
                }
                break;
            case 2:
                // Deposit Money
                {
                printf("enter money to be deposited : \n");
                double money;
                scanf("%lf", &money);
                write(sd, &money, sizeof(money));
                read(sd, BUFF, sizeof(BUFF));
                printf("%s\n", BUFF);
                }
                break;
            case 3:
                // Withdraw Amount
                {
                printf("enter the money for withdrawal: \n");
                scanf("%lf", &money);
                write(sd, &money, sizeof(money));
                if(read(sd, BUFF, sizeof(BUFF))){
                    printf("%s", BUFF);
                }
            }
                break;
            case 4:
                // Fund Transfer to Beneficiary
                {
                int r_acc;
                printf("Enter the beneficiary account number : \n");
                scanf("%d", &r_acc);
                write(sd, &r_acc, sizeof(r_acc));
                printf("Enter the Money to be transfered : \n");
                scanf("%lf", &money);
                write(sd, &money, sizeof(money));
                read(sd, BUFF, sizeof(BUFF));
                printf("%s", BUFF);
                }
                break;
            case 5:
                // Loan Application
                {
                printf("Enter the Money needed for loan : \n");
                scanf("%lf", &money);
                write(sd, &money, sizeof(money));
                read(sd, BUFF, sizeof(BUFF));
                printf("%s", BUFF);
                }
                break;
            case 6:
                // Change Password
                {
                char pass[13];
                printf("Enter new password : \n");
                scanf("%12s", pass);
                write(sd, pass, strlen(pass)+1);
                if(read(sd, BUFF, sizeof(BUFF))){
                    printf("%s", BUFF);
                }
            }
                break;
            case 7:
                // feedback
                {
                char feedback[256];
                printf("Enter your feedback : \n");
                scanf(" %[^\n]s", feedback);
                write(sd, feedback, strlen(feedback)+1);
                }
                break;
                case 8:
                // view passbook
                {
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
            case 9:
            {
                read(sd,BUFF,sizeof(BUFF));
		       printf("%s",BUFF);
               return;
            }
                break;
            default:
            {
                close(sd);
               return;
            }
        }
    }
}
