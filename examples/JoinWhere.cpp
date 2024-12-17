#include <Lz/JoinWhere.hpp>

struct customer {
    int id;
};

struct payment_bill {
    int customer_id;
    int id;
};

int main() {
    std::vector<customer> customers{
        customer{ 25 }, customer{ 1 }, customer{ 39 }, customer{ 103 }, customer{ 99 },
    };
    std::vector<payment_bill> payment_bills{
        payment_bill{ 25, 0 }, payment_bill{ 25, 2 },  payment_bill{ 25, 3 },
        payment_bill{ 99, 1 }, payment_bill{ 252, 1 }, payment_bill{ 252, 1 },
    };

    if (customers.size() > payment_bills.size()) {
        std::sort(payment_bills.begin(), payment_bills.end(),
                  [](const payment_bill& a, const payment_bill& b) { return a.customer_id < b.customer_id; });
        auto joined = lz::join_where(
            customers, payment_bills, [](const customer& p) { return p.id; }, [](const payment_bill& c) { return c.customer_id; },
            [](const customer& p, const payment_bill& c) { return std::make_tuple(p, c); });

        for (std::tuple<customer, payment_bill> join : joined) {
            fmt::print("{} and {} are the same. The corresponding payment bill id is {}\n", std::get<0>(join).id,
                       std::get<1>(join).customer_id, std::get<1>(join).id);
        }
        /* // Output:
         25 and 25 are the same. The corresponding payment bill id is 0
         25 and 25 are the same. The corresponding payment bill id is 2
         25 and 25 are the same. The corresponding payment bill id is 3
         99 and 99 are the same. The corresponding payment bill id is 1
         */
    }
    // payment_bills sequence is larger, sort customers instead
    else {
        std::sort(customers.begin(), customers.end(), [](const customer& a, const customer& b) { return a.id < b.id; });
        auto joined = lz::join_where(
            payment_bills, customers, [](const payment_bill& p) { return p.customer_id; }, [](const customer& c) { return c.id; },
            [](const payment_bill& p, const customer& c) { return std::make_tuple(p, c); });

        for (std::tuple<payment_bill, customer> join : joined) {
            fmt::print("{} and {} are the same. The corresponding payment bill id is {}\n",
					   std::get<1>(join).id, std::get<0>(join).customer_id, std::get<0>(join).id);
        }
        /* // Output:
		 25 and 25 are the same. The corresponding payment bill id is 0
		 25 and 25 are the same. The corresponding payment bill id is 2
		 25 and 25 are the same. The corresponding payment bill id is 3
		 99 and 99 are the same. The corresponding payment bill id is 1
		 */
	}
}
