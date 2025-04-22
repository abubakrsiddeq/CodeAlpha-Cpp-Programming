#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <conio.h>
#include <stdexcept>
#include <limits>  // For numeric_limits

using namespace std;

#define RESET   "\033[0m"
#define LIGHT_GREEN   "\033[92m"
#define LIGHT_RED     "\033[91m"
#define LIGHT_BLUE    "\033[94m"
#define LIGHT_YELLOW  "\033[93m"

class Customer {
public:
    string name;
    string id;
    string password;
    bool isLoggedIn = false;

    Customer(string name, string id, string password) {
        this->name = name;
        this->id = id;
        this->password = password;
    }

    void logIn() {
        this->isLoggedIn = true;
    }

    void logOut() {
        this->isLoggedIn = false;
    }

    string toString() const {
        return id + "," + name + "," + password + "\n";
    }
};

class Account {
private:
    string accountNumber;
    double balance;

public:
    Account(string accountNumber) {
        this->accountNumber = accountNumber;
        this->balance = 0.0;
    }

    string getAccountNumber() const {
        return accountNumber;
    }

    double getBalance() const {
        return balance;
    }

    void deposit(double amount) {
        if (amount < 0) throw invalid_argument("Deposit amount cannot be negative.");
        balance += amount;
    }

    bool withdraw(double amount) {
        if (amount < 0) throw invalid_argument("Withdrawal amount cannot be negative.");
        if (amount <= balance) {
            balance -= amount;
            return true;
        }
        throw runtime_error("Insufficient funds.");
    }

    void transfer(Account& target, double amount) {
        if (withdraw(amount)) {
            target.deposit(amount);
        }
    }

    string toString() const {
        stringstream ss;
        ss << fixed << setprecision(2) << balance;
        return accountNumber + "," + ss.str() + "\n";
    }
};

class Transaction {
private:
    string transactionId;
    string fromAccount;
    string toAccount;
    double amount;
    string date;

public:
    Transaction(string transactionId, string fromAccount, string toAccount, double amount, string date)
        : transactionId(transactionId), fromAccount(fromAccount), toAccount(toAccount), amount(amount), date(date) {}

    Transaction(string fromAccount, string toAccount, double amount) {
        this->transactionId = generateTransactionId();
        this->fromAccount = fromAccount;
        this->toAccount = toAccount;
        this->amount = amount;
        this->date = getCurrentDate();
    }

    string generateTransactionId() {
        return to_string(rand() % 1000000 + 1);
    }

    string getCurrentDate() {
        time_t t = time(0);
        struct tm now;
        localtime_s(&now, &t);
        stringstream ss;
        ss << now.tm_mday << "-" << now.tm_mon + 1 << "-" << now.tm_year + 1900;
        return ss.str();
    }

    string toString() const {
        stringstream ss;
        ss << fixed << setprecision(2) << amount;
        return transactionId + "," + fromAccount + "," + toAccount + "," + ss.str() + "," + date + "\n";
    }
};

class BankingSystem {
private:
    vector<Customer> customers;
    vector<Account> accounts;
    vector<Transaction> transactions;
    Customer* currentCustomer = nullptr;

public:
    BankingSystem() {
        try {
            loadCustomers();
            loadAccounts();
            loadTransactions();
        }
        catch (const exception& e) {
            cout << LIGHT_RED << "Error loading data: " << e.what() << RESET << endl;
        }
    }

