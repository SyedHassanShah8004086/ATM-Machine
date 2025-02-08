#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void displayMenu() {
    printf("\nATM Menu:\n");
    printf("1. Check Balance\n");
    printf("2. Deposit Funds\n");
    printf("3. Withdraw Funds\n");
    printf("4. Exit\n");
    printf("Choose an option: ");
}

int main() {
    int balance = 1000;  // Initial balance
    int pipe_fd[2];
    
    if (pipe(pipe_fd) == -1) {
        perror("Pipe failed");
        return 1;
    }
    
    while (1) {
        displayMenu();
        
        int choice;
        scanf("%d", &choice);
        
        if (choice == 4) {
            printf("Exiting ATM. Goodbye!\n");
            break;
        }
        
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("Fork failed");
            return 1;
        }
        
        if (pid == 0) {  // Child process
            close(pipe_fd[0]); // Close read end of pipe
            
            int amount;
            switch (choice) {
                case 1:
                    write(pipe_fd[1], &balance, sizeof(balance));  // Send balance to parent
                    break;
                
                case 2:
                    printf("Enter deposit amount: ");
                    scanf("%d", &amount);
                    balance += amount;
                    write(pipe_fd[1], &balance, sizeof(balance));  // Send updated balance
                    break;
                
                case 3:
                    printf("Enter withdrawal amount: ");
                    scanf("%d", &amount);
                    if (amount > balance) {
                        printf("Insufficient balance!\n");
                    } else {
                        balance -= amount;
                    }
                    write(pipe_fd[1], &balance, sizeof(balance));  // Send updated balance
                    break;
                
                default:
                    printf("Invalid choice. Try again.\n");
                    break;
            }
            close(pipe_fd[1]);  // Close write end
            exit(0);
        } else {  // Parent process
            close(pipe_fd[1]);  // Close write end of pipe
            
            wait(NULL);  // Wait for child process to finish
            
            if (choice == 1 || choice == 2 || choice == 3) {
                read(pipe_fd[0], &balance, sizeof(balance));  // Read balance from child
                printf("Updated Balance: %d\n", balance);
            }
            
            close(pipe_fd[0]);  // Close read end for the next loop
            pipe(pipe_fd);      // Re-initialize pipe for next iteration
        }
    }
    return 0;
}

