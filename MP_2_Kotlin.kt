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

fun report1()
{
    println(divider)
    println(" ~~~ Report 1: Banking Privilege Tiers")
    println(divider)

    data class reportEntry (
        val name: String,
        val age: Long,
        val balance: Double,
        val ave: Double, //sum of all end day balances / total number of days in a month
        val tier: String
    )

    val lastDate = transactionList.last().date
    val year = lastDate.year
    val month = lastDate.monthValue
    val daysInMonth = lastDate.lengthOfMonth()

    for (transaction in transactionList)
    {
        if (transaction.date.year == year && transaction.date.monthValue == month)
        {
            ... //di ko gets lemme wait for yall
        }
    }

}

fun report2()
{
    ...
}

fun report3()
{
    ...
}

fun report4()
{
    ...
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
        return    // account not found, back to the main menu
    }
    val account = transactionList[index]

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
        break
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
            "5" -> recordRates()
            "6" -> showInterest()
            "7" -> generateReports()
            else -> println("Invalid choice. Please enter 0 to 7.")
        }

        if (!backToMainMenu())
        {
            println("Exiting the program. Goodbye!")
            running = false
        }
    }
}
