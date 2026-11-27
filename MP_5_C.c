/***************************
Last names: Danieles, De Guzman, Nono, Suazon
Language: C
Paradigm(s): Procedural, Imperative, Structured
***************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_NAME 100
#define DEFAULT_BALANCE 0.00
#define ANNUAL_RATE 0.05
#define MAX_DAYS 3650

#define MAX_CURR 8
#define MAX_TYPE 16
#define MAX_TIME 16
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
* Holds the account name, account number, full date and time, currency,
* amount, transaction type, and the resulting balance after the transaction.
*/
typedef struct{
    char account_name[MAX_NAME];
    long account_number;
    int day, month, year;
    int hour, minute, second;
    char time_str[MAX_TIME];
    char currency[MAX_CURR];
    double amount;
    char type[MAX_TYPE];
    double current_balance;
} Transaction;

/*
* Structure representing the result of load_transactions().
* Holds the dynamic list of PHP transactions plus counters for
* total rows read, valid PHP rows, invalid rows, and skipped non-PHP rows.
*/
typedef struct{
    Transaction* list;
    int total_rows;
    int php_rows;
    int invalid_rows;
    int non_php_rows;
} LoadResult;

/*
* Structure representing one row of Report 1 (Banking Privilege Tiers).
* Holds the account name, account number, age in days, current balance,
* average daily balance, and the assigned privilege tier.
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
* Structure representing one row of Report 2 (Interest Expense Projection).
* Holds the account name, current balance, and the projected 30-day interest.
*/
typedef struct{
    char name[MAX_NAME];
    double current_balance;
    double expected_interest;
} InterestRow;

/*
* Structure representing one row of Report 3 (Suspicious Client Behavior).
* Holds the account name, account number, transaction date, aggregated
* withdrawals and deposits, turnover velocity, risk flag, and the original
* insertion index used to emulate stable sort on ties.
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
    int orig_index;        // stable sort emulation for tiebreak
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

//Function Prototypes for formatting and reports
double round2(double val);
void format_money(char* out, int out_size, double val);
long date_to_days(int day, int month, int year);
void showReportsMenu();
void generateAllReports(LoadResult* data, Account* acct);
int compare_date(int d1, int m1, int y1, int d2, int m2, int y2);
int compare_datetime(int d1, int m1, int y1, const char* t1, int d2, int m2, int y2, const char* t2);
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
* If run with the "--reports" argument, it skips the menu and generates all reports directly.
* @param (int) argc is the argument count.
* @param (char**) argv is the argument vector.
* @return (int) Returns 0 upon successful termination.
*/
int main(int argc, char* argv[]){
    Account acct;
    acct.balance = DEFAULT_BALANCE;
    acct.currency = PHP;
    acct.accountNumber = 0;
    strcpy(acct.accountName, "");

    double rates[6] = {1.0, 0, 0, 0, 0, 0};

    // single command mode for report generation
    if(argc > 1 && strcmp(argv[1], "--reports") == 0){
        LoadResult data = load_transactions(CSV_FILE);
        if(data.list != NULL){
            generateAllReports(&data, &acct);
            free_load_result(&data);
        }
        return 0;
    }

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
            case 8:
                running = 0;
                break;
        }

        // skip the back to menu prompt when the user chose to exit
        if(running == 0){
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
    printf("[7] Generate Reports\n");
    printf("[8] Exit\n\n");
}

