#include "./filtered_string_view.h"

#include <catch2/catch.hpp>
#include <set>
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
	REQUIRE(sv[0] == 'a');

	auto sv_empty = filtered_string_view{""};
	try {
		sv_empty.at(0);
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
	REQUIRE(lo != hi);
}

TEST_CASE("Compose Function") {
	auto best_languages = filtered_string_view{"c / c++"};
	auto vf = std::vector<filter>{[](const char& c) { return c == 'c' || c == '+' || c == '/'; },
	                              [](const char& c) { return c > ' '; },
	                              [](const char&) { return true; }};

	auto sv = compose(best_languages, vf);
	REQUIRE(static_cast<std::string>(sv) == "c/c++");
}