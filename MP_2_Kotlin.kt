/**
 * MCO Milestone 1: Basic I/O Operations in Kotlin
 * 
 * Last names: Danieles, De Guzman, Nono, Suazon
 * Language: Kotlin
 * Paradigm(s): Procedural, Imperative
 */

import java.io.File
import java.time.LocalDate
import java.time.LocalTime
import java.time.format.DateTimeFormatter

data class transactions (
    val name: String,
    val number: String,
    val date: LocalDate,
    val time: LocalTime,
    val currency: String,
    val amount: Double,
    val type: String,
    val balance: Double
)

val currencyRates = arrayOfNulls<Double>(6)
val dateFormat = DateTimeFormatter.ofPattern("dd/MM/yyyy")
val timeFormat = DateTimeFormatter.ofPattern("H:mm:ss")
val divider = "_____________________________________________"
val transactionList = mutableListOf<transactions>() // create empty list na pwede maedit

fun readFile()
{
    val lines = File("bank_transactions.txt").readLines().drop(1) // lines[0]...

    var invalid = 0
    for (line in lines)
    {
        val parts = line.split(",")

        if (parts.size != 8)
        {
            invalid++
            continue
        }

        val amount = parts[5].trim().toDoubleOrNull()
        val balance = parts[7].trim().toDoubleOrNull()

        if (amount == null || balance == null)
        {
            invalid++
            continue
        }

        var date: LocalDate
        var time: LocalTime
        try
        {
            date = LocalDate.parse(parts[2].trim(), dateFormat)
            time = LocalTime.parse(parts[3].trim(), timeFormat)
        } catch (e: Exception)
        {
            invalid++
            continue
        }

        val t = transactions(
            name = parts[0].trim(),
            number = parts[1].trim(),
            date = date,
            time = time,
            currency = parts[4].trim(),
            amount = amount,
            type = parts[6].trim(),
            balance = balance
        )
        transactionList.add(t)
    }

    var phpCount = 0.0
    for (transaction in transactionList)
    {
        if (transaction.currency == "PHP")
        {
            phpCount += transaction.amount
        }
    }

    println(divider)
    println(" ~~~ CSV File Log Summary")
    println(divider)

    println("Rows read: ${lines.size}")
    println("Rows invalid: ${invalid}")
    println("Rows valid: ${transactionList.size}")
    println("Transactions in PHP: $phpCount")
}

fun register()
{
    while (true)
    {
        println(divider)
        println(" ~~~ Register Account")
        println(divider)

        while (true)
        {
            println("Enter Account Name (firstName lastName): ")
            val accountName = readLine().trim()
            if (accountName.isEmpty())
            {
                println("Account Name cannot be empty.")
                continue
            }

            if (!accountName.contains(" "))
            {
                println("Please enter both first and last name.")
                continue
            }

            name = accountName
            break
        }

        while (true)
        {
            println("Enter Account Number (10 digits): ")
            val accountNumber = readLine().trim()
            if (accountNumber.length != 10 || !accountNumber.all {it.isDigit()})
            {
                println("Account Number must be exactly 10 digits.")
                continue
            }

            number = accountNumber
            break
        }

        while (true)
        {
            println("Enter Initial Deposit Amount: ")
            val depositInput = readLine().trim()
            val depositAmount = depositInput.toDoubleOrNull()
            if (depositAmount == null || depositAmount < 5000)
            {
                println("Initial deposit must be a valid number and at least PHP 5000.")
                continue
            }

            deposit = depositAmount
            break
        }

        val date = LocalDate.now()
        val time = LocalTime.now().withNano(0) // remove nanoseconds for consistency
        val dateFormatted = date.format(dateFormat)
        val timeFormatted = time.format(timeFormat)

        val row = "$name,$number,$dateFormatted,$timeFormatted,PHP,$deposit,deposit,$deposit"
        File("bank_transactions.csv").appendText("$row\n")

        val t = transactions(name, number, date, time, "PHP", deposit, "deposit", deposit)
        transactionList.add(t)

        println("\n ~~~ Registered! Account number: $number")

        while (true)
        {
            print("Register another account? (Y/N): ")
            when (readLine().trim().lowercase())
            {
                "y" -> break
                "n" -> return
                else -> println("Please enter Y or N.")
            }
        }
    }
}

