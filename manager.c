#include "bank_struct.h"
#include "common.h"
#include "customer.h"
#include "employee.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

void create_Manager(int sd, int empid, char *pass) {
    int fd = open(MANG_RECORDS, O_CREAT | O_RDWR | O_APPEND, 0744);
    if (fd < 0) { perror("open"); return; }
    // User Creation
    struct Manager us1;
    strncpy(us1.password, pass, sizeof(us1.password)-1);
    us1.password[sizeof(us1.password)-1] = '\0';
    us1.managerid=generate_unique_no(MANG_RECORDS,sizeof(struct Manager),60350);
    char acc_info[50];
         snprintf(acc_info,sizeof(acc_info),"the Managerid generated is: %d",us1.managerid);
    write(sd,acc_info,strlen(acc_info)+1);
    us1.empid=empid;
    write(fd, &us1, sizeof(us1));
    fsync(fd);
    close(fd);
    // Create Session Record
    // For Employees Employee id will be generated, For managers Manager id will be generated and id stored will be employee id
    // For customers id will be acc_no
    struct Session sess1;
    int fd_session = open(SESSION_MANG, O_CREAT | O_RDWR | O_APPEND, 0744);
    sess1.id=us1.managerid;
    sess1.status=0;
    write(fd_session, &sess1,sizeof(sess1));
    fsync(fd_session);
   close(fd_session);
}
// Login function for Employee
int login_Manager(int sd,int empid,int managerid, char *pass) {
    int fd = open(MANG_RECORDS, O_RDONLY);
    if (fd < 0) return 0;
    int session_fd=open(SESSION_MANG,O_RDWR);
     if (session_fd < 0) return 0;
    struct Manager u;
    struct Session s;
    int loginSuccess=0,sessionStatus=0;
    trim_whitespace(pass);
    while (read(fd, &u, sizeof(u)) == sizeof(u)) {
        trim_whitespace(u.password);
        // printf("Debug:Manager Details read from file: empid=%d, managerid=%d, password=%s\n", u.empid, u.managerid, u.password); // Debug info
        // printf("DEBUG login_Employee: Comparing FILE_empid=%d with CLIENT_empid=%d\n", u.empid, empid);
        // printf("DEBUG login_Employee: Comparing FILE_pass='%s' with CLIENT_pass='%s'\n", u.password, pass);
        if (u.empid == empid && u.managerid == managerid && strcmp(u.password, pass) == 0 ) {
             loginSuccess=1 ;
            close(fd);
            break;
        }
    }
    if(!loginSuccess){
      //  printf("Debug: Login failed for empid %d and managerid %d\n", empid, managerid);
        close(fd);
        return -1;
    }
    lseek(session_fd, 0, SEEK_SET);
    //Updating Session Status
    while(read(session_fd,&s,sizeof(s))==sizeof(s)){
        printf("Debug : Session Details read from file: id=%d, status=%d\n", s.id, s.status); // Debug info
        if(s.id == managerid && s.status==0){
           // printf("Debug: Updating session status for id %d\n", managerid);
            sessionStatus=1;
            lseek(session_fd,-(sizeof(s)),SEEK_CUR);
            s.status=1;
            write(session_fd,&s,sizeof(s));
            close(session_fd);
            break;
        }
    }
    if(loginSuccess && sessionStatus){
        return 1;
    }
    else{
        fsync(session_fd);
        close(session_fd);
        return -1;
    }
}
void viewLoanApplications(int sd){
        int fd = open(LOAN_DB,O_RDONLY,0744);
        if(fd ==-1){
                perror("Loan DB Unopenanable \n");
                return;
        }
        char buff[2025];
        struct Loan loan;
        lseek(fd,0,SEEK_SET);
        while(read(fd,&loan,sizeof(loan))==sizeof(loan)){
                if(loan.status==0 && loan.empid==0){
                snprintf(buff,sizeof(buff),"Acc_no: %d | empid : %d | loanid : %d | amt : %lf",loan.acc_no,loan.empid,loan.loan_id,loan.amt);
                write(sd,buff,strlen(buff)+1);
                }
        }
        if(write(sd,"END\n",5)>0){
        printf("end of feedback sent \n");
        // return;
        }
        else{
            perror("Error sending END signal");
        }
         close(fd);
}
void viewFeedback(int sd){
        int fd = open(FEEDBACK,O_RDONLY);
        if(fd ==-1){
                perror("Loan DB Unopenanable \n");
                return;
        }
        char buff[2025];
        struct feedback feed;
        lseek(fd,0,SEEK_SET);
        while(read(fd,&feed,sizeof(feed))>0){
                snprintf(buff,sizeof(buff),"Acc_no: %d | feedback : %s",feed.acc_no,feed.feedback);
                write(sd,buff,strlen(buff)+1);
                
        }

        if(write(sd,"END\n",5)>0){
        printf("end of feedback sent \n");
        // return;
        }
        else{
            perror("Error sending END signal");
        }
        close(fd);
}
void AssignLoans(int sd,int loan_id,int empid){
        int fd = open(LOAN_DB,O_RDWR|O_APPEND,0744);
        if(fd ==-1){
                perror("Loan DB Unopenanable \n");
        }
        char buff[1024];
        struct Loan loan;
        while(read(fd,&loan,sizeof(loan))==sizeof(loan)){
                if(loan.status==0 && loan.empid==0 && loan.loan_id==loan_id){
                        lseek(fd,-(sizeof(loan)),SEEK_CUR);
                        loan.empid=empid;
                        write(fd,&loan,sizeof(loan));
                        fsync(fd);
                        close(fd);
                        return;
                }
        }
        fsync(fd);
        close(fd);
}
//View Employee Details
void ManagerDetails(int sd){
    int records_found=0;
        int fd = open(MANG_RECORDS,O_RDONLY,0744);
        if(fd ==-1){
                perror("MANGER DB Unopenanable \n");
                return;
        }
       
        char buff[1024];
        struct Manager mang;
        int bytes_read_file;
        lseek(fd,0,SEEK_SET);
        while ((bytes_read_file = read(fd, &mang, sizeof(mang))) > 0) {
            if(mang.empid==0){
                records_found++;
                continue;
            }
                snprintf(buff,sizeof(buff),"empid: %d |managerid : %d \n",mang.empid,mang.managerid);
                records_found++;
                if(write(sd,buff,strlen(buff)+1) <=0){
                    perror("Write issue to client");
                }
                
    }
    printf("Records found %d \n",records_found);
    if(write(sd,"END\n",5)>0){
       printf("end of manager details sent \n");
    //    return;
        }
        else{
            perror("Error sending END signal");
        }
    close(fd);
}
int logout_Manager(int managerid){
 int fd = open(MANG_RECORDS, O_RDONLY);
    if (fd < 0) return -1;
    int session_fd=open(SESSION_MANG,O_RDWR);
    struct Session s;
    while (read(session_fd,&s,sizeof(s))==sizeof(s)) {
        if (s.id == managerid && s.status == 1) {
                lseek(session_fd,-(sizeof(s)),SEEK_CUR);
                s.status=0;
                write(session_fd,&s,sizeof(s));
                fsync(session_fd);
                close(session_fd);
            fsync(fd);
            close(fd);
            return 1;
        }
       
    }
    close(fd);
    return -1;
    }

