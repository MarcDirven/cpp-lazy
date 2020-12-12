#include <Lz/JoinWhere.hpp>

struct Customer {
    int id;
};

struct PaymentBill {
    int customerId;
    int id;
};

int main() {
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

    auto joined = lz::joinWhere(customers, paymentBills,
                                [](const Customer& p) { return p.id; },
                                [](const PaymentBill& c) { return c.customerId; },
                                [](const Customer& p, const PaymentBill& c) { return std::make_tuple(p, c); });

    for (std::tuple<Customer, PaymentBill> join : joined) {
        fmt::print("{} and {} are the same. The corresponding payment bill id is {}\n",
                   std::get<0>(join).id, std::get<1>(join).customerId, std::get<1>(join).id);
    }
}