fun removeDuplicateAccountNumbers(): List<String>
{
    val accountNumbers = mutableListOf<String>()
    for (t in transactionList)
    {
        if (!accountNumbers.contains(t.number))
        {
            accountNumbers.add(t.number)
        }
    }

    return accountNumbers
}

fun report1()
{
    println(divider)
    println(" ~~~ Report 1: Banking Privilege Tiers")
    println(divider)

    val out = File("report1_banking_privilege_tiers.csv")
    out.writeText("account_name,account_age,current_balance,average_daily_balance,privilege_tier\n")

    data class reportEntry (
        val name: String,
        val age: Long,
        val balance: Double,
        val ave: Double, //sum of all end day balances / total number of days in a month
        val tier: String
    )

    val accountNumbers = removeDuplicateAccountNumbers()
    val date = LocalDate.now().minusMonths(1) //last month
    val year = date.year
    val month = date.monthValue
    val daysInMonth = date.lengthOfMonth()
    val entries = mutableListOf<reportEntry>()

    for (accountNumber in accountNumbers)
    {
        //default valuess before hanapin ung account number
        var name = ""
        var opened = monthEnd
        var currentBalance = 0.0
        var finalCurrentBalance = 0.0
        var tier = "None"

        for (t in transactionList)
        {
            if (t.number == accountNumber && t.currency == "PHP") //find in the list
            {
                name = t.name
                opened = t.date
                age = java.time.temporal.ChronoUnit.DAYS.between(opened, LocalDate.now())
                
                var totalBalance = 0.0
                for (day in 1..daysInMonth)
                {
                    val dayEnd = date.withDayOfMonth(day)
                    for (transaction in transactionList) // scan buong month history
                    {
                        if (transaction.number == accountNumber &&
                            transaction.currency == "PHP" &&
                            transaction.date.isBefore(dayEnd))
                        {
                            currentBalance = transaction.balance
                            finalCurrentBalance = currentBalance // update the balance for the day
                        }
                    }
                    totalBalance += currentBalance
                }
                val averageDailyBalance = totalBalance / daysInMonth

                if (averageDailyBalance >= 100000)
                {
                    when (averageDailyBalance)
                    {
                        in 100000.0..499999.99 -> tier = "Silver"
                        in 500000.0..999999.99 -> tier = "Gold"
                        else -> tier = "Platinum"
                    }

                    entries.add(reportEntry(name, age, finalCurrentBalance, averageDailyBalance, tier))
                }
            }
        }
    }

    val sortedEntries = entries.sortedByDescending { it.averageDailyBalance }
    for (entry in sortedEntries)
    {
        out.appendText("${entry.name},${entry.age},${"%,.2f".format(entry.balance)},${"%,.2f".format(entry.ave)},${entry.tier}\n")
    }

    for (i in 0 until minOf(2, sortedEntries.size)) //incase there are less than 2 entries
    {
        val e = sortedEntries[i]
        println(e.name + " | " + e.age + " | " + "%,.2f".format(e.balance) + " | " +
                "%,.2f".format(e.ave) + " | " + e.tier)
    }

    println("\n~~~ Full table exported to report1_banking_privilege_tiers.csv")
}

