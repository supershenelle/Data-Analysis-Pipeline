/********************
Last names: Danieles, De Guzman, Nono, Suazon
Language: Java
Paradigm(s): Object-Oriented, Procedural
********************/


/**
 * Class represents the account of the user.
 * contains name, balance, and current currency (automatically PHP).
 */

import java.util.*;

public class Account{
    // Attributes
    private String accountName;
    private double balance;
    private Currency currency = Currency.PHP; // default currency is PHP
    private Interest interest = new Interest(); // handles daily/annual interest computation
    private long accountNumber;


    /**
     * Constructor of account class with the given name and starting balance of 0.
     */
    public Account(String accountName, long accountNumber, double startingBalance){
        this.accountName = accountName;
        this.accountNumber = accountNumber;
        this.balance = startingBalance;
    }

    /**
     * Sets the account holder's name.
     * @param accountName is the new name.
     */
    public void setAccountName(String accountName){
        this.accountName = accountName;
    }

    /**
     * Sets the account holder's balance. Used when loading accounts
     * @param balance is the newly updated balance
     */
    public void setBalance(Double balance){
        this.balance = balance;
    }

    /**
     * Returns the account holder's name.
     * @return accountName as a string.
     */
    public String getAccountName(){
        return accountName;
    }

    /**
     * Returns the account holder's account number.
     * @return accountName as a string.
     */
    public long getAccountNumber(){
        return accountNumber;
    }

    
    /**
     * Returns the current balance.
     * @return balance as a double.
     */
    public double getBalance(){
        return balance;
    }
    

    /**
     * Returns the account's currency.
     * @return the Currency enum value (always PHP by default).
     */
    public Currency getCurrency(){
        return currency;
    }

    /**
     * This function adds the given amount to balance.
     * @param depositAmt represents the amount to deposit
     */
    public boolean deposit(double depositAmt){
        if(depositAmt <=0)
            return false;

        this.balance += depositAmt;
        return true;
    }

    /**
     * This function subtracts the given amount from the balance.
     * @param drawAmt represents the amount to deduct.
     */
    public boolean withdraw(double drawAmt){
        if(drawAmt > this.balance)
            return false;

        this.balance -= drawAmt;
        return true;
    }

    /**
     * Computes the interest breakdown for the given day.
     * @param days represents the number of days to simulate.
     * @return a list of DailyEntry records in (day, interest, balance).
     */
    public List<Interest.DailyEntry> dailyInterest(int days){

        return interest.expectedDailyInterest(days, balance);

    }

    /**
     * Computes the annual interest on the current balance.
     * @return the annual interest amount as a double.
     */
    public double computeAnnualInterest(){
        return interest.annualInterest(balance);
    }



} 