#include "bank_struct.h"
#include "common.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<stddef.h>

#include "bank_struct.h"
#include "common.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>

void create_Customer(int sd, int id, char *pass, char *name, char *branch) {
    // Create User Record
    int fd = open(USER_RECORDS, O_CREAT|O_RDWR|O_APPEND, 0766);
    if (fd < 0) {
        perror("open USER_RECORDS");
        return;
    }

    struct Customer us1;
    us1.id = id;

    strncpy(us1.password, pass, sizeof(us1.password) - 1);
    us1.password[sizeof(us1.password) - 1] = '\0';

    strncpy(us1.name, name, sizeof(us1.name) - 1);
    us1.name[sizeof(us1.name) - 1] = '\0';

    strncpy(us1.branch, branch, sizeof(us1.branch) - 1);
    us1.branch[sizeof(us1.branch) - 1] = '\0';

    us1.active = 0;
    us1.acc_no = generate_unique_no(USER_RECORDS, sizeof(struct Customer), 100010);

    char acc_info[48];
    snprintf(acc_info, sizeof(acc_info), "The account no generated is: %d", us1.acc_no);
    //printf( "The account no generated is: %d \n", us1.acc_no);
    write(sd, acc_info, strlen(acc_info));

    write(fd, &us1, sizeof(us1));
    fsync(fd);
    close(fd);

    // Create Session Record
    int fd_session = open(SESSION_CUST, O_CREAT | O_RDWR | O_APPEND, 0744);
    if (fd_session < 0) {
        perror("open SESSION_CUST");
        return;
    }

    struct Session sess1;
    sess1.id = us1.acc_no;
    sess1.status = 0;
    write(fd_session, &sess1, sizeof(sess1));
    fsync(fd_session);
    close(fd_session);

    // Create Balance Record
    int fd_balance = open(ACCOUNT_MGMT, O_CREAT | O_RDWR | O_APPEND, 0744);
    if (fd_balance < 0) {
        perror("open ACCOUNT_MGMT");
        return;
    }

    struct Bank_Balance ban;
    ban.acc_no = us1.acc_no;
    ban.balance = 0.0;
    write(fd_balance, &ban, sizeof(ban));
    fsync(fd_balance);
    close(fd_balance);
}

 int login_Customer(int sd, int acc_no, int id, char *pass) {
    int fd = open(USER_RECORDS, O_RDONLY);
    if (fd < 0) {
        perror("Error opening user records");
        return -1; // System failure
    }

    int session_fd = open(SESSION_CUST, O_RDWR);
    if (session_fd < 0) {
        perror("Error opening session file");
        close(fd);
        return -1; 
    }

    struct Customer u;
    int login_successful = 0;

    trim_whitespace(pass);

    while (read(fd, &u, sizeof(u)) == sizeof(u)) {
        
        // Trim the password from the file
        trim_whitespace(u.password);
        
        // Debug print
        //printf("Comparing entered password '%s' with stored password '%s'\n", pass, u.password);

        if (u.acc_no == acc_no && u.id == id && strcmp(u.password, pass) == 0 && u.active == 1) {
            // Found the user!
            login_successful = 1;
            break;
        }
    }
    
    close(fd); // We are done with the user file

    if (!login_successful) {
        //printf("Login failed: User/password combination not found.\n");
        close(session_fd);
        return 0; // Login failure (wrong credentials)
    }

    struct Session s;
    int session_found = 0;
    
    lseek(session_fd, 0, SEEK_SET); 
    
    while (read(session_fd, &s, sizeof(s)) == sizeof(s)) {
        if (s.id == acc_no && s.status == 0) {
            lseek(session_fd, -sizeof(s), SEEK_CUR);
            s.status = 1; // Mark as logged in
            write(session_fd, &s, sizeof(s));
            
            session_found = 1;
            break;
        }
    }

    close(session_fd); 
    if (session_found) {
        return 1; // Success!
    } else {
        printf("Login failed: No available sessions for this account.\n");
        return 0; // Login failure (no session)
    }
}

