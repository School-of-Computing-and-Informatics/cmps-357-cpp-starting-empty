#include <iostream>
#include <memory>   // for std::unique_ptr, std::make_unique
#include <string>
#include <vector>
#include <algorithm>  // std::sort

class BankAccount {
public:
    BankAccount(std::string accountName, double initialBalance)
        : name(std::move(accountName)), balance(initialBalance) {}

    void deposit(double amount) {
        balance += amount;
    }

    void withdraw(double amount) {
        if (balance >= amount) {
            balance -= amount;
        } else {
            std::cout << "Insufficient funds for " << name << "\n";
        }
    }

    void printBalance() const {
        std::cout << name << "'s Balance: $" << balance << "\n";
    }

    double getBalance() const { return balance; }
    const std::string& getName() const { return name; }
private:
    std::string name;
    double balance;
};

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    std::vector<std::unique_ptr<BankAccount>> accounts;

    for (int i = 0; i < 100; i++) {
        std::string name = "Account_" + std::to_string(i + 1);
        double balance = (rand() % 10000) / 100.0;  // balance between 0.00 and 99.99

        accounts.push_back(std::make_unique<BankAccount>(name, balance));
    }

    // Print first 5 as a check
    for (int i = 0; i < 5; i++) {
        accounts[i]->printBalance();
    }

    // Sort by balance ascending
    std::sort(accounts.begin(), accounts.end(),
        [](const std::unique_ptr<BankAccount>& a,
           const std::unique_ptr<BankAccount>& b) {
            return a->getBalance() < b->getBalance();
        });

    // Print first 5 after sorting
    for (int i = 0; i < 5; i++) {
        accounts[i]->printBalance();
    }
}
