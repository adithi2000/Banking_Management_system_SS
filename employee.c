#include "common.h"
#include "customer.h"
#include "bank_struct.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

void create_Employee(int sd, int manager_id, char *pass,char *name,char *branch) {
    int fd = open(EMP_RECORDS, O_CREAT | O_RDWR | O_APPEND, 0744);
    if (fd < 0) { perror("open"); return; }
    // User Creation
    struct Employee us1;

    strncpy(us1.password, pass, sizeof(us1.password)-1);
    us1.password[sizeof(us1.password)-1] = '\0';

    strncpy(us1.name, name, sizeof(us1.name)-1);
    us1.name[sizeof(us1.name)-1] = '\0';

    strncpy(us1.branch, branch, sizeof(us1.branch)-1);
    us1.branch[sizeof(us1.branch)-1] = '\0';

    us1.empid=generate_unique_no(EMP_RECORDS,sizeof(struct Employee),30350);
    //printf("%d \n",us1.empid);
    char acc_info[50];
         snprintf(acc_info,sizeof(acc_info),"the Empid generated is: %d",us1.empid);
    write(sd,acc_info,strlen(acc_info));

    us1.managerid=manager_id;

    write(fd, &us1, sizeof(us1));
    fsync(fd);
    close(fd);
    // Create Session Record
    // For Employees Employee id will be generated, For managers Manager id will be generated and id stored will be employee id
    // For customers id will be acc_no
    struct Session sess1;
    int fd_session = open(SESSION_EMP, O_CREAT | O_RDWR | O_APPEND, 0744);
    sess1.id=us1.empid;
    sess1.status=0;
    write(fd_session, &sess1,sizeof(sess1));
    fsync(fd_session);
   close(fd_session);
}
// Login function for Employee
 int login_Employee(int sd, int empid, char *pass) {
    int fd = open(EMP_RECORDS, O_RDONLY);
    if (fd < 0) {
        perror("login_Employee: open EMP_RECORDS");
        return 0;
    }

    int session_fd = open(SESSION_EMP, O_RDWR);
    if (session_fd < 0) {
        perror("login_Employee: open SESSION_EMP");
        close(fd); // Also close fd if session_fd fails
        return 0;
    }

    struct Employee u;
    struct Session s;
    int loginSuceessful = 0;
    int sessionSuccessful = 0;
    
    trim_whitespace(pass);
    
    while (read(fd, &u, sizeof(u)) == sizeof(u)) {
        trim_whitespace(u.password);
        // printf("Debug: Employee Details read from file: empid=%d, password=%s\n", u.empid, u.password);
        // printf("DEBUG login_Employee: Comparing FILE_empid=%d with CLIENT_empid=%d\n", u.empid, empid);
        // printf("DEBUG login_Employee: Comparing FILE_pass='%s' with CLIENT_pass='%s'\n", u.password, pass);
        if (u.empid == empid && strcmp(u.password, pass) == 0) {
            loginSuceessful = 1;
            break;
        }
    }

    close(fd);

    if (!loginSuceessful) {
       // printf("Login failed: User/password combination not found.\n");
        close(session_fd);
        return -1; // Return -1 for login failure
    } 
    else {
        lseek(session_fd, 0, SEEK_SET);
        // Loop through all sessions to find a free one
        while (read(session_fd, &s, sizeof(s)) == sizeof(s)) {
            if (s.id == empid && s.status == 0) {
                lseek(session_fd, -sizeof(s), SEEK_CUR);
                s.status = 1;
                write(session_fd, &s, sizeof(s));
                sessionSuccessful = 1;
                break; // Found a session, exit the loop
            }
        }
        
        // Close the session file *after* the loop is done
        fsync(session_fd);
        close(session_fd);

        // Check the result *after* the loop
        if (sessionSuccessful) {
            return 1; // Success!
        } else {
          //  printf("Login failed: No available sessions for this employee.\n");
            return -1; // Login failure (no session)
        }
    }
}

