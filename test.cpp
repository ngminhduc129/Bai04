/****************/
/* MSSV: 202418872 */
/* Ho ten: Nguyen Minh Duc */
/****************/

#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

#define main payroll_demo_main
#include "main.cpp"
#undef main

// ============================================================
// MINI TEST FRAMEWORK
// Each result line: ID | Description | Expected | Actual | PASS/FAIL
// ============================================================

static int g_pass = 0;
static int g_fail = 0;

// Format money with dots: 15000000 -> 15.000.000
static string money(double value) {
    long long n = llround(value);
    string digits = to_string(n < 0 ? -n : n);
    string out;
    int count = 0;
    for (int i = (int)digits.size() - 1; i >= 0; --i) {
        out.insert(out.begin(), digits[i]);
        if (++count % 3 == 0 && i > 0) out.insert(out.begin(), '.');
    }
    return (n < 0 ? "-" : "") + out;
}

static void report(const string& id, const string& desc, const string& expected,
                   const string& actual, bool ok) {
    (ok ? g_pass : g_fail)++;
    cout << id << " | " << desc << " | " << expected << " | " << actual << " | "
         << (ok ? "PASS" : "FAIL") << "\n";
}

static void checkMoney(const string& id, const string& desc, double expected, double actual) {
    report(id, desc, money(expected), money(actual), fabs(expected - actual) < 1e-6);
}

static void checkText(const string& id, const string& desc, const string& expected,
                      const string& actual) {
    report(id, desc, expected, actual, expected == actual);
}

// Expect std::invalid_argument
template <typename F>
static void checkThrows(const string& id, const string& desc, F f) {
    try {
        f();
        report(id, desc, "throws invalid_argument", "no exception", false);
    } catch (const invalid_argument&) {
        report(id, desc, "throws invalid_argument", "throws invalid_argument", true);
    } catch (const exception& ex) {
        report(id, desc, "throws invalid_argument", string("other: ") + ex.what(), false);
    }
}

// Expect NO exception (valid boundary value)
template <typename F>
static void checkNoThrow(const string& id, const string& desc, F f) {
    try {
        f();
        report(id, desc, "valid, no exception", "no exception", true);
    } catch (const exception& ex) {
        report(id, desc, "valid, no exception", string("throws: ") + ex.what(), false);
    }
}

// Run f() with cout silenced (addBonus/displayPayroll print to cout)
template <typename F>
static void quiet(F f) {
    ostringstream sink;
    streambuf* old = cout.rdbuf(sink.rdbuf());
    try {
        f();
    } catch (...) {
        cout.rdbuf(old);
        throw;
    }
    cout.rdbuf(old);
}

// ============================================================
// TESTS
// ============================================================

