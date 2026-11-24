/*************************** 
Last names: Danieles, De Guzman, Nono, Suazon
Language: C
Paradigm(s): Procedural, Imperative, Structured
***************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NAME 100
#define DEFAULT_BALANCE 0.00
#define ANNUAL_RATE 0.05
#define MAX_DAYS 3650

#define MAX_CURR 8
#define MAX_TYPE 16
#define START_SIZE 1024
#define CSV_FILE "bank_transactions.csv"

/*
* Enum representing the available currencies.
* The integer values match the menu choices (1-6) minus 1.
*/
typedef enum{
    PHP, USD, JPY, GBP, EUR, CNY, CURRENCY_COUNT
} Currency;


/*
* Array containing the full names of each currency.
* Index corresponds to the Currency enum.
*/
const char* CURRENCY_NAMES[6] = {
    "Philippine Peso", "United States Dollar", "Japanese Yen",
    "British Pound Sterling", "Euro", "Chinese Yuan Renminni"
};

/*
* Array containing the 3-letter codes of each currency.
* Index corresponds to the Currency enum.
*/
const char* CURRENCY_CODES[6] = {
    "PHP", "USD", "JPY", "GBP", "EUR", "CNY"
};


/*
* Structure representing a user's bank account.
* Holds the account name, current balance, base currency, and account number.
*/
typedef struct{
    char accountName[MAX_NAME];
    double balance;
    Currency currency;
    long accountNumber;
} Account;

/*
* Structure representing a user's transaction.
*/
typedef struct{
    char account_name[MAX_NAME];
    long account_number;
    int day, month, year;
    int hour, minute, second;
    char currency[MAX_CURR];
    double amount;
    char type[MAX_TYPE];
    double current_balance;
} Transaction;

/*
* What load_transactions() returns.
*/
typedef struct{
    Transaction* list;
    int total_rows;
    int php_rows;
    int invalid_rows;
    int non_php_rows;
} LoadResult;

/*
* What load_transactions() returns.
*/
typedef struct{
    char acc_name[MAX_NAME];
    long acc_num;
    int acc_age_days;
    double current_balance;
    double avg_daily_balance;
    char privilege_tier[16];
} PrivilegeAccount;

/*
* one row of report 2
*/
typedef struct{
    char name[MAX_NAME];
    double current_balance;
    double expected_interest;
} InterestRow;

/*
* one row of report 3
*/
typedef struct{
    char name[MAX_NAME];
    long acc_num;
    int day, month, year;
    double total_withdrawals;
    int num_deposits;
    double total_deposits;
    double turnover_velocity;
    char risk_flag[16];
} DailySummary;

//Function Prototypes for main transactions
void showMenu();
int  getChoice();
void clearInput();
void clearScreen();
int  readDouble(double* value);
int  readInt(int* value);
int  is_valid_name(const char* name);
void registerAccount(Account* acct);
int find_account(const char* name, long* out_number, double* out_balance);
int  append_transaction(const char* name, long number, const char* type, double amount, double balance);
void depositAmount(Account* acct, double rates[]);
void withdrawAmount(Account* acct);
void recordExchangeRate(double rates[]);
void currencyExchange(double rates[]);
void showInterestAmount(Account* acct);
char askBackToMenu();
char askConvertAnother();

//Function Prototypes for CSV loader
LoadResult load_transactions(const char* filename);
void free_load_result(LoadResult* r);
void strip_newline(char* s);
int  split_line(char* line, char* fields[], int max_fields);
int  is_php(const char* s);

//Function Prototypes for the reports
void showReportsMenu();
void generateAllReports(LoadResult* data, Account* acct);
int compare_date(int d1, int m1, int y1, int d2, int m2, int y2);
int days_in_month(int month, int year);
int comp_adb_desc(const void* a, const void* b);
void report1_privelege_tiers(LoadResult* data);
int comp_interest_desc(const void* a, const void* b);
void report2_interest_projection(LoadResult* data);
int comp_daily(const void* a, const void* b);
void report3_suspicious_behavior(LoadResult* data);
void report4_hourly_transaction(LoadResult* data);
void write_summary_json(LoadResult* data);



/*
* Main entry point of the program.
* Initializes the account and exchange rate array, then runs the main menu loop.
* @param (void)
* @return (int) Returns 0 upon successful termination.
*/
int main(){
    Account acct;
    acct.balance = DEFAULT_BALANCE;
    acct.currency = PHP;
    acct.accountNumber = 0;
    strcpy(acct.accountName, "");

    double rates[6] = {1.0, 0, 0, 0, 0, 0};

    //main loop
    int running = 1;
    while(running){
        showMenu();
        int choice = getChoice();

        switch(choice){
            case 1:
                registerAccount(&acct);
                break;
            case 2:
                depositAmount(&acct, rates);
                break;
            case 3:
                withdrawAmount(&acct);
                break;
            case 4:
                currencyExchange(rates);
                break;
            case 5:
                recordExchangeRate(rates);
                break;
            case 6:
                showInterestAmount(&acct);
                break;
            case 7:
                showReportsMenu();
                break;
        }
        char ans = askBackToMenu();
        if(ans == 'N' || ans == 'n'){
            running = 0;
        }
    }
    printf("Thank you!\n");
    return 0;
}

/*
* This function clears the input buffer up to the newline character.
* It is used to prevent infinite loops and skipped prompts caused by leftover characters.
* @param (void)
* @return (void)
*/
void clearInput(){
    int c;
    while((c = getchar()) != '\n' && c != EOF);
}

/*
* This function clears the console screen using ANSI escape codes.
* It resets the cursor to the top-left and clears everything.
* @param (void)
* @return (void)
*/
void clearScreen(){
    printf("\033[2J\033[H\n\n");
}

/*
* This function safely reads a double from the console.
* It checks if the input is blank (just pressing Enter) or invalid.
* @param (double*) value is a pointer to the double variable where the input will be stored.
* @return (int) Returns 1 if the read was successful, 0 if invalid input was detected.
*/
int readDouble(double* value){
    char input[100];
    char* endptr;
    
    if(fgets(input, sizeof(input), stdin) == NULL) return 0;
    input[strcspn(input, "\n")] = '\0'; // remove newline

    if(strlen(input) == 0) return 0; // blank input

    *value = strtod(input, &endptr);
    
    if(endptr == input || *endptr != '\0') {
        return 0;
    }
    
    return 1;
}