int modifyCustomername(int acc_no, int id, char *name){
    int customer_fd = open(USER_RECORDS, O_RDWR);
    if (customer_fd < 0) { // Added error check
        perror("modifyCustomername: open");
        return 0;
    }

    struct Customer cust;
    while(read(customer_fd, &cust, sizeof(cust)) == sizeof(cust)){
        if(cust.acc_no == acc_no && cust.id == id){
            lseek(customer_fd, -(sizeof(cust)), SEEK_CUR);
            
            strncpy(cust.name, name, sizeof(cust.name)-1);
            cust.name[sizeof(cust.name)-1] = '\0';
            
            int retval = write(customer_fd, &cust, sizeof(cust));
            close(customer_fd); // Close file *before* returning
            
            if(retval > 0){
                return 1;
            } else {
                return 0;
            }
        }
    }
    fsync(customer_fd);
    close(customer_fd);
    return 0; 
}

int modifyCustomerbranch(int acc_no, int id, char *branch) {
    
    int customer_fd = open(USER_RECORDS, O_RDWR);
    if (customer_fd < 0) {
        perror("modifyCustomerbranch: open failed");
        return 0; 
    }

    struct Customer cust;
    while (read(customer_fd, &cust, sizeof(cust)) == sizeof(cust)) {
        
        if (cust.acc_no == acc_no && cust.id == id) {
            lseek(customer_fd, -(sizeof(cust)), SEEK_CUR);
            strncpy(cust.branch, branch, sizeof(cust.branch) - 1);
            cust.branch[sizeof(cust.branch) - 1] = '\0'; 
            int retval = write(customer_fd, &cust, sizeof(cust));
            close(customer_fd);

            if (retval > 0) {
                return 1; // Success
            } else {
                return 0; // Write failure
            } 
        }
    }
    fsync(customer_fd);
    close(customer_fd);
    return 0; 
}
void viewLoanAppln(int sd, int empid){
        int fd = open(LOAN_DB,O_CREAT|O_RDWR,0744);
        if(fd ==-1){
                perror("Loan DB Unopenanable \n");
        }
        struct Loan loan;
        int records_found=0;
          struct stat file_stat;
        if (fstat(fd, &file_stat) == -1) {
            perror("fstat failed");
            close(fd);
            return;
        }
        off_t file_size = file_stat.st_size;
        size_t record_size = sizeof(struct Loan);
        size_t total_records = file_size / record_size;
        struct Loan *loan_array = malloc(total_records* sizeof(struct Loan));
            int bytes_read_file;
            lseek(fd,0,SEEK_SET);
        while(read(fd,&loan,sizeof(loan))==sizeof(loan)){
                if(loan.empid==empid && loan.status==0){
              loan_array[records_found]= loan;
              records_found++;
               }
    }
    if(write(sd,&records_found,sizeof(records_found))>0){
        printf("records of customer found %d \n",records_found);
        // return;
        }
        else{
            perror("Error sending record numbers signal");
        }
    if(write(sd,loan_array,records_found * sizeof(struct Transaction))>0){
        printf("Customer details sent successfully\n");
    }
    else{
        perror("Error sending customer details");
    }
    free(loan_array);
         
    close(fd);
}

