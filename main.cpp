#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <map>
#include <limits>

using namespace std;

// Class for personal expenses
class Expense {
public:
    string category;
    string description;
    double amount;

    Expense(string c, string d, double a) {
        category = c;
        description = d;
        amount = a;
    }
    void display() const {
        cout << left << setw(18) << category
             << setw(25) << description
             << "Rs. " << fixed << setprecision(2)
             << amount << endl;
    }
};

// Class for shared group expenses
class GroupExpense {
public:
    string payer;
    string description;
    double amount;
    vector<string> members;

    GroupExpense(string p, string d, double a,
                 vector<string> m) {
        payer = p;
        description = d;
        amount = a;
        members = m;
    }

    void display() const {
        cout << "\nDescription: " << description
             << "\nPaid by: " << payer
             << "\nTotal amount: Rs. " << amount
             << "\nMembers: ";

        for (const string& member : members) {
            cout << member << " ";
        }

        cout << "\n";
    }
};

// Class for saving and loading data
class FileManager {
public:
    static void savePersonalExpense(const Expense& e) {
        ofstream file("personal_expenses.txt", ios::app);

        if (file.is_open()) {
            file << e.category << "|"
                 << e.description << "|"
                 << e.amount << "\n";
            file.close();
        }
    }

    static void saveGroupExpense(const GroupExpense& e) {
        ofstream file("group_expenses.txt", ios::app);

        if (file.is_open()) {
            file << e.payer << "|"
                 << e.description << "|"
                 << e.amount << "|";

            for (size_t i = 0; i < e.members.size(); i++) {
                file << e.members[i];

                if (i + 1 < e.members.size()) {
                    file << ",";
                }
            }

            file << "\n";
            file.close();
        }
    }
};

