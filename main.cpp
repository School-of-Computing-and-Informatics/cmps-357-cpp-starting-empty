#include <iostream>
#include <memory>   // for std::unique_ptr, std::make_unique
#include <string>
#include <vector>
#include <algorithm>  // std::sort
#include <chrono>
#include <regex>
#include <string>
#include <sstream>

class BankAccount {
public:
    BankAccount(std::string accountName, double initialBalance)
        : name(std::move(accountName)), balance(initialBalance) {}

    void deposit(double amount) {
        balance += amount;
        lastUpdate = std::chrono::system_clock::now();
    }

    void withdraw(double amount) {
        if (balance >= amount) {
            balance -= amount;
            lastUpdate = std::chrono::system_clock::now();
        } else {
            std::cout << "Insufficient funds for " << name << "\n";
        }
    }

    void printBalance() const {
        std::cout << name << "'s Balance: $" << balance;
        if (lastUpdate.time_since_epoch().count() > 0) {
            std::cout << "  " << getLastUpdateString();
        }
        std::cout << "\n";
    }

    double getBalance() const { return balance; }
    const std::string& getName() const { return name; }

    std::string getLastUpdateString() const {
        if (lastUpdate.time_since_epoch().count() == 0) {
            return "(no transactions yet)";
        }
        using namespace std::chrono;
        auto now = system_clock::now();
        auto diff = duration_cast<minutes>(now - lastUpdate);

        if (diff < minutes(1)) {
            return "(just now)";
        } else if (diff < hours(1)) {
            return "(" + std::to_string(diff.count()) + " minutes ago)";
        } else {
            auto h = duration_cast<hours>(diff);
            return "(" + std::to_string(h.count()) + " hours ago)";
        }
    }

private:
    std::string name;
    double balance;
    std::chrono::system_clock::time_point lastUpdate{};
};

#include <wx/wx.h>
#include <wx/choice.h>
#include <wx/valtext.h>

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

        wxFont font(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        choice->SetFont(font);
        panel->SetFont(font);

        s->Add(choice, 0, wxALL | wxEXPAND, 12);

        // info text
        auto* infoText = new wxStaticText(panel, wxID_ANY,
                                          "Select an account to view balance");
        infoText->SetFont(font);
        s->Add(infoText, 0, wxALL | wxEXPAND, 12);

        // radio buttons
        auto* radioDeposit = new wxRadioButton(panel, wxID_ANY, "Deposit", wxDefaultPosition,
                                               wxDefaultSize, wxRB_GROUP);
        auto* radioWithdraw = new wxRadioButton(panel, wxID_ANY, "Withdraw");
        radioDeposit->SetFont(font);
        radioWithdraw->SetFont(font);
        s->Add(radioDeposit, 0, wxALL, 8);
        s->Add(radioWithdraw, 0, wxALL, 8);

        // horizontal row: $ label, text field, submit button
        auto* row = new wxBoxSizer(wxHORIZONTAL);
        auto* dollarLabel = new wxStaticText(panel, wxID_ANY, "$");
        auto* amountField = new wxTextCtrl(panel, wxID_ANY);
        auto* submitBtn = new wxButton(panel, wxID_ANY, "Submit");

        // allow only numbers and dot
        wxTextValidator validator(wxFILTER_INCLUDE_CHAR_LIST);
        wxArrayString includes;
        for (char c = '0'; c <= '9'; ++c) includes.Add(wxString(c));
        includes.Add("."); // decimal point
        validator.SetIncludes(includes);
        amountField->SetValidator(validator);

        // Submit binding
        submitBtn->Bind(wxEVT_BUTTON, [amountField](wxCommandEvent&) {
            wxString val = amountField->GetValue();
            std::string str = val.ToStdString();

            // Regex for positive number with <=2 decimals
            std::regex re(R"(^\d+(\.\d{1,2})?$)");
            if (!std::regex_match(str, re)) {
                std::cout << "[DEBUG] Invalid input: " << str << "\n";
                return;
            }

            double amt = std::stod(str);
            std::cout << "[DEBUG] Valid amount entered: " << amt << "\n";
        });

        dollarLabel->SetFont(font);
        amountField->SetFont(font);
        submitBtn->SetFont(font);

        row->Add(dollarLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
        row->Add(amountField, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
        row->Add(submitBtn, 0, wxALIGN_CENTER_VERTICAL);

        s->Add(row, 0, wxALL | wxEXPAND, 12);

        panel->SetSizerAndFit(s);

        int winW = GetMaxNameWidth(panel, names) * 4;
        int winH = static_cast<int>(winW * 0.75);

        f->SetClientSize(winW, winH);
        f->Centre();
        f->Show();

        // bindings
        choice->Bind(wxEVT_CHOICE, [this, infoText, choice](wxCommandEvent& evt) {
            int sel = choice->GetSelection();
            if (sel != wxNOT_FOUND && accounts_ && sel < static_cast<int>(accounts_->size())) {
                auto& acct = *(*accounts_)[sel];
                wxString msg = wxString::Format("%s: $%.2f %s",
                                                acct.getName(),
                                                acct.getBalance(),
                                                acct.getLastUpdateString().c_str());
                infoText->SetLabel(msg);
            }
        });

        radioDeposit->Bind(wxEVT_RADIOBUTTON, [](wxCommandEvent&) {
            std::cout << "[DEBUG] Deposit radio selected\n";
        });
        radioWithdraw->Bind(wxEVT_RADIOBUTTON, [](wxCommandEvent&) {
            std::cout << "[DEBUG] Withdraw radio selected\n";
        });
        submitBtn->Bind(wxEVT_BUTTON, [amountField](wxCommandEvent&) {
            std::cout << "[DEBUG] Submit clicked, amount = "
                      << amountField->GetValue().ToStdString() << "\n";
        });

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
