/****************/
/* MSSV: 202418872 */
/* Ho ten: Nguyen Minh Duc */
/****************/

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <stdexcept>

using namespace std;

// ============================================================
// EMPLOYEE - ABSTRACT BASE CLASS
// ============================================================

class Employee {
protected:
    string employeeId;
    string fullName;
    string department;
    double monthlyBonus;

    void validateCommonInfo() {
        if (employeeId.empty())
            throw invalid_argument("Employee ID cannot be empty.");

        if (fullName.empty())
            throw invalid_argument("Full name cannot be empty.");

        if (department.empty())
            throw invalid_argument("Department cannot be empty.");

        if (monthlyBonus < 0)
            throw invalid_argument("Bonus cannot be negative.");
    }

public:
    // Constructor 1
    Employee(const string& employeeId,
             const string& fullName)
        : employeeId(employeeId),
          fullName(fullName),
          department("Unassigned"),
          monthlyBonus(0) {
        validateCommonInfo();
    }

    // Constructor 2
    Employee(const string& employeeId,
             const string& fullName,
             const string& department)
        : employeeId(employeeId),
          fullName(fullName),
          department(department),
          monthlyBonus(0) {
        validateCommonInfo();
    }

    virtual ~Employee() = default;

    // ========================================================
    // METHOD OVERLOADING: addBonus()
    // ========================================================

    // 1. Fixed bonus
    void addBonus(double amount) {
        if (amount <= 0)
            throw invalid_argument("Bonus amount must be > 0.");

        monthlyBonus += amount;
    }

    // 2. Fixed bonus + reason
    void addBonus(double amount, const string& reason) {
        if (amount <= 0)
            throw invalid_argument("Bonus amount must be > 0.");

        if (reason.empty())
            throw invalid_argument("Bonus reason cannot be empty.");

        monthlyBonus += amount;

        cout << "Bonus reason for " << employeeId
             << ": " << reason << endl;
    }

    // 3. Bonus = rate * referenceAmount
    void addBonus(double rate,
                  double referenceAmount,
                  const string& reason) {

        if (rate <= 0 || rate > 0.5)
            throw invalid_argument(
                "Bonus rate must be > 0 and <= 0.5."
            );

        if (referenceAmount <= 0)
            throw invalid_argument(
                "Reference amount must be > 0."
            );

        if (reason.empty())
            throw invalid_argument(
                "Bonus reason cannot be empty."
            );

        double bonus = rate * referenceAmount;

        monthlyBonus += bonus;

        cout << "Bonus reason for " << employeeId
             << ": " << reason
             << " | Bonus = " << bonus << endl;
    }

    // ========================================================
    // GETTERS
    // ========================================================

    string getEmployeeId() const {
        return employeeId;
    }

    string getFullName() const {
        return fullName;
    }

    string getDepartment() const {
        return department;
    }

    double getMonthlyBonus() const {
        return monthlyBonus;
    }

    // Reset bonus
    void resetBonus() {
        monthlyBonus = 0;
    }

    // ========================================================
    // PURE VIRTUAL METHODS
    // ========================================================

    virtual double calculateGrossPay() const = 0;

    virtual string getEmployeeType() const = 0;

    virtual void displayPayrollInfo() const = 0;
};


// ============================================================
// SALARIED EMPLOYEE
// ============================================================

class SalariedEmployee : public Employee {
private:
    double monthlySalary;
    double responsibilityAllowance;

public:
    // Short constructor
    SalariedEmployee(const string& employeeId,
                     const string& fullName)
        : Employee(employeeId, fullName),
          monthlySalary(0),
          responsibilityAllowance(0) {}

    // Full constructor
    SalariedEmployee(const string& employeeId,
                     const string& fullName,
                     const string& department,
                     double monthlySalary,
                     double responsibilityAllowance)
        : Employee(employeeId, fullName, department),
          monthlySalary(monthlySalary),
          responsibilityAllowance(responsibilityAllowance) {

        if (monthlySalary < 0)
            throw invalid_argument(
                "Monthly salary cannot be negative."
            );

        if (responsibilityAllowance < 0)
            throw invalid_argument(
                "Responsibility allowance cannot be negative."
            );
    }

    double calculateGrossPay() const override {
        return monthlySalary
             + responsibilityAllowance
             + monthlyBonus;
    }

    string getEmployeeType() const override {
        return "SalariedEmployee";
    }

    void displayPayrollInfo() const override {
        cout << fixed << setprecision(0);

        cout << "\n----------------------------------\n";
        cout << "Employee type : " << getEmployeeType() << endl;
        cout << "ID            : " << employeeId << endl;
        cout << "Name          : " << fullName << endl;
        cout << "Department    : " << department << endl;
        cout << "Monthly salary: " << monthlySalary << endl;
        cout << "Allowance     : " << responsibilityAllowance << endl;
        cout << "Bonus         : " << monthlyBonus << endl;
        cout << "Gross pay     : " << calculateGrossPay() << endl;
    }
};


