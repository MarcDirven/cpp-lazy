#include <Lz/group_by.hpp>

int main() {
	std::vector<std::string> vec = {
		"hello", "hellp", "i'm", "done"
	};

	std::sort(vec.begin(), vec.end(), [](const std::string& a, const std::string& b) { return a.length() < b.length(); });
	auto grouper = lz::group_by(vec, [](const std::string& a, const std::string& b) { return a.length() == b.length(); });

	for (auto&& pair : grouper) {
		fmt::print("String length group: {}\n", pair.first.length());
		for (const auto& str : pair.second) {
			fmt::print("value: {}\n", str);
		}
	}

	/* Output:
	String length group: 3
	value: i'm
	String length group: 4
	value: done
	String length group: 5
	value: hello
	value: hellp
	 * */

	fmt::print("\n");

	std::sort(vec.begin(), vec.end(), [](const std::string& a, const std::string& b) { return a.length() > b.length(); });
	auto grouper_desc = lz::group_by(vec, [](const std::string& a, const std::string& b) { return a.length() == b.length(); });
	for (auto&& pair : grouper_desc) {
		fmt::print("String length group: {}\n", pair.first.length());
		for (const auto& str : pair.second) {
			fmt::print("value: {}\n", str);
		}
	}
	/* Output:
	String length group: 5
	value: hello
	value: hellp
	String length group: 4
	value: done
	String length group: 3
	value: i'm
	 * */
}