/*
* This function prompts the user for their menu choice.
* It loops until a valid integer (1-8) is entered, then prints the *** echo.
* @param (void)
* @return (int) The valid integer choice entered by the user.
*/
int getChoice(){
    int choice;
    while(1){
        printf("Choice: ");
        if(readInt(&choice)){
            if(choice >= 1 && choice <= 8){
                printf("\n***\n");
                printf("Choice = %d\n\n", choice);
                clearScreen();
                return choice;
            }
            printf("Error: Invalid choice. Please enter a number from 1 to 8.\n\n");
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
* It loops until a valid Y/N character is entered.
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
* This function checks whether the given name is valid for registration.
* A valid name must have at least one space (first + last), must not start
* or end with a space, and may only contain letters, spaces, periods, hyphens, and apostrophes.
* @param (const char*) name is the account name to validate.
* @return (int) Returns 1 if the name is valid, 0 otherwise.
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
* This function handles the full account registration process.
* It validates the account name, finds the highest existing account number
* in the CSV file to assign the next one, validates the initial deposit,
* and appends the opening deposit row to the CSV file.
* @param (Account*) acct is a pointer to the user's Account struct.
* @return (void)
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
* This function searches the CSV file for the latest transaction row
* belonging to the given account name and returns the account number
* and current balance from that row.
* @param (const char*) name is the account name to look up.
* @param (long*) out_number is where the account number will be stored.
* @param (double*) out_balance is where the latest balance will be stored.
* @return (int) Returns 1 if the account was found, 0 otherwise.
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
            found = 1;   // keep scanning bcs later rows are newer
        }
    }

    fclose(fp);
    return found;
}

/*
* This function appends one transaction row to the CSV file using the
* current local date and time.
* @param (const char*) name is the account name.
* @param (long) number is the account number.
* @param (const char*) type is the transaction type ("deposit" or "withdrawal").
* @param (double) amount is the transaction amount.
* @param (double) balance is the resulting balance after the transaction.
* @return (int) Returns 1 if the write succeeded, 0 otherwise.
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
* @param (Account*) acct is a pointer to the user's Account struct.
* @param (double[]) rates is the exchange rates array (kept for compatibility).
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
* @param (Account*) acct is a pointer to the user's Account struct.
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

    // validate the withdrawal amount against the current balance
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
* It displays the list of currencies, validates the selection (PHP cannot be set),
* and stores the rate in the rates array.
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

    // validate the currency selection
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

    // validate the exchange rate
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
* It asks for the source and target currencies plus the amount, then converts
* through PHP as the base currency. It loops if the user wants to convert again.
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

        // validate the source currency
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

        // validate the source amount
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

        // validate the target currency
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

        // conversion of the amount through PHP as the base currency
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
* @param (Account*) acct is a pointer to the user's Account struct.
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

    // validate the number of days
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

    // print the interest table with running balance
    double current = acct->balance;
    for(int i = 1; i <= days; i++){
        double interest = current * (ANNUAL_RATE / 365.0);
        current += interest;
        printf("%-4d| %-9.2f| %-8.2f|\n", i, interest, current);
    }
    printf("\n");
}

/*
* This function loads and validates transactions from the given CSV file.
* It skips the header and blank lines, rejects malformed rows into the
* invalid_rows counter, skips non-PHP rows into the non_php_rows counter,
* and stores only valid PHP rows in a dynamically growing array.
* @param (const char*) filename is the path to the CSV file.
* @return (LoadResult) The populated load result with the transaction list and counters.
*/
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
        // store the raw time string
        strncpy(t.time_str, fields[3], sizeof(t.time_str) - 1);
        t.time_str[sizeof(t.time_str) - 1] = '\0';

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

/*
* This function releases the dynamically allocated list inside a LoadResult
* and resets the pointer to NULL to avoid double frees.
* @param (LoadResult*) r is the load result to free.
* @return (void)
*/
void free_load_result(LoadResult* r){
    if (r->list != NULL) {
        free(r->list);
        r->list = NULL;
    }
}

/*
* This function removes trailing \n and \r characters from a string in place.
* @param (char*) s is the string to trim.
* @return (void)
*/
void strip_newline(char* s){
    int len = strlen(s);
    while(len > 0 && (s[len-1] == '\n' || s[len-1] == '\r')){
        s[len-1] = '\0';
        len--;
    }
}