// ============================================================
// HOURLY EMPLOYEE
// ============================================================

class HourlyEmployee : public Employee {
private:
    double hourlyRate;
    double workedHours;

public:
    // Short constructor
    HourlyEmployee(const string& employeeId,
                   const string& fullName)
        : Employee(employeeId, fullName),
          hourlyRate(0),
          workedHours(0) {}

    // Full constructor
    HourlyEmployee(const string& employeeId,
                   const string& fullName,
                   const string& department,
                   double hourlyRate,
                   double workedHours)
        : Employee(employeeId, fullName, department),
          hourlyRate(hourlyRate),
          workedHours(workedHours) {

        if (hourlyRate < 0)
            throw invalid_argument(
                "Hourly rate cannot be negative."
            );

        if (workedHours < 0 || workedHours > 250)
            throw invalid_argument(
                "Worked hours must be between 0 and 250."
            );
    }

    double calculateGrossPay() const override {
        double basePay;

        if (workedHours <= 160) {
            basePay = workedHours * hourlyRate;
        }
        else {
            basePay =
                160 * hourlyRate
                + (workedHours - 160)
                    * hourlyRate
                    * 1.5;
        }

        return basePay + monthlyBonus;
    }

    string getEmployeeType() const override {
        return "HourlyEmployee";
    }

    void displayPayrollInfo() const override {
        cout << fixed << setprecision(0);

        double basePay;

        if (workedHours <= 160) {
            basePay = workedHours * hourlyRate;
        }
        else {
            basePay =
                160 * hourlyRate
                + (workedHours - 160)
                    * hourlyRate
                    * 1.5;
        }

        cout << "\n----------------------------------\n";
        cout << "Employee type : " << getEmployeeType() << endl;
        cout << "ID            : " << employeeId << endl;
        cout << "Name          : " << fullName << endl;
        cout << "Department    : " << department << endl;
        cout << "Hourly rate   : " << hourlyRate << endl;
        cout << "Worked hours  : " << workedHours << endl;
        cout << "Base pay      : " << basePay << endl;
        cout << "Bonus         : " << monthlyBonus << endl;
        cout << "Gross pay     : " << calculateGrossPay() << endl;
    }
};


// ============================================================
// SALES EMPLOYEE
// ============================================================

class SalesEmployee : public Employee {
private:
    double baseSalary;
    double salesRevenue;
    double commissionRate;

public:
    // Short constructor
    SalesEmployee(const string& employeeId,
                  const string& fullName)
        : Employee(employeeId, fullName),
          baseSalary(0),
          salesRevenue(0),
          commissionRate(0) {}

    // Full constructor
    SalesEmployee(const string& employeeId,
                  const string& fullName,
                  const string& department,
                  double baseSalary,
                  double salesRevenue,
                  double commissionRate)
        : Employee(employeeId, fullName, department),
          baseSalary(baseSalary),
          salesRevenue(salesRevenue),
          commissionRate(commissionRate) {

        if (baseSalary < 0)
            throw invalid_argument(
                "Base salary cannot be negative."
            );

        if (salesRevenue < 0)
            throw invalid_argument(
                "Sales revenue cannot be negative."
            );

        if (commissionRate < 0 || commissionRate > 0.3)
            throw invalid_argument(
                "Commission rate must be between 0 and 0.3."
            );
    }

    double calculateGrossPay() const override {
        return baseSalary
             + salesRevenue * commissionRate
             + monthlyBonus;
    }

    string getEmployeeType() const override {
        return "SalesEmployee";
    }

    void updateSalesRevenue(double newRevenue) {
        if (newRevenue < 0)
            throw invalid_argument(
                "Sales revenue cannot be negative."
            );

        salesRevenue = newRevenue;
    }

    void displayPayrollInfo() const override {
        cout << fixed << setprecision(0);

        double commission =
            salesRevenue * commissionRate;

        cout << "\n----------------------------------\n";
        cout << "Employee type : " << getEmployeeType() << endl;
        cout << "ID            : " << employeeId << endl;
        cout << "Name          : " << fullName << endl;
        cout << "Department    : " << department << endl;
        cout << "Base salary   : " << baseSalary << endl;
        cout << "Sales revenue : " << salesRevenue << endl;
        cout << "Commission    : " << commission << endl;
        cout << "Bonus         : " << monthlyBonus << endl;
        cout << "Gross pay     : " << calculateGrossPay() << endl;
    }
};


// ============================================================
// PAYROLL
// ============================================================

class Payroll {
private:
    string period;

    vector<Employee*> employees;

public:
    Payroll(const string& period)
        : period(period) {}

    // Destructor
    ~Payroll() {
        for (Employee* employee : employees) {
            delete employee;
        }
    }

    // ========================================================
    // ADD EMPLOYEE
    // ========================================================

