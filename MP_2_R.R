# 
# Last names: Danieles, De Guzman, Nono, Suazon
# Language: R
# Paradigm(s): Procedural
#
######################################
#------------------------------ INSTRUCTIONS -------------------------------#
# TO RUN THIS PROGRAM SA VSCODE USE ctrl + shift + S kasi di gumagana read line if hindi
# DONT FORGET NA AYUSIN DIRECTORY PARA MAACCESS FILE (getwd() to check cur dir) 
# setwd("C:/Users/mavrick de guzman/Downloads/adprg mco2")



# walang constants sa R so wag nlng baguhin pag naka caps
MAX_NAME <- 100
DEFAULT_BALANCE <- 0.00
ANNUAL_RATE <- 0.05
MAX_DAYS <- 3650

# contains the full names of each currency.
CURRENCY_NAMES <- c(
  "Philippine Peso", "United States Dollar", "Japanese Yen",
  "British Pound Sterling", "Euro", "Chinese Yuan Renminni"
)
# contains the currency codes for each currency.
CURRENCY_CODES <- c("PHP", "USD", "JPY", "GBP", "EUR", "CNY")

# MAIN PROGRAM
main <- function(){
    accountName <- ""
    balance <- DEFAULT_BALANCE
    accountCurrency <- 1L
    rates <- c(1.0, 0, 0, 0, 0, 0)

    while(TRUE)
    {
        clearScreen()
        showMenu()
        choice <- readline("choice: ")
        choice <- as.integer(choice)

        # add this para di mag error if nag enter ng string 
        if (is.na(choice)) {
            cat("Error: Please enter a valid choice.\n\n")
            next
        }
        
        if(choice < 1 || choice > 7) {
            cat("Error: Invalid choice.\n")
            next
        }

        if(choice == 1){
            registerAccount()
        } else if(choice == 2 ){
            depositAmount(rates)
        } else if(choice == 3){
            withdrawAmount()
        }else if(choice == 4){
            currencyExchange(rates)
        } else if(choice == 5){
            rates <- recordExchange(rates)
        } else if(choice == 6){
            showInterestAmount()
        }else if(choice == 7){
            generateReports()
        }else{
            cat("Error: Invalid choice")
        }
        askBackToMenu()
    }
}

#####################################
# FUNCTIONS FOR CHOICES AND MAIN MENU

# This function displays the main menu options to the user.
showMenu <- function(){
    cat("Select Transaction:\n")
    cat("[1] Register Account Name\n")
    cat("[2] Deposit Amount\n")
    cat("[3] Withdraw Amount\n")
    cat("[4] Currency Exchange\n")
    cat("[5] Record Exchange Rates\n")
    cat("[6] Show Interest Amount\n")
    cat("[7] Generate Reports\n")
}

# This function clears the console screen using ANSI escape codes.
# It resets the cursor to the top-left and clears everything.
clearScreen <- function() {
  cat("\033[2J\033[H\n\n")
}

# This function asks the user if they want to return to the main menu.
# @return (ans) The validated character entered by the user.
askBackToMenu <- function() {
  while(TRUE) {
    ans <- readline("\nBack to the Main Menu (Y/N): ")
    if(is.na(ans)){
        next
    } else if (ans == "Y" || ans == "y") {
      break
    } else if (ans == "N" || ans == "n") {
      cat("Thank you!\n")
      quit(save = "no")
    } else {
      cat("Error: Please enter Y or N.\n\n")
    }
  }
}

# This function asks the user if they want to convert another currency.
# @return (ans) The user's response (Y/N).
askConvertAnother <- function() {
  while(TRUE) {
    ans <- readline("Convert another currency (Y/N)? ")
    if(is.na(ans)){
        next
    } else if (ans == "Y" || ans == "y" || ans == "N" || ans == "n") {
        return(ans)
    } else {
      cat("Error: Please enter Y or N.\n\n")
    }
}
}

# Generates the next available account number.
# @return(nextNumber) The next available account number.
generateAccountNumber <- function(file = "bank_transactions.csv") {

    if (file.exists(file)) {
        data <- read.csv(file)
        highestNumber <- max(data$account_number)
        nextNumber <- highestNumber + 1

    } else {
        stop("File does not exist or is a directory: ", file)
    }

    return(nextNumber)
}

