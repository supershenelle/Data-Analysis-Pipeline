/********************
Last names: Danieles, De Guzman, Nono, Suazon
Language: Java
Paradigm(s): Object-Oriented, Procedural
********************/

import java.io.*;
import java.time.*;
import java.time.format.DateTimeFormatter;
import java.util.*;

/**
 * Serves as the GUI, handling all user interactions, displaying the main menu
 * and transaction screens, and input validation.
 * Delegates logic to the Account class, interest logic to the Interest class,
 * and currency conversion to the ExchangeRate class.
 */
public class MainMenu{
    private Scanner sc = new Scanner(System.in);
    private Account account;
    private ExchangeRate exchangeRate = new ExchangeRate();
    private ArrayList<Account> accounts = new ArrayList<>();
    private ArrayList<TransactionRecords.Transaction> transactions = new ArrayList<>();

    /**
     * Runs the main menu loop
     */
    public void mainMenu(){
        loadAccounts();
        clearScreen();
        while(true){
            showMenu();
            int choice = ifChoiceValid(7, 0, "Choice: ");

            if(choice == 0)
                return;

            clearScreen();

            switch (choice) {
                case 1: registerAccount();
                    break;
                case 2: depositAmount();
                    break;
                case 3: withdrawAmount();
                    break;
                case 4: exchangeCurrency();
                    break;
                case 5: recordExchange();
                    break;
                case 6: showInterest(); 
                    break;
                case 7: generateReports();
            }

            clearScreen();
        }
    }

    /**
     * Searches the account list for the given account name
     * @param name represents the account name to look for
     * @return the matching Account, or null if nothing exists.
     */
    public Account findAccount(String name){
        for(Account a: accounts){
            if(a.getAccountName().equalsIgnoreCase(name)){
                return a;
            }
        }

        return null;
    }


    /**
     * Loads every saved valid account thats read from bank_transactions.csv into the accounts arraylist.
     * The balance of each account is based on the user's most recent transaction.
     */
    public void loadAccounts(){
        // Creates an arraylist of transaction records to store accounts in
        ArrayList<TransactionRecords.Transaction> rows = TransactionRecords.loadTransactions();

        // This portion is to set the user's balance based on their recent transaction
        for(TransactionRecords.Transaction t: rows){
            // Checks if the account from a row already exists
            Account existing = findAccount(t.accountName());


            // if not yet, adds to the list muna and creates a new account object
            if(existing == null){
                accounts.add(new Account(t.accountName(), t.accountNumber(), t.currentBalance()));
            } else{
            // if oo, update the setBalance based on recent transaction
                existing.setBalance(t.currentBalance());
            }
        }
    }


    /**
     * Displays the main menu options
     */
    public void showMenu(){
        System.out.println("Select Transaction:");
        System.out.println("[1] Register Account");
        System.out.println("[2] Deposit Amount");
        System.out.println("[3] Withdraw Amount");
        System.out.println("[4] Currency Exchange");
        System.out.println("[5] Record Exchange Rates");
        System.out.println("[6] Show Interest Amount");
        System.out.println("[7] Generate Reports");
        System.out.println("[0] Exit");
    }

    /**
     * Reads the input and continues to prompt until it is within the given range (hi, lo).
     * @param hi represents the highest acceptable value.
     * @param lo represents the lowest acceptable value.
     * @param prompt represents the prompt shown to the user.
     * @return a value integer/choice within the given range.
     */
    public int ifChoiceValid(int hi, int lo, String prompt){

        while(true){
            try {
            System.out.print(prompt);
            int choice = sc.nextInt();
            sc.nextLine();

            if(choice >= lo && choice <= hi){
                return choice;
            }
            
            System.out.println("Error: Invalid choice");

            } catch (InputMismatchException e) {
                System.out.println("Error: Invalid choice");
                sc.nextLine();
            }
        }
    }

