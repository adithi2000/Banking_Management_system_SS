
#define ADMIN_H
void create_Admin();
int Login_Admin(int sd, char *pass);
int changePasswordAdmin(char *newpass);
void Admin(int sd);
int logout_Admin();
void DeleteEmployee(int empid);
void DeleteManager(int empid,int managerid);
int modifyEmployeename(int empid,char *name);
int modifyEmployeebranch(int empid,char *branch);
int DeleteCustomer(int acc_no,int id);
int modifyEmpMangID(int empid,int managerid);