double check_balance_for_fund_and_general(int acc_no) {
    int fd = open(ACCOUNT_MGMT, O_RDONLY, 0744);
    if (fd < 0) {
        perror("open ACCOUNT_MGMT");
        return 0.0;
    }

    struct Bank_Balance ban;
    while (read(fd, &ban, sizeof(ban)) == sizeof(ban)) {
        if (ban.acc_no == acc_no) {
            fsync(fd);
            close(fd);
            return ban.balance;
        }
    }

    close(fd);
    return 0.0;
}

int deposit(int acc_no, double money) {
    int fd = open(ACCOUNT_MGMT, O_RDWR);
    if (fd < 0) {
        perror("open ACCOUNT_MGMT");
        return -1;
    }

    struct Bank_Balance ban;
    struct flock lock;
    while(read(fd, &ban, sizeof(ban)) == sizeof(ban)) {
        if (ban.acc_no == acc_no) {
            off_t offset=lseek(fd,-sizeof(ban),SEEK_CUR);
            //Acquiring write lock
            lock.l_type=F_WRLCK;
            lock.l_whence=SEEK_SET;
            lock.l_start=offset;
            lock.l_len=sizeof(ban);
            lock.l_pid=getpid();
            fcntl(fd,F_SETLKW,&lock);
            ban.balance += money;
            lseek(fd,offset,SEEK_SET);
            if (write(fd, &ban, sizeof(ban)) != sizeof(ban)) {
                perror("write failed");
                return -1;
            }
            lock.l_type=F_UNLCK;
            fcntl(fd,F_SETLKW,&lock); //Releasing lock
            fsync(fd);
            close(fd);
            break;
        }
    }   
    // Record Transaction
    int fd_t= open(transactionFile(acc_no), O_CREAT|O_RDWR|O_APPEND, 0744);
    if (fd_t < 0) {
        perror("open transaction file");
        return -1;
    }
    struct Transaction trans_;
    trans_.acc_no = acc_no;
    trans_.amt = money;
    strncpy(trans_.operation, "Deposit", sizeof(trans_.operation) - 1);
    trans_.operation[sizeof(trans_.operation)-1] = '\0';
    trans_.timestamp = getDateTime();
     lock.l_type=F_WRLCK;
            lock.l_whence=SEEK_SET;
            lock.l_start=0;
            lock.l_len=0;
            lock.l_pid=getpid();
            fcntl(fd_t,F_SETLKW,&lock);
    if (write(fd_t, &trans_, sizeof(trans_)) != sizeof(trans_)) {
        perror("write transaction");
        close(fd_t);
        return -1;
    }
    lock.l_type=F_UNLCK;
    fcntl(fd_t,F_SETLKW,&lock);
    fsync(fd_t);
    close(fd_t);
    return 1;
}

int Withdrawal(int acc_no, double money) {
    int fd = open(ACCOUNT_MGMT, O_RDWR);
    if (fd < 0) {
        perror("open ACCOUNT_MGMT");
        return -1;
    }

    struct Bank_Balance ban;
    struct flock lock;

     while(read(fd, &ban, sizeof(ban)) == sizeof(ban)) {
        if (ban.acc_no == acc_no) {
            off_t offset=lseek(fd,-sizeof(ban),SEEK_CUR);
            //Acquiring write lock
            lock.l_type=F_WRLCK;
            lock.l_whence=SEEK_SET;
            lock.l_start=offset;
            lock.l_len=sizeof(ban);
            lock.l_pid=getpid();
            fcntl(fd,F_SETLKW,&lock);
            ban.balance -= money;
            lseek(fd,offset,SEEK_SET);
            if (write(fd, &ban, sizeof(ban)) != sizeof(ban)) {
                perror("write failed");
                return -1;
            }
            lock.l_type=F_UNLCK;
            fcntl(fd,F_SETLKW,&lock); //Releasing lock
            fsync(fd);
            close(fd);
            break;
        }
    }   
    // Record Transaction
    char *trans = transactionFile(acc_no);
    int fd_t = open(trans, O_CREAT|O_RDWR|O_APPEND, 0744);
    if (fd_t < 0) {
        perror("open transaction file");
        return -1;
    }

    struct Transaction trans_;
    trans_.acc_no = acc_no;
    trans_.amt = money;
    strncpy(trans_.operation, "Withdrawal", strlen("Withdrawal")+1);
    trans_.operation[sizeof(trans_.operation) - 1] = '\0';
    trans_.timestamp = getDateTime();
      lock.l_type=F_WRLCK;
            lock.l_whence=SEEK_SET;
            lock.l_start=0;
            lock.l_len=0;
            lock.l_pid=getpid();
            fcntl(fd_t,F_SETLKW,&lock);
    if (write(fd_t, &trans_, sizeof(trans_)) != sizeof(trans_)) {
        perror("write transaction");
        close(fd_t);
        return -1;
    }
    lock.l_type=F_UNLCK;
    fcntl(fd_t,F_SETLKW,&lock);

    fsync(fd_t);
    close(fd_t);
    return 1;
}