fun report2()
{
    println(divider)
    println(" ~~~ Report 2: Interest Expense Projection")
    println(divider)

    val out = File("report2_interest_expense_projection.csv")
    out.writeText("account_name,current_balance,expected_interest\n")

    data class reportEntry (
        val name: String,
        val balance: Double,
        val interest: Double    // expected interest after 30 days
    )

    val accountNumbers = removeDuplicateAccountNumbers()
    val entries = mutableListOf<reportEntry>()
    val totalInterest = 0.0

    val reversedList = transactionList.reversed() // para latest nlng kunin
    for (accountNumber in accountNumbers)
    {
        var name = ""
        var currentBalance = 0.0

        for (t in reversedList)
        {
            if (t.number == accountNumber && t.currency == "PHP")
            {
                name = t.name
                currentBalance = t.balance
                break
            }
        }

        // 2. keep only balances of 10,000 and above
        if (currentBalance >= 10000.0)
        {
            val dailyInterest = currentBalance * 0.05 / 365
            val expected = Math.round(dailyInterest * 30 * 100) / 100.0   // 30 days, 2 decimals

            entries.add(reportEntry(name, currentBalance, expected))
            totalInterest = totalInterest + expected
        }
    }

    val sortedEntries = entries.sortedByDescending {it.interest}

    for (entry in sortedEntries)
    {
        out.appendText("${entry.name},${"%,.2f".format(entry.balance)},${"%,.2f".format(entry.interest)}\n")
    }

    for (i in 0 until minOf(2, sortedEntries.size)) //incase there are less than 2 entries
    {
        val e = sortedEntries[i]
        println(e.name + " | " + "%,.2f".format(e.balance) + " | " + "%,.2f".format(e.interest))
    }

    println("\n~~~ Full table exported to report2_interest_expense_projection.csv")
}

fun report3()
{
    println(divider)
    println(" ~~~ Report 3: Suspicious Client Behavior")
    println(divider)

    val out = File("report3_suspicious_client_behavior.csv")
    out.writeText("account_name,current_balance,expected_interest\n")

    data class dayEntry (
        val name: String,
        val date: LocalDate,
        var totalWithdrawal: Double = 0.0,
        var numDeposits: Int = 0,
        var totalDeposit: Double = 0.0,
        var turnover: Double = 0.0,
        var flag: String = "Normal"
    )

    // one acc per day sa history so ccheck lahat ("1092000100" -> [ 01/12/2025 entry, 03/12/2025 entry, ... ])
    val accountTransactions = mutableMapOf<String, MutableList<dayEntry>>() // key: account number, value: list of dayEntry
    for (t in transactionList)
    {
        if (t.currency == "PHP")
        {
            var days = accountTransactions[t.number] // kunin ung values
            if (days == null)
            {
                days = mutableListOf()
                accountTransactions[t.number] = days //create new empty if wala pa
            }

            if (days.isEmpty() || days.last().date != t.date) // check if new day ba to
            {
                days.add(dayEntry(t.name, t.date)) // new day: add a fresh entry
            }
            val entry = days.last() //* */

            if (t.type == "withdrawal")
            {
                entry.totalWithdrawal += t.amount
            }
            else if (t.type == "deposit")
            {
                entry.numDeposits++
                entry.totalDeposit += t.amount
            }
        }
    }

    val entries = mutableListOf<dayEntry>()
    for (days in accountTransactions.values)
    {
        for (e in days)
        {
            if (e.totalDeposit > 0)
            {
                e.turnover = e.totalWithdrawal / e.totalDeposit * 100
            }

            if (e.totalWithdrawal >= 500000.0 || e.turnover >= 90.0)
            {
                e.flag = "Suspicious"
            }
            entries.add(e)
        }
    }

    // ung suspicious magiging 0 sa sorting, tapos ung normal magiging 1, so mauna ang suspicious
    val sortedEntries = entries.sortedWith(compareBy({ it.date }, { if (it.flag == "Suspicious") 0 else 1 }))

    for (entry in sortedEntries)
    {
        out.appendText("${entry.name},${entry.date.format(dateFormat)},${"%,.2f".format(entry.totalWithdrawal)}," +
                "${entry.numDeposits},${"%,.2f".format(entry.totalDeposit)},${"%,.2f".format(entry.turnover)}," +
                "${entry.flag}\n")
    }

    for (i in 0 until minOf(2, sortedEntries.size)) //incase there are less than 2 entries
    {
        val e = sortedEntries[i]
        println(e.name + " | " + e.date.format(dateFormat) + " | " + "%,.2f".format(e.totalWithdrawal) + " | " +
                e.numDeposits + " | " + "%,.2f".format(e.totalDeposit) + " | " +
                "%,.2f".format(e.turnover) + " | " + e.flag)
    }

    println("\n~~~ Full table exported to report3_suspicious_client_behavior.csv")
}