# This function finds and selects an existing bank account using account name
# @param file of CSV file containing bank transactions
# @return (list) A list containing accountName, accountNumber, and balance;
selectAccount <- function(file = "bank_transactions.csv") {

    # check if CSV file exists
    if(!file.exists(file)) {
        cat("Error: file not found.\n")
        return(NULL)
    }

    # read file
    data <- read.csv(file)

    # ask for account name
    while(TRUE) {
        accountName <- trimws(readline("Account Name: "))

        # find yng matching account
        accountRows <- which(
            tolower(trimws(data$account_name)) == tolower(accountName)
        )

        # check if account exists
        if(length(accountRows) == 0) {
            cat("Error: Account not found.\n\n")
            next
        }

        # get latest transaction
        latestRecord <- data[tail(accountRows, 1), ]

        # Display current balance (for checking lng)
        #cat("Account Name:", latestRecord$account_name, "\n")
        #cat(sprintf("Current Balance: PHP %.2f\n", latestRecord$current_balance))

        # return account information
        return(list(
            accountName = latestRecord$account_name,
            accountNumber = as.character(latestRecord$account_number),
            balance = latestRecord$current_balance
        ))
    }

}

# This function registers a new account 
# It loops until a non-blank name is entered and stores it in the Account struct.
# @param file of CSV file containing bank transactions
# @return (accountName) The name of the account holder.
registerAccount <- function(file = "bank_transactions.csv") {
    loop <- "Y"

    while(loop == "Y" || loop == "y"){
        cat("\nRegister Account Name\n")

    # get account name
    while(TRUE){
        accountName <- trimws(readline("Account Name: "))

        if(accountName == ""){
            cat("Error: Name cannot be empty.\n")
        } else {

            # Check if the account name already exists.
            if(file.exists(file)) {
                data <- read.csv(file)

                existingNames <- tolower(trimws(data$account_name))

                if(tolower(accountName) %in% existingNames) {
                    cat("Error: Account name already exists.\n")
                    next
                }
            }

            break
        }
    }
        
        # generate account number
        accountNumber <- generateAccountNumber(file = file)

        # Get initial deposit
        while(TRUE){
            initialDeposit <- as.numeric(readline("Initial Deposit: "))
            if (is.na(initialDeposit)) {
                cat("Error: Please enter a valid amount.\n")
            } else if (initialDeposit < 5000) {
                cat("Error: Initial deposit must be at least PHP 5,000.\n")
            } else {
                break
            }    
        }


        # get curr time and date
        currentDate <- format(Sys.Date(), "%d/%m/%Y")
        currentTime <- format(Sys.time(), "%H:%M:%S")

        # Create record
        newTransaction <- data.frame(
            account_name = accountName,
            account_number = accountNumber,
            date = currentDate,
            time = currentTime,
            currency = "PHP",
            amount = initialDeposit,
            type = "deposit",
            current_balance = initialDeposit
        )

        # write to existing csv file
        write.table(
            newTransaction,
            file = file,
            sep = ",",
            row.names = FALSE,
            col.names = FALSE,
            append = TRUE,
            quote = FALSE
        )

        cat("\nAccount created successfully!\n")

        # Ask if the user wants to register another account.
        while(TRUE){

            again <- readline("Register another account? (Y/N): ")

            if(again == "Y" || again == "y" || again == "N" || again == "n") {
                loop <- again
                break
            } else {
                cat("Error: Please enter Y or N.\n")
            }
        }
    }
}


