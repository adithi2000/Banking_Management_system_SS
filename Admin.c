#include "bank_struct.h"
#include "common.h"
#include "customer.h"
#include "employee.h"
#include "manager.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

void create_Admin() {
    int fd = open(ADMIN_CRED, O_CREAT|O_RDWR, 0744);
    if (fd < 0) { perror("open"); return; }
    // AdminCreation
    struct Admin us1;
    us1.adminid=ADMINID;
    strncpy(us1.username,"Admin2025",sizeof(us1.username) - 1);
    strncpy(us1.password,"Admin@#20256",sizeof(us1.password) - 1);
    us1.password[sizeof(us1.password) - 1] = '\0';
    write(fd, &us1, sizeof(us1));
    close(fd);
    // Session Creation
    struct Session sess1;
    int fd_session = open(SESSION_ADMIN, O_CREAT | O_RDWR, 0744);
    if(fd_session < 0) { perror("Failed to open Session file during creation"); return; }
    sess1.id=ADMINID;
    sess1.status=0;
    write(fd_session, &sess1,sizeof(sess1));
   close(fd_session);
}

// Login function for Admin
int Login_Admin(int sd, char *pass) {
    int fd = open(ADMIN_CRED, O_RDONLY);
    if (fd < 0) {
        perror("Error opening admin file");
        return -1;
    }

    int session_fd = open(SESSION_ADMIN, O_RDWR);
    if (session_fd < 0) {
        perror("Error opening session file");
        close(fd); // Close the first file before returning
        return -1;
    }

    struct Admin u;
    struct Session s;
    int count_rec = 0;
    int login_successful = 0; // Flag to check if we found a match
	
    while (read(fd, &u, sizeof(u)) == sizeof(u)) {
	   // printf("given Pass %s Recieved pass %s \n",u.password,pass);
	    u.password[strcspn(u.password, "\r\n")] = '\0';
        // Clean the password received from the client.
        pass[strcspn(pass, "\r\n")] = '\0';
        if (strcmp(u.password, pass) == 0) {
            // Found the correct admin, now find their session
            lseek(session_fd, 0, SEEK_SET); // Rewind session file to search from start
            while(read(session_fd, &s, sizeof(s)) == sizeof(s)) {
                if ( s.status == 0) {
                    lseek(session_fd, -sizeof(s), SEEK_CUR);
                    s.status = 1; // Mark as logged in
                    write(session_fd, &s, sizeof(s));
                    login_successful = 1;
                    break;
                }
            }
            if (login_successful) {
                break; 
            }
        }
    }
    close(session_fd);

    if (login_successful) {
        return 1; // Return 1 for success
    } else {
        return 0; // Return -1 for failure
    }
}

int logout_Admin(){
    int session_fd=open(SESSION_ADMIN,O_RDWR);
    struct Session s;
    int count_rec=0;
    while (read(session_fd,&s,sizeof(s))==sizeof(s)) {
        if (s.id == ADMINID && s.status == 1) {
                lseek(session_fd,count_rec*(sizeof(s)),SEEK_SET);
                s.status=0;
                write(session_fd,&s,sizeof(s));
                close(session_fd);
            return 1;
        }
        else {
                count_rec += 1;
    }
    }
    close(session_fd);
    return -1;
    }
int modifyEmployeename(int empid,char *name){  
         int emp_fd=open(EMP_RECORDS,O_RDWR);
        struct Employee emp;
        while(read(emp_fd,&emp,sizeof(emp))==sizeof(emp)){
        if(emp.empid == empid){
                lseek(emp_fd, -sizeof(emp), SEEK_CUR);

            // Safely copy new name
            strncpy(emp.name, name, sizeof(emp.name) - 1);
            emp.name[sizeof(emp.name) - 1] = '\0';

            // Write updated record
            if (write(emp_fd, &emp, sizeof(emp)) == sizeof(emp)) {
                close(emp_fd);
                return 1;
            } else {
                close(emp_fd);
                return 0;
            }
        }
    }

    close(emp_fd);
    return 0;
}

