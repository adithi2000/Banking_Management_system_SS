# Banking Management System

## Overview

This project implements a client-server based banking management system. It allows different types of users (Customers, Employees, Managers, Admins) to interact with the system via separate client and server applications.

---

## File Structure

The project is divided into client-side and server-side components, along with common utility functions.

| File(s)                     | Role                                                          | Side         |
| :-------------------------- | :------------------------------------------------------------ | :----------- |
| `server.c`                  | Main server application startup and connection handling.        | Server       |
| `client.c`                  | Main client application startup and connection handling.        | Client       |
| `common.c`                  | Contains shared utility functions (e.g., filename generation, timestamp, string trimming) used by both client and server. | Common       |
| `Admin.c`                   | Implements server-side functionalities for Administrators.      | Server       |
| `admin_client.c`            | Implements client-side interface/logic for Administrators.    | Client       |
| `customer.c`                | Implements server-side functionalities for Customers.         | Server       |
| `customer_client.c`         | Implements client-side interface/logic for Customers.       | Client       |
| `employee.c`                | Implements server-side functionalities for Employees.         | Server       |
| `employee_client.c`         | Implements client-side interface/logic for Employees.       | Client       |
| `manager.c`                 | Implements server-side functionalities for Managers.          | Server       |
| `manager_client.c`          | Implements client-side interface/logic for Managers.        | Client       |
| `bank_struct.h` (Implicit)  |  contains structure definitions (`Customer`, `Employee`, `Admin`, `Manager`, `Session`, `Transaction`, `Loan`, etc.). | Common Header |
| `common.h` (Implicit)       |  contains function prototypes for `common.c` and shared constants (`BUFFER_SIZE`, file paths). | Common Header |
| `*.h` (Implicit)            | Header files corresponding to each `.c` file (`Admin.h`, `customer.h`, etc.) containing relevant function prototypes. | Specific Headers |

---

## Compilation

Use a C compiler (like GCC) to build the executables. Ensure all necessary header files are correctly included within the `.c` files.

1.  **Server Application:**
    ```bash
    gcc common.c server.c Admin.c employee.c manager.c customer.c -o serverapp
    ```

2.  **Client Application:**
    ```bash
    gcc common.c admin_client.c employee_client.c manager_client.c customer_client.c client.c -o clientapp
    ```

---

## Usage

1.  Start the server application:
    ```bash
    ./serverapp
    ```
    *(Optionally, use `./serverapp create` for initial admin setup if implemented).*
2.  Start one or more client applications:
    ```bash
    ./clientapp
    ```
3.  Follow the prompts within the client application to connect and interact with the banking system.