# This function handles the deposit process.
# It gets the account information, displays the current balance, takes a deposit amount, validates it, and updates the account balance.
# @param (rates) A numeric vector containing the exchange rates for each currency.
# @param file of CSV file containing bank transactions
depositAmount <- function(rates, file = "bank_transactions.csv"){
    cat("\nDeposit Amount\n")

    # select account from file
    selectedAccount <- selectAccount(file)
    if(is.null(selectedAccount))
        {
            cat("Error: account not found.\n")
            return(NULL)
        }

    accountName <- selectedAccount$accountName
    accountNumber <- selectedAccount$accountNumber
    balance <- selectedAccount$balance

        loop <- "Y"

    while(loop == "Y" || loop == "y") {
        
        #cat("Account Name:", accountName, "\n")
        cat("Current Balance:", sprintf("%.2f\n", balance))
        cat("Currency: PHP\n\n")
        cat("Deposit Currency Options:\n")
        cat("[1] Philippine Peso (PHP)\n")
        cat("[2] United States Dollar (USD)\n")
        cat("[3] Japanese Yen (JPY)\n")
        cat("[4] British Pound Sterling (GBP)\n")
        cat("[5] Euro (EUR)\n")
        cat("[6] Chinese Yuan Renminbi (CNY)\n")

        # Validate the selected deposit currency.
        while(TRUE){
            depositCurrency <- readline("Select Deposit Currency: ")
            depositCurrency <- as.numeric(depositCurrency)

            if(is.na(depositCurrency) || depositCurrency != as.integer(depositCurrency)){
                cat("Error: Please enter a valid whole number.\n")
            } else if(depositCurrency < 1 || depositCurrency > 6){
                cat("Error: Please select a currency from 1 to 6.\n")
            } else {
                depositCurrency <- as.integer(depositCurrency)
                break
            }
        }

        # Foreign currency rates must be recorded first
        if(depositCurrency != 1 && rates[depositCurrency] == 0){
            cat(sprintf("Rate for %s has not been recorded. Please record it first.\n", CURRENCY_CODES[depositCurrency]))
            return(balance)
        }

        # input validation for negative values
        while(TRUE){
            depostAmt <- readline(sprintf("Deposit Amount (%s): ", CURRENCY_CODES[depositCurrency]))
            depostAmt <- as.numeric(depostAmt)

            if(is.na(depostAmt)){
                cat("Error: Please enter a valid number.\n")
            } else if(depostAmt <= 0){
                cat("Error: Deposit amount must be greater than zero.\n")
            } else {
                break
            }
        }

        # Convert  deposit to PHP before adding to the account balance
        if(depositCurrency == 1){
            amountInPHP <- depostAmt
        } else {
            amountInPHP <- depostAmt * rates[depositCurrency]
        }

        newBalance <- balance + amountInPHP
        cat(sprintf("\nDeposit Amount: %.2f %s\n",
                    depostAmt, CURRENCY_CODES[depositCurrency]))
        cat(sprintf("Converted Amount: %.2f PHP\n", amountInPHP))
        cat(sprintf("Updated Balance: %.2f PHP\n", newBalance))


        # create transaction record
        newTransaction <- data.frame(
            account_name = accountName,
            account_number = accountNumber,
            date = format(Sys.Date(), "%d/%m/%Y"),
            time = format(Sys.time(), "%H:%M:%S"),
            currency = "PHP",
            amount = amountInPHP,
            type = "deposit",
            current_balance = newBalance
        )

        # save transaction to file
        write.table(
            newTransaction,
            file = file,
            sep = ",",
            row.names = FALSE,
            col.names = FALSE,
            append = TRUE,
            quote = FALSE
        )

        # update balance for next loop
        balance <- newBalance

        # make another deposit to another acc
        while(TRUE){
            loop <- readline("\nDeposit another amount? (Y/N): ")

            if(loop == "Y" || loop == "y" || loop == "N" || loop == "n") {
                break
            } else {
                cat("Error: Please enter Y or N.\n")
            }
        }

    }
}


# This function handles the withdrawal process.
# It gets the account information, displays the current balance, takes a withdrawal amount, validates it, checks for sufficient funds, 
# and updates the account balance.
# @param (rates) A numeric vector containing the exchange rates for each currency.
withdrawAmount <- function(file = "bank_transactions.csv") {
    cat("\nWithdraw Amount\n")

    selectedAccount <- selectAccount(file)
    if(is.null(selectedAccount))
        {
            cat("Error: account not found.\n")
            return(NULL)
        }

    accountName <- selectedAccount$accountName
    accountNumber <- selectedAccount$accountNumber
    balance <- selectedAccount$balance
    accountCurrency <- 1L  

        loop <- "Y"

    while(loop == "Y" || loop == "y") {
        #cat("Account Name:", accountName, "\n")
        cat("Current Balance:", sprintf("%.2f\n", balance))
        cat("Currency:", CURRENCY_CODES[accountCurrency], "\n")

        #input validation for negative values 
        while(TRUE){
            withdrawAmount <- readline("\nWithdraw Amount: ")
            withdrawAmount <- as.numeric(withdrawAmount)
            if(is.na(withdrawAmount)){
                cat("Error: Please enter a valid number.\n")
            }
            else if(withdrawAmount < 0){
                cat("Error: Cannot withdraw negative number")            
            }
            else if(withdrawAmount > balance){
                cat("Error: insufficient funds")
            }
            else{
                break
            }
        }

        newBalance <- balance - withdrawAmount
        cat("Updated Balance =", sprintf("%.2f", newBalance), "\n")
            
        # create transaction record
        newTransaction <- data.frame(
            account_name = accountName,
            account_number = accountNumber,
            date = format(Sys.Date(), "%d/%m/%Y"),
            time = format(Sys.time(), "%H:%M:%S"),
            currency = "PHP",
            amount = withdrawAmount,
            type = "withdrawal",
            current_balance = newBalance
        )

        # save transaction to file
        write.table(
            newTransaction,
            file = file,
            sep = ",",
            row.names = FALSE,
            col.names = FALSE,
            append = TRUE,
            quote = FALSE
        )

        # update balance for next loop
        balance <- newBalance

        # make another deposit to another acc
        while(TRUE){
            loop <- readline("\nWithdraw another amount? (Y/N): ")

            if(loop == "Y" || loop == "y" || loop == "N" || loop == "n") {
                break
            } else {
                cat("Error: Please enter Y or N.\n")
            }
        }
    }

}


