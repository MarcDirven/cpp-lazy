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
		PaymentBill{252, 1},
		PaymentBill{252, 1},
	};

	if (std::distance(customers.begin(), customers.end()) > std::distance(paymentBills.begin(), paymentBills.end())) {
		std::sort(paymentBills.begin(), paymentBills.end(), [](const PaymentBill& a, const PaymentBill& b) { return a.customerId < b.customerId; });
		auto joined = lz::joinWhere(customers, paymentBills,
									[](const Customer& p) { return p.id; },
									[](const PaymentBill& c) { return c.customerId; },
									[](const Customer& p, const PaymentBill& c) { return std::make_tuple(p, c); });

		for (std::tuple<Customer, PaymentBill> join : joined) {
			fmt::print("{} and {} are the same. The corresponding payment bill id is {}\n",
					   std::get<0>(join).id, std::get<1>(join).customerId, std::get<1>(join).id);
		}
		/* // Output:
		 25 and 25 are the same. The corresponding payment bill id is 0
		 25 and 25 are the same. The corresponding payment bill id is 2
		 25 and 25 are the same. The corresponding payment bill id is 3
		 99 and 99 are the same. The corresponding payment bill id is 1
		 */
	}
	// paymentBills sequence is larger, sort customers instead
	else {
		std::sort(customers.begin(), customers.end(), [](const Customer& a, const Customer& b) { return a.id < b.id; });
		auto joined = lz::joinWhere(paymentBills, customers,
									[](const PaymentBill& p) { return p.customerId; },
									[](const Customer& c) { return c.id; },
									[](const PaymentBill& p, const Customer& c) { return std::make_tuple(p, c); });

		for (std::tuple<PaymentBill, Customer> join : joined) {
			fmt::print("{} and {} are the same. The corresponding payment bill id is {}\n",
					   std::get<1>(join).id, std::get<0>(join).customerId, std::get<0>(join).id);
		}
		/* // Output:
		 25 and 25 are the same. The corresponding payment bill id is 0
		 25 and 25 are the same. The corresponding payment bill id is 2
		 25 and 25 are the same. The corresponding payment bill id is 3
		 99 and 99 are the same. The corresponding payment bill id is 1
		 */
	}
}
