#include <Lz/c_string.hpp>
#include <Lz/join_where.hpp>
#include <catch2/catch.hpp>
#include <list>

struct customer {
    int id;
};

struct payment_bill {
    int customer_id;
    int id;
};

TEST_CASE("Join where with sentinels") {
    auto c_str = lz::c_string("To join on");
    auto sorted_seq = lz::c_string("Toxzzzz");
    auto joined = lz::join_where(
        c_str, sorted_seq, [](char c) { return c; }, [](char c) { return c; },
        [](char c, char c2) { return std::make_tuple(c, c2); });
    static_assert(!std::is_same<decltype(joined.begin()), decltype(joined.end())>::value, "Should be sentinel");
    std::vector<std::tuple<char, char>> expected = {
        std::make_tuple('T', 'T'),
        std::make_tuple('o', 'o'),
        std::make_tuple('o', 'o'),
        std::make_tuple('o', 'o'),
    };
    auto vec = joined.to_vector();
    CHECK(vec == expected);
}

TEST_CASE("Left join changing and creating elements", "[join_where_iterable][Basic functionality]") {
    std::vector<customer> customers{
        customer{ 25 }, customer{ 1 }, customer{ 39 }, customer{ 103 }, customer{ 99 },
    };
    std::vector<payment_bill> payment_bills{
        payment_bill{ 25, 0 }, payment_bill{ 25, 2 },    payment_bill{ 25, 3 },
        payment_bill{ 99, 1 }, payment_bill{ 2523, 52 }, payment_bill{ 2523, 53 },
    };

    auto joined = lz::join_where(
        customers, payment_bills, [](const customer& p) { return p.id; }, [](const payment_bill& c) { return c.customer_id; },
        [](const customer& p, const payment_bill& c) { return std::make_tuple(p, c); });

    SECTION("Should initialized with first match") {
        std::tuple<customer, payment_bill> match = *joined.begin();
        customer& customer = std::get<0>(match);
        payment_bill& payment_bill = std::get<1>(match);

        CHECK(customer.id == payment_bill.customer_id);
        CHECK(customer.id == 25);
        CHECK(payment_bill.id == 0);
    }
}

TEST_CASE("Left join binary operations", "[join_where_iterable][Binary ops]") {
    std::vector<customer> customers{
        customer{ 25 }, customer{ 1 }, customer{ 39 }, customer{ 103 }, customer{ 99 },
    };
    std::vector<payment_bill> payment_bills{
        payment_bill{ 25, 0 }, payment_bill{ 25, 2 },    payment_bill{ 25, 3 },
        payment_bill{ 99, 1 }, payment_bill{ 2523, 52 }, payment_bill{ 2523, 53 },
    };

    auto joined = lz::join_where(
        customers, payment_bills, [](const customer& p) { return p.id; }, [](const payment_bill& c) { return c.customer_id; },
        [](const customer& p, const payment_bill& c) { return std::make_tuple(p, c); });
    auto it = joined.begin();

    SECTION("Operator++") {
        CHECK(lz::distance(joined.begin(), joined.end()) == 4);
        ++it;
        customer customer = std::get<0>(*it);
        payment_bill payment_bill = std::get<1>(*it);
        CHECK(customer.id == 25);
        CHECK(payment_bill.customer_id == 25);
        CHECK(payment_bill.id == 2);
    }

    SECTION("Operator== & operator!=") {
        CHECK(it != joined.end());
        while (it != joined.end()) {
            ++it;
        }
        CHECK(it == joined.end());
    }
}

