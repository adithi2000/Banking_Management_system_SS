#ifndef EMPLOYEE_H
#define EMPLOYEE_H

void create_Employee(int sd, int manager_id, char *pass,char *name,char *branch);
int login_Employee(int sd,int empid, char *pass);
// use create_Customer for creating customer from employee side
// use changePassword based function to modify customer detail
int modifyCustomerbranch(int acc_no,int id,char *branch);
int modifyCustomername(int acc_no,int id,char *name);
int approveLoans(int sd,int loan_id,int acc_no);
//for deposit to account after loan use deposit() function
void viewLoanAppln(int sd, int empid);
int logout_Employee(int empid);
void employee(int empid,int sd);
int DeleteCustomer(int acc_no,int id);
int changePasswordEmployee(int empid,char *pass);
void EmployeesDetails(int sd);

#endif
