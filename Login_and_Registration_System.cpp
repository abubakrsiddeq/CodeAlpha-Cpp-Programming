#include <iostream> 
#include <fstream>
#include <string>
#include <algorithm>
#include <cstdlib>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

// Color Macros
#define RESET   "\033[0m"
#define CYAN    "\033[96m"
#define GREEN   "\033[92m"
#define RED     "\033[91m"
#define YELLOW  "\033[93m"

// Get hidden password input
string getPasswordInput(const string& prompt) {
    string password;
    char ch;

    cout << prompt;
#ifdef _WIN32
    while ((ch = _getch()) != '\r') {
        if (ch == '\b' && !password.empty()) {
            password.pop_back();
            cout << "\b \b";
        }
        else if (isprint(ch) || ch == ' ') {
            password += ch;
            cout << '*';
        }
    }
#else
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (read(STDIN_FILENO, &ch, 1) && ch != '\n') {
        if ((ch == 127 || ch == '\b') && !password.empty()) {
            password.pop_back();
            cout << "\b \b";
        }
        else if (isprint(ch) || ch == ' ') {
            password += ch;
            cout << '*';
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    cout << endl;
    return password;
}

class UserManager {
private:
    string toLower(const string& str) {
        string lowered = str;
        transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);
        return lowered;
    }

    void createUsersFolder() {
        system("mkdir users >nul 2>&1");
    }

    bool fileExists(const string& filename) {
        ifstream f(filename.c_str());
        return f.good();
    }

public:
    void registerUser() {
        string username, password, confirmPass;
        cout << CYAN << "\n[User Registration]\n" << RESET;

        cout << "Enter username: ";
        cin >> username;
        username = toLower(username);
        string filePath = "users/" + username + ".txt";

        if (fileExists(filePath)) {
            cout << RED << "Username already exists. Try another.\n" << RESET;
            return;
        }

        cin.ignore(); // Clear buffer
        password = getPasswordInput("Enter password: ");
        confirmPass = getPasswordInput("Confirm password: ");

        if (password != confirmPass) {
            cout << RED << "Passwords do not match.\n" << RESET;
            return;
        }

        createUsersFolder();

        try {
            ofstream file(filePath);
            if (!file) throw runtime_error("Could not create user file.");

            file << username << endl;
            file << password << endl;
            file.close();

            cout << GREEN << "User registered successfully!\n" << RESET;
        }
        catch (const exception& e) {
            cout << RED << "Registration failed: " << e.what() << RESET << endl;
        }
    }

    void loginUser() {
        string username, password;
        cout << CYAN << "\n[User Login]\n" << RESET;

        cout << "Enter username: ";
        cin >> username;
        username = toLower(username);
        string filePath = "users/" + username + ".txt";

        if (!fileExists(filePath)) {
            cout << RED << "Account does not exist.\n" << RESET;
            return;
        }

        cin.ignore(); // Clear buffer
        password = getPasswordInput("Enter password: ");

        try {
            ifstream file(filePath);
            if (!file) throw runtime_error("Could not open user file.");

            string storedUser, storedPass;
            getline(file, storedUser);
            getline(file, storedPass);

            if (storedUser == username && storedPass == password) {
                cout << GREEN << "Login successful. Welcome, " << username << "!\n" << RESET;
            }
            else {
                cout << RED << "Invalid credentials.\n" << RESET;
            }
        }
        catch (const exception& e) {
            cout << RED << "Login error: " << e.what() << RESET << endl;
        }
    }
};

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void displayMenu() {
    cout << YELLOW << "\n========= Welcome to Secure Login System =========\n" << RESET;
    cout << "1. Register a New User\n";
    cout << "2. Login\n";
    cout << "3. Exit\n";
    cout << CYAN << "Choose an option: " << RESET;
}

int main() {
    UserManager userManager;
    int choice;

    while (true) {
        displayMenu();

        if (!(cin >> choice)) {
            cout << RED << "Please enter a valid numeric option.\n" << RESET;
            clearInput();
            continue;
        }

        switch (choice) {
        case 1:
            userManager.registerUser();
            break;
        case 2:
            userManager.loginUser();
            break;
        case 3:
            cout << CYAN << "Thank you for using the system. Goodbye!\n" << RESET;
            return 0;
        default:
            cout << RED << "Invalid option. Try again.\n" << RESET;
        }
    }
    return 0;
}
