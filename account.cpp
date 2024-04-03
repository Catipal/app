#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class Account {
private:
    int accountNumber;
    std::string password;
    std::string fileName;

public:
    // Constructor
    Account(int number, const std::string& pass) : accountNumber(number), password(pass) {
        // Set file name based on account number
        fileName = "account_" + std::to_string(accountNumber) + ".txt";
    }

    // Function to authenticate password
    bool authenticate(const std::string& pass) {
        return pass == password;
    }

    // Function to record user activity in the account file
    void recordActivity(const std::string& activity) {
        std::ofstream file(fileName, std::ios_base::app);
        if (file.is_open()) {
            file << activity << std::endl;
            file.close();
        } else {
            std::cerr << "Error: Unable to open file for account " << accountNumber << std::endl;
        }
    }
};

class AccountManager {
private:
    std::vector<Account> accounts;
    int nextAccountNumber;

public:
    // Constructor
    AccountManager() : nextAccountNumber(0) {}

    // Function to create a new account
    void createAccount(const std::string& password) {
        Account newAccount(nextAccountNumber++, password);
        accounts.push_back(newAccount);
        std::cout << "Account created successfully with number " << (nextAccountNumber - 1) << std::endl;
    }

    // Function to perform an activity on an account
    void performActivity(int accountNumber, const std::string& password, const std::string& activity) {
        if (accountNumber >= 0 && accountNumber < nextAccountNumber) {
            Account& account = accounts[accountNumber];
            if (account.authenticate(password)) {
                account.recordActivity(activity);
                std::cout << "Activity recorded successfully for account " << accountNumber << std::endl;
            } else {
                std::cerr << "Error: Authentication failed for account " << accountNumber << std::endl;
            }
        } else {
            std::cerr << "Error: Account number " << accountNumber << " does not exist" << std::endl;
        }
    }
};

int main() {
    AccountManager manager;

    // Create accounts
    manager.createAccount("password1");
    manager.createAccount("password2");

    // Perform activities on accounts
    manager.performActivity(0, "password1", "Deposit $100");
    manager.performActivity(1, "password2", "Withdraw $50");

    return 0;
}