    /**
     * Clears the console using the ANSI escape codes
     */
    public static void clearScreen(){
        System.out.print("\033[H\033[2J");
        System.out.flush();
    }

    /**
     * Checks if the entered name is valid
     * A name is invalid if it only contains 1 whole name, in a different format, 
     * or ends with an initial, such as Juan D., Dela Cruz, Juan, Julio
     */
    public boolean isValidName(String name){
        // Rejects if empty input or contains a comma
        if(name.isEmpty() || name.contains(",")){
            return false;
        }

        // splits into substring of names since it will be checked individually
        String[]names = name.split("\\s+");
        int fullNames = 0;

        // count the full names
        for(String n: names){
            if(n.length() > 2 || (n.length() == 2 && !n.endsWith("."))){
                fullNames++;
            }
        }

        // checks if last name is an initial
        String lastName = names[names.length - 1];
        boolean endsWithInitial = lastName.length() == 1 || (lastName.length() == 2 && lastName.endsWith("."));

        // returns true of more than 2 full names and it doesnt end with an initial
        return fullNames >=2 && !endsWithInitial;
    }

    /**
     * Simulates account registration with validation. User's account number is updated based on last number generated
     */
    public void registerAccount(){
        // asks for name and initial deposit
        // creates a new instance and adds to accounts list
        // writes to the csv file since it is a transaction

        boolean registerAgain = true;

        while(registerAgain){
            System.out.println("Register Account");

            // For name, input and storing
            String name = "";
            boolean isValid = false;

            while(!isValid){
                System.out.print("Account Name: ");
                name = sc.nextLine().trim();

                // checks if valid or if already exists
                if(!isValidName(name)){
                    System.out.println("Error: Invalid Name.");
                } else if(findAccount(name) != null){
                    System.out.println("Error: Account name already exists.");
                } else{
                    isValid = true;
                }
            }

            // For initial deposit, input and storing
            double initialDeposit = readAmount("Initial Deposit: ");
            while(initialDeposit < 5000){
                System.out.println("Error: Minimum initial deposit is 5,000 PHP");
                initialDeposit = readAmount("Initial Deposit: ");
            }

            // generating a new account number
            long max = 1092000000L;
            for(Account a: accounts){
                if(a.getAccountNumber() > max){
                    max = a.getAccountNumber();
                }
            }
            long accountNum = max+1;

            // Creation of new account
            Account newAccount = new Account(name, accountNum, initialDeposit);
            accounts.add(newAccount);
            // Since transaction, write to the CSV
            appendTransactionToCsv(newAccount, "deposit", initialDeposit);

            System.out.println();
            System.out.println("Account created successfully!");
            System.out.println();

            registerAgain = exit("Register another account? (Y/N): ");

            if(registerAgain){
                clearScreen(); // clear screen if register again
            }
        }
    }

    /**
     * Asks whether to return to the main menu and loops until valid input (Y/N).
     * @param prompt is the string prompt for given transaction.
     * @return true if user will exit and false otherwise.
     */
    public boolean exit(String prompt){
        while (true){
            System.out.print(prompt);
            String choice = sc.nextLine().trim().toUpperCase();

            if(choice.equals("N") || choice.equals("n")){
                return false;
            }
            if(choice.equals("Y") || choice.equals("y")){
                return true;
            }

            System.out.println("Error: Invalid choice");
        }
    }

    /**
     * This function is an overload of the exit since it contains various uses
     */
    public void exit(){
        while(!exit("Back to the Main Menu (Y/N): ")){
        }
    }