# This function performs currency exchange calculations.
# @param (rates) A numeric vector containing the exchange rates for each currency.
# @return (void)
currencyExchange <- function(rates)
{
    again <- 'Y'
    
    while(again == 'Y' || again == 'y'){
        cat("\nForeign Currency Exchange\n")
        cat("Source Currency Option:\n")
        cat("[1] Philippine Peso (PHP)\n")
        cat("[2] United States Dollar (USD)\n")
        cat("[3] Japanese Yen (JPY)\n")
        cat("[4] British Pound Sterling (GBP)\n")
        cat("[5] Euro (EUR)\n")
        cat("[6] Chinese Yuan Renminni (CNY)\n\n")
        
        while(TRUE){
            source <- readline("Source Currency: ")
            source <- as.numeric(source)

            if (is.na(source)) {
                cat("Invalid input. Please enter a number.\n\n")
                next
            }
            if (source < 1 || source > 6) {
                cat("Invalid source currency. Must be 1-6.\n\n")
                next
            }
            break
        }

        while(TRUE){
            amount <- readline("Source Amount: ")
            amount <- as.numeric(amount)

            if (is.na(amount)) {
                cat("Invalid input. Please enter a number.\n\n")
                next
            }
            if (amount <= 0) {
                cat("Invalid source amount. Must be a positive number.\n\n")
                next
            }
            break
        }
        cat("\n")

        cat("Exchanged Currency Options:\n")
        cat("[1] Philippine Peso (PHP)\n")
        cat("[2] United States Dollar (USD)\n")
        cat("[3] Japanese Yen (JPY)\n")
        cat("[4] British Pound Sterling (GBP)\n")
        cat("[5] Euro (EUR)\n")
        cat("[6] Chinese Yuan Renminni (CNY)\n\n")

        while(TRUE){
            target <- readline("Exchange Currency: ")
            target <- as.numeric(target)

            if (is.na(target)) {
                cat("Invalid input. Please enter a number.\n\n")
                next
            }
            if (target < 1 || target > 6) {
                cat("Invalid exchange currency. Must be 1-6.\n\n")
                next
            }
            break
        }

    if (source != 1 && rates[source] == 0) {
        cat(sprintf("Rate for %s not yet recorded. Please record it first.\n\n", CURRENCY_CODES[source]))
        again <- askConvertAnother()
        next
    }
    if (target != 1 && rates[target] == 0) {
      cat(sprintf("Rate for %s not yet recorded. Please record it first.\n\n", CURRENCY_CODES[target]))
      again <- askConvertAnother()
      next
    }
    
    if(source == 1){
        inPhp <- amount
    }else{
        inPhp <- amount * rates[source]
    }

    if(target == 1){
        result <- inPhp
    } else{
        result <- inPhp / rates[target]
    }

    cat(sprintf("Exchange Amount: %.2f\n\n", result))

    again <- askConvertAnother()
    }
}


# This function allows the user to record an exchange rate for a foreign currency.
# @param (rates) A numeric vector containing the exchange rates for each currency.
# @return (rates) The updated numeric vector containing the exchange rates for each currency.
recordExchange <- function(rates)
{
    cat("\nRecord Exchange rate\n\n")
    cat("[1] Philippine Peso (PHP)\n")
    cat("[2] United States Dollar (USD)\n")
    cat("[3] Japanese Yen (JPY)\n")
    cat("[4] British Pound Sterling (GBP)\n")
    cat("[5] Euro (EUR)\n")
    cat("[6] Chinese Yuan Renminni (CNY)\n\n")

    while(TRUE){
        choice <- readline("Select Foreign Currency: ")
        choice <- as.numeric(choice)
        if (is.na(choice)) {
        cat("Invalid input. Please enter a number.\n\n")
        next
        }
        if (choice < 1 || choice > 6) {
        cat("Invalid currency choice. Must be 1-6.\n\n")
        next
        }
        if (choice == 1) {
        cat("PHP is the base currency. Cannot set its rate.\n\n")
        next
        }
        break
    }

        while(TRUE){
            rate <- readline("Exchange rate: ")
            rate <- as.numeric(rate)
            if (is.na(rate)) {
                cat("Invalid input. Please enter a number.\n\n")
                next
            }
            if (rate <= 0) {
                cat("ERROR: Invalid exchange rate. Must be greater than 0.\n\n")
                next
            }
            break
    }

  rates[choice] <- rate
  cat("\n")
  return(rates)
}

#
# This function calculates and displays the expected daily interest over N days.
# It displays the current account details, takes the number of days, and prints
# a formatted table showing the day, interest earned, and running balance.
# @param file that contains the bank transactions in CSV format.

