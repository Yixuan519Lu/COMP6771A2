#include "./filtered_string_view.h"

#include <catch2/catch.hpp>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace fsv;

TEST_CASE("Default Constructor") {
	auto sv = filtered_string_view{};
	REQUIRE(sv.size() == 0);
}

TEST_CASE("Implicit String Constructor") {
	auto s = std::string{"cat"};
	auto sv = filtered_string_view{s};
	REQUIRE(sv.size() == 3);
}

TEST_CASE("String Constructor with Predicate") {
	auto s = std::string{"cat"};
	auto pred = [](const char& c) { return c == 'a'; };
	auto sv = filtered_string_view{s, pred};
	REQUIRE(sv.size() == 1);
}

TEST_CASE("Implicit Null-Terminated String Constructor") {
	auto sv = filtered_string_view{"cat"};
	REQUIRE(sv.size() == 3);
}

TEST_CASE("Null-Terminated String with Predicate Constructor") {
	auto pred = [](const char& c) { return c == 'a'; };
	auto sv = filtered_string_view{"cat", pred};
	REQUIRE(sv.size() == 1);
}

TEST_CASE("Copy Constructor and Move Constructor") {
	auto sv1 = filtered_string_view{"bulldog"};
	const auto copy = sv1;
	REQUIRE(copy.data() == sv1.data());
	const auto move = std::move(sv1);
	REQUIRE(sv1.data() == nullptr);
}

TEST_CASE("Subscript Operator with Predicate") {
	auto pred = [](const char& c) { return c == '9' || c == '0' || c == ' '; };
	auto fsv1 = filtered_string_view{"only 90s kids understand", pred};
	REQUIRE(fsv1[2] == '0');
}

TEST_CASE("at()") {
	auto vowels = std::set<char>{'a', 'A', 'e', 'E', 'i', 'I', 'o', 'O', 'u', 'U'};
	auto is_vowel = [&vowels](const char& c) { return vowels.contains(c); };
	auto sv = filtered_string_view{"Malamute", is_vowel};
	REQUIRE(sv.at(0) == 'a');

	auto sv_empty = filtered_string_view{""};
	try {
		[[maybe_unused]] const char& result = sv_empty.at(0);
	} catch (const std::domain_error& e) {
		REQUIRE(std::string(e.what()) == "filtered_string_view::at(0): invalid index");
	}
}

TEST_CASE("String Type Conversion") {
	auto sv = filtered_string_view("vizsla");
	auto s = static_cast<std::string>(sv);
	REQUIRE(s == "vizsla");
	REQUIRE(sv.data() != s.data());
}

TEST_CASE("Equality Comparison") {
	auto lo = filtered_string_view{"aaa"};
	auto hi = filtered_string_view{"zzz"};
	auto lolo = filtered_string_view{"aaa"};
	REQUIRE(lo != hi);
	REQUIRE(lo == lolo);
}

TEST_CASE("Relational Comparison") {
	auto const lo = filtered_string_view{"aaa"};
	auto const hi = filtered_string_view{"zzz"};
	REQUIRE((lo < hi) == true);
	REQUIRE((lo <= hi) == true);
	REQUIRE((lo > hi) == false);
	REQUIRE((lo >= hi) == false);
	REQUIRE((lo <=> hi) == std::strong_ordering::less);
}

TEST_CASE("Predicate") {
	auto filter_pred = [](const char& c) { return !(c == 'v'); };
	auto sv = filtered_string_view{"vizsla", filter_pred};
	const auto& predicate = sv.predicate();
	REQUIRE(!predicate('v'));
	REQUIRE(predicate('w'));
}

TEST_CASE("Compose Function") {
	auto best_languages = filtered_string_view{"c / c++"};
	auto vf = std::vector<filter>{[](const char& c) { return c == 'c' || c == '+' || c == '/'; },
	                              [](const char& c) { return c > ' '; },
	                              [](const char&) { return true; }};

	auto sv = compose(best_languages, vf);
	std::ostringstream test_os_stream;
	test_os_stream << sv;
	REQUIRE(test_os_stream.str() == "c/c++");
	std::string result = static_cast<std::string>(sv);
	REQUIRE(result == "c/c++");
}

TEST_CASE("Output Stream") {
	auto fsv = filtered_string_view{"c++ > rust > java", [](const char& c) { return c == 'c' || c == '+'; }};
	std::ostringstream test_os_stream;
	test_os_stream << fsv;
	REQUIRE(test_os_stream.str() == "c++");
}