int modifyEmployeebranch(int empid,char *branch){  
         int emp_fd=open(EMP_RECORDS,O_RDWR);
        struct Employee emp;
        while(read(emp_fd,&emp,sizeof(emp))==sizeof(emp)){
        if(emp.empid == empid){
                lseek(emp_fd, -sizeof(emp), SEEK_CUR);

            // Safely copy new name
            strncpy(emp.branch,branch, sizeof(emp.branch) - 1);
            emp.branch[sizeof(emp.branch) - 1] = '\0';

            // Write updated record
            if (write(emp_fd, &emp, sizeof(emp)) == sizeof(emp)) {
                close(emp_fd);
                return 1;
            } else {
                close(emp_fd);
                return 0;
            }
        }
    }

    close(emp_fd);
    return 0;
}
// Modifying Manager ID on Manager Deletion and Reaasgniment as well
int modifyEmpMangID(int empid,int managerid){  
         int emp_fd=open(EMP_RECORDS,O_RDWR);
        struct Employee emp;
        while(read(emp_fd,&emp,sizeof(emp))==sizeof(emp)){
        if(emp.empid == empid){
                lseek(emp_fd, -sizeof(emp), SEEK_CUR);

            // Safely copy new name
            emp.managerid=managerid;

            // Write updated record
            if (write(emp_fd, &emp, sizeof(emp)) == sizeof(emp)) {
                close(emp_fd);
                return 1;
            } else {
                close(emp_fd);
                return 0;
            }
        }
    }

    close(emp_fd);
    return 0;
}

int DeleteEmployee(int empid) {
    int emp_fd = open(EMP_RECORDS, O_RDWR);
    if (emp_fd < 0) return -1;

    struct Employee emp;

    while ((read(emp_fd, &emp, sizeof(emp))) == sizeof(emp)) {
        if (emp.empid == empid) {
            // Overwrite employee record with blank
            lseek(emp_fd, -sizeof(emp), SEEK_CUR);
            struct Employee blank = {0};
            if (write(emp_fd, &blank, sizeof(blank)) != sizeof(blank)) {
                close(emp_fd);
                return 0;
            }

            // Now handle session deletion
            int fd_session = open(SESSION_EMP, O_RDWR);
            if (fd_session < 0) {
                close(emp_fd);
                return 0;
            }

            struct Session sess1;
            while (read(fd_session, &sess1, sizeof(sess1)) == sizeof(sess1)) {
                if (sess1.id == empid) {
                    lseek(fd_session, -sizeof(sess1), SEEK_CUR);
                    struct Session blank_sess = {0};
                    if (write(fd_session, &blank_sess, sizeof(blank_sess)) != sizeof(blank_sess)) {
                        close(fd_session);
                        close(emp_fd);
                        return 0;
                    }
                    break; // Session found and deleted
                }
            }

            printf("Employee record deleted \n");
	    close(fd_session);
            close(emp_fd);
            return 1;
        }
    }

     printf("Employee record not  deleted \n");
    close(emp_fd);
    return 0; // Employee not found
}

int  DeleteManager(int empid,int managerid){
        int mgr_del=0;//flag for manager record deletion
        int mgr_sess_del=0;//flag for manager session deletion
        int emp_fd = open(MANG_RECORDS, O_RDWR);
        if (emp_fd < 0) { perror("Error opening manager records file"); return 0; }
        struct Manager emp;
        struct Session sess1;
        int fd_session = open(SESSION_MANG, O_RDWR, 0744);
        if (fd_session < 0) { perror("Error opening manager session file"); close(emp_fd); return 0; }
        int emp_rec= open(EMP_RECORDS,O_RDWR);
        if(emp_rec < 0){perror("Error opening employee records file"); close(emp_fd); close(fd_session); return 0;}      
        int retval_rec=0,retval_sess=0,retval_emp=0;
        while (read(emp_fd, &emp, sizeof(emp)) == sizeof(emp)) {
        if (emp.empid == empid && emp.managerid == managerid) {
            lseek(emp_fd, -(sizeof(emp)), SEEK_CUR);
            struct Manager blank = {0};
            retval_rec = write(emp_fd, &blank, sizeof(blank));
            if(retval_rec == sizeof(blank)){
                mgr_del=1;
                break;
            }
        }
}
        if(mgr_del==1){
                while (read(fd_session, &sess1, sizeof(sess1))) {
                    if (sess1.id == managerid) {
                        lseek(fd_session, -(sizeof(sess1)), SEEK_CUR);
                        struct Session bla = {0};
                        retval_sess = write(fd_session, &bla, sizeof(bla));
                         if(retval_sess == sizeof(bla)){
                                mgr_sess_del=1;
                                break;
                         }

                        }
                }
                if(mgr_sess_del==1){
                        //Reassign Employees of deleted manager to Admin (managerid=0)
                        struct Employee emp_mang;
                        lseek(emp_rec,0,SEEK_SET);
                        while (read(emp_rec, &emp_mang, sizeof(emp_mang)) == sizeof(emp_mang)) {
                            if (emp_mang.managerid == managerid) {
                                lseek(emp_rec, -(sizeof(emp_mang)), SEEK_CUR);
                                emp_mang.managerid = 1; // Reassign to Admin
                                retval_emp=write(emp_rec, &emp_mang, sizeof(emp_mang));
                                if(retval_emp != sizeof(emp_mang)){
                                        //failed to reassign employee
                                        close(fd_session);
                                        close(emp_fd);
                                        close(emp_rec);
                                        printf("Reassignment to admin Failed \n");
                                        return 0;
                                }
                                else{
                                       printf("Done updating maanger id to 1\n");
                                }
                            }
                        }
                }
                else{
                        //failed to delete manager session record
                        close(fd_session);
                        close(emp_fd);
                        close(emp_rec);
                        printf("failed to delete manager session \n");
                        return 0;
                }
        }
        else{
                //failed to delete manager record
                close(fd_session);
                close(emp_fd);
                 printf("failed to delete manager record \n");
                return 0;
        }
        close(fd_session);
        close(emp_fd);
        close(emp_rec);
        return 1;
}


