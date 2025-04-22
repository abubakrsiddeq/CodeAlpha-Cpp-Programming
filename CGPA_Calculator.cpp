#include <iostream> 
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <limits>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <iomanip>
#include <set>
#include <map>

using namespace std;

// ---------- Constants ----------
const unordered_map<string, float> GRADE_MAP = {
    {"A", 4.0f}, {"A-", 3.7f}, {"B+", 3.3f}, {"B", 3.0f},
    {"B-", 2.7f}, {"C+", 2.3f}, {"C", 2.0f}, {"D", 1.0f}, {"F", 0.0f}
};

const string LIGHT_CYAN = "\033[96m";
const string GREEN = "\033[32m";
const string RESET = "\033[0m";
const string YELLOW = "\033[33m";
const string LIGHT_YELLOW = "\033[93m";
const string RED = "\033[31m";
const string LIGHT_MAGENTA = "\033[95m";
const string LIGHT_GREEN = "\033[92m";
const string WHITE = "\033[97m";

// ---------- Helper Template ----------
template<typename T>
T getValidatedInput(const string& prompt, const string& errorMessage) {
    T value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cerr << RED << errorMessage << RESET << endl;
    }
}

// ---------- Person Base Class ----------
class Person {
protected:
    string name;

public:
    virtual void setName(const string& studentName) {
        name = studentName;
    }
    virtual void displayProfile() const = 0;
    virtual ~Person() = default;
};

// ---------- Course Class ----------
class Course {
private:
    string courseName;
    int credit;
    string letterGrade;
    float gradePoint;

    float convertGradeToPoint(const string& grade) const {
        auto it = GRADE_MAP.find(grade);
        if (it == GRADE_MAP.end()) {
            throw invalid_argument("Invalid grade input.");
        }
        return it->second;
    }

public:
    Course(const string& name, int cred, const string& grade)
        : courseName(name), credit(cred), letterGrade(grade), gradePoint(convertGradeToPoint(grade)) {
        if (credit <= 0) throw invalid_argument("Credit hours must be > 0.");
    }

    string getName() const { return courseName; }
    int getCredit() const { return credit; }
    string getLetterGrade() const { return letterGrade; }
    float getGradePoint() const { return gradePoint; }
};

// ---------- Semester Class ----------
class Semester {
private:
    string semesterName;
    vector<Course> courses;
    unordered_set<string> courseNames;

public:
    Semester(const string& name) : semesterName(name) {}

    string getName() const { return semesterName; }

    void addCourse(const string& courseName, int credit, const string& grade) {
        if (!courseNames.insert(courseName).second) {
            throw invalid_argument("Duplicate course name.");
        }
        courses.emplace_back(courseName, credit, grade);
    }

    float calculateGPA() const {
        float totalCredits = 0, totalPoints = 0;
        for (const auto& c : courses) {
            totalCredits += c.getCredit();
            totalPoints += c.getCredit() * c.getGradePoint();
        }
        return totalCredits > 0 ? totalPoints / totalCredits : 0;
    }

    int totalCredits() const {
        int total = 0;
        for (const auto& c : courses)
            total += c.getCredit();
        return total;
    }

    float totalGradePoints() const {
        float total = 0;
        for (const auto& c : courses)
            total += c.getCredit() * c.getGradePoint();
        return total;
    }

    void displayCourses() const {
        cout << "\n" << LIGHT_CYAN << semesterName << RESET << "\n";
        cout << WHITE << left << setw(30) << "Course" << setw(10) << "Credit"
            << setw(10) << "Grade" << setw(15) << "Grade Point" << RESET << "\n";
        cout << WHITE << string(62, '-') << RESET << "\n";
        for (const auto& c : courses) {
            cout << left << setw(30) << c.getName()
                << setw(7) << LIGHT_GREEN << c.getCredit() << RESET
                << LIGHT_YELLOW << setw(10) << "      " << c.getLetterGrade() << RESET
                << setw(15) << WHITE << c.getGradePoint() << RESET << "\n";
        }
        cout << WHITE << string(62, '-') << RESET << "\n";
        cout << fixed << setprecision(2);
        cout << LIGHT_MAGENTA << "Semester GPA: " << calculateGPA() << RESET << "\n";
    }
};