/*
* This function splits a line by commas into an array of field pointers.
* It modifies the line in place by replacing commas with null terminators.
* @param (char*) line is the line to split (modified in place).
* @param (char*[]) fields is the output array of field pointers.
* @param (int) max_fields is the maximum number of fields allowed.
* @return (int) The number of fields found, or -1 if max_fields was exceeded.
*/
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

/*
* This function checks whether a currency code is PHP (case-insensitive).
* It is used to filter the dataset so only PHP transactions are kept,
* even if the source CSV has inconsistent casing.
* @param (const char*) s is the 3-letter currency code to check.
* @return (int) Returns 1 if the code is PHP, 0 otherwise.
*/
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

/*
* This function rounds a double to 2 decimal places.
* @param (double) val is the value to round.
* @return (double) The rounded value.
*/
double round2(double val){
    return round(val * 100.0) / 100.0;
}

/*
* This function formats a monetary value into a string with comma
* thousands separators and exactly 2 decimal places.
* @param (char*) out is the output buffer.
* @param (int) out_size is the size of the output buffer.
* @param (double) val is the value to format.
* @return (void)
*/
void format_money(char* out, int out_size, double val){
    double r = round2(val);
    if(r > -0.005 && r < 0.005){
        r = 0.0;
    }

    int neg = 0;
    if(r < 0.0){
        neg = 1;
        r = -r;
    }

    long whole = (long)r;
    int cents = (int)round((r - (double)whole) * 100.0);
    if(cents == 100){
        whole = whole + 1;
        cents = 0;
    }

    char digits[64];
    sprintf(digits, "%ld", whole);
    int len = strlen(digits);

    char with_commas[96];
    int idx = 0;
    int i;
    for(i = 0; i < len; i++){
        if(i > 0 && (len - i) % 3 == 0){
            with_commas[idx] = ',';
            idx++;
        }
        with_commas[idx] = digits[i];
        idx++;
    }
    with_commas[idx] = '\0';

    if(neg == 1){
        snprintf(out, out_size, "-%s.%02d", with_commas, cents);
    } else {
        snprintf(out, out_size, "%s.%02d", with_commas, cents);
    }
}

/*
* This function converts a calendar date into a day count, which is
* useful for computing account age and for comparing dates efficiently.
* @param (int) day is the day of the month.
* @param (int) month is the month (1-12).
* @param (int) year is the four-digit year.
* @return (long) The day count for the given date.
*/
long date_to_days(int day, int month, int year){
    long y = (long)year;
    if(month <= 2){
        y -= 1;
    }
    long leap_days = (y / 4) - (y / 100) + (y / 400);
    long total = (long)year * 365 + leap_days + day;

    int m;
    for(m = 1; m < month; m++){
        total += days_in_month(m, year);
    }
    return total;
}

/*
* This function displays the reports submenu.
* It offers loading the dataset, generating all reports, or going back.
* The loaded dataset is kept in a static variable so it can be reused
* between menu iterations without reloading.
* @param (void)
* @return (void)
*/
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
            if(data.list != NULL){
                loaded = 1;
                printf("Loaded %d PHP rows.\n\n", data.php_rows);
            }
        }
        else if(choice == 2){
            if(!loaded || data.list == NULL){
                printf("Error: Please load the file first.\n\n");
                continue;
            }
            Account dummy;
            dummy.balance = DEFAULT_BALANCE;
            dummy.currency = PHP;
            dummy.accountNumber = 0;
            strcpy(dummy.accountName, "");
            generateAllReports(&data, &dummy);
        }
        else if(choice == 3){
            break;
        }
        else {
            printf("Error: Invalid choice. Please enter 1 to 3.\n\n");
        }
    }
}

