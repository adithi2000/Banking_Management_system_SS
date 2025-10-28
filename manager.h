#ifndef MANAGER_H
#define MANAGER_H

void create_Manager(int sd, int empid, char *pass);
int login_Manager(int sd, int empid, int id, char *pass);
void viewLoanApplications(int sd);
int ActivateAcc(int acc_no,int id);
int DeactivateAcc(int acc_no,int id);
void AssignLoans(int sd,int loan_id,int empid);
int changePasswordManager(int managerid,char *pass);
int logout_Manager(int managerid);
void Manager(int managerid,int sd);
void viewFeedback(int sd);
void ManagerDetails(int sd);
#endif