// ---------- Student Class ----------
class Student : public Person {
private:
    map<string, Semester> semesters;
    set<string> semesterNames;

public:
    void addSemester(const string& semName) {
        if (!semesterNames.insert(semName).second) {
            throw invalid_argument("Duplicate semester name.");
        }
        semesters.emplace(semName, Semester(semName));
    }

    void addCourseToSemester(const string& semName, const string& courseName, int credit, const string& grade) {
        semesters.at(semName).addCourse(courseName, credit, grade);
    }

    void displayProfile() const override {
        cout << "\n" << LIGHT_CYAN << string(60, '=') << RESET << "\n";
        cout << LIGHT_CYAN << setw(38) << "STUDENT PROFILE" << RESET << "\n";
        cout << LIGHT_CYAN << string(60, '=') << RESET << "\n";
        cout << WHITE << "Name: " << name << "\n";
        cout << "Total Semesters: " << semesters.size() << RESET << "\n";
    }

    void displayResults() const {
        float totalCredits = 0, totalGradePoints = 0;
        for (const auto& pair : semesters) {
            const auto& sem = pair.second;
            sem.displayCourses();
            totalCredits += sem.totalCredits();
            totalGradePoints += sem.totalGradePoints();
        }
        cout << "\n" << LIGHT_CYAN << string(60, '=') << RESET << "\n";
        cout << fixed << setprecision(2);
        cout << LIGHT_GREEN << "Total Credits: " << totalCredits << RESET << "\n";
        cout << LIGHT_GREEN << "Total Grade Points: " << totalGradePoints << RESET << "\n";
        float cgpa = totalCredits > 0 ? totalGradePoints / totalCredits : 0;
        cout << LIGHT_MAGENTA << "Final CGPA: " << cgpa << RESET << "\n";

        if (cgpa >= 3.5f)
            cout << GREEN << "Status: First Class with Distinction\n" << RESET;
        else if (cgpa >= 3.0f)
            cout << GREEN << "Status: First Class\n" << RESET;
        else if (cgpa >= 2.0f)
            cout << GREEN << "Status: Passed\n" << RESET;
        else
            cout << RED << "Status: Probation / Fail\n" << RESET;
    }
};

// ---------- Main Function ----------
int main() {
    auto student = make_unique<Student>();
    string studentName;

    cout << LIGHT_CYAN << "===== CGPA CALCULATOR =====" << RESET << "\n";
    cout << "Enter student name: ";
    getline(cin, studentName);
    student->setName(studentName);

    int semCount = getValidatedInput<int>("Enter number of semesters: ", "Invalid input.");

    for (int i = 1; i <= semCount; ++i) {
        string semName;
        while (true) {
            cout << "\nEnter unique name for Semester " << i << ": ";
            getline(cin, semName);
            if (semName.empty()) {
                cerr << RED << "Semester name cannot be empty.\n" << RESET;
                continue;
            }
            try {
                student->addSemester(semName);
                break;
            }
            catch (const exception& e) {
                cerr << RED << "Error: " << e.what() << "\n" << RESET;
            }
        }

        int courseCount = getValidatedInput<int>("Enter number of courses for this semester: ", "Invalid input.");
        for (int j = 1; j <= courseCount; ++j) {
            cout << "\n--- Enter details for Course " << j << " ---\n";
            string courseName, grade;
            int credit;

            while (true) {
                cout << "Course name: ";
                getline(cin, courseName);
                if (courseName.empty()) {
                    cerr << RED << "Course name cannot be empty.\n" << RESET;
                    continue;
                }
                try {
                    credit = getValidatedInput<int>("Credit hours: ", "Invalid input.");
                    cout << "Grade (A, A-, B+, etc.): ";
                    getline(cin, grade);
                    transform(grade.begin(), grade.end(), grade.begin(), ::toupper);
                    if (GRADE_MAP.find(grade) == GRADE_MAP.end()) {
                        throw invalid_argument("Grade not in valid set.");
                    }
                    student->addCourseToSemester(semName, courseName, credit, grade);
                    break;
                }
                catch (const exception& e) {
                    string errorMsg = e.what();
                    if (errorMsg == "Duplicate course name.") {
                        cerr << RED << "Error: Duplicate course name.\n" << RESET;
                    }
                    else {
                        cerr << RED << "Error: " << errorMsg << "\n" << RESET;
                    }
                }
            }
        }
    }

    student->displayProfile();
    student->displayResults();
    return 0;
}