/*
* This function generates all four reports and the summary JSON in order.
* The account parameter is currently unused and is kept for API compatibility.
* @param (LoadResult*) data is the loaded dataset.
* @param (Account*) acct is the user's account (unused).
* @return (void)
*/
void generateAllReports(LoadResult* data, Account* acct){
    (void)acct;
    report1_privelege_tiers(data);
    report2_interest_projection(data);
    report3_suspicious_behavior(data);
    report4_hourly_transaction(data);
    write_summary_json(data);
}

/*
* This function compares two calendar dates.
* @param (int) d1, (int) m1, (int) y1 are the first date (day, month, year).
* @param (int) d2, (int) m2, (int) y2 are the second date (day, month, year).
* @return (int) Returns 1 if the first date is later, -1 if earlier, 0 if equal.
*/
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

/*
* This function compares two transactions by date first, then by raw time string using strcmp.
* @param (int) d1, m1, y1 are the first date.
* @param (const char*) t1 is the first raw time string.
* @param (int) d2, m2, y2 are the second date.
* @param (const char*) t2 is the second raw time string.
* @return (int) Returns 1 if first > second, -1 if first < second, 0 if equal.
*/
int compare_datetime(int d1, int m1, int y1, const char* t1,
                     int d2, int m2, int y2, const char* t2){
    int date_cmp = compare_date(d1, m1, y1, d2, m2, y2);
    if(date_cmp != 0){
        return date_cmp;
    }
    return strcmp(t1, t2);
}

/*
* This function returns the number of days in a given month.
* It accounts for leap years using the standard Gregorian rules.
* @param (int) month is the month (1-12).
* @param (int) year is the four-digit year.
* @return (int) The number of days in the given month.
*/
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

/*
* This function is the qsort comparator for Report 1.
* It sorts accounts by average daily balance in descending order with no tiebreak.
* @param (const void*) a is the first PrivilegeAccount.
* @param (const void*) b is the second PrivilegeAccount.
* @return (int) Negative if a should come first, positive if b should come first.
*/
int comp_adb_desc(const void* a, const void* b){
    const PrivilegeAccount* first = (const PrivilegeAccount*) a;
    const PrivilegeAccount* second = (const PrivilegeAccount*) b;

    if(first->avg_daily_balance < second->avg_daily_balance){
        return 1;
    } else if(first->avg_daily_balance > second->avg_daily_balance){
        return -1;
    }
    return 0;
}

