import java.io.*;
import java.time.*;
import java.time.format.DateTimeFormatter;
import java.util.*;


public class TransactionRecords{
    
    // Record that holds the clean row sa bank_transactions
    // no more currency cuz its already php anyways
    public record Transaction(
        String accountName, 
        long accountNumber, 
        LocalDate date, 
        LocalTime time, 
        double amount, 
        String type, // so either deposit or withdrawal
        double currentBalance
    ){}

    // Read the file and count the rows
    // Filter only PHP
    // Validate each rows
    // Data cleaning
    // Log totals
    // return a list of clean transaction records

    /**
     * This function is responsible for reading the CSV file, it does the following:
     * Keeps only the valid PHP rows and store in an arraylist of Transaction records
     * Rows are invalid if they have incomplete columns, empty values, or mismatched data types.
     * @return an ArrayList of clean/valid transaction rows, and empty if no file is found.
     */
    public static ArrayList<Transaction> loadTransactions(){

        // Creation of an arraylist of transaction records to store the cleaned/valid rows
        ArrayList<Transaction> cleanRows = new ArrayList<>();

        int totalRows = 0;
        int phpRows = 0;
        int invalidRows = 0;

        // To validate the date and time sa rows
        DateTimeFormatter dateFmt = DateTimeFormatter.ofPattern("dd/MM/yyyy");
        DateTimeFormatter timeFmt = DateTimeFormatter.ofPattern("H:mm:ss");

        // If file doesnt exist return empty arraylist
        File file = new File("bank_transactions.csv");
        if(!file.exists()){
            return cleanRows;
        }

        try {
            BufferedReader reader = new BufferedReader(new FileReader("bank_transactions.csv"));
            reader.readLine(); // skips the header muna

            String row;
            while((row = reader.readLine()) != null){ // continues to read until empty row
                // adds to the counter and divides data to be stored in columns
                // [0] name, [1] accNum, [2] date, [3] time, [4] currency, [5] amount, [6] type, [7] balance
                totalRows++;
                String[] columns = row.trim().split(",");

                // Checks muna if complete laman ng row
                if(columns.length != 8){
                    invalidRows++;
                    continue;
                }

                // skips if not php
                if(!columns[4].equals("PHP")){
                    continue;
                }
                phpRows++;

                // Skips if empty yung name and type since strings sila and baka di macatch yung error
                if(columns[0].isEmpty() || columns[6].isEmpty()){
                    invalidRows++;
                    continue;
                }

                // Attemps to separate the data and add to the new list of clean/valid data
                try {
                    String name = columns[0];
                    long accNum = Long.parseLong(columns[1]);
                    LocalDate date = LocalDate.parse(columns[2], dateFmt); // conversions ofc
                    LocalTime time = LocalTime.parse(columns[3], timeFmt);
                    double amount = Double.parseDouble(columns[5].replace(",",""));
                    String type = columns[6];
                    double balance = Double.parseDouble(columns[7].replace(",", ""));

                    //creates a new record of a row and adds to the list
                    cleanRows.add(new Transaction(name, accNum, date, time, amount, type, balance));
                } catch (Exception e) {
                    invalidRows++; // if theres an error like incomplete rows or nonconvertable data types, its automatically a bad row
                }
            }

            reader.close();

        } catch (IOException e) {
            System.out.println("Error: Cannot read file.");
        }

        System.out.println("Processing dataset... (" + String.format("%,d", totalRows)
                + " rows loaded, " + String.format("%,d", phpRows)
                + " filtered for PHP currency)");
        System.out.println("Bad rows removed: " + invalidRows);

        return cleanRows;
    }
}
