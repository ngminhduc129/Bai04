/****************/
/* MSSV: 202418872 */
/* Ho ten: Nguyen Minh Duc */
/****************/

#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "HourlyEmployee.h"
#include "Payroll.h"
#include "SalariedEmployee.h"
#include "SalesEmployee.h"

using std::make_unique;
using std::string;

// ---------------------------------------------------------------
// Khung kiểm thử tối giản. Mỗi dòng: Mã | Mô tả | Mong đợi | Thực tế | Kết quả
// ---------------------------------------------------------------
static int g_pass = 0, g_fail = 0;

static void report(const string& id, const string& desc, const string& expected,
                   const string& actual, bool ok) {
    (ok ? g_pass : g_fail)++;
    std::cout << id << " | " << desc << " | " << expected << " | " << actual << " | "
              << (ok ? "PASS" : "FAIL") << "\n";
}

// So sánh số tiền.
static void checkMoney(const string& id, const string& desc, double expected, double actual) {
    report(id, desc, Employee::formatMoney(expected), Employee::formatMoney(actual),
           std::fabs(expected - actual) < 1e-6);
}

// So sánh chuỗi / giá trị rời rạc.
static void checkText(const string& id, const string& desc, const string& expected,
                      const string& actual) {
    report(id, desc, expected, actual, expected == actual);
}

// Kỳ vọng: hàm ném std::invalid_argument.
template <typename F>
static void checkThrows(const string& id, const string& desc, F f) {
    try {
        f();
        report(id, desc, "ném invalid_argument", "không ném ngoại lệ", false);
    } catch (const std::invalid_argument&) {
        report(id, desc, "ném invalid_argument", "ném invalid_argument", true);
    }
}

// Kỳ vọng: hàm KHÔNG ném ngoại lệ (giá trị biên hợp lệ).
template <typename F>
static void checkNoThrow(const string& id, const string& desc, F f) {
    try {
        f();
        report(id, desc, "hợp lệ, không ném", "không ném ngoại lệ", true);
    } catch (const std::exception& ex) {
        report(id, desc, "hợp lệ, không ném", string("ném: ") + ex.what(), false);
    }
}

