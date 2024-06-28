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
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = char;
			using difference_type = std::ptrdiff_t;
			using pointer = void;
			using reference = const char&;
			iter();
			iter(const filtered_string_view* fsv, std::size_t pos);
			auto operator*() const -> reference;
			auto operator->() const -> pointer;
			auto operator++() -> iter&;
			auto operator++(int) -> iter;
			auto operator--() -> iter&;
			auto operator--(int) -> iter;
			friend auto operator==(const iter& lhs, const iter& rhs) -> bool;
			friend auto operator!=(const iter& lhs, const iter& rhs) -> bool;

		 private:
			const filtered_string_view* fsv;
			std::size_t pos;
			auto move_to_next_filtered_char() -> void;
		};
		class reverse_iter {
		 public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = char;
			using difference_type = std::ptrdiff_t;
			using pointer = void;
			using reference = const char&;
			reverse_iter();
			reverse_iter(const filtered_string_view* fsv, std::size_t pos);
			auto operator*() const -> reference;
			auto operator->() const -> pointer;
			auto operator++() -> reverse_iter&;
			auto operator++(int) -> reverse_iter;
			auto operator--() -> reverse_iter&;
			auto operator--(int) -> reverse_iter;
			friend auto operator==(const reverse_iter& lhs, const reverse_iter& rhs) -> bool;
			friend auto operator!=(const reverse_iter& lhs, const reverse_iter& rhs) -> bool;

		 private:
			const filtered_string_view* fsv;
			std::size_t pos;
			auto move_to_next_filtered_char() -> void;
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

		using iterator = iter;
		auto begin() const -> iter;
		auto end() const -> iter;

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
	filtered_string_view substr(const filtered_string_view& fsv, std::size_t pos = 0, std::size_t count = 0);
} // namespace fsv

#endif // COMP6771_ASS2_FSV_H