# @return (void)
showInterestAmount <- function(file = "bank_transactions.csv"){
    cat("Show Interest Amount\n")

    selectedAccount <- selectAccount(file)

    if(is.null(selectedAccount)) {
        cat("Error: Account not found.\n")
        return(NULL)
    }
    accountName <- selectedAccount$accountName
    balance <- selectedAccount$balance

    cat("Account Name:", accountName, "\n")
    cat(sprintf("Current Balance: %.2f\n", balance))
    cat("Currency: PHP\n")
    cat("Interest Rate: 5%\n\n")

    while(TRUE) {
        days <- readline("Total Number of Days: ")
        days <- as.numeric(days)
        if (is.na(days)) {
            cat("Invalid input. Please enter a number.\n\n")
        next
        }
        if (days <= 0) {
            cat("Invalid number of days. Must be greater than 0.\n\n")
        next
        }
        if (days > MAX_DAYS) {
            cat(sprintf("Number of days too large (max %d).\n\n", MAX_DAYS))
        next
        }
        break
    }

    cat("\n")
    cat(sprintf("%-4s| %-9s| %-8s|\n", "Day", "Interest", "Balance"))
    current <- balance

    for (i in 1:days) {
        interest <- current * (ANNUAL_RATE / 365.0)
        current <- current + interest
        cat(sprintf("%-4d| %-9.2f| %-8.2f|\n", i, interest, current))
    }
    cat("\n")
}

# load, validates, and clean bank transaction data file. 
# It checks that the CSV file and required columns exist, converts columns to
# the appropriate data types, removes rows with missing or invalid values,
# and keeps valid transactions recorded in Philippine pesos (PHP).
loadFileData <- function(file = "bank_transactions.csv") {

    cat("\nProcessing dataset...\n")

    # check if the CSV file exists
    if(!file.exists(file)) {
        cat("Error: Transaction file not found.\n")
        return(NULL)
    }

    # read csv file
    data <- read.csv(file)
    if(is.null(data)) {
        return(NULL)
    }

    # display the total number of rows loaded.
    totalRows <- nrow(data)
    cat(sprintf("(%s rows loaded, ", format(totalRows, big.mark = ",")))

    # check if all required columns are present.
    requiredColumns <- c("account_name", "account_number", "date", "time", "currency", "amount", "type", "current_balance")
    # if any column is missing, error
    if(!all(requiredColumns %in% names(data))) {
        cat("Error: CSV file is missing required columns.\n")
        return(NULL)
    }

    # assign values
    data$account_number <- as.character(data$account_number)
    data$amount <- as.numeric(data$amount)
    data$current_balance <- as.numeric(data$current_balance)
    data$date <- as.Date(data$date, format = "%d/%m/%Y")
    
    # Count missing or invalid values.
    invalidRows <- (
        is.na(data$account_name) | trimws(data$account_name) == "" |
        is.na(data$account_number) | trimws(data$account_number) == "" |
        is.na(data$date) |
        is.na(data$time) | trimws(data$time) == "" |
        is.na(data$currency) | trimws(data$currency) == "" |
        is.na(data$amount) | !is.finite(data$amount) |
        is.na(data$type) | trimws(data$type) == "" |
        is.na(data$current_balance) | !is.finite(data$current_balance)
    )
    #DONT FORGET TO COMMENT THIS SINCE DI NAMAN NEED SA OUTPUT
     #cat(sprintf("Rows with missing or invalid values: %s", format(sum(invalidRows), big.mark = ",")))


    # Get the original row numbers of PHP transactions.
    phpRows <- which(!is.na(data$currency) & data$currency == "PHP")


    # keep only valid PHP transactions using their original row numbers
    validPhpRows <- phpRows[!invalidRows[phpRows]]
    cleanData <- data[validPhpRows, ]

    cat(sprintf("%s filtered for PHP currency)\n", format(nrow(cleanData), big.mark = ",")))


    return(cleanData)
}

# displays generate reports menu
# It allows the user to load and validate the transaction file, generate
# Reports 1 to 4 and the JSON summary, or return to the main menu
generateReports <- function() {

    bankData <- NULL

    while(TRUE) {
        cat("\nGenerate Reports\n\n")
        cat("[1] Load the file\n")
        cat("[2] Generate Reports\n")
        cat("[3] Back to Main Menu\n")

        choice <- readline("Enter choice: ")
        choice <- as.numeric(choice)

        # Validate menu choice
        if(choice < 1 || choice > 3 || is.na(choice)) {
            cat("Error: Please enter a valid choice.\n")
            next
        }


        if(choice == 1) {
            # load and clean transaction data
            bankData <- loadFileData()
        } else if(choice == 2) {
            # check pag naload na file, if di pa need muna iload
            if(is.null(bankData)) {
                cat("\nPlease load the transaction file first.\n")
            } else {
                cat("\nGenerating reports...\n")

                # add generation function
                report1 <- generateReport1(bankData)
                report2 <- generateReport2(bankData)
                report3 <- generateReport3(bankData)
                report4 <- generateReport4(bankData)
                generateSummary(bankData, report2)
            }

        } else if(choice == 3) {

            return()

        } else {
            cat("Error: Invalid choice. Please select 1 to 3.\n")
        }
    }
}