    void loadCustomers() {
        ifstream file("customers.txt");
        if (!file.is_open()) {
            throw runtime_error("Unable to open 'customers.txt' file. Please make sure the file exists and is accessible.");
        }
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string name, id, password;
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, password, ',');
            customers.push_back(Customer(name, id, password));
        }
        file.close();
    }

    void loadAccounts() {
        ifstream file("accounts.txt");
        if (!file.is_open()) {
            throw runtime_error("Unable to open accounts file.");
        }
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string accountNumber, balanceStr;
            getline(ss, accountNumber, ',');
            getline(ss, balanceStr, ',');
            Account acc(accountNumber);
            acc.deposit(stod(balanceStr));  // Could throw exception if the balance is invalid
            accounts.push_back(acc);
        }
        file.close();
    }

    void loadTransactions() {
        ifstream file("transactions.txt");
        if (!file.is_open()) {
            throw runtime_error("Unable to open transactions file.");
        }
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string transactionId, fromAccount, toAccount, amountStr, date;
            getline(ss, transactionId, ',');
            getline(ss, fromAccount, ',');
            getline(ss, toAccount, ',');
            getline(ss, amountStr, ',');
            getline(ss, date, ',');
            Transaction transaction(transactionId, fromAccount, toAccount, stod(amountStr), date);
            transactions.push_back(transaction);
        }
        file.close();
    }

    void saveCustomer(const Customer& customer) {
        try {
            ofstream file("customers.txt", ios::app);
            if (!file.is_open()) {
                throw runtime_error("Unable to open customers file for saving.");
            }
            file << customer.toString();
            file.close();
        }
        catch (const exception& e) {
            cout << LIGHT_RED << "Error saving customer: " << e.what() << RESET << endl;
        }
    }

    void saveAccount(const Account& account) {
        try {
            ofstream file("accounts.txt", ios::app);
            if (!file.is_open()) {
                throw runtime_error("Unable to open accounts file for saving.");
            }
            file << account.toString();
            file.close();
        }
        catch (const exception& e) {
            cout << LIGHT_RED << "Error saving account: " << e.what() << RESET << endl;
        }
    }

    void saveTransaction(const Transaction& transaction) {
        try {
            ofstream file("transactions.txt", ios::app);
            if (!file.is_open()) {
                throw runtime_error("Unable to open transactions file for saving.");
            }
            file << transaction.toString();
            file.close();
        }
        catch (const exception& e) {
            cout << LIGHT_RED << "Error saving transaction: " << e.what() << RESET << endl;
        }
    }

    Customer* findCustomerById(const string& id) {
        auto it = find_if(customers.begin(), customers.end(), [&](const Customer& customer) {
            return customer.id == id;
            });
        return (it != customers.end()) ? &(*it) : nullptr;
    }

    Account* findAccountByAccountNumber(const string& accountNumber) {
        auto it = find_if(accounts.begin(), accounts.end(), [&](const Account& account) {
            return account.getAccountNumber() == accountNumber;
            });
        return (it != accounts.end()) ? &(*it) : nullptr;
    }

    string generateCustomerId() {
        stringstream ss;
        ss << "CUST" << setw(3) << setfill('0') << customers.size() + 1;
        return ss.str();
    }

    void registerCustomer() {
        try {
            string name, password;
            cout << LIGHT_BLUE << "Please enter your name: " << RESET;
            getline(cin, name);
            cout << LIGHT_BLUE << "Please enter your password: " << RESET;

            char ch;
            password = "";
            while ((ch = _getch()) != '\r') {
                if (ch == '\b' && !password.empty()) {
                    password.pop_back();
                    cout << "\b \b";
                }
                else if (ch != '\b') {
                    password.push_back(ch);
                    cout << '*';
                }
            }
            cout << endl;

            for (auto& c : customers) {
                if (c.name == name) {
                    cout << LIGHT_RED << "A user with this name already exists! Please choose a different username." << RESET << endl;
                    return;
                }
            }

            string id = generateCustomerId();
            Customer newCustomer(name, id, password);
            customers.push_back(newCustomer);
            saveCustomer(newCustomer);

            Account newAccount(id);
            accounts.push_back(newAccount);
            saveAccount(newAccount);

            cout << LIGHT_GREEN << "Registration successful! Your ID is: " << id << RESET << endl;
        }
        catch (const exception& e) {
            cout << LIGHT_RED << "Error during registration: " << e.what() << RESET << endl;
        }
    }

    void loginCustomer() {
        try {
            string id, password;
            cout << LIGHT_BLUE << "Please enter your ID: " << RESET;
            getline(cin, id);
            cout << LIGHT_BLUE << "Please enter your password: " << RESET;

            char ch;
            password = "";
            while ((ch = _getch()) != '\r') {
                if (ch == '\b' && !password.empty()) {
                    password.pop_back();
                    cout << "\b \b";
                }
                else if (ch != '\b') {
                    password.push_back(ch);
                    cout << '*';
                }
            }
            cout << endl;

            Customer* customer = findCustomerById(id);
            if (customer && customer->password == password) {
                customer->logIn();
                currentCustomer = customer;
                cout << LIGHT_GREEN << "Login successful!" << RESET << endl;
                customerDashboard();
            }
            else {
                cout << LIGHT_RED << "Invalid credentials!" << RESET << endl;
            }
        }
        catch (const exception& e) {
            cout << LIGHT_RED << "Error during login: " << e.what() << RESET << endl;
        }
    }

    void customerDashboard() {
        try {
            int choice = 0;
            while (choice != 6) {
                cout << "\n===== Customer Dashboard =====" << endl;
                cout << "1. View Balance" << endl;
                cout << "2. Deposit" << endl;
                cout << "3. Withdraw" << endl;
                cout << "4. Transfer" << endl;
                cout << "5. View Transactions" << endl;
                cout << "6. Logout" << endl;
                cout << LIGHT_YELLOW << "Select an option: " << RESET;

                // Input validation loop for menu
                while (!(cin >> choice)) {
                    cout << LIGHT_RED << "Invalid input! Please enter a number between 1 and 6." << RESET << endl;
                    cin.clear();  // Clear the error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discard invalid input
                    cout << "Select an option: ";
                }

                cin.ignore();  // To discard any leftover newline character

                switch (choice) {
                case 1:
                    showAccountBalance(*currentCustomer);
                    break;
                case 2:
                    depositToAccount(*currentCustomer);
                    break;
                case 3:
                    withdrawFromAccount(*currentCustomer);
                    break;
                case 4:
                    transferFunds(*currentCustomer);
                    break;
                case 5:
                    viewRecentTransactions();
                    break;
                case 6:
                    cout << LIGHT_GREEN << "Logged out successfully!" << RESET << endl;
                    currentCustomer->logOut();
                    currentCustomer = nullptr;
                    break;
                default:
                    cout << LIGHT_RED << "Invalid choice!" << RESET << endl;
                }
            }
        }
        catch (const exception& e) {
            cout << LIGHT_RED << "Error in dashboard operation: " << e.what() << RESET << endl;
        }
    }

    void viewRecentTransactions() {
        cout << "===== Recent Transactions =====" << endl;
        for (const auto& tx : transactions) {
            cout << tx.toString();
        }
    }

    void showAccountBalance(Customer& customer) {
        string accountNumber = customer.id;

        Account* account = findAccountByAccountNumber(accountNumber);
        if (account) {
            cout << "Balance: $" << fixed << setprecision(2) << account->getBalance() << endl;
        }
        else {
            cout << LIGHT_RED << "Account not found!" << RESET << endl;
        }
    }

    void depositToAccount(Customer& customer) {
        try {
            double amount;
            cout << "Enter amount to deposit: $";
            cin >> amount;
            if (amount <= 0) throw invalid_argument("Deposit amount must be positive.");

            string accountNumber = customer.id;
            Account* account = findAccountByAccountNumber(accountNumber);
            if (account) {
                account->deposit(amount);
                saveAccount(*account);
                cout << LIGHT_GREEN << "Deposited $" << amount << " successfully!" << RESET << endl;
            }
            else {
                cout << LIGHT_RED << "Account not found!" << RESET << endl;
            }
        }
        catch (const exception& e) {
            cout << LIGHT_RED << "Error: " << e.what() << RESET << endl;
        }
    }

    void withdrawFromAccount(Customer& customer) {
        try {
            double amount;
            cout << "Enter amount to withdraw: $";
            cin >> amount;
            if (amount <= 0) throw invalid_argument("Withdrawal amount must be positive.");

            string accountNumber = customer.id;
            Account* account = findAccountByAccountNumber(accountNumber);
            if (account) {
                account->withdraw(amount);
                saveAccount(*account);
                cout << LIGHT_GREEN << "Withdrew $" << amount << " successfully!" << RESET << endl;
            }
            else {
                cout << LIGHT_RED << "Account not found!" << RESET << endl;
            }
        }
        catch (const exception& e) {
            cout << LIGHT_RED << "Error: " << e.what() << RESET << endl;
        }
    }

    void transferFunds(Customer& customer) {
        try {
            double amount;
            cout << "Enter amount to transfer: $";
            cin >> amount;
            if (amount <= 0) throw invalid_argument("Transfer amount must be positive.");

            string targetAccountNumber;
            cout << "Enter target account number: ";
            cin >> targetAccountNumber;

            Account* sourceAccount = findAccountByAccountNumber(customer.id);
            Account* targetAccount = findAccountByAccountNumber(targetAccountNumber);

            if (sourceAccount && targetAccount) {
                sourceAccount->transfer(*targetAccount, amount);
                Transaction newTransaction(customer.id, targetAccountNumber, amount);
                transactions.push_back(newTransaction);
                saveTransaction(newTransaction);
                saveAccount(*sourceAccount);
                saveAccount(*targetAccount);
                cout << LIGHT_GREEN << "Transferred $" << amount << " successfully!" << RESET << endl;
            }
            else {
                cout << LIGHT_RED << "Account not found!" << RESET << endl;
            }
        }
        catch (const exception& e) {
            cout << LIGHT_RED << "Error: " << e.what() << RESET << endl;
        }
    }
};

int main() {
    try {
        BankingSystem bankSystem;
        int choice;
        while (true) {
            cout << "\n===== Banking System =====" << endl;
            cout << "1. Register" << endl;
            cout << "2. Login" << endl;
            cout << "3. Exit" << endl;
            cout << LIGHT_YELLOW << "Select an option: " << RESET;
            cin >> choice;
            cin.ignore();  // Discard any leftover newline character

            switch (choice) {
            case 1:
                bankSystem.registerCustomer();
                break;
            case 2:
                bankSystem.loginCustomer();
                break;
            case 3:
                cout << "Exiting system..." << endl;
                return 0;
            default:
                cout << LIGHT_RED << "Invalid choice!" << RESET << endl;
            }
        }
    }
    catch (const exception& e) {
        cout << LIGHT_RED << "Error: " << e.what() << RESET << endl;
    }
}
