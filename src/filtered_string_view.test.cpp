#include "./filtered_string_view.h"

#include <catch2/catch.hpp>

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