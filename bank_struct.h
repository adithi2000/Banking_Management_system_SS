#ifndef BANK_STRUCT_H
#define BANK_STRUCT_H

#define BUFFER_SIZE 1024
#define USER_RECORDS "user.db"
#define EMP_RECORDS "employee.db"
#define MANG_RECORDS "manager.db"
#define ACCOUNT_MGMT "Accounts.db"
#define SESSION_CUST "session_cust.db"
#define SESSION_EMP "session_emp.db"
#define SESSION_MANG "session_mang.db"
#define SESSION_ADMIN "session_admin.db"
#define TRANSACTION_HIS "transactions.db"
#define LOAN_DB "loans.db"
#define FEEDBACK "feebacks.db"
#define ADMIN_CRED "admin_credentials.db"
#define ADMINID 12984560

struct Customer
{
        int acc_no;
        int id;
    char password[13];  // 12 chars + '\0'
    char name[20];
    char branch[4];
    int active;
};
struct Transaction{
        int acc_no;
        double amt;
        char operation[10];
        char *timestamp;
        // 1: Deposit , 2: withdrwal
};
struct Session{
        int id; // acc_no for customers
        int status;
        // 0: not active ,1: active session exists
        // to be used only on login
};
struct Loan{
        int acc_no;
        double amt;
        int empid;
        int status;
	int loan_id;
        // 0: not approved, 1: Approved
};
struct Bank_Balance{
        int acc_no;
        double balance;
};
struct Employee{
        char name[20];
        char branch[20];
        int empid;
        int managerid;
        char password[13];
};
struct Manager{
        int managerid;
        int empid;
        char password[13];
};

struct feedback{
	int acc_no;
	char feedback[BUFFER_SIZE];
};
struct Admin{
        int adminid;
        char username[20];
        char password[13];
};

#endif