int logout_Employee(int empid){
    int session_fd = open(SESSION_EMP, O_RDWR);
    if (session_fd < 0) {
        perror("logout_Employee: open");
        return -1;
    }

    struct Session s;
    
    while (read(session_fd, &s, sizeof(s)) == sizeof(s)) {
        if (s.id == empid && s.status == 1) {
            lseek(session_fd, -sizeof(s), SEEK_CUR); // Rewind
            s.status = 0; // 
            write(session_fd, &s, sizeof(s));
            close(session_fd);
            return 1; // Success
        }
    }
    fsync(session_fd);
    close(session_fd);
    return -1; // Session not found
}
//Deleting Customer
int DeleteCustomer(int acc_no, int id) {
    int customer_fd = open(USER_RECORDS, O_RDWR); 
    if (customer_fd < 0) { 
        perror("Customer file failed to open"); 
        return 0; // Return 0 for failure
    }

    int fd_session = open(SESSION_EMP, O_RDWR); 
    int fd_balance = open(ACCOUNT_MGMT, O_RDWR); 
    
    // Check if session or balance files failed to open
    if (fd_session < 0 || fd_balance < 0) {
        close(customer_fd); 
        
        if (fd_session < 0) {
            perror("Session file error"); 
        } else {
             close(fd_session); 
        }
        if (fd_balance < 0) {
            perror("Balance file error");
        } else {
            close(fd_balance); 
        }
        return 0; // Return failure since at least one file failed
        
    }

    struct Customer cust;
    int customer_deleted = 0;  // flag to track customer deletion 

    // Find and blank the customer record
    while (read(customer_fd, &cust, sizeof(cust)) == sizeof(cust)) {
        if (cust.acc_no == acc_no && cust.id == id) { 
            lseek(customer_fd, -(sizeof(cust)), SEEK_CUR);
            struct Customer blank = {0}; // Create a blank record
            if (write(customer_fd, &blank, sizeof(blank)) == sizeof(blank)) {
                customer_deleted = 1; 
            }
            break; 
        }
    }

    // If customer wasn't found 
    if (!customer_deleted) {
        close(fd_session);
        close(fd_balance);
        close(customer_fd);
        return 0; 
    }

    // Find and blank the session record
    struct Session sess1;
    int session_deleted = 0;
    lseek(fd_session, 0, SEEK_SET); 
    while (read(fd_session, &sess1, sizeof(sess1)) == sizeof(sess1)) {
        if (sess1.id == acc_no) {
            lseek(fd_session, -(sizeof(sess1)), SEEK_CUR);
            struct Session bla = {0};
            if(write(fd_session, &bla, sizeof(bla)) == sizeof(bla)){
                session_deleted = 1;
            }
            break;
        }
    }

    // If session wasn't found or couldn't be blanked, clean up (but customer IS deleted)
    if (!session_deleted) {
       //  printf("Warning: Could not find or delete session record for acc_no %d\n", acc_no);
       return 0;
    }


    // Find and blank the balance record
    struct Bank_Balance ban;
    int balance_deleted = 0;
    lseek(fd_balance, 0, SEEK_SET); // Rewind before reading
    while (read(fd_balance, &ban, sizeof(ban)) == sizeof(ban)) {
        if (ban.acc_no == acc_no) {
            lseek(fd_balance, -(sizeof(ban)), SEEK_CUR);
            struct Bank_Balance b = {0};
            if(write(fd_balance, &b, sizeof(b)) == sizeof(b)){
                balance_deleted = 1;
            }
            break;
        }
    }
     if (!balance_deleted) {
        // printf("Warning: Could not find or delete balance record for acc_no %d\n", acc_no);
        return 0;
    }

    // Delete the transaction file
    char *trans = transactionFile(acc_no);
    if (trans != NULL) {
        unlink(trans); // delete transaction file for the particular account
    }

    // Close all files
    fsync(fd_session);
    fsync(fd_balance);
    fsync(customer_fd);

    close(fd_session);
    close(fd_balance);
    close(customer_fd);

    //returning 1 for overall success
    return 1; 
}
int approveLoans(int sd, int loan_id, int acc_no) {
    int fd = open(LOAN_DB, O_RDWR); 

    if (fd == -1) { // Check if open failed
        perror("approveLoans: Error opening loan DB");
        write(sd, "ERROR: Cannot access loan database.\n", 36);
        return -1; // Return on error
    } 

    struct Loan loan;
   // int record_found = 0; // Flag to track if loan was found

    while (read(fd, &loan, sizeof(loan)) == sizeof(loan)) {
        if (loan.loan_id == loan_id && loan.acc_no == acc_no) {
           // record_found = 1; // Mark as found

            off_t current_pos = lseek(fd, 0, SEEK_CUR);
            off_t record_start_offset = current_pos - sizeof(loan);
            lseek(fd, record_start_offset, SEEK_SET);

            // Update the status
            loan.status = 1; // Mark as approved

            // Write the updated record back
            if (write(fd, &loan, sizeof(loan)) != sizeof(loan)) {
                 perror("approveLoans: Error writing updated loan status");
                 close(fd);
                // write(sd,"Loan Approval Failed (Write Error)\n", 35);
                 return -1; // Indicate write failure
            }

            if (fsync(fd) == -1) {
                perror("approveLoans: fsync failed after write");
            }

            close(fd); 

            // Deposit the loan amount
            int deposit_status = deposit(acc_no, loan.amt);
            if (deposit_status != 1) {
              //  fprintf(stderr, "approveLoans: Warning - Loan status updated, but deposit failed for acc %d\n", acc_no);
                // write(sd,"Loan Approved (Deposit Issue)\n", 30);
                 return -1; // Indicate approval succeeded, despite deposit issue
            }

           // write(sd, "Loan Approved Successfully\n", 28);
            return 1; // Indicate success
        } 
    } 
    close(fd); 

} //
//View Employee Details
void EmployeesDetails(int sd){
    int records_found=0;
        int fd = open(EMP_RECORDS,O_RDONLY,0744);
        if(fd ==-1){
                perror("EMP DB Unopenanable \n");
                return;
        }
      // getting total size of file
        struct stat file_stat;
        if (fstat(fd, &file_stat) == -1) {
            perror("fstat failed");
            close(fd);
            return;
        }
        off_t file_size = file_stat.st_size;
        size_t record_size = sizeof(struct Employee);
        size_t total_records = file_size / record_size;

        struct Employee emp;
        struct Emp{
            int empid;
            int managerid;
            char name[20];
            char branch[20];
        };
       struct Emp *emp_array = malloc(total_records* sizeof(struct Emp));
        int bytes_read_file;
        lseek(fd,0,SEEK_SET);
        while ((bytes_read_file = read(fd, &emp, sizeof(emp))) > 0) {
           // printf("Debug: Read Employee Record: empid=%d, managerid=%d, name=%s, branch=%s\n", emp.empid, emp.managerid, emp.name, emp.branch);
                if(emp.empid==0){
                    continue;
                }
                
               struct Emp emp_temp;
                emp_temp.empid = emp.empid;
                emp_temp.managerid = emp.managerid;
                strncpy(emp_temp.name, emp.name, sizeof(emp_temp.name) - 1);
                emp_temp.name[sizeof(emp_temp.name) - 1] = '\0';
                strncpy(emp_temp.branch, emp.branch, sizeof(emp_temp.branch) - 1);
                emp_temp.branch[sizeof(emp_temp.branch) - 1] = '\0';
                emp_array[records_found] = emp_temp;
                records_found++;
               
    }

   struct Emp *final_array = realloc(emp_array, records_found * sizeof(struct Emp));
    //uint32_t record_count_n = htonl((uint32_t)records_found);
     if(write(sd,&records_found,sizeof(records_found))>0){
        printf("records of customer found %d \n",records_found);
        // return;
        }
        else{
            perror("Error sending record numbers signal");
        }
    if(write(sd,final_array,records_found * sizeof(struct Emp))>0){
        printf("Employee details sent successfully\n");
    }
    else{
        perror("Error sending employee details");
    }
    free(final_array);
    close(fd);
}