int main() {
    // ===========================================================
    // PHẦN 1: dựng dữ liệu mẫu theo đề (C)
    // ===========================================================
    Payroll payroll("2026-09");

    auto e1 = make_unique<SalariedEmployee>("E001", "Nguyễn Minh An", "Đào tạo",
                                            15000000, 2000000);
    e1->addBonus(1000000);  // thưởng cố định -> addBonus(amount)

    auto e2 = make_unique<HourlyEmployee>("E002", "Trần Thu Bình", "Hỗ trợ", 100000, 150);
    e2->addBonus(500000, "Hoàn thành dự án");  // addBonus(amount, reason)

    auto e3 = make_unique<HourlyEmployee>("E003", "Lê Hoàng Chi", "Hỗ trợ", 100000, 170);

    auto e4 = make_unique<SalesEmployee>("E004", "Phạm Quốc Dũng", "Kinh doanh",
                                         8000000, 200000000, 0.05);
    e4->addBonus(0.02, 50000000, "Thưởng theo doanh thu hợp đồng");  // addBonus(rate, ref, reason)

    payroll.addEmployee(std::move(e1));
    payroll.addEmployee(std::move(e2));
    payroll.addEmployee(std::move(e3));
    payroll.addEmployee(std::move(e4));

    // ===========================================================
    // PHẦN 2: bảng kiểm thử
    // ===========================================================
    std::cout << "===== KẾT QUẢ KIỂM THỬ =====\n";
    std::cout << "Mã | Tình huống | Mong đợi | Thực tế | Kết quả\n";

    // --- Kiểm thử theo dữ liệu mẫu ---
    checkMoney("TC01", "E001 lương cố định: 15tr + 2tr + thưởng 1tr", 18000000,
               payroll.findEmployee("E001")->calculateGrossPay());
    checkMoney("TC02", "E002 theo giờ 150h (không vượt 160h), thưởng 500k", 15500000,
               payroll.findEmployee("E002")->calculateGrossPay());
    checkMoney("TC03", "E003 theo giờ 170h (10h vượt, hệ số 1,5)", 17500000,
               payroll.findEmployee("E003")->calculateGrossPay());
    checkMoney("TC04", "E004 kinh doanh: 8tr + 5% x 200tr + 2% x 50tr", 19000000,
               payroll.findEmployee("E004")->calculateGrossPay());
    checkMoney("TC05", "Tổng bảng lương 4 nhân sự", 70000000, payroll.calculateTotalPayroll());
    checkMoney("TC06", "Tổng phòng Hỗ trợ (E002 + E003)", 33000000,
               payroll.calculatePayrollByDepartment("Hỗ trợ"));
    checkText("TC07", "Người thu nhập cao nhất", "E004",
              payroll.findHighestPaidEmployee()->getEmployeeId());

    // --- Trùng mã ---
    {
        bool added = payroll.addEmployee(make_unique<SalariedEmployee>("E001", "Người Trùng", 1));
        checkText("TC08", "Thêm nhân sự trùng mã E001", "false (bị từ chối)", added ? "true" : "false (bị từ chối)");
        checkText("TC09", "Số nhân sự sau khi thêm trùng", "4", std::to_string(payroll.size()));
    }

    // --- Bảng lương rỗng ---
    {
        Payroll empty("2026-10");
        checkMoney("TC10", "Bảng lương rỗng: tổng = 0", 0, empty.calculateTotalPayroll());
        checkText("TC11", "Bảng lương rỗng: người cao nhất", "nullptr",
                  empty.findHighestPaidEmployee() == nullptr ? "nullptr" : "khác nullptr");
        checkMoney("TC12", "Bảng lương rỗng: tổng theo phòng ban = 0", 0,
                   empty.calculatePayrollByDepartment("Hỗ trợ"));
    }
    checkMoney("TC13", "Phòng ban không tồn tại: tổng = 0", 0,
               payroll.calculatePayrollByDepartment("Không có"));
    checkText("TC14", "findEmployee mã không tồn tại", "nullptr",
              payroll.findEmployee("E999") == nullptr ? "nullptr" : "khác nullptr");
    checkThrows("TC15", "addEmployee(nullptr)", [&] { payroll.addEmployee(nullptr); });

    // --- Biên số giờ làm ---
    checkMoney("TC16", "Biên: đúng 160h -> 160 x 100k (không vượt)", 16000000,
               HourlyEmployee("T1", "A", "D", 100000, 160).calculateGrossPay());
    checkMoney("TC17", "Biên: 161h -> 160 x 100k + 1 x 100k x 1,5", 16150000,
               HourlyEmployee("T2", "A", "D", 100000, 161).calculateGrossPay());
    checkMoney("TC18", "Biên: 0h -> 0", 0,
               HourlyEmployee("T3", "A", "D", 100000, 0).calculateGrossPay());
    checkMoney("TC19", "Biên: 250h -> 160h + 90h vượt", 29500000,
               HourlyEmployee("T4", "A", "D", 100000, 250).calculateGrossPay());
    checkThrows("TC20", "Số giờ 251 (vượt tối đa 250)",
                [] { HourlyEmployee("T5", "A", "D", 100000, 251); });
    checkThrows("TC21", "Số giờ -1 (âm)", [] { HourlyEmployee("T6", "A", "D", 100000, -1); });
    checkThrows("TC22", "Đơn giá giờ âm", [] { HourlyEmployee("T7", "A", "D", -1, 100); });

    // --- Biên tỷ lệ hoa hồng ---
    checkNoThrow("TC23", "Biên: hoa hồng 0", [] { SalesEmployee("S1", "A", "D", 1, 1, 0.0); });
    checkNoThrow("TC24", "Biên: hoa hồng 0,3", [] { SalesEmployee("S2", "A", "D", 1, 1, 0.3); });
    checkThrows("TC25", "Hoa hồng 0,31 (vượt 0,3)", [] { SalesEmployee("S3", "A", "D", 1, 1, 0.31); });
    checkThrows("TC26", "Hoa hồng -0,01 (âm)", [] { SalesEmployee("S4", "A", "D", 1, 1, -0.01); });
    checkThrows("TC27", "Doanh số âm", [] { SalesEmployee("S5", "A", "D", 1, -1, 0.1); });

    // --- Kiểm tra dữ liệu lớp Employee / lớp dẫn xuất ---
    checkThrows("TC28", "Mã nhân sự rỗng", [] { SalariedEmployee("", "A", 1); });
    checkThrows("TC29", "Họ tên chỉ có khoảng trắng", [] { SalariedEmployee("X1", "   ", 1); });
    checkThrows("TC30", "Phòng ban rỗng", [] { SalariedEmployee("X2", "A", "", 1, 0); });
    checkThrows("TC31", "Thưởng ban đầu âm", [] { SalariedEmployee("X3", "A", "D", 1, 0, -5); });
    checkThrows("TC32", "Lương tháng âm", [] { SalariedEmployee("X4", "A", -1); });
    checkThrows("TC33", "Phụ cấp âm", [] { SalariedEmployee("X5", "A", "D", 1, -1); });

    // --- Giá trị mặc định của constructor rút gọn ---
    {
        SalariedEmployee s("D1", "Mặc Định", 10000000);
        checkText("TC34", "Constructor rút gọn: phòng ban mặc định", "Unassigned", s.getDepartment());
        checkMoney("TC35", "Constructor rút gọn: thưởng & phụ cấp = 0 -> thu nhập = lương", 10000000,
                   s.calculateGrossPay());
    }

    // --- Nạp chồng addBonus: chọn đúng phiên bản ---
    {
        SalariedEmployee s("B1", "Thử Thưởng", "D", 0, 0);
        s.addBonus(1000000);                      // (1)
        s.addBonus(500000, "Thưởng chuyên cần");  // (2)
        s.addBonus(0.1, 2000000, "Thưởng 10%");   // (3)
        checkText("TC36", "Ba lời gọi addBonus (1),(2),(3) tạo 3 dòng lịch sử", "3",
                  std::to_string(s.getBonusHistory().size()));
        checkMoney("TC37", "Tổng thưởng 1tr + 500k + 10% x 2tr", 1700000, s.getMonthlyBonus());
        checkText("TC38", "Phiên bản (1) dùng lý do mặc định", "Thưởng cố định",
                  s.getBonusHistory()[0].reason);

        // Lỗi: trạng thái không đổi sau khi ném ngoại lệ.
        checkThrows("TC39", "addBonus(0) - số tiền không dương", [&] { s.addBonus(0); });
        checkThrows("TC40", "addBonus(-5) - số tiền âm", [&] { s.addBonus(-5); });
        checkThrows("TC41", "addBonus(100, \"\") - lý do rỗng", [&] { s.addBonus(100, ""); });
        checkThrows("TC42", "addBonus(100, \"  \") - lý do chỉ khoảng trắng", [&] { s.addBonus(100, "  "); });
        checkThrows("TC43", "addBonus(0, 1000, \"x\") - tỷ lệ 0", [&] { s.addBonus(0.0, 1000, "x"); });
        checkNoThrow("TC44", "Biên: tỷ lệ thưởng 0,5", [&] { s.addBonus(0.5, 1000, "x"); });
        checkThrows("TC45", "Tỷ lệ thưởng 0,51 (vượt 0,5)", [&] { s.addBonus(0.51, 1000, "x"); });
        checkThrows("TC46", "Giá trị tham chiếu 0", [&] { s.addBonus(0.1, 0, "x"); });
        checkThrows("TC47", "addBonus(0.1, 1000, \"\") - lý do rỗng", [&] { s.addBonus(0.1, 1000, ""); });
        checkMoney("TC48", "Sau 8 lần lỗi, tổng thưởng chỉ tăng do TC44 (500)", 1700500,
                   s.getMonthlyBonus());

        s.resetBonus();
        checkMoney("TC49", "resetBonus(): thưởng về 0 khi sang kỳ mới", 0, s.getMonthlyBonus());
    }

    // --- Cập nhật doanh số có kiểm soát ---
    {
        SalesEmployee s("U1", "Cập Nhật", "KD", 8000000, 100000000, 0.05);
        s.updateSalesRevenue(200000000);
        checkMoney("TC50", "updateSalesRevenue(200tr) -> 8tr + 10tr", 18000000, s.calculateGrossPay());
        checkThrows("TC51", "updateSalesRevenue(-1)", [&] { s.updateSalesRevenue(-1); });
        checkMoney("TC52", "Sau cập nhật lỗi, doanh số giữ nguyên", 200000000, s.getSalesRevenue());
    }

    // --- Đa hình qua con trỏ lớp cơ sở ---
    {
        std::unique_ptr<Employee> p = make_unique<HourlyEmployee>("P1", "Đa Hình", "D", 100000, 170);
        checkText("TC53", "Gọi qua Employee*: getEmployeeType() của lớp dẫn xuất", "HourlyEmployee",
                  p->getEmployeeType());
        checkMoney("TC54", "Gọi qua Employee*: calculateGrossPay() của lớp dẫn xuất", 17500000,
                   p->calculateGrossPay());
    }

    std::cout << "\nTổng kết: " << g_pass << " PASS, " << g_fail << " FAIL\n";
    return g_fail == 0 ? 0 : 1;
}