/*
* Report 1: Banking Privilege Tiers.
* @param (LoadResult*) data is the loaded dataset.
* @return (void)
*/
void report1_privelege_tiers(LoadResult* data){
    int i, j;
    printf("Report 1: Banking Privelege Tiers\n");

    if(data == NULL || data->php_rows <= 0){
        printf("Error: No data loaded.\n\n");
        return;
    }

    time_t current_time = time(NULL);
    struct tm* current_date = localtime(&current_time);

    int today_day = current_date->tm_mday;
    int today_month = current_date->tm_mon + 1;
    int today_year = current_date->tm_year + 1900;
    long today_days = date_to_days(today_day, today_month, today_year);

    //to identify last month
    int report_month = current_date->tm_mon;
    int report_year = today_year;
    if(report_month == 0){
        report_month = 12;
        report_year = report_year - 1;
    }

    int days_in_report = days_in_month(report_month, report_year);

    //for acc num distinction
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

    PrivilegeAccount results[5000];
    int result_count = 0;

    for(i = 0; i < acc_count; i++){
        long this_acc = acc_numbers[i];

        //name, first transac date
        char acc_name[MAX_NAME] = "";
        int have_name = 0;
        int first_day = 0;
        int first_month = 0;
        int first_year = 0;
        int have_first_transaction = 0;

        // collect indices of this account's rows for sorting by (date, time_str)
        int acc_indices[10000];
        int acc_idx_count = 0;

        for(j = 0; j < data->php_rows; j++){
            Transaction* transaction = &data->list[j];
            if(transaction->account_number != this_acc){
                continue;
            }

            if(acc_idx_count < 10000){
                acc_indices[acc_idx_count] = j;
                acc_idx_count++;
            }

            if(!have_name){
                strcpy(acc_name, transaction->account_name);
                have_name = 1;
            }

            if(have_first_transaction == 0){
                first_day = transaction->day;
                first_month = transaction->month;
                first_year = transaction->year;
                have_first_transaction = 1;
            } else {
                int cmp = compare_date(transaction->day, transaction->month, transaction->year, first_day, first_month, first_year);

                if(cmp < 0){
                    first_day = transaction->day;
                    first_month = transaction->month;
                    first_year = transaction->year;
                }
            }
        }

        if(!have_first_transaction){
            continue;
        }

        //age ng account in days
        long first_days = date_to_days(first_day, first_month, first_year);
        int acc_age_days = (int)(today_days - first_days);

        // sort this account's transaction indices by (date, time_str) ascending
        for(int a = 0; a < acc_idx_count - 1; a++){
            for(int b = 0; b < acc_idx_count - 1 - a; b++){
                Transaction* t1 = &data->list[acc_indices[b]];
                Transaction* t2 = &data->list[acc_indices[b + 1]];
                int cmp = compare_datetime(t1->day, t1->month, t1->year, t1->time_str, t2->day, t2->month, t2->year, t2->time_str);
                if(cmp > 0){
                    int tmp = acc_indices[b];
                    acc_indices[b] = acc_indices[b + 1];
                    acc_indices[b + 1] = tmp;
                }
            }
        }

        // current balance = balance of latest transaction by (date, time_str)
        double final_current_balance = 0.0;
        if(acc_idx_count > 0){
            final_current_balance = data->list[acc_indices[acc_idx_count - 1]].current_balance;
        }

        //for each day, EOD = latest transaction's balance with date <= that day
        double total_eod_balances = 0.0;

        int day;
        for(day = 1; day <= days_in_report; day++){
            double end_of_day_balance = 0.0;
            for(int k = 0; k < acc_idx_count; k++){
                Transaction* t = &data->list[acc_indices[k]];
                if(compare_date(t->day, t->month, t->year,
                                day, report_month, report_year) <= 0){
                    end_of_day_balance = t->current_balance;   // last one wins = latest by datetime
                } else {
                    break;   // sorted ascending, so we can stop early
                }
            }
            total_eod_balances = total_eod_balances + end_of_day_balance;
        }

        double avg_daily_balance = total_eod_balances / (double)days_in_report;

        if(avg_daily_balance < 100000.0){
            continue;
        }

        PrivilegeAccount account_row;
        strcpy(account_row.acc_name, acc_name);
        account_row.acc_num = this_acc;
        account_row.acc_age_days = acc_age_days;
        account_row.current_balance = final_current_balance;
        account_row.avg_daily_balance = avg_daily_balance;

        if(avg_daily_balance >= 1000000.0){
            strcpy(account_row.privilege_tier, "Platinum");
        } else if(avg_daily_balance >= 500000.0){
            strcpy(account_row.privilege_tier, "Gold");
        } else {
            strcpy(account_row.privilege_tier, "Silver");
        }

        if(result_count < 5000){
            results[result_count] = account_row;
            result_count++;
        }
    }

    qsort(results, result_count, sizeof(PrivilegeAccount), comp_adb_desc);

    FILE* output_file = fopen("report1_banking_privilege_tiers.csv", "w");
    if(output_file == NULL){
        printf("Error: Cannot write report1_banking_privilege_tiers.csv\n\n");
        return;
    }

    fprintf(output_file, "AccountName,AccountAge,CurrentBalance,AverageDailyBalance,PrivilegeTier\n");
    char cur_bal_str[64], adb_str[64];
    for(i = 0; i < result_count; i++){
        format_money(cur_bal_str, sizeof(cur_bal_str), results[i].current_balance);
        format_money(adb_str, sizeof(adb_str), results[i].avg_daily_balance);
        fprintf(output_file, "%s,%d,\"%s\",\"%s\",%s\n", results[i].acc_name, results[i].acc_age_days,
                cur_bal_str, adb_str, results[i].privilege_tier);
    }
    fclose(output_file);

    printf("%-20s | %-10s | %-18s | %-20s | %s\n",
           "AccountName", "AccountAge", "CurrentBalance", "AverageDailyBalance", "PrivilegeTier");
    for(i = 0; i < result_count; i++){
        if(i >= 2){
            break;
        }
        char cur_bal_str2[64], adb_str2[64];
        format_money(cur_bal_str2, sizeof(cur_bal_str2), results[i].current_balance);
        format_money(adb_str2, sizeof(adb_str2), results[i].avg_daily_balance);
        printf("%-20s | %-10d | %-18s | %-20s | %s\n", results[i].acc_name, results[i].acc_age_days,
               cur_bal_str2, adb_str2, results[i].privilege_tier);
    }
    printf("(%d rows written to report1_banking_privilege_tiers.csv)\n\n", result_count);
}