/*
* This function safely reads an integer from the console.
* It checks if the input is blank (just pressing Enter) or invalid.
* @param (int*) value is a pointer to the integer variable where the input will be stored.
* @return (int) Returns 1 if the read was successful, 0 if invalid input was detected.
*/
int readInt(int* value){
    char input[100];
    char* endptr;
    
    if(fgets(input, sizeof(input), stdin) == NULL) return 0;
    input[strcspn(input, "\n")] = '\0'; // remove newline

    if(strlen(input) == 0) return 0; // blank input

    long val = strtol(input, &endptr, 10);
    
    if(endptr == input || *endptr != '\0') {
        return 0;
    }
    
    *value = (int)val;
    return 1;
}

/*
* This function prints the main menu options to the console.
* @param (void)
* @return (void)
*/
void showMenu(){
    printf("Select Transaction:\n");
    printf("[1] Register Account\n");
    printf("[2] Deposit Amount\n");
    printf("[3] Withdraw Amount\n");
    printf("[4] Currency Exchange\n");
    printf("[5] Record Exchange Rates\n");
    printf("[6] Show Interest Amount\n");
    printf("[7] Generate Reports\n\n");
}

/*
* This function prompts the user for their menu choice.
* It loops until a valid integer (1-7) is entered, then prints the *** echo.
* @param (void)
* @return (int) The valid integer choice entered by the user.
*/
int getChoice(){
    int choice;
    while(1){
        printf("Choice: ");
        if(readInt(&choice)){
            if(choice >= 1 && choice <= 7){
                printf("\n***\n");
                printf("Choice = %d\n\n", choice);
                clearScreen();
                return choice;
            }
            printf("Error: Invalid choice. Please enter a number from 1 to 7.\n\n");
        } else {
            printf("Error: Invalid input. Please enter a number.\n\n");
        }
    }
}

/*
* This function asks the user if they want to return to the main menu.
* Clears the screen immediately after a valid choice is made.
* @param (void)
* @return (char) The validated character entered by the user.
*/
char askBackToMenu(){
    char input[MAX_NAME];
    while(1){
        printf("Back to the Main Menu (Y/N): ");
        fgets(input, MAX_NAME, stdin);
        input[strcspn(input, "\n")] = '\0';

        if(strlen(input) == 0){
            printf("Error: Input cannot be blank. Please enter Y or N.\n\n");
            continue;
        }

        if(strlen(input) == 1 && (input[0] == 'Y' || input[0] == 'y' || input[0] == 'N' || input[0] == 'n')){
            clearScreen();
            return input[0];
        }
        
        printf("Error: Please enter Y or N.\n\n");
    }
}

/*
* This function asks the user if they want to convert another currency.
* @param (void)
* @return (char) The validated character entered by the user.
*/
char askConvertAnother(){
    char input[MAX_NAME];
    while(1){
        printf("Convert another currency (Y/N)? ");
        fgets(input, MAX_NAME, stdin);
        input[strcspn(input, "\n")] = '\0';

        if(strlen(input) == 0){
            printf("Error: Input cannot be blank. Please enter Y or N.\n\n");
            continue;
        }

        if(strlen(input) == 1 && (input[0] == 'Y' || input[0] == 'y' || input[0] == 'N' || input[0] == 'n')){
            return input[0];
        }
        
        printf("Error: Please enter Y or N.\n\n");
    }
}

/*
* if name for registration is valid
*/
int is_valid_name(const char* name){
    int i;
    int len = strlen(name);

    //must have at least one space (first + last)
    int has_space = 0;
    for(i = 0; i < len; i++){
        if(name[i] == ' '){
            has_space = 1;
            break;
        }
    }
    if(!has_space){
        return 0;
    }

    //first and last character must not be a space
    if(name[0] == ' ' || name[len - 1] == ' '){
        return 0;
    }

    //only letters, spaces, periods, hyphens, apostrophes
    for(i = 0; i < len; i++){
        char c = name[i];
        int is_letter = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
        int is_space  = (c == ' ');
        int is_punct  = (c == '.' || c == '-' || c == '\'');
        if(!is_letter && !is_space && !is_punct){
            return 0;   //digit or bad symbol
        }
    }

    return 1;
}

