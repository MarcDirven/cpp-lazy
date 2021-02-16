#include "Lz/GroupBy.hpp"

int main() {
	std::vector<std::string> vec = {
		"hello", "hellp", "i'm", "done"
	};

	auto grouper = lz::groupBy(vec, [](const std::string& s) { return s.length(); });

	for (auto&& pair : grouper) {
		fmt::print("String length group: {}\n", pair.first);
		for (const auto& str : pair.second) {
			fmt::print("{} belongs in group {}\n", pair.first, str);
		}
	}

	/* Output:
	 * String length group: 3
	 * 3 belongs in group i'm
	 * String length group: 4
	 * 4 belongs in group done
	 * String length group: 5
	 * 5 belongs in group hello
	 * 5 belongs in group hellp
	 * */

	fmt::print("\n");

	auto grouperDesc = lz::groupByDescending(vec, [](const std::string& s) { return s.length(); });
	for (auto&& pair : grouperDesc) {
		fmt::print("String length group: {}\n", pair.first);
		for (const auto& str : pair.second) {
			fmt::print("{} belongs in group {}\n", pair.first, str);
		}
	}
	/*String length group: 5
	 * 5 belongs in group hello
	 * 5 belongs in group hellp
	 * String length group: 4
	 * 4 belongs in group done
	 * String length group: 3
	 * 3 belongs in group i'm
	 * */
}