    /**
     * This function appends a user's transaction to the CSV file
     * @param acc represents the account to append
     * @param type represents the transaction type to append (withdraw/deposit)
     * @param amount represents the transacted amount to append.
     */
    public void appendTransactionToCsv(Account acc, String type, double amount){
        // gets today's date and current time during transaction
        LocalDateTime now = LocalDateTime.now();
        String date = now.format(DateTimeFormatter.ofPattern("dd/MM/yyyy"));
        String time = now.format(DateTimeFormatter.ofPattern("H:mm:ss"));

        // writes to the CSV file, true since append mode
        try(PrintWriter out = new PrintWriter(new FileWriter("bank_transactions.csv", true))){
            out.printf("%s,%d,%s,%s,PHP,%.2f,%s,%.2f%n", 
            acc.getAccountName(), acc.getAccountNumber(), date, time, amount, type, acc.getBalance());
        } catch(IOException e){
            System.out.println("Error: Could not save to bank_transactions.csv");
        }
    }

    /** 
     * Simulates the deposit transaction.
     * Reads amount, exchanges currency, updates balance, and appends transaction accordingly.
     */
    public void depositAmount(){

        boolean depositAgain = true;

        while(depositAgain){
            System.out.print("Account Name: ");
            String name = sc.nextLine().trim();
            account = findAccount(name);

            if (account == null){
                System.out.println("Error: Account not found.");
                System.out.println();
                depositAgain = exit("Deposit another amount? (Y/N): ");
                clearScreen();
                continue; // skips the rest if no account so it loops again
            }

            // Displays information
            System.out.println("Deposit Amount");
            System.out.println("Account Name: " + account.getAccountName());
            System.out.printf("Current Balance: %.2f\n", account.getBalance());
            System.out.println();

            // Checks if user's entered currency is already recorded
            Currency depCurrency = readCurrency("Currency (PHP/USD/JPY/GBP/EUR/CNY): ");

            // error screen displayed if no currency is recorded yet
            // will keep on trying until valid currency is inputted
            while(!exchangeRate.ifRateExists(depCurrency)){
                System.out.println("Error: No exchange rate set for " + depCurrency.getCode());
                System.out.println();
                depCurrency = readCurrency("Currency (PHP/USD/JPY/GBP/EUR/CNY): ");
            }
            
            // conversion and deposits
            double depAmount = readAmount("Deposit Amount: ");
            double amountInPhp = exchangeRate.convert(depAmount, depCurrency, Currency.PHP);
            
            // after depositing, appends to the CSV file
            account.deposit(amountInPhp);
            appendTransactionToCsv(account, "deposit", amountInPhp);
            
            // extra message if currency isnt peso
            if(depCurrency != Currency.PHP){
                System.out.printf("Converted: %.2f %s to %.2f PHP %n", depAmount, depCurrency.getCode(), amountInPhp);
            }
            System.out.println();

            // shows updated balance
            System.out.printf("Updated Balance: %.2f\n", account.getBalance());
            System.out.println();

            depositAgain = exit("Deposit another amount? (Y/N): ");

            if(depositAgain){
                clearScreen();
            }
        }
    }

    /**
     * Validating for the currency selected by user during deposit.
     * @param prompt represents the prompt to be displayed to the user.
     * @return the currency selected by the user.
     */
    public Currency readCurrency(String prompt){
        while(true){
            System.out.print(prompt);
            String curr = sc.nextLine().trim().toUpperCase();

            if(curr.isEmpty()){
                System.out.println("Error: Invalid currency");
                continue;
            }

            for(Currency c: Currency.values()){
                if(c.getCode().equals(curr)){
                    return c;
                }
            }

            System.out.println("Error: Invalid currency. Please use PHP, USD, JPY, GBP, EUR, or CNY.");
        }
    }

    /**
     * Validating for amount entered by user, continues to prompt until a numerical and positive value is inputted.
     * @param prompt represents the prompt to be displayed to the user.
     * @return the valid positive amount to be transacted.
     */
    public double readAmount(String prompt){
        while(true){
            try {
                System.out.print(prompt);
                double amount = sc.nextDouble();
                sc.nextLine();

                if(amount > 0){
                    return amount;
                }

                System.out.println("Error: Amount must be positive");

            } catch (InputMismatchException e) {
                System.out.println("Error: Invalid Amount");
                sc.nextLine();
            }
        }
    }

