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
    std::vector<Customer> customers{
        Customer{25},
        Customer{1},
        Customer{39},
        Customer{103},
        Customer{99},
    };
    std::vector<PaymentBill> paymentBills{
        PaymentBill{25, 0},
        PaymentBill{25, 2},
        PaymentBill{25, 3},
        PaymentBill{99, 1},
        PaymentBill{2523, 52},
        PaymentBill{2523, 53},
    };

    auto joined = lz::leftJoin(customers, paymentBills,
                               [](const Customer& p) { return p.id; },
                               [](const PaymentBill& c) { return c.customerId; },
                               [](const Customer& p, const PaymentBill& c) { return std::make_tuple(p, c); });
    
    SECTION("Should initialized with first match") {
        std::tuple<Customer, PaymentBill> match = *joined.begin();
        Customer& customer = std::get<0>(match);
        PaymentBill& paymentBill = std::get<1>(match);

        CHECK(customer.id == paymentBill.customerId);
        CHECK(customer.id == 25);
        CHECK(paymentBill.id == 0);
    }
}

TEST_CASE("Left join binary operations", "[LeftJoin][Binary ops]") {
    std::vector<Customer> customers{
        Customer{25},
        Customer{1},
        Customer{39},
        Customer{103},
        Customer{99},
    };
    std::vector<PaymentBill> paymentBills{
        PaymentBill{25, 0},
        PaymentBill{25, 2},
        PaymentBill{25, 3},
        PaymentBill{99, 1},
        PaymentBill{2523, 52},
        PaymentBill{2523, 53},
    };

    auto joined = lz::leftJoin(customers, paymentBills,
                               [](const Customer& p) { return p.id; },
                               [](const PaymentBill& c) { return c.customerId; },
                               [](const Customer& p, const PaymentBill& c) { return std::make_tuple(p, c); });
    auto it = joined.begin();

    SECTION("Operator++") {
        CHECK(std::distance(joined.begin(), joined.end()) == 4);
        ++it;
        Customer customer = std::get<0>(*it);
        PaymentBill paymentBill = std::get<1>(*it);
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

TEST_CASE("LeftJoin to containers", "[LeftJoin][To container]") {
    std::vector<Customer> customers{
        Customer{25},
        Customer{1},
        Customer{39},
        Customer{103},
        Customer{99},
    };
    std::vector<PaymentBill> paymentBills{
        PaymentBill{25, 0},
        PaymentBill{25, 2},
        PaymentBill{25, 3},
        PaymentBill{99, 1},
        PaymentBill{2523, 52},
        PaymentBill{2523, 53},
    };

    auto joined = lz::leftJoin(customers, paymentBills,
                               [](const Customer& p) { return p.id; },
                               [](const PaymentBill& c) { return c.customerId; },
                               [](const Customer& p, const PaymentBill& c) { return std::make_tuple(p, c); });

    SECTION("To array") {
        std::array<std::tuple<Customer, PaymentBill>, 4> expected = {
            std::make_tuple(Customer{25}, PaymentBill{25, 0}),
            std::make_tuple(Customer{25}, PaymentBill{25, 2}),
            std::make_tuple(Customer{25}, PaymentBill{25, 3}),
            std::make_tuple(Customer{99}, PaymentBill{99, 1})
        };

        auto array = joined.toArray<4>();
        CHECK(std::equal(array.begin(), array.end(), expected.begin(),
                         [](const std::tuple<Customer, PaymentBill>& a, const std::tuple<Customer, PaymentBill>& b) {
                             auto& aFst = std::get<0>(a);
                             auto& aSnd = std::get<1>(a);
                             auto& bFst = std::get<0>(b);
                             auto& bSnd = std::get<1>(b);
                             return aFst.id == bFst.id && aSnd.id == bSnd.id && aSnd.customerId == bSnd.customerId;
                         })
        );
    }

    SECTION("To vector") {
        std::vector<std::tuple<Customer, PaymentBill>> expected = {
            std::make_tuple(Customer{25}, PaymentBill{25, 0}),
            std::make_tuple(Customer{25}, PaymentBill{25, 2}),
            std::make_tuple(Customer{25}, PaymentBill{25, 3}),
            std::make_tuple(Customer{99}, PaymentBill{99, 1})
        };

        auto vec = joined.toVector();
        CHECK(std::equal(vec.begin(), vec.end(), expected.begin(),
                         [](const std::tuple<Customer, PaymentBill>& a, const std::tuple<Customer, PaymentBill>& b) {
                             auto& aFst = std::get<0>(a);
                             auto& aSnd = std::get<1>(a);
                             auto& bFst = std::get<0>(b);
                             auto& bSnd = std::get<1>(b);
                             return aFst.id == bFst.id && aSnd.id == bSnd.id && aSnd.customerId == bSnd.customerId;
                         })
        );
    }

    SECTION("To other container using to<>()") {
        std::list<std::tuple<Customer, PaymentBill>> expected = {
            std::make_tuple(Customer{25}, PaymentBill{25, 0}),
            std::make_tuple(Customer{25}, PaymentBill{25, 2}),
            std::make_tuple(Customer{25}, PaymentBill{25, 3}),
            std::make_tuple(Customer{99}, PaymentBill{99, 1})
        };

        auto list = joined.to<std::list>();
        CHECK(std::equal(list.begin(), list.end(), expected.begin(),
                         [](const std::tuple<Customer, PaymentBill>& a, const std::tuple<Customer, PaymentBill>& b) {
                             auto& aFst = std::get<0>(a);
                             auto& aSnd = std::get<1>(a);
                             auto& bFst = std::get<0>(b);
                             auto& bSnd = std::get<1>(b);
                             return aFst.id == bFst.id && aSnd.id == bSnd.id && aSnd.customerId == bSnd.customerId;
                         })
        );
    }

    SECTION("To map") {
        using Pair = std::pair<int, std::tuple<Customer, PaymentBill>>;

        std::map<int, std::tuple<Customer, PaymentBill>> expected = {
            {0, std::make_tuple(Customer{25}, PaymentBill{25, 0})},
            {2, std::make_tuple(Customer{25}, PaymentBill{25, 2})},
            {3, std::make_tuple(Customer{25}, PaymentBill{25, 3})},
            {1, std::make_tuple(Customer{99}, PaymentBill{99, 1})}
        };
        decltype(expected) actual = joined.toMap([](const std::tuple<Customer, PaymentBill>& val) { return std::get<1>(val).id;});
        CHECK(std::equal(expected.begin(), expected.end(), actual.begin(), [](const Pair& a, const Pair& b) {
            return a.first == b.first &&
                   std::get<1>(a.second).id == std::get<1>(b.second).id &&
                   std::get<1>(a.second).customerId == std::get<1>(b.second).customerId;
        }));
    }

    SECTION("To unordered map") {
        using Pair = std::pair<int, std::tuple<Customer, PaymentBill>>;

        std::unordered_map<int, std::tuple<Customer, PaymentBill>> expected = {
            {0, std::make_tuple(Customer{25}, PaymentBill{25, 0})},
            {2, std::make_tuple(Customer{25}, PaymentBill{25, 2})},
            {3, std::make_tuple(Customer{25}, PaymentBill{25, 3})},
            {1, std::make_tuple(Customer{99}, PaymentBill{99, 1})}
        };
        decltype(expected) actual =
            joined.toUnorderedMap([](const std::tuple<Customer, PaymentBill>& val) { return std::get<1>(val).id;});
        CHECK(std::equal(expected.begin(), expected.end(), actual.begin(), [](const Pair& a, const Pair& b) {
            return a.first == b.first &&
                   std::get<1>(a.second).id == std::get<1>(b.second).id &&
                   std::get<1>(a.second).customerId == std::get<1>(b.second).customerId;
        }));
    }
}