int main() {
    cout << "===== TEST RESULTS =====\n";
    cout << "ID | Test case | Expected | Actual | Result\n";

    // ---------- Sample data from the assignment (part C) ----------
    // Payroll owns the employees (its destructor deletes them).
    Payroll payroll("2026-09");

    auto* e1 = new SalariedEmployee("E001", "Nguyen Minh An", "Dao tao", 15000000, 2000000);
    e1->addBonus(1000000);                                    // overload (1)
    auto* e2 = new HourlyEmployee("E002", "Tran Thu Binh", "Ho tro", 100000, 150);
    quiet([&] { e2->addBonus(500000, "Monthly performance"); });  // overload (2)
    auto* e3 = new HourlyEmployee("E003", "Le Hoang Chi", "Ho tro", 100000, 170);
    auto* e4 = new SalesEmployee("E004", "Pham Quoc Dung", "Kinh doanh", 8000000, 200000000, 0.05);
    quiet([&] { e4->addBonus(0.02, 50000000, "Sales bonus"); });  // overload (3)

    payroll.addEmployee(e1);
    payroll.addEmployee(e2);
    payroll.addEmployee(e3);
    payroll.addEmployee(e4);

    checkMoney("TC01", "E001 salaried: 15m + 2m allowance + 1m bonus", 18000000, e1->calculateGrossPay());
    checkMoney("TC02", "E002 hourly 150h (no overtime) + 500k bonus", 15500000, e2->calculateGrossPay());
    checkMoney("TC03", "E003 hourly 170h (10h overtime x1.5)", 17500000, e3->calculateGrossPay());
    checkMoney("TC04", "E004 sales: 8m + 5% x 200m + 2% x 50m", 19000000, e4->calculateGrossPay());
    checkMoney("TC05", "Total payroll of 4 employees", 70000000, payroll.calculateTotalPayroll());
    checkMoney("TC06", "Department 'Ho tro' total (E002 + E003)", 33000000,
               payroll.calculatePayrollByDepartment("Ho tro"));
    checkText("TC07", "Highest paid employee", "E004", payroll.findHighestPaidEmployee()->getEmployeeId());
    checkText("TC08", "findEmployee(\"E003\") returns right person", "Le Hoang Chi",
              payroll.findEmployee("E003")->getFullName());
    checkText("TC09", "findEmployee(\"E999\") not found", "nullptr",
              payroll.findEmployee("E999") == nullptr ? "nullptr" : "not null");
    checkMoney("TC10", "Unknown department total = 0", 0, payroll.calculatePayrollByDepartment("Khong co"));

    // ---------- Payroll rules ----------
    checkThrows("TC11", "Duplicate employee ID E001 is rejected",
                [&] { payroll.addEmployee(new SalariedEmployee("E001", "Trung Ma")); });
    checkThrows("TC12", "addEmployee(nullptr)", [&] { payroll.addEmployee(nullptr); });
    {
        Payroll empty("2026-10");
        checkMoney("TC13", "Empty payroll: total = 0", 0, empty.calculateTotalPayroll());
        checkText("TC14", "Empty payroll: highest paid = nullptr", "nullptr",
                  empty.findHighestPaidEmployee() == nullptr ? "nullptr" : "not null");
        checkMoney("TC15", "Empty payroll: department total = 0", 0,
                   empty.calculatePayrollByDepartment("Ho tro"));
        checkNoThrow("TC16", "Empty payroll: displayPayroll() does not crash",
                     [&] { quiet([&] { empty.displayPayroll(); }); });
    }
    checkNoThrow("TC17", "displayPayroll() with data (polymorphic display)",
                 [&] { quiet([&] { payroll.displayPayroll(); }); });

    // ---------- HourlyEmployee boundaries ----------
    checkMoney("TC18", "Boundary: exactly 160h -> no overtime", 16000000,
               HourlyEmployee("T1", "A", "D", 100000, 160).calculateGrossPay());
    checkMoney("TC19", "Boundary: 161h -> 160h + 1h x1.5", 16150000,
               HourlyEmployee("T2", "A", "D", 100000, 161).calculateGrossPay());
    checkMoney("TC20", "Boundary: 0h -> 0", 0,
               HourlyEmployee("T3", "A", "D", 100000, 0).calculateGrossPay());
    checkMoney("TC21", "Boundary: 250h -> 160h + 90h x1.5", 29500000,
               HourlyEmployee("T4", "A", "D", 100000, 250).calculateGrossPay());
    checkThrows("TC22", "Worked hours 251 (> 250)", [] { HourlyEmployee("T5", "A", "D", 100000, 251); });
    checkThrows("TC23", "Worked hours -1", [] { HourlyEmployee("T6", "A", "D", 100000, -1); });
    checkThrows("TC24", "Negative hourly rate", [] { HourlyEmployee("T7", "A", "D", -1, 100); });

    // ---------- SalesEmployee boundaries ----------
    checkNoThrow("TC25", "Boundary: commission rate 0", [] { SalesEmployee("S1", "A", "D", 1, 1, 0.0); });
    checkNoThrow("TC26", "Boundary: commission rate 0.3", [] { SalesEmployee("S2", "A", "D", 1, 1, 0.3); });
    checkThrows("TC27", "Commission rate 0.31 (> 0.3)", [] { SalesEmployee("S3", "A", "D", 1, 1, 0.31); });
    checkThrows("TC28", "Commission rate -0.01", [] { SalesEmployee("S4", "A", "D", 1, 1, -0.01); });
    checkThrows("TC29", "Negative sales revenue", [] { SalesEmployee("S5", "A", "D", 1, -1, 0.1); });
    checkThrows("TC30", "Negative base salary", [] { SalesEmployee("S6", "A", "D", -1, 1, 0.1); });

    // ---------- SalariedEmployee / common validation ----------
    checkThrows("TC31", "Negative monthly salary", [] { SalariedEmployee("X1", "A", "D", -1, 0); });
    checkThrows("TC32", "Negative allowance", [] { SalariedEmployee("X2", "A", "D", 1, -1); });
    checkThrows("TC33", "Empty employee ID", [] { SalariedEmployee("", "A"); });
    checkThrows("TC34", "Empty full name", [] { SalariedEmployee("X3", ""); });
    checkThrows("TC35", "Empty department", [] { SalariedEmployee("X4", "A", "", 1, 0); });

    // ---------- Default values of short constructors ----------
    {
        SalariedEmployee s("D1", "Default");
        checkText("TC36", "Short ctor: department = Unassigned", "Unassigned", s.getDepartment());
        checkMoney("TC37", "Short ctor: bonus = 0 and gross pay = 0", 0, s.calculateGrossPay());
        checkMoney("TC38", "HourlyEmployee short ctor: gross pay = 0", 0, HourlyEmployee("D2", "A").calculateGrossPay());
        checkMoney("TC39", "SalesEmployee short ctor: gross pay = 0", 0, SalesEmployee("D3", "A").calculateGrossPay());
    }

    // ---------- addBonus overloading ----------
    {
        SalariedEmployee s("B1", "Bonus Test", "D", 0, 0);
        s.addBonus(1000000);                                         // (1)
        quiet([&] { s.addBonus(500000, "Attendance"); });             // (2)
        quiet([&] { s.addBonus(0.1, 2000000, "10% of reference"); }); // (3)
        checkMoney("TC40", "addBonus (1)+(2)+(3): 1m + 500k + 10% x 2m", 1700000, s.getMonthlyBonus());

        checkThrows("TC41", "addBonus(0): amount must be > 0", [&] { s.addBonus(0); });
        checkThrows("TC42", "addBonus(-5): negative amount", [&] { s.addBonus(-5); });
        checkThrows("TC43", "addBonus(100, \"\"): empty reason", [&] { s.addBonus(100, ""); });
        checkThrows("TC44", "addBonus(0, 1000, \"x\"): rate 0", [&] { s.addBonus(0.0, 1000, "x"); });
        checkThrows("TC45", "addBonus(0.51, 1000, \"x\"): rate > 0.5", [&] { s.addBonus(0.51, 1000, "x"); });
        checkThrows("TC46", "addBonus(0.1, 0, \"x\"): reference amount 0", [&] { s.addBonus(0.1, 0, "x"); });
        checkThrows("TC47", "addBonus(0.1, 1000, \"\"): empty reason", [&] { s.addBonus(0.1, 1000, ""); });
        checkMoney("TC48", "Failed addBonus calls leave bonus unchanged", 1700000, s.getMonthlyBonus());

        checkNoThrow("TC49", "Boundary: bonus rate 0.5", [&] { quiet([&] { s.addBonus(0.5, 1000, "x"); }); });
        checkMoney("TC50", "Bonus after rate 0.5 x 1000", 1700500, s.getMonthlyBonus());

        s.resetBonus();
        checkMoney("TC51", "resetBonus(): bonus back to 0", 0, s.getMonthlyBonus());
    }

    // ---------- updateSalesRevenue ----------
    {
        SalesEmployee s("U1", "Update", "KD", 8000000, 100000000, 0.05);
        s.updateSalesRevenue(200000000);
        checkMoney("TC52", "updateSalesRevenue(200m): 8m + 5% x 200m", 18000000, s.calculateGrossPay());
        checkThrows("TC53", "updateSalesRevenue(-1)", [&] { s.updateSalesRevenue(-1); });
        checkMoney("TC54", "Failed update leaves revenue unchanged", 18000000, s.calculateGrossPay());
    }

    // ---------- Polymorphism through Employee* ----------
    {
        unique_ptr<Employee> p = make_unique<HourlyEmployee>("P1", "Poly", "D", 100000, 170);
        checkText("TC55", "Employee*: getEmployeeType() is the derived version", "HourlyEmployee",
                  p->getEmployeeType());
        checkMoney("TC56", "Employee*: calculateGrossPay() is the derived version", 17500000,
                   p->calculateGrossPay());
    }

    cout << "\nSummary: " << g_pass << " PASS, " << g_fail << " FAIL\n";
    return g_fail == 0 ? 0 : 1;
}