    /**
     * Similar to readAmount, validating for an integer entered by user. Continues
     * to prompt until a positive integer is inputted.
     * @param prompt represents the prompt shown to user.
     * @return the valid positive integer.
     */
    public int readInt(String prompt){
        while(true){
            try{
                System.out.print(prompt);
                int value = sc.nextInt();
                sc.nextLine();

                if(value > 0){
                    return value;
                }

                System.out.println("Error: Input must be positive");
            } catch (InputMismatchException e){
                System.out.println("Error: Invalid Input");
                sc.nextLine();
            }
        }
    }

    /** 
     * Simulates the withdraw transaction.
     * Reads amount and updates balance, and appends to the csv file accordingly.
     */
    public void withdrawAmount(){

        boolean withdrawAgain = true;

        while(withdrawAgain){      
            System.out.print("Account Name: ");
            String name = sc.nextLine().trim();
            account = findAccount(name);
     
            if (account == null){
                System.out.println("Error: No account found.");
                System.out.println();
                withdrawAgain = exit("Withdraw another amount? (Y/N): ");
                clearScreen();
                continue; // so it skips and go back to the loop
            }

            // Displays user info
            System.out.println("Withdraw Amount");
            System.out.println("Account Name: " + account.getAccountName());
            System.out.printf("Current Balance: %.2f\n", account.getBalance());
            System.out.println("Currency: PHP");
            System.out.println();

            double witAmount = readAmount("Withdraw Amount: ");
            
            // Checks of current balance is enough for withdraw and loops until may valid amount
            while (witAmount > account.getBalance()){
                System.out.println("Error: Insufficient Funds");
                witAmount = readAmount("Withdraw Amount: ");
                } 

            // withdraws and appends to csv file
            account.withdraw(witAmount);
            appendTransactionToCsv(account, "withdrawal", witAmount);

            System.out.println();
            System.out.printf("Updated Balance: %.2f\n", account.getBalance());
            System.out.println();

            // ask if withdraw again
            withdrawAgain = exit("Withdraw another amount? (Y/N): ");

            if(withdrawAgain){
                clearScreen();
            }
        }
    }

    /**
     * Sets a rate for a foreign currency.
     */
    public void recordExchange(){
        System.out.println("Record Exchange Rate");
        System.out.println();
        showCurrencies();

        int choice = ifChoiceValid(6, 1, "Select Foreign Currency: ");

        // Cannot update PHP as it is the base currency
        if(choice == 1){
            System.out.println("PHP is base currency. Its rate is fixed at 1.00");
        } else{
            double rate = readAmount("Exchange Rate: ");
            Currency currency = choiceToCurrency(choice);
            exchangeRate.setRate(currency, rate);
        }
        System.out.println();

        exit();
    }

    /**
     * Converts a menu number to its Currency enum Value.
     * @param choice represents the choice number (1-6).
     * @return the corresponding Currency.
     */
    public Currency choiceToCurrency(int choice){
        switch(choice){
            case 1: return Currency.PHP;
            case 2: return Currency.USD;
            case 3: return Currency.JPY;
            case 4: return Currency.GBP;
            case 5: return Currency.EUR;
            case 6: return Currency.CNY;
            default: return Currency.PHP;
        }
    }

    /**
     * Displays the currency selection list.
     */
    public void showCurrencies(){
        System.out.println("[1] Philippine Peso (PHP)");
        System.out.println("[2] United States Dollar (USD)");
        System.out.println("[3] Japanese Yen (JPY)");
        System.out.println("[4] British Pound Sterling (GBP)");
        System.out.println("[5] Euro (EUR)");
        System.out.println("[6] Chinese Yuan Renminbi (CNY)");
        System.out.println();
    }