/*
* from register account name but meron na storing sa csv file
*/
void registerAccount(Account* acct){
    char name[MAX_NAME];
    double initial;
    char input[100];

    printf("Register Account\n");

    //account name
    while(1){
        printf("Account Name: ");
        fgets(name, MAX_NAME, stdin);
        name[strcspn(name, "\n")] = '\0';

        int ok = 1;

        if(strlen(name) == 0){
            printf("Error: Account Name cannot be blank.\n\n");
            ok = 0;
        }
        else if(!is_valid_name(name)){
            int has_space = 0;
            int i;
            for(i = 0; name[i] != '\0'; i++){
                if(name[i] == ' '){ has_space = 1; break; }
            }

            if(!has_space){
                printf("Error: Please enter both first name and last name (e.g. Juan Dela Cruz).\n\n");
            }
            else if(name[0] == ' ' || name[strlen(name) - 1] == ' '){
                printf("Error: Name cannot start or end with a space.\n\n");
            }
            else {
                printf("Error: Account Name contains an invalid character.\n\n");
            }
            ok = 0;
        }

        if(ok){
            break;
        }

        //when invalid, ask retry or back
        char ans;
        while(1){
            printf("Retry account registration? (Y/N): ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = '\0';

            if(strlen(input) == 1 && (input[0] == 'Y' || input[0] == 'y')){
                ans = 'Y';
                break;
            }
            if(strlen(input) == 1 && (input[0] == 'N' || input[0] == 'n')){
                ans = 'N';
                break;
            }
            printf("Error: Please enter Y or N.\n\n");
        }

        if(ans == 'N'){
            printf("\nReturning to main menu.\n\n");
            return;
        }
    }

    //find highest existing account number in the csv file
    long highest = 0;
    FILE* fp = fopen(CSV_FILE, "r");
    if(fp != NULL){
        char line[2048];
        int line_number = 0;
        while(fgets(line, sizeof(line), fp) != NULL){
            strip_newline(line);
            line_number++;
            if(line_number == 1) continue;   //skip header
            if(strlen(line) == 0) continue;

            char* fields[8];
            if(split_line(line, fields, 8) != 8) continue;

            long num = strtol(fields[1], NULL, 10);
            if(num > highest){
                highest = num;
            }
        }
        fclose(fp);
    }
    long new_number = highest + 1;

    //initial deposit 
    while(1){
        printf("Initial Deposit: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        if(strlen(input) == 0){
            printf("Error: Initial deposit cannot be blank.\n\n");
            continue;
        }

        char* endptr;
        initial = strtod(input, &endptr);
        if(endptr == input || *endptr != '\0'){
            printf("Error: Invalid input. Please enter a valid number.\n\n");
            continue;
        }

        if(initial < 5000.0){
            printf("Error: Initial deposit must be at least Php 5,000.00.\n\n");
            continue;
        }
        break;
    }

    //current date and time
    time_t now = time(NULL);
    struct tm* t = localtime(&now);

    int dd   = t->tm_mday;
    int mm   = t->tm_mon + 1;
    int yyyy = t->tm_year + 1900;
    int hh   = t->tm_hour;
    int mi   = t->tm_min;
    int ss   = t->tm_sec;

    //append the opening deposit row to the csv file
    FILE* out = fopen(CSV_FILE, "a");
    if(out == NULL){
        printf("Error: Cannot write to %s\n", CSV_FILE);
        return;
    }

    fprintf(out, "%s,%ld,%02d/%02d/%04d,%02d:%02d:%02d,PHP,%.2f,deposit,%.2f\n",name, new_number,dd, mm, yyyy,
            hh, mi, ss, initial, initial);

    fclose(out);

    //update the in memory account
    strcpy(acct->accountName, name);
    acct->accountNumber = new_number;
    acct->balance = initial;
    acct->currency = PHP;

    printf("\nAccount created successfully!\n");
    printf("Account Number: %ld\n", new_number);
    printf("Current Balance: %.2f\n\n", initial);
}

/*
* to find an account in the csv file
*/
int find_account(const char* name, long* out_number, double* out_balance){
    FILE* fp = fopen(CSV_FILE, "r");
    if(fp == NULL){
        printf("Error: Cannot open %s\n", CSV_FILE);
        return 0;
    }

    char line[2048];
    int line_number = 0;
    int found = 0;

    while(fgets(line, sizeof(line), fp) != NULL){
        strip_newline(line);
        line_number++;
        if(line_number == 1) continue;   // skip header
        if(strlen(line) == 0) continue;

        char* fields[8];
        if(split_line(line, fields, 8) != 8) continue;

        if(strcmp(fields[0], name) == 0){
            *out_number = strtol(fields[1], NULL, 10);
            *out_balance = strtod(fields[7], NULL);
            found = 1;   // keep scanning — later rows are newer
        }
    }

    fclose(fp);
    return found;
}

/*
* adds one transaction row to the csv file
*/
int append_transaction(const char* name, long number, const char* type,
                       double amount, double balance){
    FILE* out = fopen(CSV_FILE, "a");
    if(out == NULL){
        printf("Error: Cannot write to %s\n", CSV_FILE);
        return 0;
    }

    time_t now = time(NULL);
    struct tm* t = localtime(&now);

    int dd   = t->tm_mday;
    int mm   = t->tm_mon + 1;
    int yyyy = t->tm_year + 1900;
    int hh   = t->tm_hour;
    int mi   = t->tm_min;
    int ss   = t->tm_sec;

    fprintf(out, "%s,%ld,%02d/%02d/%04d,%02d:%02d:%02d,PHP,%.2f,%s,%.2f\n",
            name, number, dd, mm, yyyy, hh, mi, ss, amount, type, balance);

    fclose(out);
    return 1;
}

/*
* This function handles the deposit process.
* It asks for the account name, looks it up in the CSV, then shows
* that account's number and balance before taking the deposit.
* After updating the balance, it appends the transaction to the CSV.
* @param (Account*) acct is a pointer to the user's Account struct
* @param (double[]) rates is the exchange rates array (kept for compatibility)
* @return (void)
*/
void depositAmount(Account* acct, double rates[]){
    double amount;
    long number;
    char name[MAX_NAME];
    double balance;

    printf("Deposit Amount\n");

    // ask for account name
    while(1){
        printf("Account Name: ");
        fgets(name, MAX_NAME, stdin);
        name[strcspn(name, "\n")] = '\0';

        if(strlen(name) == 0){
            printf("Error: Account Name cannot be blank.\n\n");
            continue;
        }
        break;
    }

    // look up the account in the csv
    if(!find_account(name, &number, &balance)){
        printf("Error: Account name \"%s\" not found.\n\n", name);
        return;
    }

    printf("Account Number: %ld\n", number);
    printf("Current Balance: %.2f\n", balance);
    printf("Currency: PHP\n");

    // ask for amount
    while(1){
        printf("Deposit Amount: ");
        if(!readDouble(&amount)){
            printf("Error: Invalid input. Please enter a valid number.\n\n");
            continue;
        }
        if(amount <= 0){
            printf("Error: Invalid deposit amount. Must be greater than 0.\n\n");
            continue;
        }
        break;
    }

    balance += amount;
    printf("Updated Balance: %.2f\n\n", balance);

    // save the transaction to the csv
    append_transaction(name, number, "deposit", amount, balance);

    if(acct->accountNumber == number){
        acct->balance = balance;
    }
}

/*
* This function handles the withdrawal process.
* It asks for the account name, looks it up in the CSV, then shows
* that account's number and balance before taking the withdrawal.
* After updating the balance, it appends the transaction to the CSV.
* @param (Account*) acct is a pointer to the user's Account struct
* @return (void)
*/
void withdrawAmount(Account* acct){
    double withdraw;
    long number;
    char name[MAX_NAME];
    double balance;

    printf("Withdraw Amount\n");

    // ask for account name
    while(1){
        printf("Account Name: ");
        fgets(name, MAX_NAME, stdin);
        name[strcspn(name, "\n")] = '\0';

        if(strlen(name) == 0){
            printf("Error: Account Name cannot be blank.\n\n");
            continue;
        }
        break;
    }

    // look up the account in the csv
    if(!find_account(name, &number, &balance)){
        printf("Error: Account name \"%s\" not found.\n\n", name);
        return;
    }

    printf("Account Number: %ld\n", number);
    printf("Current Balance: %.2f\n", balance);
    printf("Currency: PHP\n");

    while(1){
        printf("Withdraw Amount: ");
        if(!readDouble(&withdraw)){
            printf("Error: Invalid input. Please enter a valid number.\n\n");
            continue;
        }
        if(withdraw <= 0){
            printf("Error: Invalid withdraw amount. Must be greater than 0.\n\n");
            continue;
        }
        if(withdraw > balance){
            printf("Error: Insufficient funds. You only have %.2f.\n\n", balance);
            continue;
        }
        break;
    }

    balance -= withdraw;
    printf("Updated Balance: %.2f\n\n", balance);

    // save the transaction to the csv
    append_transaction(name, number, "withdrawal", withdraw, balance);

    if(acct->accountNumber == number){
        acct->balance = balance;
    }
}

/*
* This function allows the user to record an exchange rate for a foreign currency.
* @param (double[]) rates is an array of size 6 storing the exchange rates relative to PHP.
* @return (void)
*/
void recordExchangeRate(double rates[]){
    int choice;
    double rate;

    printf("Record Exchange Rate\n\n");
    printf("[1] Philippine Peso (PHP)\n");
    printf("[2] United States Dollar (USD)\n");
    printf("[3] Japanese Yen (JPY)\n");
    printf("[4] British Pound Sterling (GBP)\n");
    printf("[5] Euro (EUR)\n");
    printf("[6] Chinese Yuan Renminni (CNY)\n\n");

    while(1){
        printf("Select Foreign Currency: ");
        if(!readInt(&choice)){
            printf("Error: Invalid input. Please enter a number.\n\n");
            continue;
        }
        if(choice < 1 || choice > 6){
            printf("Error: Invalid currency choice. Must be 1-6.\n\n");
            continue;
        }
        if(choice == 1){
            printf("Error: PHP is the base currency. Cannot set its rate.\n\n");
            continue;
        }
        break;
    }

    while(1){
        printf("Exchange Rate: ");
        if(!readDouble(&rate)){
            printf("Error: Invalid input. Please enter a valid number.\n\n");
            continue;
        }
        if(rate <= 0){
            printf("Error: Invalid exchange rate. Must be greater than 0.\n\n");
            continue;
        }
        break;
    }

    rates[choice - 1] = rate;
    printf("\n");
}

/*
* This function performs currency exchange calculations.
* @param (double[]) rates is an array of size 6 storing the exchange rates relative to PHP.
* @return (void)
*/
void currencyExchange(double rates[]){
    char again = 'Y';

    while(again == 'Y' || again == 'y'){
        int source, target;
        double amount, result;

        printf("Foreign Currency Exchange\n");
        printf("Source Currency Option:\n");
        printf("[1] Philippine Peso (PHP)\n");
        printf("[2] United States Dollar (USD)\n");
        printf("[3] Japanese Yen (JPY)\n");
        printf("[4] British Pound Sterling (GBP)\n");
        printf("[5] Euro (EUR)\n");
        printf("[6] Chinese Yuan Renminni (CNY)\n\n");

        while(1){
            printf("Source Currency: ");
            if(!readInt(&source)){
                printf("Error: Invalid input. Please enter a number.\n\n");
                continue;
            }
            if(source < 1 || source > 6){
                printf("Error: Invalid source currency. Must be 1-6.\n\n");
                continue;
            }
            break;
        }

        while(1){
            printf("Source Amount: ");
            if(!readDouble(&amount)){
                printf("Error: Invalid input. Please enter a valid number.\n\n");
                continue;
            }
            if(amount <= 0){
                printf("Error: Invalid amount. Must be greater than 0.\n\n");
                continue;
            }
            break;
        }
        printf("\n");

        printf("Exchanged Currency Options:\n");
        printf("[1] Philippine Peso (PHP)\n");
        printf("[2] United States Dollar (USD)\n");
        printf("[3] Japanese Yen (JPY)\n");
        printf("[4] British Pound Sterling (GBP)\n");
        printf("[5] Euro (EUR)\n");
        printf("[6] Chinese Yuan Renminni (CNY)\n\n");

        while(1){
            printf("Exchange Currency: ");
            if(!readInt(&target)){
                printf("Error: Invalid input. Please enter a number.\n\n");
                continue;
            }
            if(target < 1 || target > 6){
                printf("Error: Invalid target currency. Must be 1-6.\n\n");
                continue;
            }
            break;
        }

        // to check if rates are set
        if(source != 1 && rates[source - 1] == 0){
            printf("Error: Rate for %s not yet recorded. Please record it first.\n\n", CURRENCY_CODES[source - 1]);
            again = askConvertAnother();
            continue;
        }
        if(target != 1 && rates[target - 1] == 0){
            printf("Error: Rate for %s not yet recorded. Please record it first.\n\n", CURRENCY_CODES[target - 1]);
            again = askConvertAnother();
            continue;
        }

        // conversion of the amount
        double inPhp;
        if(source == 1){
            inPhp = amount;
        } else {
            inPhp = amount * rates[source - 1];
        }

        if(target == 1){
            result = inPhp;
        } else {
            result = inPhp / rates[target - 1];
        }

        printf("Exchange Amount: %.2f\n\n", result);

        again = askConvertAnother();
    }
}

/*
* This function calculates and displays the expected daily interest over N days.
* It displays the current account details, takes the number of days, and prints
* a formatted table showing the day, interest earned, and running balance.
* @param (Account*) acct is a pointer to the user's Account struct
* @return (void)
*/
void showInterestAmount(Account* acct){
    int days;

    // check if account name is set
    if(strlen(acct->accountName) == 0){
        printf("Error: Please register an account name first.\n\n");
        return;
    }

    printf("Show Interest Amount\n");
    printf("Account Name: %s\n", acct->accountName);
    printf("Current Balance: %.2f\n", acct->balance);
    printf("Currency: %s\n", CURRENCY_CODES[acct->currency]);
    printf("Interest Rate: 5%%\n\n");

    while(1){
        printf("Total Number of Days: ");
        if(!readInt(&days)){
            printf("Error: Invalid input. Please enter a number.\n\n");
            continue;
        }
        if(days <= 0){
            printf("Error: Invalid number of days. Must be greater than 0.\n\n");
            continue;
        }
        if(days > MAX_DAYS){
            printf("Error: Number of days too large (max %d).\n\n", MAX_DAYS);
            continue;
        }
        break;
    }

    printf("\n");

    printf("%-4s| %-9s| %-8s|\n", "Day", "Interest", "Balance");

    double current = acct->balance;
    for(int i = 1; i <= days; i++){
        double interest = current * (ANNUAL_RATE / 365.0);
        current += interest;
        printf("%-4d| %-9.2f| %-8.2f|\n", i, interest, current);
    }
    printf("\n");
}

//functions for the csv loader
//loads back_transactions.csv
LoadResult load_transactions(const char* filename){
    LoadResult r;
    r.list = NULL;
    r.total_rows = 0;
    r.php_rows = 0;
    r.invalid_rows = 0;
    r.non_php_rows = 0;
    int capacity = START_SIZE;
    char line[2048];
    int line_number = 0;
    
    FILE* fp = fopen(filename, "r");
    if(fp == NULL){
        printf("Error: Cannot Open %s\n", filename);
        return r;
    }

    r.list = (Transaction*)malloc(sizeof(Transaction) * capacity);
    if(r.list == NULL){
        printf("Error: Out of memory.\n");
        fclose(fp);
        return r;
    }

    while(fgets(line, sizeof(line), fp) != NULL){
        strip_newline(line);
        line_number++;

        //skip the header
        if(line_number == 1){
            continue;
        }
        //skip blanks
        if(strlen(line) == 0){
            continue;
        }

        r.total_rows++;
        char* fields[8];
        int field_count = split_line(line, fields, 8);

        if(field_count != 8){
            r.invalid_rows++;
            continue;
        }

        Transaction t;
        char* endptr;

        //account name
        if(strlen(fields[0]) == 0 || strlen(fields[0]) >= MAX_NAME){
            r.invalid_rows++;
            continue;
        }
        strcpy(t.account_name, fields[0]);

        //account number
        t.account_number = strtol(fields[1], &endptr, 10);
        if(endptr == fields[1] || *endptr != '\0'){
            r.invalid_rows++;
            continue;
        }

        //date
        if(sscanf(fields[2], "%d/%d/%d", &t.day, &t.month, &t.year) != 3){
            r.invalid_rows++;
            continue;
        }
        if(t.day < 1 || t.day > 31 || t.month < 1 || t.month > 12){
            r.invalid_rows++;
            continue;
        }

        //time
        if(sscanf(fields[3], "%d:%d:%d", &t.hour, &t.minute, &t.second) != 3){
            r.invalid_rows++;
            continue;
        }
        if(t.hour < 0 || t.hour > 23){
            r.invalid_rows++;
            continue;
        }

        //currency
        if(strlen(fields[4]) == 0 || strlen(fields[4]) >= MAX_CURR){
            r.invalid_rows++;
            continue;
        }
        strcpy(t.currency, fields[4]);

        //amount
        t.amount = strtod(fields[5], &endptr);
        if(endptr == fields[5] || *endptr != '\0'){
            r.invalid_rows++;
            continue;
        }

        //type
        if(strlen(fields[6]) == 0 || strlen(fields[6]) >= MAX_TYPE){
            r.invalid_rows++;
            continue;
        }
        strcpy(t.type, fields[6]);

        //current balance
        t.current_balance = strtod(fields[7], &endptr);
        if(endptr == fields[7] || *endptr != '\0'){
            r.invalid_rows++;
            continue;
        }

        //keep PHP rows only
        if(!is_php(t.currency)){
            r.non_php_rows++;
            continue;
        }

        // palakihin yung array if full
        if(r.php_rows >= capacity){
            capacity = capacity * 2;
            Transaction* bigger = (Transaction*) realloc(r.list, sizeof(Transaction) * capacity);
            if(bigger == NULL){
                printf("Error: Out of memory.\n");
                free(r.list);
                r.list = NULL;
                fclose(fp);
                return r;
            }
            r.list = bigger;
        }
        r.list[r.php_rows] = t;
        r.php_rows++;
    }

    fclose(fp);
    return r;
}

// js a helper func para ma free up ung loadresult
void free_load_result(LoadResult* r){
    if (r->list != NULL) {
        free(r->list);
        r->list = NULL;
    }
}

//removes trailing \n and \r from a string
void strip_newline(char* s){
    int len = strlen(s);
    while(len > 0 && (s[len-1] == '\n' || s[len-1] == '\r')){
        s[len-1] = '\0';
        len--;
    }
}

//splits a line by commas
int split_line(char* line, char* fields[], int max_fields){
    int count = 0;
    char* p = line;

    fields[count] = p;
    count++;

    while(*p != '\0'){
        if(*p == ','){
            *p = '\0';
            if(count >= max_fields){
                return -1;
            }
            fields[count] = p + 1;
            count++;
        }
        p++;
    }
    return count;
}

//check if currency is php na case-insensitive js to make sure if not polished ung dataset given
int is_php(const char* s){
    if(strlen(s) != 3){
        return 0;
    }

    char a = s[0], b = s[1], c = s[2];

    if(a >= 'a' && a <= 'z'){
        a -= 32;
    }
    if(b >= 'a' && b <= 'z'){
        b -= 32;
    }
    if(c >= 'a' && c <= 'z'){
        c -= 32;
    }

    return (a == 'P' && b == 'H' && c == 'P');

}

//functions for the reports
void showReportsMenu(){
    static LoadResult data;
    static int loaded = 0;

    if(loaded == -1){
        loaded = 0;
    }

    while(1){
        int choice;

        printf("Generate Reports\n");
        printf("[1] Load the file\n");
        printf("[2] Generate Reports\n");
        printf("[3] Back to Main Menu\n\n");

        while(1){
            printf("Enter choice: ");
            if(!readInt(&choice)){
                printf("Error: Invalid input. Please enter a number.\n\n");
                continue;
            }
            break;
        }

        if(choice == 1){
            if(loaded){
                free_load_result(&data);
            }

            printf("\nProcessing dataset...\n");
            data = load_transactions(CSV_FILE);
            loaded = 1;

            if(data.list == NULL || data.php_rows == 0){
                printf("Error: Could not load %s\n\n", CSV_FILE);
                loaded = 0;
                continue;
            }

            printf("%d rows loaded, %d filtered for PHP currency\n\n", data.total_rows, data.php_rows);
        } else if (choice == 2){
            if(!loaded){
                printf("Error: Please load the file first (option 1). \n\n");
                continue;
            }
            generateAllReports(&data, NULL);
            printf("\n");
        } else if(choice == 3){
            if(loaded){
                free_load_result(&data);
                loaded = 0;
            }
            return;
        } else {
            printf("Error: Invalid choice. Please enter 1, 2, or 3.\n\n");
        }
    }
}

void generateAllReports(LoadResult* data, Account* acct){
    printf("Generating reports...\n");
    printf("Outputs saved to individual files...\n\n");

    report1_privelege_tiers(data);
    report2_interest_projection(data);
    report3_suspicious_behavior(data);
    report4_hourly_transaction(data);
    write_summary_json(data);

    (void) acct;
}

int compare_date(int d1, int m1, int y1, int d2, int m2, int y2){
    if (y1 > y2) {
        return 1;
    } else if (y1 < y2){
        return -1;
    }

    if (m1 > m2) {
        return 1;
    } else if (m1 < m2){
        return -1;
    }

    if (d1 > d2) {
        return 1;
    } else if (d1 < d2){
        return -1;
    } else {
    return 0;
    }
}

//to get number of days in a given month
int days_in_month(int month, int year){
    if(month == 2){
        if(year % 4 == 0){
            if(year % 100 == 0){
                if(year % 400 == 0){
                    return 29;
                } else {
                    return 28;
                }
            } else {
                return 29;
            }
        } else {
            return 28;
        }
    }
    if(month == 4 || month == 6 || month == 9 || month == 11){
        return 30;
    }
    return 31;
}

//compare adb descending
int comp_adb_desc(const void* a, const void* b){
    const  PrivilegeAccount* first = (const PrivilegeAccount*) a;
    const  PrivilegeAccount* second = (const PrivilegeAccount*) b;

    if(first->avg_daily_balance < second->avg_daily_balance){
        return 1;
    } else if(first->avg_daily_balance > second->avg_daily_balance){
        return -1;
    } else {
    return 0;
    }
}


void report1_privelege_tiers(LoadResult* data){
    int i, j;
    printf("Report 1: Banking Privelege Tiers\n");

    //to identify last month
    time_t current_time = time(NULL);
    struct tm* current_date = localtime(&current_time);

    int report_month = current_date->tm_mon;
    int report_year = current_date->tm_year + 1900;

    if(report_month == 0){
        report_month = 12;
        report_year = report_year - 1;
    }
    
    int days_in_report = days_in_month(report_month, report_year);

    int latest_day = 0;
    int latest_month = 0;
    int latest_year = 0;

    for(i = 0; i < data->php_rows; i++){
        Transaction* transaction = &data->list[i];
        int cmp = compare_date(transaction->day, transaction->month, transaction->year, 
            latest_day, latest_month, latest_year);

        if (cmp >= 0){
            latest_day = transaction->day;
            latest_month = transaction->month;
            latest_year = transaction->year;
        }
    }
    if(latest_year == 0){
        printf("Error: No data loaded.\n\n");
        return;
    }

    //for acc num distinction
    long acc_numbers[5000];
    int acc_count = 0;

    for(i=0; i < data->php_rows; i++){
        long current_number = data->list[i].account_number;
        int alr_seen = 0;
        for(j = 0; j < acc_count; j++){
            if(acc_numbers[j] == current_number){
                alr_seen = 1;
                break;
            }
        }
        if(alr_seen == 0){
            if(acc_count < 5000){
                acc_numbers[acc_count] = current_number;
                acc_count++;
            }
        }
    }

    PrivilegeAccount results[5000];
    int result_count = 0;

    for(i = 0; i < acc_count; i++){
        long this_acc = acc_numbers[i];

        //name, first transac date, latest balance niya
        char acc_name[MAX_NAME] = "";
        int first_day = 0;
        int first_month = 0;
        int first_year = 0;
        double curr_balance = 0.0;
        int have_first_transaction = 0;

        for(j = 0; j < data->php_rows; j++){
            Transaction* transaction = &data->list[j];
            if(transaction->account_number != this_acc){
                continue;
            }
            
            strcpy(acc_name, transaction->account_name);
            curr_balance = transaction->current_balance;

            if(have_first_transaction == 0){
                first_day = transaction->day;
                first_month = transaction->month;
                first_year = transaction->year;
                have_first_transaction = 1;
            } else {
                int cmp = compare_date(transaction->day, transaction->month, transaction->year, 
            first_day, first_month, first_year);

                if(cmp < 0){
                    first_day = transaction->day;
                    first_month = transaction->month;
                    first_year = transaction->year;
                }
            }
        }

        //age ng account in days
        int acc_age_days = 0;
        long first_transac = (long)first_year * 365;
        first_transac = first_transac + (first_year / 4);
        for(int k=1; k < first_month; k++){
            first_transac = first_transac + days_in_month(k, first_year);
        }
        first_transac = first_transac + first_day;

        long latest_transac = (long)latest_year * 365;
        latest_transac = latest_transac + (latest_year / 4);
        for(int k=1; k < latest_month; k++){
            latest_transac = latest_transac + days_in_month(k, latest_year);
        }
        latest_transac = latest_transac + latest_day;

        acc_age_days = (int)(latest_transac - first_transac);

        //average daily balance(adb)
        double end_day_balance = 0.0;
        double total_eod_balances = 0.0;

        for(int day = 1; day <= days_in_report; day++){
            int found_transaction_today = 0;
            double last_balance_of_the_day = 0.0;

        for(j = 0; j < data->php_rows; j++){
            Transaction* transaction = &data->list[j];
            if(transaction->account_number != this_acc){
                continue;
            }
            if(transaction->day != day){
                continue;
            }
            if(transaction->month != report_month){
                continue;
            }
            if(transaction->year != report_year){
                continue;
            }
            last_balance_of_the_day = transaction->current_balance;
            found_transaction_today = 1;
        }
        
        if(found_transaction_today == 1){
            end_day_balance = last_balance_of_the_day;
        }

        total_eod_balances = total_eod_balances + end_day_balance;
    }

    double avg_daily_balance = total_eod_balances / (double)days_in_report;

        if(avg_daily_balance < 100000.0){
            continue;
        }

        PrivilegeAccount account_row;
        strcpy(account_row.acc_name, acc_name);
        account_row.acc_num = this_acc;
        account_row.acc_age_days = acc_age_days;
        account_row.current_balance = curr_balance;
        account_row.avg_daily_balance = avg_daily_balance;

        if(avg_daily_balance >= 1000000.0){
            strcpy(account_row.privilege_tier, "Platinum");
        } else if(avg_daily_balance >= 500000.0){
            strcpy(account_row.privilege_tier, "Gold");
        } else {
            strcpy(account_row.privilege_tier, "Silver");
        }

        results[result_count] = account_row;
        result_count++;
    }

    qsort(results, result_count, sizeof(PrivilegeAccount), comp_adb_desc);

    FILE* output_file = fopen("report1_banking_privilege_tiers.csv", "w");
    if(output_file == NULL){
        printf("Error: Cannot write report1_banking_privilege_tiers.csv\n\n");
        return;
    }

    fprintf(output_file, "AccountName,AccountAge,CurrentBalance,AverageDailyBalance,PrivilegeTier\n");
    for(i = 0; i < result_count; i++){
        fprintf(output_file, "%s,%d,%.2f,%.2f,%s\n", results[i].acc_name, results[i].acc_age_days, results[i].current_balance,
                results[i].avg_daily_balance, results[i].privilege_tier);
    }
    fclose(output_file);

    printf("AccountName | AccountAge | CurrentBalance | AverageDailyBalance | PrivilegeTier\n");
    for(i = 0; i < result_count; i++){
        if(i >= 2){
            break;
        }
        printf("%s | %d | %.2f | %.2f | %s\n", results[i].acc_name, results[i].acc_age_days, results[i].current_balance,
               results[i].avg_daily_balance, results[i].privilege_tier);
    }
    printf("(%d rows written to report1_banking_privilege_tiers.csv)\n\n", result_count);
}

//sort descending by expected interest
int comp_interest_desc(const void* a, const void* b){
    const InterestRow* first = (const InterestRow*)a;
    const InterestRow* second = (const InterestRow*)b;
    if(first->expected_interest < second->expected_interest){
        return 1;
    } else if(first->expected_interest > second->expected_interest){
        return -1;
    } else {
        return 0;
    }
}

void report2_interest_projection(LoadResult* data){
    int i, j;
    printf("Report 2: Interest Expense Projection\n");

    //distinct acc numbers 
    long acc_numbers[5000];
    int acc_count = 0;

    for(i = 0; i < data->php_rows; i++){
        long current_number = data->list[i].account_number;
        int alr_seen = 0;
        for(j = 0; j < acc_count; j++){
            if(acc_numbers[j] == current_number){
                alr_seen = 1;
                break;
            }
        }
        if(alr_seen == 0){
            if(acc_count < 5000){
                acc_numbers[acc_count] = current_number;
                acc_count++;
            }
        }
    }

    InterestRow results[5000];
    int result_count = 0;

    for(i = 0; i < acc_count; i++){
        long this_acc = acc_numbers[i];

        // latest balance plus yung name
        char acc_name[MAX_NAME] = "";
        double current_balance = 0.0;

        for(j = 0; j < data->php_rows; j++){
            Transaction* transaction = &data->list[j];
            if(transaction->account_number != this_acc){
                continue;
            }
            strcpy(acc_name, transaction->account_name);
            current_balance = transaction->current_balance;   // later rows overwrite
        }

        // filter balance >= 10000
        if(current_balance < 10000.0){
            continue;
        }

        // for the 30 day interest
        double daily_rate = ANNUAL_RATE / 365.0;
        double daily_interest = current_balance * daily_rate;
        double expected_interest = daily_interest * 30.0;

        InterestRow row;
        strcpy(row.name, acc_name);
        row.current_balance = current_balance;
        row.expected_interest = expected_interest;

        results[result_count] = row;
        result_count++;
    }

    qsort(results, result_count, sizeof(InterestRow), comp_interest_desc);

    FILE* output_file = fopen("report2_interest_expense_projection.csv", "w");
    if(output_file == NULL){
        printf("Error: Cannot write report2_interest_expense_projection.csv\n\n");
        return;
    }

    fprintf(output_file, "AccountName,CurrentBalance,ExpectedInterest\n");
    for(i = 0; i < result_count; i++){
        fprintf(output_file, "%s,%.2f,%.2f\n", results[i].name, results[i].current_balance, results[i].expected_interest);
    }
    fclose(output_file);

    // first 2 rows
    printf("AccountName | CurrentBalance | ExpectedInterest\n");
    for(i = 0; i < result_count; i++){
        if(i >= 2){
            break;
        }
        printf("%s | %.2f | %.2f\n", results[i].name, results[i].current_balance, results[i].expected_interest);
    }
    printf("(%d rows written to report2_interest_expense_projection.csv)\n\n", result_count);

}

//sort by date ascending then risk flag descending
int comp_daily(const void* a, const void* b){
    const DailySummary* first  = (const DailySummary*)a;
    const DailySummary* second = (const DailySummary*)b;

    // date ascending 
    int date_cmp = compare_date(first->day, first->month, first->year, second->day, second->month, second->year);
    if(date_cmp != 0){
        return date_cmp;
    }

    // same date: Suspicious first
    int first_suspicious = 0;
    int second_suspicious = 0;
    if(strcmp(first->risk_flag, "Suspicious") == 0){
        first_suspicious = 1;
    }
    if(strcmp(second->risk_flag, "Suspicious") == 0){
        second_suspicious = 1;
    }

    if(first_suspicious > second_suspicious){
        return -1;
    } else if(first_suspicious < second_suspicious){
        return 1;
    } else {
        return 0;
    }
}

void report3_suspicious_behavior(LoadResult* data){
    int i, j;
    printf("Report 3: Suspicious Client Behavior\n");

    DailySummary results[20000];
    int result_count = 0;

    // group transactions by (account, date)
    for(i = 0; i < data->php_rows; i++){
        Transaction* transaction = &data->list[i];

        // find matching (account, date) entry in results
        int found_index = -1;
        for(j = 0; j < result_count; j++){
            if(results[j].acc_num == transaction->account_number){
                if(results[j].day == transaction->day){
                    if(results[j].month == transaction->month){
                        if(results[j].year == transaction->year){
                            found_index = j;
                            break;
                        }
                    }
                }
            }
        }

        // create a new entry if not found
        if(found_index == -1){
            if(result_count < 20000){
                strcpy(results[result_count].name, transaction->account_name);
                results[result_count].acc_num = transaction->account_number;
                results[result_count].day = transaction->day;
                results[result_count].month = transaction->month;
                results[result_count].year = transaction->year;
                results[result_count].total_withdrawals = 0.0;
                results[result_count].num_deposits = 0;
                results[result_count].total_deposits = 0.0;
                results[result_count].turnover_velocity = 0.0;
                strcpy(results[result_count].risk_flag, "Normal");
                found_index = result_count;
                result_count++;
            } else {
                continue;   // if table full
            }
        }

        // add to the group
        if(strcmp(transaction->type, "withdrawal") == 0){
            results[found_index].total_withdrawals += transaction->amount;
        } else if(strcmp(transaction->type, "deposit") == 0){
            results[found_index].num_deposits += 1;
            results[found_index].total_deposits += transaction->amount;
        }
    }

    // compute turnover velocity and risk flag for each group
    for(i = 0; i < result_count; i++){
        if(results[i].total_deposits > 0.0){
            results[i].turnover_velocity =
                (results[i].total_withdrawals / results[i].total_deposits) * 100.0;
        } else {
            results[i].turnover_velocity = 0.0;   
        }

        int is_suspicious = 0;
        if(results[i].total_withdrawals >= 500000.0){
            is_suspicious = 1;
        }
        if(results[i].turnover_velocity >= 90.0){
            is_suspicious = 1;
        }

        if(is_suspicious == 1){
            strcpy(results[i].risk_flag, "Suspicious");
        } else {
            strcpy(results[i].risk_flag, "Normal");
        }
    }

    qsort(results, result_count, sizeof(DailySummary), comp_daily);

    FILE* output_file = fopen("report3_suspicious_client_behavior.csv", "w");
    if(output_file == NULL){
        printf("Error: Cannot write report3_suspicious_client_behavior.csv\n\n");
        return;
    }

    fprintf(output_file, "AccountName,Date,TotalWithdrawals,NumDeposits,TotalDeposits,TurnoverVelocity,RiskFlag\n");
    for(i = 0; i < result_count; i++){
        fprintf(output_file, "%s,%02d/%02d/%04d,%.2f,%d,%.2f,%.2f,%s\n",
                results[i].name,
                results[i].day,
                results[i].month,
                results[i].year,
                results[i].total_withdrawals,
                results[i].num_deposits,
                results[i].total_deposits,
                results[i].turnover_velocity,
                results[i].risk_flag);
    }
    fclose(output_file);

    // first 2 rows
    printf("AccountName | Date | TotalWithdrawals | NumDeposits | TotalDeposits | TurnoverVelocity | RiskFlag\n");
    for(i = 0; i < result_count; i++){
        if(i >= 2){
            break;
        }
        printf("%s | %02d/%02d/%04d | %.2f | %d | %.2f | %.2f | %s\n",
               results[i].name,
               results[i].day,
               results[i].month,
               results[i].year,
               results[i].total_withdrawals,
               results[i].num_deposits,
               results[i].total_deposits,
               results[i].turnover_velocity,
               results[i].risk_flag);
    }
    printf("(%d rows written to report3_suspicious_client_behavior.csv)\n\n", result_count);
}

void report4_hourly_transaction(LoadResult* data){
    int i;
    printf("Report 4: Hourly Transaction Report\n");

    // count, total amount, average (24 kasi 24 hours in a day)
    int    counts[24];
    double totals[24];
    double averages[24];

    for(i = 0; i < 24; i++){
        counts[i] = 0;
        totals[i] = 0.0;
        averages[i] = 0.0;
    }

    // group every transaction by hour
    for(i = 0; i < data->php_rows; i++){
        Transaction* transaction = &data->list[i];
        int hour = transaction->hour;

        if(hour < 0 || hour > 23){
            continue;   // skip bad hour
        }

        counts[hour] = counts[hour] + 1;
        totals[hour] = totals[hour] + transaction->amount;
    }

    // compute averages
    for(i = 0; i < 24; i++){
        if(counts[i] > 0){
            averages[i] = totals[i] / (double)counts[i];
        } else {
            averages[i] = 0.0;
        }
    }

    // write CSV
    FILE* output_file = fopen("report4_hourly_transaction_report.csv", "w");
    if(output_file == NULL){
        printf("Error: Cannot write report4_hourly_transaction_report.csv\n\n");
        return;
    }

    fprintf(output_file, "Hour,TransactionCount,TotalAmount,AverageAmount\n");
    for(i = 0; i < 24; i++){
        fprintf(output_file, "%02d:00,%d,%.2f,%.2f\n",
                i, counts[i], totals[i], averages[i]);
    }
    fclose(output_file);

    // first 2 rows
    printf("Hour | TransactionCount | TotalAmount | AverageAmount\n");
    for(i = 0; i < 2; i++){
        printf("%02d:00 | %d | %.2f | %.2f\n",
               i, counts[i], totals[i], averages[i]);
    }
    printf("(24 rows written to report4_hourly_transaction_report.csv)\n\n");
}

void write_summary_json(LoadResult* data){
    int i, j;
    printf("Summary State (summary.json):\n");

    // distinct accounts
    long acc_numbers[5000];
    int acc_count = 0;

    for(i = 0; i < data->php_rows; i++){
        long current_number = data->list[i].account_number;
        int alr_seen = 0;
        for(j = 0; j < acc_count; j++){
            if(acc_numbers[j] == current_number){
                alr_seen = 1;
                break;
            }
        }
        if(alr_seen == 0){
            if(acc_count < 5000){
                acc_numbers[acc_count] = current_number;
                acc_count++;
            }
        }
    }

    // totals
    double total_deposits = 0.0;
    double total_withdrawals = 0.0;

    for(i = 0; i < data->php_rows; i++){
        Transaction* transaction = &data->list[i];
        if(strcmp(transaction->type, "deposit") == 0){
            total_deposits = total_deposits + transaction->amount;
        } else if(strcmp(transaction->type, "withdrawal") == 0){
            total_withdrawals = total_withdrawals + transaction->amount;
        }
    }

    // total balance (latest per account) + total expected interest
    double total_balance = 0.0;
    double total_interest_expense = 0.0;

    for(i = 0; i < acc_count; i++){
        long this_acc = acc_numbers[i];
        double latest_balance = 0.0;

        // find latest balance for this account
        for(j = 0; j < data->php_rows; j++){
            Transaction* transaction = &data->list[j];
            if(transaction->account_number != this_acc){
                continue;
            }
            latest_balance = transaction->current_balance;
        }

        total_balance = total_balance + latest_balance;

        // only count interest for accounts >= 10,000 
        if(latest_balance >= 10000.0){
            double daily_rate = ANNUAL_RATE / 365.0;
            double daily_interest = latest_balance * daily_rate;
            double expected_interest = daily_interest * 30.0;
            total_interest_expense = total_interest_expense + expected_interest;
        }
    }

    // write JSON file
    FILE* output_file = fopen("summary.json", "w");
    if(output_file == NULL){
        printf("Error: Cannot write summary.json\n\n");
        return;
    }

    fprintf(output_file,
        "{\"total_accounts\": %d, \"total_deposits\": %.2f, \"total_withdrawals\": %.2f, \"total_balance\": %.2f, \"total_interest_expense\": %.2f}\n",
        acc_count, total_deposits, total_withdrawals, total_balance, total_interest_expense);

    fclose(output_file);

    // preview
    printf("{\"total_accounts\": %d, \"total_deposits\": %.2f, \"total_withdrawals\": %.2f, \"total_balance\": %.2f, \"total_interest_expense\": %.2f}\n\n",
        acc_count, total_deposits, total_withdrawals, total_balance, total_interest_expense);
}