fun report4()
{
    println(divider)
    println(" ~~~ Report 4: Monthly Transaction Summary")
    println(divider)

    val out = File("report4_monthly_transaction_summary.csv")
    out.writeText("year,month,num_deposits,total_deposits,num_withdrawals,total_withdrawals,net_cash_flow\n")
    
    data class monthEntry (
        val year: Int,
        val month: Int,
        var numDeposits: Int = 0,
        var totalDeposits: Double = 0.0,
        var numWithdrawals: Int = 0,
        var totalWithdrawals: Double = 0.0
    )

    val months = mutableListOf<monthEntry>()

    // add every transaction to its month's entry parang ung sa report3, pero ung sa report4 is per month lang
    for (t in transactionList)
    {
        if (t.currency != "PHP")
        {
            continue
        }

        val year = t.date.year
        val month = t.date.monthValue

        // first transaction of a new month: start a new entry
        if (months.isEmpty() || months.last().year != year || months.last().month != month)
        {
            months.add(monthEntry(year, month))
        }

        // the current month's entry is always the last one in the list
        val entry = months.last()

        if (t.type == "deposit")
        {
            entry.numDeposits = entry.numDeposits + 1
            entry.totalDeposits = entry.totalDeposits + t.amount
        }
        else
        {
            entry.numWithdrawals = entry.numWithdrawals + 1
            entry.totalWithdrawals = entry.totalWithdrawals + t.amount
        }
    }
    
    for (entry in months)
    {
        out.appendText("${entry.year},${entry.month},${entry.numDeposits},${"%,.2f".format(entry.totalDeposits)}," +
                "${entry.numWithdrawals},${"%,.2f".format(entry.totalWithdrawals)}," +
                "${"%,.2f".format(entry.totalDeposits - entry.totalWithdrawals)}\n")
    }

    for (i in 0 until minOf(2, months.size)) //incase there are less than 2 entries
    {
        val e = months[i]
        println(e.year + " | " + e.month + " | " + e.numDeposits + " | " +
                "%,.2f".format(e.totalDeposits) + " | " + e.numWithdrawals + " | " +
                "%,.2f".format(e.totalWithdrawals) + " | " +
                "%,.2f".format(e.totalDeposits - e.totalWithdrawals))
    }
    
    println("\n~~~ Full table exported to report4_monthly_transaction_summary.csv")
}

fun findAccount(accountNumber: String): Int
{
    for (i in transactionList.indices.reversed())
    {
        if (transactionList[i].number == accountNumber)
        {
            return i
        }
    }
    return -1
}

fun askAccount(): Int
{
    while (true)
    {
        println("Enter Account Number: ")
        val accountNumber = readLine().trim()
        val index = findAccount(accountNumber)

        if (accountNumber.length != 10 || !accountNumber.all {it.isDigit()})
        {
            println("Account Number must be exactly 10 digits.")
            continue
        }

        if (index == -1)
        {
            println("Account not found.")
            continue
        }
        
        println("Account Name: ${transactionList[index].name}")
        return index
    }
}

fun deposit()
{
    println(divider)
    println(" ~~~ Deposit Amount")
    println(divider)

    val index = askAccount()
    if (index == -1)
    {
        return    // account not found, back to the main menu
    }
    var account = transactionList[index]

    while (true)
    {
        println("Account Name: ${account.name}")
        println("Current Balance: PHP ${"%,.2f".format(account.balance)}")

        while (true)
        {
            print("Enter deposit amount: ")
            val amount = readLine().trim().toDoubleOrNull()
            if (amount == null || amount <= 0)
            {
                println("Enter a valid amount greater than 0.")
                continue
            }

            val newBalance = addTransaction(account, amount, "deposit")
            println("Deposit successful. New balance: PHP ${"%,.2f".format(newBalance)}")
            account = transactionList[findAccount(account.number)]
            break
        }

        while (true)
        {
            print("Deposit another amount (Y/N): ")
            when (readLine().trim().lowercase())
            {
                "y" -> break //exit this loop lang
                "n" -> return //exit entire function
                else -> println("Please enter Y or N.")
            }
        }
    }
}