int ActivateAcc(int acc_no,int id){
	int customer_fd=open(USER_RECORDS,O_RDWR);
	struct Customer cust;
	while(read(customer_fd,&cust,sizeof(cust))==sizeof(cust)){
		if(cust.acc_no == acc_no && cust.id == id && cust.active==0){
			lseek(customer_fd,-(sizeof(cust)),SEEK_CUR);
			cust.active=1;
            int retval=write(customer_fd,&cust,sizeof(cust));
            if(retval == -1){
                    perror("Activation Failed \n");
                    close(customer_fd);
                    return -1;
            }
            else{
                    fsync(customer_fd);
                    close(customer_fd);
                    return 1;
            }

		}
	}
}
int DeactivateAcc(int acc_no,int id){
	int customer_fd=open(USER_RECORDS,O_RDWR);
	struct Customer cust;
	while(read(customer_fd,&cust,sizeof(cust))==sizeof(cust)){
		if(cust.acc_no == acc_no && cust.id == id && cust.active==1){
			lseek(customer_fd,-(sizeof(cust)),SEEK_CUR);
			cust.active=0;
            int retval=write(customer_fd,&cust,sizeof(cust));
            if(retval == -1){
                    perror("Deactivation Failed \n");
                    close(customer_fd);
                    return -1;
            }
            else{
                    fsync(customer_fd);
                    close(customer_fd);
                    return 1;
            }

		}
	}
                              }