# generates Banking Privilege Tiers
# It calculates each account's age, current balance, and average daily balance
# for the previous month, assigns a privilege tier to qualifying accounts,
# sorts the results by average daily balance, and exports the report to CSV.
# @param data contains data frame containing validated PHP bank transactions.
# @return (report) A data frame containing accounts that qualify for a
# privilege tier, sorted by average daily balance in descending order.
generateReport1 <- function(data) {

    # get previous month's start and end dates
    currentMonth <- as.Date(format(Sys.Date(), "%Y-%m-01"))
    lastMonthEnd <- currentMonth - 1
    lastMonthStart <- as.Date(format(lastMonthEnd, "%Y-%m-01"))

    # get total number of days in previous month
    daysInMonth <- as.integer(format(lastMonthEnd, "%d"))

    # Sort transactions by date and time (oldest first)
    data <- data[order(data$date, data$time), ]

    # get all unique account num
    accountNumbers <- unique(data$account_number)

    # create empty data frame for report
    report <- data.frame(
        AccountName = character(),
        AccountAge = integer(),
        CurrentBalance = numeric(),
        AverageDailyBalance = numeric(),
        PrivilegeTier = character()
    )

    # process each account
    for(account in accountNumbers) {

        # get all transactions belonging to account
        accountData <- data[data$account_number == account, ]

        # sort account's transactions by date and time (oldest first)
        accountData <- accountData[order(accountData$date, accountData$time), ]

        # get account's opening date
        openingDate <- min(accountData$date)

        # calculate account age in days
        accountAge <- as.integer(Sys.Date() - openingDate)

        # get latest transac and curr balance
        latestRecord <- accountData[nrow(accountData), ]
        currentBalance <- latestRecord$current_balance

        # calculate total of all daily ending balances
        totalDailyBalance <- 0

        day <- lastMonthStart
        while(day <= lastMonthEnd) {

            # Find transactions made on or before this day.
            previousTransactions <- accountData[accountData$date <= day, ]

            # Get the ending balance for the day.
            if(nrow(previousTransactions) > 0) {
                endOfDayBalance <- previousTransactions[nrow(previousTransactions), ]$current_balance #gets last row since yun latest
            } else {
                endOfDayBalance <- 0
            }

            totalDailyBalance <- totalDailyBalance + endOfDayBalance
            day <- day + 1
        }

        # calculate average daily balance
        averageDailyBalance <- totalDailyBalance / daysInMonth

        # AverageDailyBalance (filter >= Php. 100, 000.00)
        if(averageDailyBalance >= 100000) {

            # Assign the privilege tier.
            if(averageDailyBalance < 500000) {
                privilegeTier <- "Silver"
            } else if(averageDailyBalance < 1000000) {
                privilegeTier <- "Gold"
            } else {
                privilegeTier <- "Platinum"
            }

            # add the account to the report(use rbind para ma append sa same date frame)
            report <- rbind(report, data.frame(
                AccountName = latestRecord$account_name,
                AccountAge = accountAge,
                CurrentBalance = currentBalance,
                AverageDailyBalance = averageDailyBalance,
                PrivilegeTier = privilegeTier
            ))
        }
    }

    # sort by Average Daily Balance, highest first (kaya may negative para descending)
    report <- report[order(-report$AverageDailyBalance), ]

    # export the report
    write.csv(
        report,
        file = "report1_banking_privilege_tiers.csv",
        row.names = FALSE
    )

    # display the first two rows as preview
    cat("\nReport 1: Banking Privilege Tiers\n")

    if(nrow(report) > 0) {
        print(head(report, 2))
    } else {
        cat("No accounts qualified for the privilege tiers.\n")
    }

    cat("\nFull table exported to report1_banking_privilege_tiers.csv\n")

    return(report)
}