    /**
     * Simulates conversion from source currency to target currency.
     */
    public void exchangeCurrency(){
        boolean willConvert = true;

        while(willConvert){
            System.out.println("Foreign Currency Exchange");
            System.out.println("Source Currency Option:");
            showCurrencies();

            // Check if input is valid
            int c1 = ifChoiceValid(6, 1, "Source Currency: ");
            Currency sourceCurr = choiceToCurrency(c1);
            double sourceAmt = readAmount("Source Amount: ");
            System.out.println();

            System.out.println("Exchanged Currency Options:");
            showCurrencies();
            int c2 = ifChoiceValid(6, 1, "Exchange Currency: ");
            Currency exchangeCurr = choiceToCurrency(c2);

             // Implementation of a checker in case no value is set to the chosen currency.
            if(!exchangeRate.ifRateExists(sourceCurr) && !exchangeRate.ifRateExists(exchangeCurr)){
                System.out.println("Error: No rate set for " + sourceCurr.getCode() + " and " + exchangeCurr.getCode());
            } else if(!exchangeRate.ifRateExists(sourceCurr)){
                System.out.println("Error: No rate set for " + sourceCurr.getCode());
            } else if(!exchangeRate.ifRateExists(exchangeCurr)){
                System.out.println("Error: No rate set for " + exchangeCurr.getCode());
            } else{
                System.out.printf("Exchange Amount: %.2f\n", exchangeRate.convert(sourceAmt, sourceCurr, exchangeCurr));
            }

            boolean convertAgain = false;
            System.out.println();

            // If user will convert again
            while(!convertAgain){
                System.out.print("Convert another currency (Y/N)? . . . ");
                String choice = sc.nextLine().trim().toUpperCase();

                if(choice.equals("Y")){
                    willConvert = true;
                    convertAgain = true;

                } else if(choice.equals("N")){
                    willConvert = false;
                    convertAgain = true;
                } else{
                    System.out.println("Error: Invalid Choice");
                }
            }

            if(willConvert){
                clearScreen();
            }
        }   

        exit();
    }

    /**
     * Computes and displays the daily interest table.
     */
    public void showInterest(){

        System.out.print("Account Name: ");
        String name = sc.nextLine().trim();
        account = findAccount(name);

         if (account == null){
            System.out.println("Error: No account found.");
            System.out.println();
            exit();
            return;
        }

        System.out.println("Show Interest Amount");
        System.out.println("Account Name: " + account.getAccountName());
        System.out.printf("Current Balance: %.2f\n", account.getBalance());
        System.out.println("Currency: PHP");
        System.out.println("Interest Rate: 5%");
        System.out.println();

        int days = readInt("Total Number of Days: ");

        System.out.printf("%-4s | %-10s | %-10s |%n", "Day", "Interest", "Balance");

        List<Interest.DailyEntry> entries = account.dailyInterest(days);
        
        for(Interest.DailyEntry e: entries){
            System.out.printf("%-4d | %-10.2f | %-10.2f |%n", e.day(), e.interest(), e.balance());
        }

        System.out.println();
        exit();
    }

    /**
     * This function is used to compile all functions to generate all 4 reports on csv files
     */
    public void generateReports(){
        while(true){
            System.out.println("Generate Reports");
            System.out.println();
            System.out.println("[1] Load the file");
            System.out.println("[2] Generate Reports");
            int choice = ifChoiceValid(2, 1, "Choice: ");
            System.out.println();

            if(choice == 1){
                transactions = TransactionRecords.loadTransactions();
                // incase file was empty
                if(transactions.isEmpty()){
                    System.out.println("Error: No transactions loaded.");
                }
                System.out.println();
            } else {
                // incase di pa naload
                if(transactions.isEmpty()){
                    System.out.println("Error: Load file first");
                    continue;
                } else{
                    System.out.println("Generating reports...");
                    System.out.println("Outputs saved to individual files...");
                    System.out.println();
                    
                    Reports reports = new Reports(transactions);
                    reports.report1();
                    reports.report2();
                    reports.report3();
                    reports.report4();

                    exit();
                    return;
                }
            }
        }
    }
}