int changePasswordManager(int managerid,char *pass){
        int emp_fd=open(MANG_RECORDS,O_RDWR);
        struct Manager emp;
        while(read(emp_fd,&emp,sizeof(emp))==sizeof(emp)){
        if(emp.managerid == managerid){
                lseek(emp_fd,-(sizeof(emp)),SEEK_CUR);
                strncpy(emp.password,pass,sizeof(emp.password)-1);
                emp.password[sizeof(emp.password)-1]='\0';
                int retval=write(emp_fd,&emp,sizeof(emp));
                if(retval > 0){
                        fsync(emp_fd);
                        close(emp_fd);
                        return 1;
                }
                else{
                        close(emp_fd);
                        return 0;
                }
        }
        }}
void Manager(int managerid, int sd) {
    char cmd[BUFFER_SIZE];
    char buf[BUFFER_SIZE];
    while(1){
    memset(cmd, 0, sizeof(cmd));
    memset(buf, 0, sizeof(buf));
    read(sd, cmd, sizeof(cmd));

    int choice;
    sscanf(cmd, "%d", &choice);

    switch (choice) {
        case 1: {
            // View Loan Applications
            viewLoanApplications(sd);
            break;
        }

        case 2: {
            // Activate Customer Account
            read(sd, buf, sizeof(buf));
            int acc_no_act, id_act;
            sscanf(buf, "%d %d", &acc_no_act, &id_act);
            int retval_act = ActivateAcc(acc_no_act, id_act);
            if (retval_act == 1)
                write(sd, "Account Activated Successfully \n", 32);
            else
                write(sd, "Account Activation Failed \n", 27);
            break;
        }

        case 3: {
            // Deactivate Customer Account
            read(sd, buf, sizeof(buf));
            int acc_no_deact, id_deact;
            sscanf(buf, "%d %d", &acc_no_deact, &id_deact);
            int retval_deact = DeactivateAcc(acc_no_deact, id_deact);
            if (retval_deact == 1)
                write(sd, "Account Deactivated Successfully \n", 32);
            else
                write(sd, "Account Deactivation Failed \n", 27);
            break;
        }

        case 4: {
            // Assign Loan Application to Employee
            read(sd, buf, sizeof(buf));
            int loan_id, empid;
            sscanf(buf, "%d %d", &loan_id, &empid);
            AssignLoans(sd, loan_id, empid);
            write(sd, "Loan Assigned Successfully \n", 29);
            break;
        }
        case 5:
        // Remove Customer
        {
            read(sd, buf, sizeof(buf));
            int acc_no_rem, id_rem;
            sscanf(buf, "%d %d", &acc_no_rem, &id_rem);
            int retval_rem = DeleteCustomer(acc_no_rem, id_rem);
            if (retval_rem == 1)
                write(sd, "Customer Removed Successfully \n", 31);
            else
                write(sd, "Customer Removal Failed \n", 26);
            break;
        }
        case 6: {
            // View Employee Details
            EmployeesDetails(sd);
            break;
        }

        case 7: {
            // Change Password
            read(sd, buf, sizeof(buf));
            char pass[13];
            sscanf(buf, "%12s", pass);
            int retval_pass = changePasswordManager(managerid, pass);
         //   printf("[DEBUG] Manager %d requested password change.\n", managerid);
            if (retval_pass == 1)
                write(sd, "Password Change Successfull \n", 29);
            else
                write(sd, "Password Change Failed \n", 25);
            break;
        }

        case 8: {
            // View Feedback
            read(sd, buf, sizeof(buf));
            viewFeedback(sd);
            break;
        }

        case 9: {
            // Logout
            int retval_logout = logout_Manager(managerid);
            if (retval_logout == 1){
                write(sd, "Logout Successful \n", 19);
                return;
            }
            else
                write(sd, "Logout Failed \n", 16);
            break;
        }

        default: {
            logout_Manager(managerid);
            return;
            break;
        }
    }
}
}