TEST_CASE("Count Filtered Chars Before Index") {
	auto fsv = fsv::filtered_string_view("example string with spaces", [](const char& c) { return c != ' '; });
	SECTION("Count filtered characters before index in filtered string") {
		REQUIRE(fsv.count_filtered_chars_before(0) == 0);
		REQUIRE(fsv.count_filtered_chars_before(7) == 0);
		REQUIRE(fsv.count_filtered_chars_before(8) == 1);
		REQUIRE(fsv.count_filtered_chars_before(14) == 2);
		REQUIRE(fsv.count_filtered_chars_before(19) == 3);
		REQUIRE(fsv.count_filtered_chars_before(25) == 3);
	}
}
TEST_CASE("Substr Function") {
	auto sv = fsv::filtered_string_view{"Siberian Husky"};
	auto result = fsv::substr(sv, 9);
	REQUIRE(result == "Husky");
	auto is_upper = [](const char& c) { return std::isupper(static_cast<unsigned char>(c)); };
	auto sv_upper = fsv::filtered_string_view{"Sled Dog", is_upper};
	auto result_upper = fsv::substr(sv_upper, 0, 2);
	REQUIRE(result_upper == "SD");
	auto result_out_of_range = fsv::substr(sv, 20);
	REQUIRE(result_out_of_range == "");
	auto result2 = fsv::substr(sv, 5, 0);
	REQUIRE(result2 == "ian Husky");
	auto fsv = fsv::filtered_string_view("example string with spaces", [](const char& c) { return c != ' '; });
	auto result3 = fsv::substr(fsv, 8, 0);
	REQUIRE(result3 == "tringwithspaces");
}
TEST_CASE("Split Function") {
	auto interest = std::set<char>{'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F', ' ', '/'};
	auto sv = fsv::filtered_string_view{"0xDEADBEEF / 0xdeadbeef / 0xDEAD",
	                                    [&interest](const char& c) { return interest.contains(c); }};
	auto tok = fsv::filtered_string_view{" / "};
	auto v = fsv::split(sv, tok);

	std::ostringstream os1, os2, os3;
	os1 << v[0];
	os2 << v[1];
	os3 << v[2];
	REQUIRE(os1.str() == "DEADBEEF");
	REQUIRE(os2.str() == "deadbeef");
	REQUIRE(os3.str() == "DEAD");
}

TEST_CASE("Split Empty") {
	auto sv = fsv::filtered_string_view{"xax"};
	auto tok = fsv::filtered_string_view{"x"};
	auto v = fsv::split(sv, tok);
	auto expected = std::vector<fsv::filtered_string_view>{"", "a", ""};
	CHECK(v == expected);

	sv = fsv::filtered_string_view{"xx"};
	tok = fsv::filtered_string_view{"x"};
	v = fsv::split(sv, tok);
	expected = std::vector<fsv::filtered_string_view>{"", "", ""};
	CHECK(v == expected);
}

TEST_CASE("Filtered String View Iterator") {
	const auto s1 = fsv::filtered_string_view{"puppy", [](const char& c) { return !(c == 'u' || c == 'y'); }};
	auto v1 = std::vector<char>{s1.begin(), s1.end()};
	REQUIRE(v1.size() == 3);
	CHECK(v1[0] == 'p');
	CHECK(v1[1] == 'p');
	CHECK(v1[2] == 'p');

	const auto s2 = fsv::filtered_string_view{"milo", [](const char& c) { return !(c == 'i' || c == 'o'); }};
	auto v2 = std::vector<char>{s2.rbegin(), s2.rend()};
	REQUIRE(v2.size() == 2);
	CHECK(v2[0] == 'l');
	CHECK(v2[1] == 'm');
}

TEST_CASE("With default predicate") {
	auto fsv1 = fsv::filtered_string_view{"corgi"};
	std::ostringstream output;
	std::copy(fsv1.begin(), fsv1.end(), std::ostream_iterator<char>(output, " "));
	CHECK(output.str() == "c o r g i ");
}

TEST_CASE("With predicate which removes lowercase vowels") {
	auto fsv = fsv::filtered_string_view{"samoyed", [](const char& c) {
		                                     return !(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
	                                     }};
	auto it = fsv.begin();
	CHECK(*it == 's');
	CHECK(*std::next(it) == 'm');
	CHECK(*std::next(it, 2) == 'y');
	CHECK(*std::next(it, 3) == 'd');
}

TEST_CASE("Reverse with predicate which removes lowercase vowels") {
	auto fsv = fsv::filtered_string_view{"samoyed", [](const char& c) {
		                                     return !(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
	                                     }};
	auto it = fsv.cend();
	CHECK(*std::prev(it) == 'd');
	CHECK(*std::prev(it, 2) == 'y');
	CHECK(*std::prev(it, 3) == 'm');
	CHECK(*std::prev(it, 4) == 's');
}

TEST_CASE("Check if constructors are noexcept") {
	static_assert(noexcept(fsv::filtered_string_view()), "Default constructor is not noexcept");
}

TEST_CASE("default_predicate returns true for all characters") {
	for (char c = std::numeric_limits<char>::min(); c < std::numeric_limits<char>::max(); ++c) {
		REQUIRE(default_predicate(c));
	}
	REQUIRE(default_predicate(std::numeric_limits<char>::max()));
}