#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>
#include "customer.h"
#include "bank_struct.h"
#include "common.h"
#include "employee.h"
#include "Admin.h"
#include "manager.h"

#include <netinet/tcp.h> // For TCP_NODELAY


// Define BUFFER_SIZE if not in common.h
#define BUFFER_SIZE 1024


int main(int argc, char *argv[]) {
    struct sockaddr_in serv, client;
    int old_sd, new_sd;
    socklen_t sz;

    old_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (old_sd < 0) {
        perror("socket");
        exit(1);
    }
    int opt_nodelay = 1;
    if (setsockopt(old_sd, IPPROTO_TCP, TCP_NODELAY, &opt_nodelay, sizeof(opt_nodelay))) {
    perror("setsockopt TCP_NODELAY failed");
    fprintf(stderr, "Warning: Could not set TCP_NODELAY. Buffering might occur.\n");
    
} else {
    printf("TCP_NODELAY option enabled.\n");
}

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(8083); // Port from previous examples

    if (bind(old_sd, (void *)(&serv), sizeof(serv)) < 0) {
        perror("bind");
        exit(1);
    }
    int fd_check = open(ADMIN_CRED, O_RDONLY);
        if (fd_check == -1) {
            printf("Admin file not found. Creating it now...\n");
            create_Admin(); // Pass -1 so create_Admin receives an int as expected
            printf("Admin file created.\n");
        } else {
            printf("Admin file already exists. Skipping creation.\n");
            close(fd_check);
        }
    
    if (listen(old_sd, 5) < 0) {
        perror("listen");
        exit(1);
    }
    printf("The Server is Running on %s port %d \n", inet_ntoa(serv.sin_addr), ntohs(serv.sin_port));

    sz = sizeof(client);

    while ((new_sd = accept(old_sd, (struct sockaddr *)(&client), &sz))) {
        if (new_sd < 0) {
            perror("accept");
            continue;
        }
        printf("Connected to client\n");

        if (fork() == 0) {
            // Child process starts here
            close(old_sd);

            char cmd[BUFFER_SIZE];
            char buffer[BUFFER_SIZE];

            memset(cmd, 0, sizeof(cmd));
            memset(buffer, 0, sizeof(buffer));

            // Read the main choice (1, 2, 3, 4)
            if (read(new_sd, cmd, sizeof(cmd)) <= 0) {
                 printf("Client disconnected before sending choice.\n");
                 close(new_sd);
                 exit(0);
            }
           // printf("The choice chosen was %s \n", cmd);
            int choice;
            sscanf(cmd, "%d", &choice);

            const int MAX_ATTEMPTS = 3; // Max login tries

            if (choice == 1) {
                // Customer logic with retry
              //  printf("Inside Customer menu\n");
                int login_attempts = 0;
                while(1) { // Loop for login attempts
                    memset(buffer, 0, sizeof(buffer));
                    int bytes_read = read(new_sd, buffer, sizeof(buffer));
                     if (bytes_read <= 0) {
                      //  printf("Client disconnected during customer login.\n");
                        break; // Exit login loop
                    }

                    int id;
                    char pass[13];
                    int acc_no;
                    sscanf(buffer, "%d %d %12s", &acc_no, &id, pass);
                    
                   // printf("Attempting customer login for acc_no: %d, id: %d\n", acc_no, id);
                    int retval = login_Customer(new_sd, acc_no, id, pass);

                    if (retval == 1) { // Success
                        write(new_sd, "Login Success\n", 14);
                        customer(acc_no, new_sd);
                        break; // Exit login loop
                    } else if (retval == 0) { // Login failure
                        login_attempts++;
                       // printf("Customer login failed. Attempt %d of %d.\n", login_attempts, MAX_ATTEMPTS);
                        if (login_attempts >= MAX_ATTEMPTS) {
                            write(new_sd, "Too many failed attempts. Closing connection.\n", 44);
                            break; // Exit login loop
                        }
                        write(new_sd, "Login Failed, please try again.\n", 32);
                    } else { // System failure (retval == -1)
                        write(new_sd, "System Error during login. Exiting.\n", 36);
                        break; // Exit login loop
                    }
                }

            } else if (choice == 2) {
                // Employee logic with retry
                //printf("Inside Employee menu\n");
                int login_attempts = 0;
                while(1) {
                     memset(buffer, 0, sizeof(buffer));
                    int bytes_read = read(new_sd, buffer, sizeof(buffer));
                     if (bytes_read <= 0) {
                      //  printf("Client disconnected during employee login.\n");
                        break;
                    }

                    int id;
                    char pass[13];
                    sscanf(buffer, "%d %12s", &id, pass);

                  //  printf("Attempting employee login for id: %d\n", id);
                    int retval = login_Employee(new_sd, id, pass);

                    if (retval == 1) { // Success
                        write(new_sd, "Login Success\n", 14);
                        employee(id, new_sd);
                        break;
                    } else if (retval == -1) { // Treat system fail on empty pass also as login fail
                         // Assuming login_Employee returns 0 for fail, -1 for sys error
                        login_attempts++;
                      //  printf("Employee login failed. Attempt %d of %d.\n", login_attempts, MAX_ATTEMPTS);
                         if (login_attempts >= MAX_ATTEMPTS) {
                            write(new_sd, "Too many failed attempts. Closing connection.\n", 44);
                            break;
                        }
                        write(new_sd, "Login Failed, please try again.\n", 32);
                    }
                     else { // System failure (retval == -1)
                         write(new_sd, "System Error during login. Exiting.\n", 36);
                        break;
                    }
                }

            } else if (choice == 3) {
                // Manager logic with retry
                // printf("Inside Manager menu\n");
                int login_attempts = 0;
                while(1) {
                    memset(buffer, 0, sizeof(buffer));
                    int bytes_read = read(new_sd, buffer, sizeof(buffer));
                     if (bytes_read <= 0) {
                      //  printf("Client disconnected during manager login.\n");
                        break;
                    }

                    int id; // This seems to be managerid based on client code
                    int empid;
                    char pass[13];
                    sscanf(buffer, "%d %d %12s", &id, &empid, pass); // Assuming client sends managerid first

                   // printf("Attempting manager login for managerid: %d, empid: %d\n", id, empid);
                    int retval = login_Manager(new_sd, empid, id, pass);

                     if (retval == 1) { // Success
                        write(new_sd, "Login Success\n", 14);
                        Manager(id, new_sd); // Pass managerid to Manager function
                        break;
                    } else if (retval == -1) { // Login failure
                        login_attempts++;
                      //  printf("Manager login failed. Attempt %d of %d.\n", login_attempts, MAX_ATTEMPTS);
                         if (login_attempts >= MAX_ATTEMPTS) {
                            write(new_sd, "Too many failed attempts. Closing connection.\n", 44);
                            break;
                        }
                        write(new_sd, "Login Failed, please try again.\n", 32);
                    } else { // System failure (retval == -1)
                         write(new_sd, "System Error during login. Exiting.\n", 36);
                        break;
                    }
                }

            } else if (choice == 4) {
                // Admin logic (already has retry)
              //  printf("Inside Admin menu \n");
                int login_attempts = 0;
                // const int MAX_ATTEMPTS = 3; // Already defined above

                while (1) {
                    char buffer_admin[BUFFER_SIZE] = {0}; // Use a different buffer name to avoid confusion

                    int bytes_read = read(new_sd, buffer_admin, sizeof(buffer_admin));
                    if (bytes_read <= 0) {
                     //   printf("Client disconnected or read error during admin login.\n");
                        break;
                    }

                    char pass[13];
                    sscanf(buffer_admin, "%12s", pass);
                    // No need for pass[12] = '\0'; sscanf does it.
                 //   printf("Password received, attempting admin login...\n");
                    int retval = Login_Admin(new_sd, pass);

                    if (retval == 1) { // Success
                         write(new_sd, "Login Success\n", 14);
                        // printf("Login success\n");
                         Admin(new_sd);
                         break;
                    } else if (retval == 0) { // Login Failure
                        login_attempts++;
                       // printf("Admin login failed. Attempt %d of %d.\n", login_attempts, MAX_ATTEMPTS);
                        if (login_attempts >= MAX_ATTEMPTS) {
                            write(new_sd, "Too many failed attempts. Closing connection.\n", 44);
                          //  printf("Max login attempts reached. Terminating session.\n");
                            break;
                        }
                        write(new_sd, "Login Failed, please try again.\n", 32);
                    } else { // System Failure (retval == -1)
                         write(new_sd, "System Error during login. Exiting.\n", 36);
                        // printf("System error during admin login.\n");
                        break;
                    }
                }
            } else {
                write(new_sd, "Invalid Choice Received. Exiting.\n", 34);
            }

            // Child process exits cleanly after its task (login loop finished or menu function returned)
            printf("Client Disconnected or Reconnecting if needed\n");
            close(new_sd);
            exit(0);

        } else {
            // Parent process
            close(new_sd);
        }
    }
    close(old_sd);
    return 0;
}