fun withdraw()
{
    println(divider)
    println(" ~~~ Withdraw Amount")
    println(divider)

    val index = askAccount()
    if (index == -1)
    {
        return
    }
    var account = transactionList[index]

    while (true)
    {
        println("Account Name: ${account.name}")
        println("Current Balance: PHP ${"%,.2f".format(account.balance)}")

        while (true)
        {
            print("Enter withdrawal amount: ")
            val amount = readLine().trim().toDoubleOrNull()
            if (amount == null || amount <= 0)
            {
                println("Enter a valid amount greater than 0.")
                continue
            }

            if (amount > account.balance)
            {
                println("Insufficient balance. Enter a smaller amount.")
                continue
            }

            val newBalance = addTransaction(account, amount, "withdrawal")
            println("Withdrawal successful. New balance: PHP ${"%,.2f".format(newBalance)}")
            account = transactionList[findAccount(account.number)]
            break
        }

        while (true)
        {
            print("Withdraw another amount (Y/N): ")
            when (readLine().trim().lowercase())
            {
                "y" -> break //exit this loop lang
                "n" -> return
                else -> println("Please enter Y or N.")
            }
        }
    }
}

fun addTransaction(account: transactions, amount: Double, type: String): Double
{
    val date = LocalDate.now()
    val time = LocalTime.now().withNano(0) // remove nanoseconds for consistency
    val dateFormatted = date.format(dateFormat)
    val timeFormatted = time.format(timeFormat)

    var newBalance = account.balance
    if (type == "deposit")
    {
        newBalance = newBalance + amount
    }
    else
    {
        newBalance = newBalance - amount
    }

    newBalance = String.format("%.2f", newBalance).toDouble() // round to 2 decimal places
    val row = "${account.name},${account.number},$dateFormatted,$timeFormatted,${account.currency},$amount,$type,$newBalance"
    val file = File("bank_transactions.csv")
    if (!file.readText().endsWith("\n"))
    {
        file.appendText("\n")
    }
    file.appendText("$row\n")

    val t = transactions(account.name, account.number, date, time, account.currency, amount, type, newBalance)
    transactionList.add(t)

    return newBalance
}

fun currencyExchange()
{
    val currencyNames = arrayOf(
        "Philippine Peso (PHP)",
        "United States Dollar (USD)",
        "Japanese Yen (JPY)",
        "British Pound Sterling (GBP)",
        "Euro (EUR)",
        "Chinese Yuan Renminbi (CNY)"
    )

    while (true) // para magrestart loop kung may error sa input
    {
        println(divider)
        println(" ~~~ Currency Exchange")
        println(divider)
        
        println("Source Currency Option:")
        println("[1] Philippine Peso (PHP)")
        println("[2] United States Dollar (USD)")
        println("[3] Japanese Yen (JPY)")
        println("[4] British Pound Sterling (GBP)")
        println("[5] Euro (EUR)")
        println("[6] Chinese Yuan Renminbi (CNY)")

        print("\nSource Currency: ")
        val sourceCurrency = readln().toInt()
        if (sourceCurrency !in 1..6)
        {
            println("Error: Invalid source currency.")
            continue
        }

        print("Source Amount: ")
        val sourceAmount = readln().toDouble()

        println("\nExchanged Currency Options:")
        println("[1] Philippine Peso (PHP)")
        println("[2] United States Dollar (USD)")
        println("[3] Japanese Yen (JPY)")
        println("[4] British Pound Sterling (GBP)")
        println("[5] Euro (EUR)")
        println("[6] Chinese Yuan Renminbi (CNY)")

        print("\nExchange Currency: ")
        val exchangeCurrency = readln().toInt()
        if (exchangeCurrency !in 1..6)
        {
            println("Error: Invalid exchange currency.")
            continue
        }

        // formulas and values para sa exchange
        val sourceName = currencyNames[sourceCurrency - 1]
        val exchangeName = currencyNames[exchangeCurrency - 1]
        val sourceRate = currencyRates[sourceCurrency - 1]
        val exchangeRate = currencyRates[exchangeCurrency - 1]

        if (sourceRate == null)
            println("Error: No rate set for ${currencyNames[sourceCurrency - 1]}")

        if (exchangeRate == null)
            println("Error: No rate set for ${currencyNames[exchangeCurrency - 1]}")
        

        if (sourceRate == null || exchangeRate == null)
        {
            print("\nConvert another currency (Y/N)? ")
            
            if (readln().trim().lowercase() != "y")
                return // stop function

            else
                continue // start again from the top of the loop
        }

        val exchangeAmount = sourceAmount * sourceRate / exchangeRate // main output
        println("Exchange Amount: ${String.format("%.2f", exchangeAmount)}")

        print("\nConvert another currency (Y/N)? ")
        if (readln().trim().lowercase() != "y")
        {
            return //stop function
        }
    }
}

