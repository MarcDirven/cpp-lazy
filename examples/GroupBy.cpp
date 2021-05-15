#include "Lz/GroupBy.hpp"

int main() {
	std::vector<std::string> vec = {
		"hello", "hellp", "i'm", "done"
	};

	std::sort(vec.begin(), vec.end(), [](const std::string& a, const std::string& b) { return a.length() < b.length(); });
	auto grouper = lz::groupBy(vec, [](const std::string& a, const std::string& b) { return a.length() == b.length(); });

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

	std::sort(vec.begin(), vec.end(), [](const std::string& a, const std::string& b) { return a.length() > b.length(); });
	auto grouperDesc = lz::groupBy(vec, [](const std::string& a, const std::string& b) { return a.length() == b.length(); });
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
