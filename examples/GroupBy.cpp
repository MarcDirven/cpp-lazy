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
}
