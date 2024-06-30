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
			using pointer = const char*;
			using reference = const char&;
			iter();
			iter(const filtered_string_view* fsv, std::size_t pos);
			auto operator*() const -> reference;
			auto operator->() const -> pointer;
			auto operator++() -> iter&;
			auto operator++(int) -> iter;
			auto operator--() -> iter&;
			auto operator--(int) -> iter;
			friend auto operator==(const iter& lhs, const iter& rhs) -> bool {
				return lhs.fsv == rhs.fsv and lhs.pos == rhs.pos;
			}
			friend auto operator!=(const iter& lhs, const iter& rhs) -> bool {
				return !(lhs == rhs);
			}

		 private:
			const filtered_string_view* fsv;
			std::size_t pos;
		};
		class const_iter {
		 public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = char;
			using difference_type = std::ptrdiff_t;
			using pointer = const char*;
			using reference = const char&;
			const_iter();
			const_iter(const filtered_string_view* fsv, std::size_t pos);
			auto operator*() const -> reference;
			auto operator->() const -> pointer;
			auto operator++() -> const_iter&;
			auto operator++(int) -> const_iter;
			auto operator--() -> const_iter&;
			auto operator--(int) -> const_iter;
			friend auto operator==(const const_iter& lhs, const const_iter& rhs) -> bool {
				return lhs.fsv == rhs.fsv and lhs.pos == rhs.pos;
			}
			friend auto operator!=(const const_iter& lhs, const const_iter& rhs) -> bool {
				return !(lhs == rhs);
			}

		 private:
			const filtered_string_view* fsv;
			std::size_t pos;
		};

	 public:
		filtered_string_view() noexcept;
		filtered_string_view(const std::string& str) noexcept;
		filtered_string_view(const std::string& str, filter predicate) noexcept;
		filtered_string_view(const char* str) noexcept;
		filtered_string_view(const char* str, filter predicate) noexcept;
		filtered_string_view(const char* str, std::size_t str_len, filter predicate) noexcept;
		filtered_string_view(const filtered_string_view& other) noexcept;
		filtered_string_view(filtered_string_view&& other) noexcept;
		auto operator=(const filtered_string_view& other) -> filtered_string_view&;
		auto operator=(filtered_string_view&& other) -> filtered_string_view&;
		~filtered_string_view();
		static filter default_predicate;
		auto at(std::size_t n) const -> const char&;
		auto operator[](std::size_t n) const -> const char&;
		auto size() const -> std::size_t;
		auto empty() const -> bool;
		auto data() const -> const char*;
		auto predicate() const -> const filter&;
		explicit operator std::string() const;
		auto count_filtered_chars_before(std::size_t index) const -> std::size_t;
		using iterator = iter;
		using const_iterator = const_iter;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		auto begin() const -> iterator;
		auto end() const -> iterator;
		auto rbegin() const -> reverse_iterator;
		auto rend() const -> reverse_iterator;
		auto cbegin() const -> const_iterator;
		auto cend() const -> const_iterator;
		auto crbegin() const -> const_reverse_iterator;
		auto crend() const -> const_reverse_iterator;

	 private:
		const char* ptr;
		std::size_t str_length;
		filter str_pred;
	};
	auto operator==(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool;
	auto operator<=>(const filtered_string_view& lhs, const filtered_string_view& rhs) -> std::strong_ordering;
	auto operator<<(std::ostream& os, const filtered_string_view& fsv) -> std::ostream&;
	auto compose(const filtered_string_view& fsv, const std::vector<filter>& filts) -> filtered_string_view;
	auto split(const filtered_string_view& fsv, const filtered_string_view& tok) -> std::vector<filtered_string_view>;
	auto substr(const filtered_string_view& fsv, std::size_t pos = 0, std::size_t count = 0) -> filtered_string_view;
} // namespace fsv
#endif // COMP6771_ASS2_FSV_H