int fund_to_beneficiary(int s_acc, int r_acc, double money) {
    double bal_sender = check_balance_for_fund_and_general(s_acc);
    double bal_reciever = check_balance_for_fund_and_general(r_acc);

    if ((bal_sender > 1000.0) && (bal_sender > money)) {
        int withdrawal_status = Withdrawal(s_acc, money);
        if (withdrawal_status == 1) {
            int deposit_status = deposit(r_acc, money);
            if (deposit_status == -1) {
                // Rollback: remit money back to sender
                deposit(s_acc, money);
                return -1;
            } else {
                return 1;
            }
        } else {
            return -1;
        }
    } else {
        // -2 is a special code for low balance
        return -2;
    }
}


// View Passbook
void viewPassbook(int sd,int acc_no){
        char buff[1024];
         struct Transaction trans_;
           char *trans=transactionFile(acc_no);
           //printf("Transaction file name : %s\n",trans);
 	        int fd = open(trans,O_RDONLY);
            if(fd < 0){
                perror("Transaction file unopenanable \n");
                return;
            }
            if(fsync(fd)==-1){
                perror("fsync failed \n");
                return;
            }
            int bytes_read_file;
            fsync(fd);
        lseek(fd,0,SEEK_SET);
        while ((bytes_read_file = read(fd, &trans_, sizeof(trans_))) > 0) {
            sprintf(buff,"Transaction Time : %s | %s : Rs. %lf \n",trans_.timestamp,trans_.operation,trans_.amt);
           // printf("Sending transaction detail: %s\n", buff);
            fflush(stdout);
            write(sd,buff,strlen(buff) + 1);
         }
          if(write(sd,"END\n",strlen("END\n")+1)>0){
        printf("end of customer details sent \n");
        }
        else{
            perror("Error sending END signal");
        }
    close(fd);
         }
         
void apply_Loan(int acc_no,double money){
        int fd = open(LOAN_DB,O_CREAT|O_RDWR,0744);
        if(fd ==-1){
                perror("Loan DB Unopenanable \n");
        }
        struct Loan loan;
	int loan_id=generate_unique_no(LOAN_DB,sizeof(struct Loan),12012);
        loan.acc_no=acc_no;
	loan.loan_id=loan_id;
        loan.amt=money;
        loan.empid=0;
        loan.status=0;
        write(fd,&loan,sizeof(loan));
        fsync(fd);
        close(fd);
                        }
