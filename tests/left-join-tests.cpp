#include <Lz/LeftJoin.hpp>
#include <catch2/catch.hpp>

struct Customer {
    int id;
};

struct PaymentBill {
    int customerId;
    int id;
};


TEST_CASE("Left join changing and creating elements", "[LeftJoin][Basic functionality]") {
    std::vector<Customer> customers {
        Customer{25},
        Customer{1},
        Customer{39},
        Customer{103},
        Customer{99},
    };
    std::vector<PaymentBill> paymentBills {
        PaymentBill{25, 0},
        PaymentBill{25, 2},
        PaymentBill{25, 3},
        PaymentBill{99, 1},
        PaymentBill{2523, 52},
        PaymentBill{2523, 53},
    };

    auto joined = lz::leftJoin(paymentBills, customers,
                               [](const PaymentBill& p) { return p.customerId; },
                               [](const Customer& c) { return c.id; },
                               [](const PaymentBill& p, const Customer& c) { return std::make_tuple(p, c); });

    SECTION("Should initialized with first match") {
        std::tuple<PaymentBill, Customer> match = *joined.begin();
        Customer& customer = std::get<1>(match);
        PaymentBill& paymentBill = std::get<0>(match);

        CHECK(customer.id == paymentBill.customerId);
        CHECK(customer.id == 25);
        CHECK(paymentBill.id == 0);
    }
}

TEST_CASE("Left join binary operations", "[LeftJoin][Binary ops]") {
    std::vector<Customer> customers {
        Customer{25},
        Customer{1},
        Customer{39},
        Customer{103},
        Customer{99},
    };
    std::vector<PaymentBill> paymentBills {
        PaymentBill{25, 0},
        PaymentBill{25, 2},
        PaymentBill{25, 3},
        PaymentBill{99, 1},
        PaymentBill{2523, 52},
        PaymentBill{2523, 53},
    };

    auto joined = lz::leftJoin(paymentBills, customers,
                               [](const PaymentBill& p) { return p.customerId; },
                               [](const Customer& c) { return c.id; },
                               [](const PaymentBill& p, const Customer& c) { return std::make_tuple(p, c); });
    auto it = joined.begin();

    SECTION("Operator++") {
        CHECK(std::distance(joined.begin(), joined.end()) == 4);
        ++it;
        Customer customer = std::get<1>(*it);
        PaymentBill paymentBill = std::get<0>(*it);
        CHECK(customer.id == 25);
        CHECK(paymentBill.customerId == 25);
        CHECK(paymentBill.id == 2);
    }

    SECTION("Operator== & operator!=") {
        CHECK(it != joined.end());
        it = joined.end();
        CHECK(it == joined.end());
    }
}