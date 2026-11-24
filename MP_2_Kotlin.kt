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

val fileName = "bank_transactions.csv"
val dateFormat = DateTimeFormatter.ofPattern("dd/MM/yyyy")
val timeFormat = DateTimeFormatter.ofPattern("H:mm:ss")
val divider = "_____________________________________________"
val transactionList = mutableListOf<transactions>()

// comma-formatted, 2 decimals, in quotes so the commas don't split the CSV columns
fun formatCsvNumber(value: Double): String
{
    return "\"${"%,.2f".format(value)}\""
}

// adds one line to the end of the transactions CSV
fun appendRow(row: String)
{
    val file = File(fileName)
    if (!file.readText().endsWith("\n"))
    {
        file.appendText("\n")
    }
    file.appendText("$row\n")
}

fun readFile()
{
    val lines = File(fileName).readLines().drop(1)

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
            type = parts[6].trim().lowercase(),
            balance = balance
        )
        transactionList.add(t)
    }

    var phpCount = 0
    for (transaction in transactionList)
    {
        if (transaction.currency == "PHP")
        {
            phpCount++
        }
    }

    println(divider)
    println(" ~~~ CSV File Log Summary")
    println(divider)

    println("Rows read: ${lines.size}")
    println("Rows invalid: $invalid")
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

        var name = ""
        var number = ""
        var deposit = 0.0

        while (true)
        {
            print("Enter Account Name (firstName lastName): ")
            val accountName = readLine()!!.trim()
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
            print("Enter Account Number (10 digits): ")
            val accountNumber = readLine()!!.trim()
            if (accountNumber.length != 10 || !accountNumber.all { it.isDigit() })
            {
                println("Account Number must be exactly 10 digits.")
                continue
            }

            if (findAccount(accountNumber) != -1)
            {
                println("Account Number already exists.")
                continue
            }

            number = accountNumber
            break
        }

        while (true)
        {
            print("Enter Initial Deposit Amount: ")
            val depositInput = readLine()!!.trim()
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
        val time = LocalTime.now().withNano(0)
        val dateFormatted = date.format(dateFormat)
        val timeFormatted = time.format(timeFormat)

        val depositText = "%.2f".format(deposit)
        val row = "$name,$number,$dateFormatted,$timeFormatted,PHP,$depositText,deposit,$depositText"
        appendRow(row)

        val t = transactions(name, number, date, time, "PHP", deposit, "deposit", deposit)
        transactionList.add(t)

        println("\n ~~~ Registered! Account number: $number")

        var another = true
        while (true)
        {
            print("Register another account? (Y/N): ")
            val answer = readLine()!!.trim().lowercase()
            if (answer == "y")
            {
                break
            }
            if (answer == "n")
            {
                another = false
                break
            }
            println("Please enter Y or N.")
        }

        if (!another)
        {
            return
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

    data class reportEntry (
        val name: String,
        val age: Long,
        val balance: Double,
        val ave: Double,
        val tier: String
    )

    val accountNumbers = removeDuplicateAccountNumbers()
    val lastDate = transactionList.last().date
    val year = lastDate.year
    val month = lastDate.monthValue
    val daysInMonth = lastDate.lengthOfMonth()
    val monthEnd = lastDate.withDayOfMonth(daysInMonth)
    val entries = mutableListOf<reportEntry>()

    for (accountNumber in accountNumbers)
    {
        var name = ""
        var opened = monthEnd
        var startBalance = 0.0
        var currentBalance = 0.0

        val dayBalance = DoubleArray(daysInMonth + 1)
        dayBalance.fill(-1.0)

        for (t in transactionList)
        {
            if (t.number == accountNumber && t.currency == "PHP")
            {
                if (name == "")
                {
                    name = t.name
                    opened = t.date
                }

                if (t.date.year == year && t.date.monthValue == month)
                {
                    dayBalance[t.date.dayOfMonth] = t.balance
                }
                else
                {
                    startBalance = t.balance
                }

                currentBalance = t.balance
            }
        }

        var carry = startBalance
        var sum = 0.0
        for (day in 1..daysInMonth)
        {
            if (dayBalance[day] != -1.0)
            {
                carry = dayBalance[day]
            }
            sum = sum + carry
        }

        val ave = sum / daysInMonth
        val age = monthEnd.toEpochDay() - opened.toEpochDay()

        if (ave >= 100000.0)
        {
            var tier = "Silver"
            if (ave >= 1000000.0)
            {
                tier = "Platinum"
            }
            else if (ave >= 500000.0)
            {
                tier = "Gold"
            }
            entries.add(reportEntry(name, age, currentBalance, ave, tier))
        }
    }

    val sortedEntries = entries.sortedByDescending { it.ave }

    val out = File("report1_banking_privilege_tiers.csv")
    out.writeText("AccountName,AccountAge,CurrentBalance,AverageDailyBalance,PrivilegeTier\n")
    for (e in sortedEntries)
    {
        out.appendText(e.name + "," + e.age + "," + formatCsvNumber(e.balance) + "," +
                       formatCsvNumber(e.ave) + "," + e.tier + "\n")
    }

    for (i in 0 until minOf(2, sortedEntries.size))
    {
        val e = sortedEntries[i]
        println(e.name + " | " + e.age + " | " + "%,.2f".format(e.balance) + " | " +
                "%,.2f".format(e.ave) + " | " + e.tier)
    }
    println("(Full table exported to report1_banking_privilege_tiers.csv)")
}

fun report2()
{
    println(divider)
    println(" ~~~ Report 2: Interest Expense Projection")
    println(divider)

    data class interestEntry (
        val name: String,
        val balance: Double,
        val interest: Double
    )

    val accountNumbers = removeDuplicateAccountNumbers()
    val reversedList = transactionList.reversed()
    val entries = mutableListOf<interestEntry>()

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

        if (currentBalance >= 10000.0)
        {
            val dailyInterest = currentBalance * 0.05 / 365
            val expected = Math.round(dailyInterest * 30 * 100) / 100.0
            entries.add(interestEntry(name, currentBalance, expected))
        }
    }

    val sortedEntries = entries.sortedByDescending { it.interest }

    val out = File("report2_interest_expense_projection.csv")
    out.writeText("AccountName,CurrentBalance,ExpectedInterest\n")
    for (e in sortedEntries)
    {
        out.appendText(e.name + "," + formatCsvNumber(e.balance) + "," + formatCsvNumber(e.interest) + "\n")
    }

    for (i in 0 until minOf(2, sortedEntries.size))
    {
        val e = sortedEntries[i]
        println(e.name + " | " + "%,.2f".format(e.balance) + " | " + "%,.2f".format(e.interest))
    }
    println("(Full table exported to report2_interest_expense_projection.csv)")
}

