#if !defined(LZ_HAS_STRING_VIEW)

#include <Lz/StringView.hpp>

#include <catch2/catch.hpp>

TEST_CASE("String view basic functionality", "[String view][Basic functionality]") {
    const std::string str = "Hello world";
    lz::StringView view(str.data(), str.size());

    SECTION("Should have correct size") {
        CHECK(view.size() == str.size());
    }

    SECTION("Should have correct data") {
        CHECK(view.data() == str.data());
    }

    SECTION("Should have correct length") {
        CHECK(view.length() == str.size());
    }

    SECTION("Should have correct begin") {
        CHECK(view.begin() == str.data());
    }

    SECTION("Should have correct end") {
        CHECK(view.end() == str.data() + str.size());
    }

    SECTION("Should convert to string") {
        CHECK(view.toStdString() == str);
    }

    SECTION("Use static_cast") {
        CHECK(static_cast<std::string>(view) == str);
    }

    SECTION("Prefix and suffix removal") {
        view = lz::StringView("Hello world");
        view.remove_prefix(6);
        CHECK(view == "world");
        view = lz::StringView("Hello world");
        view.remove_suffix(5);
        CHECK(view == "Hello");
    }

    SECTION("Contains") {
        CHECK(view.contains("Hello"));
    }

    SECTION("Find") {
        CHECK(view.find("world") == 6);
        CHECK(view.find("Hello") == 0);
        CHECK(view.find("zzz") == lz::StringView::npos);
    }

    SECTION("Operator==") {
        CHECK(view == str);
    }

    SECTION("Operator!=") {
        view = lz::StringView("Hello");
        CHECK(view != "Hello world!");
    }

    SECTION("Should have correct end") {
        CHECK(view.end() == str.data() + str.size());
    }

    SECTION("Should have correct operator[]") {
        CHECK(view[0] == str[0]);
        CHECK(view[1] == str[1]);
        CHECK(view[2] == str[2]);
        CHECK(view[3] == str[3]);
        CHECK(view[4] == str[4]);
        CHECK(view[5] == str[5]);
        CHECK(view[6] == str[6]);
        CHECK(view[7] == str[7]);
        CHECK(view[8] == str[8]);
        CHECK(view[9] == str[9]);
    }
}	
#endif