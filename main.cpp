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


#include <wx/wx.h>
#include <wx/choice.h>

class App : public wxApp {
public:
    bool OnInit() override {
        auto* f = new wxFrame(nullptr, wxID_ANY, "Bank Accounts");
        auto* panel = new wxPanel(f);
        auto* s = new wxBoxSizer(wxVERTICAL);

        wxArrayString names;
        if (accounts_) {
            for (auto& acct : *accounts_) {
                names.Add(acct->getName());
            }
        }

        auto* choice = new wxChoice(panel, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, names);
        s->Add(choice, 0, wxALL | wxEXPAND, 12);
        panel->SetSizerAndFit(s);

        // use helper to compute dimensions
        int winW = GetMaxNameWidth(panel, names) * 2;
        int winH = static_cast<int>(winW * 0.75);

        f->SetClientSize(winW, winH);
        f->Centre();
        f->Show();
        return true;
    }

    void SetAccounts(std::vector<std::unique_ptr<BankAccount>>* accounts) {
        accounts_ = accounts;
    }

private:
    std::vector<std::unique_ptr<BankAccount>>* accounts_ = nullptr;

    int GetMaxNameWidth(wxWindow* refWin, const wxArrayString& names) {
        wxClientDC dc(refWin);
        dc.SetFont(refWin->GetFont());
        int maxW = 0, h = 0;
        for (auto& name : names) {
            int w, th;
            dc.GetTextExtent(name, &w, &th);
            if (w > maxW) maxW = w;
            h = th;
        }
        return maxW;
    }
};

wxIMPLEMENT_APP_NO_MAIN(App);

static std::vector<std::unique_ptr<BankAccount>> accounts;

int main(int argc, char** argv) {
    const char* path = std::getenv("PATH");
    if (path) {
        std::cout << "PATH = " << path << "\n";
    } else {
        std::cout << "PATH not set\n";
    }

    srand(static_cast<unsigned>(time(nullptr)));

    //std::vector<std::unique_ptr<BankAccount>> accounts;

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

    // start wx, then set data, then run
    if ( !wxEntryStart(argc, argv) ) return 1;
    auto* app = wxTheApp;                    // now non-null
    static_cast<App*>(app)->SetAccounts(&accounts);
    if ( !app->CallOnInit() ) { wxEntryCleanup(); return 1; }
    int code = app->OnRun();
    app->OnExit();
    wxEntryCleanup();
    return code;

}