int changePasswordEmployee(int empid,char *pass){
        int emp_fd=open(EMP_RECORDS,O_RDWR);
        if(emp_fd <0){
                perror("Employee Record Unopenable \n");
                return 0;
        }
        struct Employee emp;
        while(read(emp_fd,&emp,sizeof(emp))==sizeof(emp)){
        if(emp.empid == empid){
                lseek(emp_fd,-(sizeof(emp)),SEEK_CUR);
                strncpy(emp.password,pass,sizeof(emp.password)-1);
                emp.password[sizeof(emp.password)-1] = '\0';
                int retval=write(emp_fd,&emp,sizeof(emp));
                fsync(emp_fd);
                close(emp_fd);
                if(retval > 0){
                        return 1;
                }
                else{
                        return 0;
                }
        }
        }}

void employee(int empid, int sd) {
    while(1){
            char cmd[BUFFER_SIZE];
    char buf[BUFFER_SIZE];
    memset(cmd, 0, sizeof(cmd));
    memset(buf, 0, sizeof(buf));
    read(sd, cmd, sizeof(cmd));

    int choice;
    sscanf(cmd, "%d", &choice);

    switch (choice) {
        case 1: {
            // Create Customer
            read(sd, buf, sizeof(buf));
            int id;
            char pass[13];
            char name[20];
            char branch[20];
            sscanf(buf, "%d %12s %s %s", &id, pass, name, branch);
            create_Customer(sd, id, pass, name, branch);
            write(sd, "User Created\n", 13);
            break;
        }

        case 2: {
            // Modify Customer name
            read(sd, buf, sizeof(buf));
            int acc_no_name, id_name;
            char name[20];
            sscanf(buf, "%d %d %s", &acc_no_name, &id_name, name);
            int retval_name = modifyCustomername(acc_no_name, id_name, name);
            if (retval_name == 1)
                write(sd, "Name Change Successful \n", 24);
            else
                write(sd, "Name Change Failed \n", 20);
            break;
        }

        case 3: {
            // Modify Customer branch
            read(sd, buf, sizeof(buf));
            int acc_no_branch, id_branch;
            char branch[20];
            sscanf(buf, "%d %d %s", &acc_no_branch, &id_branch, branch);
            int retval_branch = modifyCustomerbranch(acc_no_branch, id_branch, branch);
            if (retval_branch == 1)
                write(sd, "Branch Change Successful \n", 26);
            else
                write(sd, "Branch Change Failed \n", 22);
            break;
        }

       

        case 4: {
            // View Loan Applications
            viewLoanAppln(sd, empid);  // corrected function name
            break;
        }

        case 5: {
            // Approve Loan
            read(sd, buf, sizeof(buf));
            int loan_id, acc_no_loan;
            sscanf(buf, "%d %d", &acc_no_loan, &loan_id);
            int retval_loan = approveLoans(sd, loan_id, acc_no_loan);
            if (retval_loan == 1)
                write(sd, "Loan Approved \n", 15);
            else
                write(sd, "Loan Approval Failed \n", 22);
            break;
        }

        case 6: {
            // Change Password
            read(sd, buf, sizeof(buf));
            char pass_emp[13];
            sscanf(buf, "%s", pass_emp);
            int retval_pass = changePasswordEmployee(empid, pass_emp);
            if (retval_pass == 1)
                write(sd, "Password Change Successful \n", 29);
            else
                write(sd, "Password Change Failed \n", 25);
            break;
        }
        case 7:{
            // See Customer transactions
            read(sd, buf, sizeof(buf));
            int acc_no_trans;
            sscanf(buf, "%d", &acc_no_trans);
            viewPassbook(sd,acc_no_trans);
           break;
        }
        case 8: {
            // Logout
            int retval_logout = logout_Employee(empid);
            if (retval_logout == 1){
                write(sd, "Logout Successful \n", 19);
                return;
        }
            else
                write(sd, "Logout Failed \n", 16);
        }
        break;

        default: {
        logout_Employee(empid);
          return;
            break;
        }
    }
}
}
