#ifndef COMP6771_ASS2_FSV_H
#define COMP6771_ASS2_FSV_H

#include <compare>
#include <cstring>
#include <functional>
#include <iostream>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>

namespace fsv {
	using filter = std::function<bool(const char&)>;
	class filtered_string_view {
		class iter {
		 public:
			using MEMBER_TYPEDEFS_GO_HERE = void;

			iter();

			auto operator*() const -> void; // change this
			auto operator->() const -> void; // change this

			auto operator++() -> iter&;
			auto operator++(int) -> iter;
			auto operator--() -> iter&;
			auto operator--(int) -> iter;

			friend auto operator==(const iter&, const iter&) -> bool;
			friend auto operator!=(const iter&, const iter&) -> bool;

		 private:
			/* Implementation-specific private members */
		};

	 public:
		filtered_string_view();
		filtered_string_view(const std::string& str);
		filtered_string_view(const std::string& str, filter predicate);
		filtered_string_view(const char* str);
		filtered_string_view(const char* str, filter predicate);
		filtered_string_view(const char* str, std::size_t str_len, filter predicate);
		filtered_string_view(const filtered_string_view& other);
		filtered_string_view(filtered_string_view&& other) noexcept;
		filtered_string_view& operator=(const filtered_string_view& other);
		filtered_string_view& operator=(filtered_string_view&& other) noexcept;
		~filtered_string_view();
		static filter default_predicate;
		const char& at(std::size_t n) const;
		const char& operator[](std::size_t n) const;
		std::size_t size() const;
		bool empty() const;
		const char* data() const;
		const filter& predicate() const;
		explicit operator std::string() const;
		std::size_t count_filtered_chars_before(std::size_t index) const;

	 private:
		const char* ptr;
		std::size_t str_length;
		filter str_pred;
	};
	bool operator==(const filtered_string_view& lhs, const filtered_string_view& rhs);
	auto operator<=>(const filtered_string_view& lhs, const filtered_string_view& rhs) -> std::strong_ordering;
	auto operator<<(std::ostream& os, const filtered_string_view& fsv) -> std::ostream&;
	filtered_string_view compose(const filtered_string_view& fsv, const std::vector<filter>& filts);
	auto split(const filtered_string_view& fsv, const filtered_string_view& tok) -> std::vector<filtered_string_view>;
} // namespace fsv

#endif // COMP6771_ASS2_FSV_H