#generate Report 2: Interest Expense Projection
# It finds each account's latest balance, calculates its expected interest
# for 30 days when the balance is at least PHP 10,000, sorts the results by
# expected interest in descending order, and exports the report to CSV.
# @param data A data frame containing validated PHP bank transactions.
# @return (report) A data frame containing qualifying accounts and their
# projected interest, sorted from highest to lowest
generateReport2 <- function(data) {

    # Sort transactions by date and time (oldest first)
    data <- data[order(data$date, data$time), ]

    # get all unique account num
    accountNumbers <- unique(data$account_number)

    # create an empty report
    report <- data.frame(
        AccountName = character(),
        CurrentBalance = numeric(),
        ExpectedInterest = numeric()
    )

    # process each account
    for(account in accountNumbers) {

        # Get all transactions belonging to this account.
        accountData <- data[data$account_number == account, ]

        # Get the latest transaction.
        latestRecord <- accountData[nrow(accountData), ]

        currentBalance <- latestRecord$current_balance

        # ilter >= Php. 10, 000.00 current balance
        if(currentBalance >= 10000) {

            # Calculate the expected interest for 30 days.
            totalInterest <- currentBalance * (ANNUAL_RATE / 365) * 30

            # append to current report
            report <- rbind(report, data.frame(
                AccountName = latestRecord$account_name,
                CurrentBalance = currentBalance,
                ExpectedInterest = totalInterest
            ))
        }
    }

    # Sort by ExpectedInterest, highest to lowest.
    report <- report[order(-report$ExpectedInterest), ]

    # Export the report.
    write.csv(
        report,
        file = "report2_interest_expense_projection.csv",
        row.names = FALSE
    )

    # Display the first two rows.
    cat("\nReport 2: Interest Expense Projection\n")

    if(nrow(report) > 0) {
        print(head(report, 2))
    } else {
        cat("No accounts qualified for the interest projection.\n")
    }

    cat("\nFull table exported to report2_interest_expense_projection.csv\n")

    return(report)
}


# generate Report 3: Suspicious Client Behavior
# It examines each account's transactions for each date, calculates total
# withdrawals, total deposits, number of deposits, and turnover velocity,
# assigns a risk flag, sorts the results, and exports the report to CSV.
# @param data A data frame containing validated PHP bank transactions.
# @return (report) A data frame containing the daily transaction activity
# and risk flag for each account.
generateReport3 <- function(data) {

    # Get all unique account and date combinations
    accountDates <- unique(data[c("account_number", "date")])

    # Create an empty report.
    report <- data.frame(
        AccountName = character(),
        Date = as.Date(character()),
        TotalWithdrawalAmounts = numeric(),
        NumberOfDeposits = integer(),
        TotalDepositAmounts = numeric(),
        TurnoverVelocity = numeric(),
        RiskFlagLevel = character()
    )

    # process each account for each day
    for(i in 1:nrow(accountDates)) {

        account <- accountDates$account_number[i]
        transactionDate <- accountDates$date[i]

        # get transactions for this account on this date
        dailyData <- data[data$account_number == account & data$date == transactionDate, ]
        accountName <- dailyData$account_name[1]

        # get withdrawal transactions
        withdrawals <- dailyData[ tolower(dailyData$type) == "withdrawal", ]

        # Get deposit transactions
        deposits <- dailyData[tolower(dailyData$type) == "deposit", ]

        # Calculate total withdrawals and deposits
        totalWithdrawals <- sum(withdrawals$amount)
        totalDeposits <- sum(deposits$amount)
        numberOfDeposits <- nrow(deposits)

        # Calculate turnover velocity
        if(totalDeposits == 0) {
            if(totalWithdrawals > 0) {
                turnoverVelocity <- Inf
            } else {
                turnoverVelocity <- 0
            }
        } else {
            turnoverVelocity <- (totalWithdrawals / totalDeposits) * 100
        }

        # determine risk level
        if(totalWithdrawals >= 500000 || turnoverVelocity >= 90) {
            riskFlagLevel <- "Suspicious"
        } else {
            riskFlagLevel <- "Normal"
        }

        # Add the results to the report
        report <- rbind(report, data.frame(
            AccountName = accountName,
            Date = transactionDate,
            TotalWithdrawalAmounts = totalWithdrawals,
            NumberOfDeposits = numberOfDeposits,
            TotalDepositAmounts = totalDeposits,
            TurnoverVelocity = turnoverVelocity,
            RiskFlagLevel = riskFlagLevel
        ))
    }

    # Sort by date (ascending) and risk level (descending).
    report <- report[order(report$Date, -as.integer(report$RiskFlagLevel == "Suspicious")), ]

    # Format the date for the CSV.
    report$Date <- format(report$Date, "%d/%m/%Y")

    # Export the report.
    write.csv(
        report,
        file = "report3_suspicious_client_behavior.csv",
        row.names = FALSE
    )

    # Display the first two rows.
    cat("\nReport 3: Suspicious Client Behavior\n")

    if(nrow(report) > 0) {
        print(head(report, 2))
    } else {
        cat("No transactions found.\n")
    }

    cat("\nFull table exported to report3_suspicious_client_behavior.csv\n")

    return(report)
}