/*
* This function is the qsort comparator for Report 2.
* It sorts rows by expected interest in descending order, then by
* account name ascending as a tiebreak.
* @param (const void*) a is the first InterestRow.
* @param (const void*) b is the second InterestRow.
* @return (int) Negative if a should come first, positive if b should come first.
*/
int comp_interest_desc(const void* a, const void* b){
    const InterestRow* first = (const InterestRow*)a;
    const InterestRow* second = (const InterestRow*)b;
    if(first->expected_interest < second->expected_interest){
        return 1;
    } else if(first->expected_interest > second->expected_interest){
        return -1;
    } else {
        return strcmp(first->name, second->name);
    }
}

/*
* Report 2: Interest Expense Projection.
* @param (LoadResult*) data is the loaded dataset.
* @return (void)
*/
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
        double expected_interest = round2(daily_interest * 30.0);

        InterestRow row;
        strcpy(row.name, acc_name);
        row.current_balance = current_balance;
        row.expected_interest = expected_interest;

        if(result_count < 5000){
            results[result_count] = row;
            result_count++;
        }
    }

    qsort(results, result_count, sizeof(InterestRow), comp_interest_desc);

    FILE* output_file = fopen("report2_interest_expense_projection.csv", "w");
    if(output_file == NULL){
        printf("Error: Cannot write report2_interest_expense_projection.csv\n\n");
        return;
    }

    fprintf(output_file, "AccountName,CurrentBalance,ExpectedInterest\n");
    char cur_bal_str[64], exp_int_str[64];
    for(i = 0; i < result_count; i++){
        format_money(cur_bal_str, sizeof(cur_bal_str), results[i].current_balance);
        format_money(exp_int_str, sizeof(exp_int_str), results[i].expected_interest);
        fprintf(output_file, "%s,\"%s\",\"%s\"\n", results[i].name, cur_bal_str, exp_int_str);
    }
    fclose(output_file);

    // first 2 rows
    printf("%-20s | %-18s | %s\n", "AccountName", "CurrentBalance", "ExpectedInterest");
    for(i = 0; i < result_count; i++){
        if(i >= 2){
            break;
        }
        char cur_bal_str2[64], exp_int_str2[64];
        format_money(cur_bal_str2, sizeof(cur_bal_str2), results[i].current_balance);
        format_money(exp_int_str2, sizeof(exp_int_str2), results[i].expected_interest);
        printf("%-20s | %-18s | %s\n", results[i].name, cur_bal_str2, exp_int_str2);
    }
    printf("(%d rows written to report2_interest_expense_projection.csv)\n\n", result_count);

}

