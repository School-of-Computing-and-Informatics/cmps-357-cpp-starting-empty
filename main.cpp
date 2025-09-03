#include <iostream>
#include <memory>   // for std::unique_ptr, std::make_unique
#include <string>

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

private:
    std::string name;
    double balance;
};

int main() {
    // Create a smart pointer that owns a BankAccount
    std::unique_ptr<BankAccount> account =
        std::make_unique<BankAccount>("Alice", 100.0);

    account->deposit(50);
    account->withdraw(30);
    account->printBalance();

    // Memory freed automatically when 'account' goes out of scope
}