//View User Details
void CustomerDetails(int sd){
    int records_found=0;
        int fd = open(USER_RECORDS,O_RDONLY,0744);
        if(fd ==-1){
                perror("Customer DB Unopenanable \n");
                return;
        }
       
        char buff[1024];
        struct Customer u;
        int bytes_read_file;
        lseek(fd,0,SEEK_SET);
        while ((bytes_read_file = read(fd, &u, sizeof(u))) > 0) {
                 if(u.id==0){
                    records_found++;
                    continue;
                 }
                snprintf(buff,sizeof(buff),"id: %d |Account no : %d| Name: %s | Branch: %3s | Active : %d \n",u.id,u.acc_no,u.name,u.branch,u.active);
               // printf("Sending customer detail: %s\n", buff);
                 records_found++;
                int bytes_wrote=write(sd,buff,strlen(buff)+1);
                if(bytes_wrote <0){
                    perror("Error writing to socket \n");
                    return;
                }
    }   
    printf("Records found %d \n",records_found);
        if(write(sd,"END\n",5)>0){
        printf("end of customer details sent \n");
        // return;
        }
        else{
            perror("Error sending END signal");
        }
    close(fd);
}
int logout_Customer(int acc_no) {
    int session_fd = open(SESSION_CUST, O_RDWR);
    if (session_fd < 0) {
        perror("logout_Customer open failed"); // Added function name
        return -1;
    }
  //  printf("DEBUG logout: Opened SESSION_CUST for acc_no %d\n", acc_no); // Added

    struct Session s;
    int record_found_and_updated = 0; // More descriptive flag

    // --- Loop through the session file ---
    while (read(session_fd, &s, sizeof(s)) == sizeof(s)) {
        //printf("DEBUG logout: Read session record: id=%d, status=%d\n", s.id, s.status);
        if (s.id == acc_no && s.status == 1) {
           // printf("DEBUG logout: Found ACTIVE session for acc_no %d!\n", acc_no); // Added
            off_t current_pos = lseek(session_fd, 0, SEEK_CUR); // Get pos after read
            off_t record_start = current_pos - sizeof(s);
            lseek(session_fd, record_start, SEEK_SET);      // Seek back

            s.status = 0; // Set to logged out

           // printf("DEBUG logout: Attempting to write status=0 at offset %ld\n", (long)record_start); // Added
            // Write the update
            if (write(session_fd, &s, sizeof(s)) == sizeof(s)) {
                // printf("DEBUG logout: Write successful!\n"); // Added
                 fsync(session_fd); // Force write to disk
                 record_found_and_updated = 1; // Mark success
            } else {
                 perror("DEBUG logout: Write FAILED!"); // Added perror
                 // Don't set flag if write failed
            }
            break;
        }
    } 
    close(session_fd); // Close the file descriptor

    if (record_found_and_updated) {
       // printf("DEBUG logout: Successfully logged out acc_no %d.\n", acc_no); // Added
        return 1; // Success
    } else {
      //  printf("DEBUG logout: Failed to find or update active session for acc_no %d.\n", acc_no); // Added
        return -1; // Session not found or write failed
    }
}
void addFeedback(int acc_no,char feedback[1024]){
        struct feedback feed;
        feed.acc_no=acc_no;
        strncpy(feed.feedback,feedback,sizeof(feed.feedback)-1);
	feed.feedback[sizeof(feed.feedback)-1]='\0';
        int fd = open(FEEDBACK,O_CREAT|O_RDWR ,0744);
        int retval=write(fd,&feed,sizeof(feed));
        if(retval == -1){
                perror("error writing db \n");
                return;
        }
        fsync(fd);
        close(fd);
        }

int changePasswordCustomer(int acc_no,char *pass) {
    int customer_fd = open(USER_RECORDS, O_RDWR);
    int retval=0;
    if (customer_fd < 0) { // Added safety check
        perror("changePasswordCustomer open");
        return 0;
    }
    
    struct Customer cust;
    lseek(customer_fd, 0, SEEK_SET); // Ensure we start from the beginning
    while (read(customer_fd, &cust, sizeof(cust)) == sizeof(cust)) {
        if (cust.acc_no == acc_no) {
            lseek(customer_fd, -(sizeof(cust)), SEEK_CUR);
            strncpy(cust.password, pass, sizeof(cust.password) - 1);
            cust.password[sizeof(cust.password) - 1] = '\0';
            // Inside changePasswordCustomer, before strncpy
    // printf("[DEBUG]changePasswordCustomer: Received pass='%s' (len %zu)\n", pass, strlen(pass));
    // printf("[DEBUG]changePasswordCustomer: sizeof(cust.password)=%zu\n", sizeof(cust.password)); // Verify size
    // printf("[DEBUG]changePasswordCustomer: Updated cust.password='%s'\n", cust.password);
           retval = write(customer_fd, &cust, sizeof(cust));
            
            fsync(customer_fd);
            close(customer_fd); 
            break;
        }
    }
            if (retval > 0) {
                return 1;
            } else {
                return 0;
            }
    close(customer_fd);
    return 0; 
}