TEST_CASE("join_where_iterable to containers", "[join_where_iterable][To container]") {
    std::vector<customer> customers{
        customer{ 25 }, customer{ 1 }, customer{ 39 }, customer{ 103 }, customer{ 99 },
    };
    std::vector<payment_bill> payment_bills{
        payment_bill{ 25, 0 }, payment_bill{ 25, 2 },    payment_bill{ 25, 3 },
        payment_bill{ 99, 1 }, payment_bill{ 2523, 52 }, payment_bill{ 2523, 53 },
    };

    auto joined = lz::join_where(
        customers, payment_bills, [](const customer& p) { return p.id; }, [](const payment_bill& c) { return c.customer_id; },
        [](const customer& p, const payment_bill& c) { return std::make_tuple(p, c); });

    SECTION("To array") {
        std::array<std::tuple<customer, payment_bill>, 4> expected = { std::make_tuple(customer{ 25 }, payment_bill{ 25, 0 }),
                                                                       std::make_tuple(customer{ 25 }, payment_bill{ 25, 2 }),
                                                                       std::make_tuple(customer{ 25 }, payment_bill{ 25, 3 }),
                                                                       std::make_tuple(customer{ 99 }, payment_bill{ 99, 1 }) };

        auto array = joined.to<std::array<std::tuple<customer, payment_bill>, 4>>();
        CHECK(std::equal(array.begin(), array.end(), expected.begin(),
                         [](const std::tuple<customer, payment_bill>& a, const std::tuple<customer, payment_bill>& b) {
                             auto& a_fst = std::get<0>(a);
                             auto& a_snd = std::get<1>(a);
                             auto& b_fst = std::get<0>(b);
                             auto& b_snd = std::get<1>(b);
                             return a_fst.id == b_fst.id && a_snd.id == b_snd.id && a_snd.customer_id == b_snd.customer_id;
                         }));
    }

    SECTION("To vector") {
        std::vector<std::tuple<customer, payment_bill>> expected = { std::make_tuple(customer{ 25 }, payment_bill{ 25, 0 }),
                                                                     std::make_tuple(customer{ 25 }, payment_bill{ 25, 2 }),
                                                                     std::make_tuple(customer{ 25 }, payment_bill{ 25, 3 }),
                                                                     std::make_tuple(customer{ 99 }, payment_bill{ 99, 1 }) };

        auto vec = joined.to_vector();
        CHECK(std::equal(vec.begin(), vec.end(), expected.begin(),
                         [](const std::tuple<customer, payment_bill>& a, const std::tuple<customer, payment_bill>& b) {
                             auto& a_fst = std::get<0>(a);
                             auto& a_snd = std::get<1>(a);
                             auto& b_fst = std::get<0>(b);
                             auto& b_snd = std::get<1>(b);
                             return a_fst.id == b_fst.id && a_snd.id == b_snd.id && a_snd.customer_id == b_snd.customer_id;
                         }));
    }

    SECTION("To other container using to<>()") {
        std::list<std::tuple<customer, payment_bill>> expected = { std::make_tuple(customer{ 25 }, payment_bill{ 25, 0 }),
                                                                   std::make_tuple(customer{ 25 }, payment_bill{ 25, 2 }),
                                                                   std::make_tuple(customer{ 25 }, payment_bill{ 25, 3 }),
                                                                   std::make_tuple(customer{ 99 }, payment_bill{ 99, 1 }) };

        auto list = joined.to<std::list<std::tuple<customer, payment_bill>>>();
        CHECK(std::equal(list.begin(), list.end(), expected.begin(),
                         [](const std::tuple<customer, payment_bill>& a, const std::tuple<customer, payment_bill>& b) {
                             auto& a_fst = std::get<0>(a);
                             auto& a_snd = std::get<1>(a);
                             auto& b_fst = std::get<0>(b);
                             auto& b_snd = std::get<1>(b);
                             return a_fst.id == b_fst.id && a_snd.id == b_snd.id && a_snd.customer_id == b_snd.customer_id;
                         }));
    }

    SECTION("To map") {
        using pair = std::pair<int, std::tuple<customer, payment_bill>>;

        std::map<int, std::tuple<customer, payment_bill>> expected = {
            { 0, std::make_tuple(customer{ 25 }, payment_bill{ 25, 0 }) },
            { 2, std::make_tuple(customer{ 25 }, payment_bill{ 25, 2 }) },
            { 3, std::make_tuple(customer{ 25 }, payment_bill{ 25, 3 }) },
            { 1, std::make_tuple(customer{ 99 }, payment_bill{ 99, 1 }) }
        };

        decltype(expected) actual =
            joined.to_map([](const std::tuple<customer, payment_bill>& val) { return std::make_pair(std::get<1>(val).id, val); });

        CHECK(std::equal(expected.begin(), expected.end(), actual.begin(), [](const pair& a, const pair& b) {
            return a.first == b.first && std::get<1>(a.second).id == std::get<1>(b.second).id &&
                   std::get<1>(a.second).customer_id == std::get<1>(b.second).customer_id;
        }));
    }

    SECTION("To unordered map") {
        using pair = std::pair<int, std::tuple<customer, payment_bill>>;

        std::unordered_map<int, std::tuple<customer, payment_bill>> expected = {
            { 0, std::make_tuple(customer{ 25 }, payment_bill{ 25, 0 }) },
            { 2, std::make_tuple(customer{ 25 }, payment_bill{ 25, 2 }) },
            { 3, std::make_tuple(customer{ 25 }, payment_bill{ 25, 3 }) },
            { 1, std::make_tuple(customer{ 99 }, payment_bill{ 99, 1 }) }
        };

        decltype(expected) actual = joined.to_unordered_map(
            [](const std::tuple<customer, payment_bill>& val) { return std::make_pair(std::get<1>(val).id, val); });

        CHECK(std::equal(expected.begin(), expected.end(), actual.begin(), [](const pair& a, const pair& b) {
            return a.first == b.first && std::get<1>(a.second).id == std::get<1>(b.second).id &&
                   std::get<1>(a.second).customer_id == std::get<1>(b.second).customer_id;
        }));
    }
}