/*
* This function is the qsort comparator for Report 3.
* It sorts rows by date ascending, then by risk flag (Suspicious first),
* then by original insertion index to emulate stable sort.
* @param (const void*) a is the first DailySummary.
* @param (const void*) b is the second DailySummary.
* @return (int) Negative if a should come first, positive if b should come first.
*/
int comp_daily(const void* a, const void* b){
    const DailySummary* first  = (const DailySummary*)a;
    const DailySummary* second = (const DailySummary*)b;

    // date ascending 
    int date_cmp = compare_date(first->day, first->month, first->year, second->day, second->month, second->year);
    if(date_cmp != 0){
        return date_cmp;
    }

    // same date then Suspicious first
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
    }

    // stable tiebreak then original insertion order
    if(first->orig_index < second->orig_index) return -1;
    if(first->orig_index > second->orig_index) return 1;
    return 0;
}

/*
* Report 3: Suspicious Client Behavior.
* @param (LoadResult*) data is the loaded dataset.
* @return (void)
*/
void report3_suspicious_behavior(LoadResult* data){
    int i, j;
    printf("Report 3: Suspicious Client Behavior\n");

    if(data == NULL || data->php_rows <= 0){
        printf("Error: No data loaded.\n\n");
        return;
    }

    DailySummary results[20000];
    int result_count = 0;

    // single pass through transactions in CSV order
    for(i = 0; i < data->php_rows; i++){
        Transaction* tx = &data->list[i];

        // find existing (account, date) entry
        int found_index = -1;
        for(j = 0; j < result_count; j++){
            if(results[j].acc_num == tx->account_number &&
               results[j].day == tx->day &&
               results[j].month == tx->month &&
               results[j].year == tx->year){
                found_index = j;
                break;
            }
        }

        // create a new entry if not found
        if(found_index == -1){
            if(result_count < 20000){
                strcpy(results[result_count].name, tx->account_name);
                results[result_count].acc_num = tx->account_number;
                results[result_count].day = tx->day;
                results[result_count].month = tx->month;
                results[result_count].year = tx->year;
                results[result_count].total_withdrawals = 0.0;
                results[result_count].num_deposits = 0;
                results[result_count].total_deposits = 0.0;
                results[result_count].turnover_velocity = 0.0;
                strcpy(results[result_count].risk_flag, "Normal");
                results[result_count].orig_index = result_count;
                found_index = result_count;
                result_count++;
            } else {
                continue;   // if table full
            }
        }

        // add to the group
        if(strcmp(tx->type, "withdrawal") == 0){
            results[found_index].total_withdrawals += tx->amount;
        } else if(strcmp(tx->type, "deposit") == 0){
            results[found_index].num_deposits += 1;
            results[found_index].total_deposits += tx->amount;
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
    char w_str[64], d_str[64], v_str[64];
    for(i = 0; i < result_count; i++){
        format_money(w_str, sizeof(w_str), results[i].total_withdrawals);
        format_money(d_str, sizeof(d_str), results[i].total_deposits);
        format_money(v_str, sizeof(v_str), results[i].turnover_velocity);
        fprintf(output_file, "%s,%02d/%02d/%04d,\"%s\",%d,\"%s\",\"%s\",%s\n", results[i].name, results[i].day, results[i].month, results[i].year,
                w_str, results[i].num_deposits, d_str, v_str, results[i].risk_flag);
    }
    fclose(output_file);

    // first 2 rows
    printf("%-20s | %-10s | %-18s | %-11s | %-16s | %-16s | %s\n",
           "AccountName", "Date", "TotalWithdrawals", "NumDeposits",
           "TotalDeposits", "TurnoverVelocity", "RiskFlag");
    for(i = 0; i < result_count; i++){
        if(i >= 2){
            break;
        }
        char w_str2[64], d_str2[64], v_str2[64];
        format_money(w_str2, sizeof(w_str2), results[i].total_withdrawals);
        format_money(d_str2, sizeof(d_str2), results[i].total_deposits);
        format_money(v_str2, sizeof(v_str2), results[i].turnover_velocity);
        printf("%-20s | %02d/%02d/%04d | %-18s | %-11d | %-16s | %-16s | %s\n", results[i].name, results[i].day, results[i].month, results[i].year,
               w_str2, results[i].num_deposits, d_str2, v_str2, results[i].risk_flag);
    }
    printf("(%d rows written to report3_suspicious_client_behavior.csv)\n\n", result_count);
}

/*
* Report 4: Hourly Transaction Report.
* @param (LoadResult*) data is the loaded dataset.
* @return (void)
*/
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
    char tot_str[64], avg_str[64];
    for(i = 0; i < 24; i++){
        format_money(tot_str, sizeof(tot_str), totals[i]);
        format_money(avg_str, sizeof(avg_str), averages[i]);
        fprintf(output_file, "%02d:00,%d,\"%s\",\"%s\"\n", i, counts[i], tot_str, avg_str);
    }
    fclose(output_file);

    // first 2 rows
    printf("%-6s | %-16s | %-14s | %s\n",
           "Hour", "TransactionCount", "TotalAmount", "AverageAmount");
    for(i = 0; i < 2; i++){
        char tot_str2[64], avg_str2[64];
        format_money(tot_str2, sizeof(tot_str2), totals[i]);
        format_money(avg_str2, sizeof(avg_str2), averages[i]);
        printf("%02d:00  | %-16d | %-14s | %s\n", i, counts[i], tot_str2, avg_str2);
    }
    printf("(24 rows written to report4_hourly_transaction_report.csv)\n\n");
}

/*
* This function writes the summary JSON in multi-line format.
* @param (LoadResult*) data is the loaded dataset.
* @return (void)
*/
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

        for(j = 0; j < data->php_rows; j++){
            Transaction* transaction = &data->list[j];
            if(transaction->account_number != this_acc){
                continue;
            }
            latest_balance = transaction->current_balance;
        }

        total_balance = total_balance + latest_balance;

        if(latest_balance >= 10000.0){
            double daily_rate = ANNUAL_RATE / 365.0;
            double daily_interest = latest_balance * daily_rate;
            double expected_interest = round2(daily_interest * 30.0);
            total_interest_expense = total_interest_expense + expected_interest;
        }
    }

    // round before printing so json shows exactly 2 decimals
    total_deposits = round2(total_deposits);
    total_withdrawals = round2(total_withdrawals);
    total_balance = round2(total_balance);
    total_interest_expense = round2(total_interest_expense);

    // write JSON file 
    FILE* output_file = fopen("summary.json", "w");
    if(output_file == NULL){
        printf("Error: Cannot write summary.json\n\n");
        return;
    }

    fprintf(output_file, "{\"total_accounts\": %d, \n", acc_count);
    fprintf(output_file, "  \"total_deposits\": %.2f, \n", total_deposits);
    fprintf(output_file, "  \"total_withdrawals\": %.2f, \n", total_withdrawals);
    fprintf(output_file, "  \"total_balance\": %.2f, \n", total_balance);
    fprintf(output_file, "  \"total_interest_expense\": %.2f}\n", total_interest_expense);

    fclose(output_file);

    // preview
    char dep_str[64], wd_str[64], bal_str[64], int_str[64];
    format_money(dep_str, sizeof(dep_str), total_deposits);
    format_money(wd_str, sizeof(wd_str), total_withdrawals);
    format_money(bal_str, sizeof(bal_str), total_balance);
    format_money(int_str, sizeof(int_str), total_interest_expense);

    printf("{\"total_accounts\": %d,\n", acc_count);
    printf("  \"total_deposits\": \"%s\",\n", dep_str);
    printf("  \"total_withdrawals\": \"%s\",\n", wd_str);
    printf("  \"total_balance\": \"%s\",\n", bal_str);
    printf("  \"total_interest_expense\": \"%s\"}\n\n", int_str);
}
