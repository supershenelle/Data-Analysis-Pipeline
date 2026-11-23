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

//Function Prototypes from MC01
void showMenu();
int  getChoice();
void clearInput();
void clearScreen();
int  readDouble(double* value);
int  readInt(int* value);
int  is_valid_name(const char* name);
void registerAccount(Account* acct);
int  find_account(long number, char* out_name, double* out_balance);
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
void report1_privelege_tiers(LoadResult* data);
void report2_interest_projection(LoadResult* data);
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
int find_account(long number, char* out_name, double* out_balance){
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

        long num = strtol(fields[1], NULL, 10);
        if(num == number){
            strcpy(out_name, fields[0]);
            *out_balance = strtod(fields[7], NULL);
            found = 1;   // js keep scanning bcs newer ung lower rows
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
* It first asks for the account number, looks it up in the CSV, then
* shows that account's name and balance before taking the deposit.
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

    // ask for account number
    while(1){
        printf("Account Number: ");
        if(!readInt((int*)&number)){
            printf("Error: Invalid input. Please enter a valid account number.\n\n");
            continue;
        }
        break;
    }

    // look up the account in the csv
    if(!find_account(number, name, &balance)){
        printf("Error: Account number %ld not found.\n\n", number);
        return;
    }

    printf("Account Name: %s\n", name);
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
* It first asks for the account number, looks it up in the CSV, then
* shows that account's name and balance before taking the withdrawal.
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

    // ask for account number
    while(1){
        printf("Account Number: ");
        if(!readInt((int*)&number)){
            printf("Error: Invalid input. Please enter a valid account number.\n\n");
            continue;
        }
        break;
    }

    // look up the account in the csv
    if(!find_account(number, name, &balance)){
        printf("Error: Account number %ld not found.\n\n", number);
        return;
    }

    printf("Account Name: %s\n", name);
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

}

void report1_privelege_tiers(LoadResult* data){

}

void report2_interest_projection(LoadResult* data){

}

void report3_suspicious_behavior(LoadResult* data){

}

void report4_hourly_transaction(LoadResult* data){

}

void write_summary_json(LoadResult* data){

}
