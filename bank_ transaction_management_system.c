#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ADMIN_PASSWORD "admin123"

typedef struct {
    char username[50];
    char password[50];
    char pin[10];
} User;

typedef struct {
    int accountNumber;
    char username[50];
    float balance;
} Account;

// Function Prototypes
void mainMenu();
void adminMenu();
void userMenu();
void registerUser();
int loginUser(User *u);
void userFunctions(User u);

void deposit(User u);
void withdraw(User u);
void transferFunds(User u);
void checkBalance(User u);
void viewTransactionHistory(User u);
void applyLoan(User u);
void clearBuffer();

int main() {
    mainMenu();
    return 0;
}

void mainMenu() {
    int choice;
    while (1) {
        printf("\n--- Main Menu ---\n");
        printf("1. Admin Menu\n");
        printf("2. User Menu\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearBuffer();

        switch (choice) {
            case 1:
                adminMenu();
                break;
            case 2:
                userMenu();
                break;
            case 3:
                printf("Exiting...\n");
                exit(0);
            default:
                printf("Invalid choice!\n");
        }
    }
}

void adminMenu() {
    char password[50];
    printf("\nEnter Admin Password: ");
    scanf("%s", password);
    clearBuffer();

    if (strcmp(password, ADMIN_PASSWORD) == 0) {
        int choice;
        while (1) {
            printf("\n--- Admin Panel ---\n");
            printf("1. Register New User\n");
            printf("2. View All Users\n");
            printf("3. Logout\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);
            clearBuffer();

            switch (choice) {
                case 1:
                    registerUser();
                    break;
                case 2: {
                    FILE *f = fopen("users.txt", "r");
                    if (f == NULL) {
                        printf("No users found.\n");
                        break;
                    }
                    User u;
                    printf("\nRegistered Users:\n");
                    while (fscanf(f, "%s %s %s", u.username, u.password, u.pin) == 3) {
                        printf("Username: %s\n", u.username);
                    }
                    fclose(f);
                    break;
                }
                case 3:
                    printf("Logging out of admin panel...\n");
                    return;
                default:
                    printf("Invalid choice.\n");
            }
        }
    } else {
        printf("Incorrect Admin Password!\n");
    }
}

void registerUser() {
    FILE *f = fopen("users.txt", "a");
    if (f == NULL) {
        printf("Error opening users file.\n");
        return;
    }

    User u;
    printf("\n--- Register New User ---\n");
    printf("Enter username: ");
    scanf("%s", u.username);
    printf("Enter password: ");
    scanf("%s", u.password);
    printf("Enter PIN: ");
    scanf("%s", u.pin);

    fprintf(f, "%s %s %s\n", u.username, u.password, u.pin);
    fclose(f);
    printf("User registered successfully!\n");

    // Also create an account for the new user.
    FILE *fa = fopen("accounts.txt", "a");
    if (fa == NULL) {
        printf("Error opening accounts file.\n");
        return;
    }
    // Generate a random 5-digit account number.
    int accountNumber = rand() % 90000 + 10000;
    fprintf(fa, "%d %s %.2f\n", accountNumber, u.username, 0.0);
    fclose(fa);
    // Print the new account number so the user knows which account to use.
    printf("New account created for %s. Your account number is: %d\n", u.username, accountNumber);
}

int loginUser(User *loggedInUser) {
    FILE *f = fopen("users.txt", "r");
    if (f == NULL) {
        printf("User database not found.\n");
        return 0;
    }

    char uname[50], pass[50];
    printf("\n--- User Login ---\n");
    printf("Enter username: ");
    scanf("%s", uname);
    printf("Enter password: ");
    scanf("%s", pass);

    User u;
    while (fscanf(f, "%s %s %s", u.username, u.password, u.pin) == 3) {
        if (strcmp(u.username, uname) == 0 && strcmp(u.password, pass) == 0) {
            *loggedInUser = u;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

void userMenu() {
    User u;
    if (loginUser(&u)) {
        printf("Welcome, %s!\n", u.username);
        userFunctions(u);
    } else {
        printf("Login failed. Invalid credentials.\n");
    }
}

void userFunctions(User u) {
    int choice;
    while (1) {
        printf("\n--- User Dashboard ---\n");
        printf("1. Deposit\n");
        printf("2. Withdraw\n");
        printf("3. Transfer Funds\n");
        printf("4. Check Balance\n");
        printf("5. View Transaction History\n");
        printf("6. Apply for Loan\n");
        printf("7. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearBuffer();

        switch (choice) {
            case 1:
                deposit(u);
                break;
            case 2:
                withdraw(u);
                break;
            case 3:
                transferFunds(u);
                break;
            case 4:
                checkBalance(u);
                break;
            case 5:
                viewTransactionHistory(u);
                break;
            case 6:
                applyLoan(u);
                break;
            case 7:
                printf("Logging out...\n");
                return;
            default:
                printf("Invalid choice.\n");
        }
    }
}

void deposit(User u) {
    int acc;
    float amount;
    FILE *f = fopen("accounts.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (f == NULL || temp == NULL) {
        printf("Error opening accounts file.\n");
        return;
    }

    Account a;
    printf("Enter your account number: ");
    scanf("%d", &acc);
    printf("Enter amount to deposit: ");
    scanf("%f", &amount);

    while (fscanf(f, "%d %s %f", &a.accountNumber, a.username, &a.balance) == 3) {
        if (a.accountNumber == acc && strcmp(a.username, u.username) == 0) {
            a.balance += amount;
            printf("Deposit successful.\n");
            FILE *tf = fopen("transactions.txt", "a");
            if (tf != NULL) {
                fprintf(tf, "%s deposited %.2f\n", a.username, amount);
                fclose(tf);
            }
        }
        fprintf(temp, "%d %s %.2f\n", a.accountNumber, a.username, a.balance);
    }

    fclose(f);
    fclose(temp);
    remove("accounts.txt");
    rename("temp.txt", "accounts.txt");
}

void withdraw(User u) {
    int acc;
    float amount;
    FILE *f = fopen("accounts.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (f == NULL || temp == NULL) {
        printf("Error opening accounts file.\n");
        return;
    }

    Account a;
    int found = 0;
    printf("Enter your account number: ");
    scanf("%d", &acc);
    printf("Enter amount to withdraw: ");
    scanf("%f", &amount);

    while (fscanf(f, "%d %s %f", &a.accountNumber, a.username, &a.balance) == 3) {
        if (a.accountNumber == acc && strcmp(a.username, u.username) == 0) {
            found = 1;
            if (a.balance >= amount) {
                a.balance -= amount;
                printf("Withdrawal successful.\n");
                printf("Remaining balance: %.2f\n", a.balance);
                FILE *tf = fopen("transactions.txt", "a");
                if (tf != NULL) {
                    fprintf(tf, "%s withdrew %.2f\n", a.username, amount);
                    fclose(tf);
                }
            } else {
                printf("Insufficient balance!\n");
            }
        }
        fprintf(temp, "%d %s %.2f\n", a.accountNumber, a.username, a.balance);
    }

    if (!found) {
        printf("Account not found or does not belong to you.\n");
    }

    fclose(f);
    fclose(temp);
    remove("accounts.txt");
    rename("temp.txt", "accounts.txt");
}

void transferFunds(User u) {
    int senderAcc, receiverAcc;
    float amount;
    printf("Enter your account number: ");
    scanf("%d", &senderAcc);
    printf("Enter receiver's account number: ");
    scanf("%d", &receiverAcc);
    printf("Enter amount to transfer: ");
    scanf("%f", &amount);

    FILE *f1 = fopen("accounts.txt", "r");
    FILE *temp1 = fopen("temp1.txt", "w");
    if (f1 == NULL || temp1 == NULL) {
        printf("Error opening accounts file.\n");
        return;
    }

    Account a;
    int senderSuccess = 0;
    float senderNewBalance = 0.0;

    while (fscanf(f1, "%d %s %f", &a.accountNumber, a.username, &a.balance) == 3) {
        if (a.accountNumber == senderAcc && strcmp(a.username, u.username) == 0) {
            if (a.balance >= amount) {
                a.balance -= amount;
                senderNewBalance = a.balance;
                senderSuccess = 1;
            } else {
                printf("Insufficient balance!\n");
            }
        }
        fprintf(temp1, "%d %s %.2f\n", a.accountNumber, a.username, a.balance);
    }
    fclose(f1);
    fclose(temp1);
    remove("accounts.txt");
    rename("temp1.txt", "accounts.txt");

    if (!senderSuccess) return;

    FILE *f2 = fopen("accounts.txt", "r");
    FILE *temp2 = fopen("temp2.txt", "w");
    if (f2 == NULL || temp2 == NULL) {
        printf("Error opening accounts file.\n");
        return;
    }

    int receiverFound = 0;
    while (fscanf(f2, "%d %s %f", &a.accountNumber, a.username, &a.balance) == 3) {
        if (a.accountNumber == receiverAcc) {
            a.balance += amount;
            receiverFound = 1;
        }
        fprintf(temp2, "%d %s %.2f\n", a.accountNumber, a.username, a.balance);
    }
    fclose(f2);
    fclose(temp2);
    remove("accounts.txt");
    rename("temp2.txt", "accounts.txt");

    if (receiverFound) {
        printf("Transfer successful.\n");
        printf("Remaining balance: %.2f\n", senderNewBalance);
        FILE *tf = fopen("transactions.txt", "a");
        if (tf != NULL) {
            fprintf(tf, "%s transferred %.2f to account %d\n", u.username, amount, receiverAcc);
            fclose(tf);
        }
    } else {
        printf("Receiver account not found.\n");
    }
}


void checkBalance(User u) {
    int acc;
    Account a;
    FILE *f = fopen("accounts.txt", "r");
    if (f == NULL) {
        printf("Error opening accounts file.\n");
        return;
    }

    printf("Enter your account number: ");
    if (scanf("%d", &acc) != 1) {
        printf("Invalid input.\n");
        fclose(f);
        clearBuffer();
        return;
    }
    clearBuffer();

    int found = 0;
    while (fscanf(f, "%d %s %f", &a.accountNumber, a.username, &a.balance) == 3) {
        if (a.accountNumber == acc && strcmp(a.username, u.username) == 0) {
            printf("Your balance for account %d is: %.2f\n", a.accountNumber, a.balance);
            found = 1;
            break;
        }
    }
    fclose(f);
    if (!found) {
        printf("Account not found or does not belong to you.\n");
    }
}

void viewTransactionHistory(User u) {
    FILE *f = fopen("transactions.txt", "r");
    if (f == NULL) {
        printf("No transaction history available.\n");
        return;
    }
    char line[100];
    printf("\nTransaction History for %s:\n", u.username);
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, u.username)) {
            printf("%s", line);
        }
    }
    fclose(f);
}

void applyLoan(User u) {
    int acc;
    float amount;
    printf("Enter your account number: ");
    scanf("%d", &acc);
    printf("Enter loan amount: ");
    scanf("%f", &amount);

    FILE *f = fopen("loans.txt", "a");
    if (f == NULL) {
        printf("Error opening loans file.\n");
        return;
    }
    fprintf(f, "%d %s %.2f pending\n", acc, u.username, amount);
    fclose(f);
    printf("Loan application submitted.\n");
}

void clearBuffer() {
    while (getchar() != '\n');
}