int changePasswordAdmin(char *pass) {
    int emp_fd = open(ADMIN_CRED, O_RDWR);
    if (emp_fd < 0) {
        perror("error opening admin cred file \n");
        return -1;
    }

    struct Admin adm;
    read(emp_fd, &adm, sizeof(adm));
    lseek(emp_fd, -(sizeof(adm)), SEEK_CUR);

    strncpy(adm.password, pass, sizeof(adm.password)-1); 
    // This guarantees the string is properly terminated, fixing the login bug.
    adm.password[sizeof(adm.password) - 1] = '\0';
    int retval = write(emp_fd, &adm, sizeof(adm));
    close(emp_fd);
    if (retval > 0) {
        return 1; // Success
    } else {
        return 0; // Failure
    }
}
void Admin(int sd){
	while(1){
        char cmd[BUFFER_SIZE];
        char buf[BUFFER_SIZE];
        memset(cmd, 0, sizeof(cmd));
        memset(buf, 0, sizeof(buf));
        read(sd, cmd, sizeof(cmd));
        //  create_Admin(sd,"Admin2025@12");
	//printf("Inside Admin Server \n");
        int choice;
        sscanf(cmd, "%d", &choice);
	//printf("excercised choice is %d \n",choice);
	
          switch(choice){
          case 1:
		  {
          // Change Admin Password
          read(sd, buf, sizeof(buf));
                char pass[13];
                sscanf(buf, "%12s",pass);
                int retval=changePasswordAdmin(pass);
                if(retval == 1){
                        write(sd,"Password Changed Successfully \n",32);
                }
                else if(retval==0){
                        write(sd,"Password Change Failed \n",25);
                }
                else{
                        write(sd,"Error in opening file \n",24);
                }
		  }
        break;
        case 2:
	{
        // create Customer
                read(sd, buf, sizeof(buf));
                int id;
                char name[20];
                char branch[20];
                char password[13];
		//printf("%s \n",buf);
                sscanf(buf, "%d %s %s %s",&id,name,branch,password);
		//printf("Inside create_Customer: id=%d, name=%s, branch=%s, password=%s\n", id, name, branch, password);
                create_Customer(sd,id,password,name,branch);
        }

                break;
            case 3:
	{
         // create Employee
                read(sd, buf, sizeof(buf));
                int managerid;
		char name[20];
		char branch[20];
                char password[13];
                sscanf(buf, "%d %s %s %s",&managerid,password,name,branch);
                create_Employee(sd,managerid,password,name,branch);
        }
                break;
	
            case 4:
	{
            // create Manager
                    read(sd, buf, sizeof(buf));
                    int empid;
                    char password[13];
                    sscanf(buf, "%d %s",&empid,password);
                    create_Manager(sd,empid,password);
        }
                    break;
	
           case 5:
	{
                // Modify Customer name
                read(sd, buf, sizeof(buf));
                int acc_no;
                int id;
                char name[20];
                sscanf(buf, "%d %d %s", &acc_no,&id,name);
                int retval_name=modifyCustomername(acc_no,id,name);
                if(retval_name == 1){
                        write(sd,"Customer Name Modified Successfully \n",38);
                }
                else{
                        write(sd,"Customer Name Modification Failed \n",36);
                }
        }
          break;
	
          case 6:
	{
         // modify Customer branch
                read(sd, buf, sizeof(buf));
                int acc_no;
                int id;
                char branch[20];
                sscanf(buf, "%d %d %s", &acc_no,&id,branch);
                int retval_branch=modifyCustomerbranch(acc_no,id,branch);
                if(retval_branch == 1){
                        write(sd,"Customer Branch Modified Successfully \n",39);
                }
                else{
                        write(sd,"Customer Branch Modification Failed \n",36);
                }
        }
          break;
          case 7:
	{
                // Delete Customer
                        read(sd, buf, sizeof(buf));
                        int acc_no;
                        int id;
                        sscanf(buf, "%d %d", &acc_no,&id);
                        int retval=DeleteCustomer(acc_no,id);
                        if(retval == 1){
                        write(sd,"Customer Deleted Successfully \n",32);
                }
                else{
                        write(sd,"Customer deleted Failed \n",26);
                }
        }
        break;
        case 8:
	{
        // modify Employee name
                read(sd, buf, sizeof(buf));
                int empid;
                char name[20];
                sscanf(buf, "%d %s", &empid,name);
                int retval_name=modifyEmployeename(empid,name);
                if(retval_name == 1){
                        write(sd,"Employee Name Modified Successfully \n",38);
                }
                else{
                        write(sd,"Employee Name Modification Failed \n",36);
                }
        }
                break;
        case 9:
	{
        // modify Employee branch
                read(sd, buf, sizeof(buf));
                int empid;
                char branch[20];
                sscanf(buf, "%d %s", &empid,branch);
                int retval_branch=modifyEmployeebranch(empid,branch);
                if(retval_branch == 1){
                        write(sd,"Employee Branch Modified Successfully \n",39);
                }
                else{
                        write(sd,"Employee Branch Modification Failed \n",36);
                }
        }
                break;
         case 10:
	{
        // modify Employee managerid
                read(sd, buf, sizeof(buf));
                int empid;
                int managerid;
                sscanf(buf, "%d %d", &empid,&managerid);
                int retval_mang=modifyEmpMangID(empid,managerid);
                if(retval_mang == 1){
                        write(sd,"Employee managerid Modified Successfully \n",39);
                }
                else{
                        write(sd,"Employee managerid Modification Failed \n",39);
                }
        }
                break;
        case 11:
	{
                // Delete Employee
                                read(sd, buf, sizeof(buf));
                                int empid;
                                sscanf(buf, "%d",&empid);
                                int retval=DeleteEmployee(empid);
                                if((retval==1)){
                                //write(sd,"Employee Deleted Successfully \n",32);
                                write(sd,"Employee Deleted Successfully \n",32); 

                        }
                        else{
                                write(sd,"Employee deleted Failed \n",26);
                        }
                }
			break;
         case 12:
	{
                // Delete Manager
                        read(sd, buf, sizeof(buf));
                        int managerid;
                        int empid;
                        sscanf(buf,"%d %d",&managerid,&empid);
                       int retval=DeleteManager(empid,managerid);
                        if(retval == 1){
                                write(sd,"Manager Removed Successfully \n",31);
                        }
                        else{
                                write(sd,"Manager Removal Failed \n",25);
                        }
                }
        break;
         case 13: {
            // View customers
            CustomerDetails(sd);
             //printf("end of customer details sent response from admin function \n");  // corrected function name
            break;
        }
        case 14:
        {
                // View employees
                EmployeesDetails(sd);  // corrected function name
        }
                break;
         case 15:
        {
                // View managers
                ManagerDetails(sd);  // corrected function name
        }
                break;
        case 16:
	{
              // logout
                  int retval=logout_Admin();
                        if(retval == 1){
                                write(sd,"Logout Successful \n",19);
                        }
                        else{
                                write(sd,"Logout Failed \n",16);
                        }
                }
                  return;
                  break;

                default:
                logout_Admin();
                return;
                break;      
          }
	}
}