fun report3()
{
    println(divider)
    println(" ~~~ Report 3: Suspicious Client Behavior")
    println(divider)

    data class dayEntry (
        val name: String,
        val date: LocalDate,
        var totalWithdrawal: Double = 0.0,
        var numDeposits: Int = 0,
        var totalDeposit: Double = 0.0,
        var turnover: Double = 0.0,
        var flag: String = "Normal"
    )

    val accountTransactions = mutableMapOf<String, MutableList<dayEntry>>()
    for (t in transactionList)
    {
        if (t.currency == "PHP")
        {
            var days = accountTransactions[t.number]
            if (days == null)
            {
                days = mutableListOf()
                accountTransactions[t.number] = days
            }

            if (days.isEmpty() || !days.last().date.isEqual(t.date))
            {
                days.add(dayEntry(t.name, t.date))
            }
            val entry = days.last()

            if (t.type == "withdrawal")
            {
                entry.totalWithdrawal = entry.totalWithdrawal + t.amount
            }
            else
            {
                entry.numDeposits = entry.numDeposits + 1
                entry.totalDeposit = entry.totalDeposit + t.amount
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

    val sortedEntries = entries.sortedWith(
        compareBy({ it.date }, { if (it.flag == "Suspicious") 0 else 1 }, { it.name })
    )

    val out = File("report3_suspicious_client_behavior.csv")
    out.writeText("AccountName,Date,TotalWithdrawalAmounts,NumberOfDeposits,TotalDepositAmounts,TurnoverVelocity,RiskFlagLevel\n")
    for (e in sortedEntries)
    {
        out.appendText(e.name + "," + e.date.format(dateFormat) + "," + formatCsvNumber(e.totalWithdrawal) + "," +
                       e.numDeposits + "," + formatCsvNumber(e.totalDeposit) + "," + formatCsvNumber(e.turnover) + "," + e.flag + "\n")
    }

    for (i in 0 until minOf(2, sortedEntries.size))
    {
        val e = sortedEntries[i]
        println(e.name + " | " + e.date.format(dateFormat) + " | " + "%,.2f".format(e.totalWithdrawal) + " | " +
                e.numDeposits + " | " + "%,.2f".format(e.totalDeposit) + " | " +
                "%,.2f".format(e.turnover) + "% | " + e.flag)
    }
    println("(Full table exported to report3_suspicious_client_behavior.csv)")
}

fun report4()
{
    println(divider)
    println(" ~~~ Report 4: Monthly Transaction Summary")
    println(divider)

    data class monthEntry (
        val year: Int,
        val month: Int,
        var numDeposits: Int = 0,
        var totalDeposits: Double = 0.0,
        var numWithdrawals: Int = 0,
        var totalWithdrawals: Double = 0.0
    )

    val months = mutableListOf<monthEntry>()

    for (t in transactionList)
    {
        if (t.currency != "PHP")
        {
            continue
        }

        val year = t.date.year
        val month = t.date.monthValue

        if (months.isEmpty() || months.last().year != year || months.last().month != month)
        {
            months.add(monthEntry(year, month))
        }

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

    val out = File("report4_monthly_transaction_summary.csv")
    out.writeText("Month,NumberOfDeposits,TotalDepositAmounts,NumberOfWithdrawals,TotalWithdrawalAmounts,NetCashFlow\n")
    for (m in months)
    {
        val label = "%04d-%02d".format(m.year, m.month)
        val net = Math.round((m.totalDeposits - m.totalWithdrawals) * 100) / 100.0

        out.appendText(label + "," + m.numDeposits + "," + formatCsvNumber(m.totalDeposits) + "," +
                       m.numWithdrawals + "," + formatCsvNumber(m.totalWithdrawals) + "," + formatCsvNumber(net) + "\n")
    }

    for (i in 0 until minOf(2, months.size))
    {
        val m = months[i]
        val label = "%04d-%02d".format(m.year, m.month)
        val net = Math.round((m.totalDeposits - m.totalWithdrawals) * 100) / 100.0

        println(label + " | " + m.numDeposits + " | " + "%,.2f".format(m.totalDeposits) + " | " +
                m.numWithdrawals + " | " + "%,.2f".format(m.totalWithdrawals) + " | " + "%,.2f".format(net))
    }
    println("(Full table exported to report4_monthly_transaction_summary.csv)")
}

fun summaryJson()
{
    var totalDeposits = 0.0
    var totalWithdrawals = 0.0
    val latestBalance = mutableMapOf<String, Double>()

    for (t in transactionList)
    {
        if (t.currency != "PHP")
        {
            continue
        }

        if (t.type == "deposit")
        {
            totalDeposits = totalDeposits + t.amount
        }
        else
        {
            totalWithdrawals = totalWithdrawals + t.amount
        }

        latestBalance[t.number] = t.balance
    }

    val totalAccounts = latestBalance.size
    var totalBalance = 0.0
    var totalInterest = 0.0

    for (balance in latestBalance.values)
    {
        totalBalance = totalBalance + balance

        if (balance >= 10000.0)
        {
            val interest = balance * 0.05 / 365 * 30
            totalInterest = totalInterest + Math.round(interest * 100) / 100.0
        }
    }

    val json = """
    {
      "total_accounts": $totalAccounts,
      "total_deposits": ${"%.2f".format(totalDeposits)},
      "total_withdrawals": ${"%.2f".format(totalWithdrawals)},
      "total_balance": ${"%.2f".format(totalBalance)},
      "total_interest_expense": ${"%.2f".format(totalInterest)}
    }
    """.trimIndent()

    File("summary.json").writeText(json + "\n")

    println("Summary State (summary.json):")
    println(json)
}

fun generateReports()
{
    println(divider)
    println(" ~~~ Generate Reports")
    println(divider)

    report1()
    report2()
    report3()
    report4()
    summaryJson()
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
        print("Enter Account Number: ")
        val accountNumber = readLine()!!.trim()

        if (accountNumber.length != 10 || !accountNumber.all { it.isDigit() })
        {
            println("Account Number must be exactly 10 digits.")
            continue
        }

        val index = findAccount(accountNumber)
        if (index == -1)
        {
            println("Account not found.")
            continue
        }
        return index
    }
}

fun deposit()
{
    println(divider)
    println(" ~~~ Deposit")
    println(divider)

    val index = askAccount()
    if (index == -1)
    {
        return
    }
    val account = transactionList[index]

    println("Account Name: ${account.name}")
    println("Current Balance: PHP ${"%,.2f".format(account.balance)}")

    while (true)
    {
        print("Enter deposit amount: ")
        val amount = readLine()!!.trim().toDoubleOrNull()
        if (amount == null || amount <= 0)
        {
            println("Enter a valid amount greater than 0.")
            continue
        }

        val newBalance = addTransaction(account, amount, "deposit")
        println("Deposit successful. New balance: PHP ${"%,.2f".format(newBalance)}")
        break
    }
}

fun withdraw()
{
    println(divider)
    println(" ~~~ Withdraw")
    println(divider)

    val index = askAccount()
    if (index == -1)
    {
        return
    }
    val account = transactionList[index]

    println("Account Name: ${account.name}")
    println("Current Balance: PHP ${"%,.2f".format(account.balance)}")

    while (true)
    {
        print("Enter withdrawal amount: ")
        val amount = readLine()!!.trim().toDoubleOrNull()
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
        break
    }
}

fun addTransaction(account: transactions, amount: Double, type: String): Double
{
    val date = LocalDate.now()
    val time = LocalTime.now().withNano(0)
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
    newBalance = Math.round(newBalance * 100) / 100.0

    val row = "${account.name},${account.number},$dateFormatted,$timeFormatted,${account.currency}," +
              "${"%.2f".format(amount)},$type,${"%.2f".format(newBalance)}"
    appendRow(row)

    val t = transactions(account.name, account.number, date, time, account.currency, amount, type, newBalance)
    transactionList.add(t)

    return newBalance
}

fun currencyExchange()
{
    println("[Currency Exchange] not added yet")
}

fun recordRates()
{
    println("[Record Exchange Rates] not added yet")
}

fun showInterest()
{
    println("[Show Interest Amount] not added yet")
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
    readFile()

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
            "5" -> recordRates()
            "6" -> showInterest()
            "7" -> generateReports()
            "0" -> running = false
            else -> println("Invalid choice. Please enter 0 to 7.")
        }

        if (running && !backToMainMenu())
        {
            running = false
        }
    }
    println("Exiting the program. Goodbye!")
}