    void addEmployee(Employee* employee) {

        if (employee == nullptr)
            throw invalid_argument(
                "Employee cannot be null."
            );

        if (findEmployee(employee->getEmployeeId()) != nullptr)
            throw invalid_argument(
                "Employee ID already exists."
            );

        employees.push_back(employee);
    }

    // ========================================================
    // FIND EMPLOYEE
    // ========================================================

    Employee* findEmployee(const string& employeeId) const {

        for (Employee* employee : employees) {

            if (employee->getEmployeeId() == employeeId)
                return employee;
        }

        return nullptr;
    }

    // ========================================================
    // TOTAL PAYROLL
    // ========================================================

    double calculateTotalPayroll() const {

        double total = 0;

        for (Employee* employee : employees) {
            total += employee->calculateGrossPay();
        }

        return total;
    }

    // ========================================================
    // PAYROLL BY DEPARTMENT
    // ========================================================

    double calculatePayrollByDepartment(
        const string& department) const {

        double total = 0;

        for (Employee* employee : employees) {

            if (employee->getDepartment() == department) {
                total += employee->calculateGrossPay();
            }
        }

        return total;
    }

    // ========================================================
    // HIGHEST PAID EMPLOYEE
    // ========================================================

    Employee* findHighestPaidEmployee() const {

        if (employees.empty())
            return nullptr;

        Employee* highest = employees[0];

        for (Employee* employee : employees) {

            if (employee->calculateGrossPay()
                > highest->calculateGrossPay()) {

                highest = employee;
            }
        }

        return highest;
    }

    // ========================================================
    // DISPLAY PAYROLL
    // ========================================================

    void displayPayroll() const {

        cout << "\n========================================\n";
        cout << "              PAYROLL\n";
        cout << "Period: " << period << endl;
        cout << "========================================\n";

        if (employees.empty()) {
            cout << "Payroll is empty.\n";
            return;
        }

        for (Employee* employee : employees) {
            employee->displayPayrollInfo();
        }

        cout << "\n========================================\n";
        cout << "TOTAL PAYROLL: "
             << fixed << setprecision(0)
             << calculateTotalPayroll()
             << endl;
        cout << "========================================\n";
    }
};


// ============================================================
// MAIN - TEST DATA
// ============================================================

int main() {

    try {

        Payroll payroll("2026-09");

        // ----------------------------------------------------
        // E001
        // ----------------------------------------------------

        auto* e1 = new SalariedEmployee(
            "E001",
            "Nguyen Minh An",
            "Dao tao",
            15000000,
            2000000
        );

        // Bonus: 1,000,000
        e1->addBonus(1000000);

        payroll.addEmployee(e1);


        // ----------------------------------------------------
        // E002
        // ----------------------------------------------------

        auto* e2 = new HourlyEmployee(
            "E002",
            "Tran Thu Binh",
            "Ho tro",
            100000,
            150
        );

        e2->addBonus(
            500000,
            "Monthly performance"
        );

        payroll.addEmployee(e2);


        // ----------------------------------------------------
        // E003
        // ----------------------------------------------------

        auto* e3 = new HourlyEmployee(
            "E003",
            "Le Hoang Chi",
            "Ho tro",
            100000,
            170
        );

        payroll.addEmployee(e3);


        // ----------------------------------------------------
        // E004
        // ----------------------------------------------------

        auto* e4 = new SalesEmployee(
            "E004",
            "Pham Quoc Dung",
            "Kinh doanh",
            8000000,
            200000000,
            0.05
        );

        // 2% of 50,000,000
        e4->addBonus(
            0.02,
            50000000,
            "Sales bonus"
        );

        payroll.addEmployee(e4);


        // ----------------------------------------------------
        // DISPLAY
        // ----------------------------------------------------

        payroll.displayPayroll();


        // ----------------------------------------------------
        // TOTAL PAYROLL
        // ----------------------------------------------------

        cout << "\nTotal payroll = "
             << fixed << setprecision(0)
             << payroll.calculateTotalPayroll()
             << endl;


        // ----------------------------------------------------
        // DEPARTMENT
        // ----------------------------------------------------

        cout << "Ho tro payroll = "
             << payroll.calculatePayrollByDepartment(
                    "Ho tro"
                )
             << endl;


        // ----------------------------------------------------
        // HIGHEST PAID
        // ----------------------------------------------------

        Employee* highest =
            payroll.findHighestPaidEmployee();

        if (highest != nullptr) {

            cout << "\nHighest paid employee:\n";
            cout << highest->getEmployeeId()
                 << " - "
                 << highest->getFullName()
                 << " - "
                 << highest->calculateGrossPay()
                 << endl;
        }


        // ----------------------------------------------------
        // TEST FIND
        // ----------------------------------------------------

        Employee* found =
            payroll.findEmployee("E003");

        if (found != nullptr) {

            cout << "\nFound employee: "
                 << found->getFullName()
                 << endl;
        }

    }
    catch (const exception& e) {

        cerr << "Error: "
             << e.what()
             << endl;
    }

    return 0;
}