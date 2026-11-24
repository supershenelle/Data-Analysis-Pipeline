import java.time.*;
import java.util.*;


public class Reports{

    ArrayList<TransactionRecords.Transaction> transactions = TransactionRecords.loadTransactions();
    ArrayList<Account> accounts;

    public Reports( ArrayList<TransactionRecords.Transaction> transactions, ArrayList<Account> accounts){
        this.transactions = transactions;
        this.accounts = accounts;
    }

    // groups the data by account first
    // maybe a record where it contains the user's name,  number, and then a list of its transactions
    
    /**
     * This function returns the list of transactions of a SINGLE account
     */
    public ArrayList<TransactionRecords.Transaction> getUserTransactions(long accountNumber){
        ArrayList <TransactionRecords.Transaction> userTransactions = new ArrayList<>();

        // Goes through every row in original list of transactions 
        // adds to list if it belongs to the same user
        for(TransactionRecords.Transaction t: transactions){
            if (t.accountNumber() == accountNumber){
                userTransactions.add(t);
            }
        }

        return userTransactions;
    }

    /**
     * This function returns the latest date of transaction in the file
     */
    public LocalDate latestDate(){
        // incase of empty
        if(transactions.isEmpty()){
            return null;
        }

        // gets the date in the first row
        LocalDate latestDate = transactions.get(0).date();

        // then continues to compare once a later date is found
        // its organized in terms of date and time of transaction anyways
        for(TransactionRecords.Transaction t: transactions){
            if(t.date().isAfter(latestDate)){
                latestDate = t.date();
            }
        }

        return latestDate;
    }

    /**
     * This function computes the individual end of day balance of the user
     */
    public double computeEndOfDayBalance(ArrayList <TransactionRecords.Transaction> userTransactions, LocalDate day){
        double endOfDayBal = 0;

        // iterates thru ever row of the user's transaction history
        for(TransactionRecords.Transaction t: userTransactions){
            // this js checks over every row and updates the current balance
            // it stops once it reaches the day after
            if(!t.date().isAfter(day)){
                endOfDayBal = t.currentBalance();
            } else{
                break;
            }
        }

        return endOfDayBal;
    }




}