void customer(int acc_no, int sd) {
    while(1){
          char cmd[BUFFER_SIZE];
         memset(cmd, 0, sizeof(cmd));
          read(sd, cmd, sizeof(cmd));
        if(strlen(cmd) == 0){
           // printf("empty command issue seen\n");
            continue;
        }
         int choice;
         sscanf(cmd, "%d", &choice);
        // printf("Customer choice received: %d\n", choice); // Debug print

    switch (choice) {
        case 1: {
            double balance = check_balance_for_fund_and_general(acc_no);
            write(sd, &balance, sizeof(balance));
            break;
        }

        case 2: {
            double money;
            read(sd, &money, sizeof(money));
            int retval = deposit(acc_no, money);
            if (retval == 1)
                write(sd, "Deposit Successful \n", 21);
            else
                write(sd, "Issues with Deposit \n", 22);
            break;
        }


        case 3: {
            double money;
            read(sd, &money, sizeof(money));
            double balance = check_balance_for_fund_and_general(acc_no);
            if (balance < money)
                write(sd, "Account balance low \n", 22);
            else {
                int retval = Withdrawal(acc_no, money);
                if (retval == 1)
                    write(sd, "Withdrawal Successful \n", 23);
                else if (retval == -1)
                    write(sd, "Withdrawal Unsuccessful \n", 25);
            }
            break;
        }

        case 4: {
            int r_acc;
            double money;
            read(sd, &r_acc, sizeof(r_acc));
            read(sd, &money, sizeof(money));
            int retval = fund_to_beneficiary(acc_no, r_acc, money);
            if (retval == 1)
                write(sd, "Fund Transfer Successful \n", 26);
            else if (retval == -1)
                write(sd, "Issues with fund transfer \n", 28);
            else
                write(sd, "Account balance low \n", 22);
            break;
        }

        case 5: {
            double money;
            read(sd, &money, sizeof(money));
            apply_Loan(acc_no, money);
            write(sd, "Loan application submitted.\n", 28);
            break;
        }

        case 6: {
            char pass[13];
            memset(pass, 0, sizeof(pass)); // Clear buffer
            // Read password string (adjust max read size if needed)
            int bytes_read = read(sd, pass, sizeof(pass)-1);
            pass[bytes_read] = '\0'; 
            //printf("Received password: '%12s'\n", pass); // Debug print
            if (bytes_read <= 0) { perror("read password Error"); break; }
            //printf("Changing password for acc_no: %d\n", acc_no); // Debug print
            int retval = changePasswordCustomer(acc_no, pass);
            if (retval == 1)
                write(sd, "Password Changed Successfully \n", 32);
            else
                write(sd, "Password Change Unsuccessful \n", 31);
            break;
        }

        case 7: {
            char feedback[1024];
            memset(feedback, 0, sizeof(feedback));
            int bytes_read = read(sd, feedback, sizeof(feedback) - 1);
            if (bytes_read <= 0) { break;  }
            feedback[bytes_read] = '\0';
            addFeedback(acc_no, feedback);
            write(sd, "Feedback received.\n", 20); 
            break;
        }
        case 8: {
            viewPassbook(sd, acc_no);
            break;
        }
        case 9: {
            logout_Customer(acc_no);
            return;
        }

        default: {
            logout_Customer(acc_no);
            return;
        }
    }
    }
}
