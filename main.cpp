#include <iostream>
#include <memory>  // for std::unique_ptr, std::make_unique

class BankAccount {
public:
    BankAccount(double initialBalance) : balance(initialBalance) {}

    void deposit(double amount) {
        balance += amount;
    }

    void withdraw(double amount) {
        if (balance >= amount) {
            balance -= amount;
        } else {
            std::cout << "Insufficient funds\n";
        }
    }

    void printBalance() const {
        std::cout << "Balance: $" << balance << "\n";
    }

private:
    double balance;
};

int main() {
    // Create a smart pointer that owns a BankAccount
    std::unique_ptr<BankAccount> account = std::make_unique<BankAccount>(100.0);

    account->deposit(50);
    account->withdraw(30);
    account->printBalance();


    // No need to delete manually; memory is freed automatically
}