// Class for personal expense management
class ExpenseTracker {
private:
    vector<Expense> expenses;

public:
    void addExpense() {
        string category, description;
        double amount;

        cout << "Enter category (Food, Travel, etc.): ";
        getline(cin, category);

        cout << "Enter description: ";
        getline(cin, description);

        cout << "Enter amount: Rs. ";
        cin >> amount;

        if (cin.fail() || amount <= 0) {
            cout << "Invalid amount!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        Expense e(category, description, amount);
        expenses.push_back(e);
        FileManager::savePersonalExpense(e);

        cout << "Personal expense added successfully!\n";
    }

    void viewExpenses() const {
        if (expenses.empty()) {
            cout << "No personal expenses added in this session.\n";
            return;
        }

        cout << "\n--- Personal Expenses ---\n";
        cout << left << setw(18) << "Category"
             << setw(25) << "Description"
             << "Amount\n";

        for (const Expense& e : expenses) {
            e.display();
        }
    }

    void showSummary() const {
        double total = 0;

        for (const Expense& e : expenses) {
            total += e.amount;
        }

        cout << "\nTotal personal spending in this session: Rs. "
             << fixed << setprecision(2) << total << "\n";
    }
};

// Class for shared expense management and fairness detection
class SharedExpenseManager {
private:
    vector<GroupExpense> groupExpenses;

public:
    void addSharedExpense() {
        string payer, description;
        double amount;
        int numberOfMembers;

        cout << "Enter expense description: ";
        getline(cin, description);

        cout << "Enter name of person who paid: ";
        getline(cin, payer);

        cout << "Enter total expense amount: Rs. ";
        cin >> amount;

        if (cin.fail() || amount <= 0) {
            cout << "Invalid amount!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cout << "Enter number of group members, including the payer: ";
        cin >> numberOfMembers;

        if (cin.fail() || numberOfMembers <= 0 ||
            numberOfMembers > 100) {
            cout << "Invalid number of members!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        vector<string> members;
        bool payerFound = false;

        for (int i = 0; i < numberOfMembers; i++) {
            string name;
            cout << "Enter name of member " << i + 1 << ": ";
            getline(cin, name);

            members.push_back(name);

            if (name == payer) {
                payerFound = true;
            }
        }

        if (!payerFound) {
            cout << "Error: The payer must be included in the group.\n";
            return;
        }

        GroupExpense e(payer, description, amount, members);
        groupExpenses.push_back(e);
        FileManager::saveGroupExpense(e);

        cout << "Shared expense added successfully!\n";
    }

    void viewSharedExpenses() const {
        if (groupExpenses.empty()) {
            cout << "No shared expenses added in this session.\n";
            return;
        }

        cout << "\n--- Shared Expenses ---\n";

        for (const GroupExpense& e : groupExpenses) {
            e.display();
        }
    }

    map<string, double> calculateBalances() const {
        map<string, double> balances;

        for (const GroupExpense& e : groupExpenses) {
            if (e.members.empty()) {
                continue;
            }

            double share = e.amount / e.members.size();

            // Every member owes their equal share.
            for (const string& member : e.members) {
                balances[member] -= share;
            }

            // The payer gets credit for the amount paid.
            balances[e.payer] += e.amount;
        }

        return balances;
    }

    void showBalances() const {
        map<string, double> balances = calculateBalances();

        if (balances.empty()) {
            cout << "No shared expense data available.\n";
            return;
        }

        cout << "\n--- Member Balances ---\n";
        cout << "Positive balance = money to receive\n";
        cout << "Negative balance = money to pay\n\n";

        for (const auto& entry : balances) {
            cout << left << setw(20) << entry.first
                 << "Rs. " << fixed << setprecision(2)
                 << entry.second << "\n";
        }
    }

    void fairnessReport() const {
        map<string, double> balances = calculateBalances();

        if (balances.empty()) {
            cout << "No shared expense data available.\n";
            return;
        }

        cout << "\n--- Shared Expense Fairness Report ---\n";

        bool balanced = true;

        for (const auto& entry : balances) {
            cout << entry.first << ": ";

            if (entry.second > 0.01) {
                cout << "Should receive Rs. "
                     << fixed << setprecision(2)
                     << entry.second << "\n";
                balanced = false;
            }
            else if (entry.second < -0.01) {
                cout << "Should pay Rs. "
                     << fixed << setprecision(2)
                     << -entry.second << "\n";
                balanced = false;
            }
            else {
                cout << "Settled fairly\n";
            }
        }

        if (balanced) {
            cout << "\nAll members are financially balanced.\n";
        } else {
            cout << "\nThe group has pending amounts to settle.\n";
            cout << "The balances above show how much each member "
                 << "should pay or receive.\n";
        }
    }
};

int main() {
    ExpenseTracker personalTracker;
    SharedExpenseManager sharedManager;

    int choice;

    do {
        cout << "\n====================================\n";
        cout << "     PERSONAL EXPENSE TRACKER\n";
        cout << "====================================\n";
        cout << "1. Add Personal Expense\n";
        cout << "2. View Personal Expenses\n";
        cout << "3. Personal Spending Summary\n";
        cout << "4. Add Shared Expense\n";
        cout << "5. View Shared Expenses\n";
        cout << "6. View Member Balances\n";
        cout << "7. Shared Expense Fairness Report\n";
        cout << "0. Exit\n";
        cout << "====================================\n";
        cout << "Enter your choice: ";

        cin >> choice;

        if (cin.fail()) {
            cout << "Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                personalTracker.addExpense();
                break;

            case 2:
                personalTracker.viewExpenses();
                break;

            case 3:
                personalTracker.showSummary();
                break;

            case 4:
                sharedManager.addSharedExpense();
                break;

            case 5:
                sharedManager.viewSharedExpenses();
                break;

            case 6:
                sharedManager.showBalances();
                break;

            case 7:
                sharedManager.fairnessReport();
                break;

            case 0:
                cout << "Thank you for using Personal Expense Tracker!\n";
                break;

            default:
                cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 0);

    return 0;
}
