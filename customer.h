
#define CUSTOMER_H

void create_Customer(int sd, int id, char *pass,char *name,char *branch);
int login_Customer(int sd,int acc_no,int id, char *pass);
double check_balance_for_fund_and_general(int acc_no);
int deposit(int acc_no, double money);
int Withdrawal(int acc_no, double money);
int fund_to_beneficiary(int s_acc,int r_acc,double money);
void apply_Loan(int acc_no,double money);
int logout_Customer(int acc_no);
void addFeedback(int acc_no,char feedback[1024]);
int changePasswordCustomer(int acc_no,char *pass);
void customer(int acc_no,int sd);
void CustomerDetails(int sd);
void viewPassbook(int sd,int acc_no);