fun recordExchange()
{
    println(divider)
    println(" ~~~ Record Exchange Rates")
    println(divider)

    println("[1] Philippine Peso (PHP)")
    println("[2] United States Dollar (USD)")
    println("[3] Japanese Yen (JPY)")
    println("[4] British Pound Sterling (GBP)")
    println("[5] Euro (EUR)")
    println("[6] Chinese Yuan Renminni (CNY)\n")

    print("Select Currency: ")
    val choice = readln().trim().toIntOrNull()

    if (choice == null || choice !in 1..6)
    {
        println("Error: Invalid choice.")
        return
    }

    while (true)
    {
        print("Exchange Rate: ")
        val rate = readln().trim().toDoubleOrNull()

        if (rate == null)
        {
            println("Error: Exchange rate must be numerical.")
            continue
        }

        if (!rate.isFinite() || rate <= 0)
        {
            println("Error: Exchange rate must be greater than zero.")
            continue
        }

        currencyRates[choice - 1] = rate // set the new exchange rate
        println("Exchange rate recorded successfully.")
        return
    }
}

fun showInterest()
{
    println(divider)
    println(" ~~~ Show Interest Amount")
    println(divider)

    // ask for the account number and check that it exists in the CSV
    val index = askAccount()
    if (index == -1)
    {
        return    // not found, back to the main menu
    }
    val account = transactionList[index]

    println("Account Name: ${account.name}")
    println("Current Balance: ${"%,.2f".format(account.balance)}")
    println("Currency: PHP")
    println("Interest Rate: 5%")

    // ask for the number of days until it's a valid whole number above 0
    val days: Int
    while (true)
    {
        print("Total Number of Days: ")
        val input = readln().trim().toIntOrNull()

        if (input == null || input <= 0)
        {
            println("Error: Number of days must be a whole number greater than zero.")
            continue
        }
        days = input
        break
    }

    var newBalance = account.balance
    println("\n| Day |     Interest |          Balance |")
    for (i in 1..days)
    {
        val interest = account.balance * 0.05 / 365
        newBalance += interest

        println("| ${"%3d".format(i)} | ${"%,12.2f".format(interest)} | ${"%,16.2f".format(newBalance)} |")
    }
}

fun backToMainMenu(): Boolean
{
    while (true)
    {
        print("\nBack to the Main Menu (Y/N): ")
        when (readln().trim().lowercase())
        {
            "y" -> return true
            "n" -> return false
            else -> println("Error: Please enter Y or N.")
        }
    }
}

fun main()
{
    readFile()    // load the CSV once at the start

    var running = true
    while (running)
    {
        println(divider)
        println(" Main Menu")
        println(divider)

        println("Select Transaction:")
        println("[1] Register Account")
        println("[2] Deposit Amount")
        println("[3] Withdraw Amount")
        println("[4] Currency Exchange")
        println("[5] Record Exchange Rates")
        println("[6] Show Interest Amount")
        println("[7] Generate Reports")
        println("[0] Exit")
        print("Enter choice: ")

        val choice = readLine()!!.trim()

        when (choice)
        {
            "1" -> register()
            "2" -> deposit()
            "3" -> withdraw()
            "4" -> currencyExchange()
            "5" -> recordExchange()
            "6" -> showInterest()
            "7" -> generateReports()
            else -> println("Invalid choice. Please enter 1 to 7.")
        }

        if (!backToMainMenu())
        {
            println("Exiting the program. Goodbye!")
            running = false
        }
    }
}