# generate Report 4: Top Depositors
# It calculates each account's total deposits, number of deposits, and
# average deposit, sorts accounts by total deposits, selects the top 10,
# and exports the results to CSV.
# @param data A data frame containing validated PHP bank transactions.
# @return (topDepositors) A data frame containing up to 10 accounts with
# the highest total deposit amounts.
generateReport4 <- function(data) {

    # get all unique account numbers
    accountNumbers <- unique(data$account_number)

    # create empty report
    report <- data.frame(
        AccountName = character(),
        AccountNumber = character(),
        TotalDeposits = numeric(),
        NumberOfDeposits = integer(),
        AverageDeposit = numeric()
    )

    # process each account
    for(account in accountNumbers) {

        # Get all transactions belonging to this account
        accountData <- data[data$account_number == account, ]

        # get deposit transactions only
        deposits <- accountData[
            tolower(accountData$type) == "deposit",
        ]

        # calculate deposit statistics
        totalDeposits <- sum(deposits$amount)
        numberOfDeposits <- nrow(deposits)

        # Calculate average deposit.
        if(numberOfDeposits > 0) {
            averageDeposit <- totalDeposits / numberOfDeposits
        } else {
            averageDeposit <- 0
        }

        # Add the account to the report.
        report <- rbind(report, data.frame(
            AccountName = accountData$account_name[1],
            AccountNumber = as.character(account),
            TotalDeposits = totalDeposits,
            NumberOfDeposits = numberOfDeposits,
            AverageDeposit = averageDeposit
        ))
    }

    # sort by TotalDeposits, highest to lowest
    report <- report[order(-report$TotalDeposits), ]

    # get top 10 depositors
    topDepositors <- head(report, 10)

    # export the report
    write.csv(
        topDepositors,
        file = "report4_top_depositors.csv",
        row.names = FALSE
    )

    # Display the results
    cat("\nReport 4: Top Depositors\n")


    if(nrow(topDepositors) > 0) {
        print(head(topDepositors, 2))
    } else {
        cat("No deposit transactions found.\n")
    }

    cat("\nFull table exported to report4_top_depositors.csv\n")

    return(topDepositors)
}


# This function generates the overall banking summary and produce a summary.json
# It calculates the total number of accounts, total deposits, total
# withdrawals, combined current balance, and projected interest expense,
# then saves these values in summary.json and displays them in the console.
# @param data A data frame containing validated PHP bank transactions.
# @param report2 A data frame returned by generateReport2(), containing
# the expected interest for qualifying accounts.
generateSummary <- function(data, report2) {
    cat("\nSummary State (summary.json)\n")

    # get all unique account numbers
    accountNumbers <- unique(data$account_number)

    # count the total number of accounts (use length kasi vector nirereturn ng unique)
    totalAccounts <- length(accountNumbers)

    # calculate total deposits
    deposits <- data[tolower(data$type) == "deposit", ]
    totalDeposits <- sum(deposits$amount)

    # calculate total withdrawals
    withdrawals <- data[tolower(data$type) == "withdrawal", ]
    totalWithdrawals <- sum(withdrawals$amount)

    # calculate the total current balance of all accounts
    totalBalance <- 0

    for(account in accountNumbers) {

        # get all transactions belonging to this account
        accountData <- data[data$account_number == account, ]

        # get the latest transaction
        latestRecord <- accountData[nrow(accountData), ]

        # add the latest balance to the total
        totalBalance <- totalBalance + latestRecord$current_balance
    }

    # calculate total projected interest from Report 2.
    totalInterestExpense <- sum(report2$ExpectedInterest)

    # Create the JSON content.
    summary <- c(
        "{",
        sprintf('  "total_accounts": %d,', totalAccounts),
        sprintf('  "total_deposits": %s,', sprintf("%.2f", totalDeposits)),
        sprintf('  "total_withdrawals": %s,', sprintf("%.2f", totalWithdrawals)),
        sprintf('  "total_balance": %s,', sprintf("%.2f", totalBalance)),
        sprintf('  "total_interest_expense": %s', sprintf("%.2f", totalInterestExpense)),
        "}"
    )

    # Save the summary to a JSON file.
    writeLines(summary, "summary.json")

    cat(sprintf(
    '{" total_accounts ": %d, "total_deposits": %.2f, "total_withdrawals": %.2f, "total_balance": %.2f, "total_interest_expense": %.2f}\n',
    totalAccounts,
    totalDeposits,
    totalWithdrawals,
    totalBalance,
    totalInterestExpense
    ))
}

main()