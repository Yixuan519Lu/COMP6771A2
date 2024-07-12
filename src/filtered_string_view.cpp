#include "./filtered_string_view.h"

namespace fsv {
	filtered_string_view::filtered_string_view() noexcept
	: ptr(nullptr)
	, str_length(0)
	, str_pred(default_predicate) {}
	filtered_string_view::filtered_string_view(const std::string& str, filter predicate) noexcept
	: ptr(str.data())
	, str_length(str.size())
	, str_pred(predicate) {}
	filtered_string_view::filtered_string_view(const char* str, filter predicate) noexcept
	: ptr(str)
	, str_length(std::strlen(str))
	, str_pred(predicate) {}
	filtered_string_view::filtered_string_view(const filtered_string_view& other) noexcept
	: ptr(other.ptr)
	, str_length(other.str_length)
	, str_pred(other.str_pred) {}
	filtered_string_view::filtered_string_view(filtered_string_view&& other) noexcept
	: ptr(other.ptr)
	, str_length(other.str_length)
	, str_pred(std::move(other.str_pred)) {
		other.ptr = nullptr;
		other.str_length = 0;
		other.str_pred = default_predicate;
	}
	filtered_string_view::filtered_string_view(const char* str, std::size_t str_len, filter predicate) noexcept
	: ptr(str)
	, str_length(str_len)
	, str_pred(predicate) {}
	auto filtered_string_view::operator=(const filtered_string_view& other) -> filtered_string_view& {
		if (this != &other) {
			ptr = other.ptr;
			str_length = other.str_length;
			str_pred = other.str_pred;
		}
		return *this;
	}
	auto filtered_string_view::operator=(filtered_string_view&& other) -> filtered_string_view& {
		if (this != &other) {
			ptr = other.ptr;
			str_length = other.str_length;
			str_pred = std::move(other.str_pred);
			other.ptr = nullptr;
			other.str_length = 0;
			other.str_pred = default_predicate;
		}
		return *this;
	}
	filtered_string_view::~filtered_string_view() {}
	auto filtered_string_view::at(std::size_t n) const -> const char& {
		std::size_t index = 0;
		for (std::size_t i = 0; i < str_length; i++) {
			if (str_pred(ptr[i])) {
				if (index == n) {
					return ptr[i];
				}
				index++;
			}
		}
		throw std::domain_error("filtered_string_view::at(" + std::to_string(n) + "): invalid index");
	}
	auto filtered_string_view::operator[](std::size_t n) const -> const char& {
		return this->at(n);
	}
	auto filtered_string_view::size() const -> std::size_t {
		std::size_t count = 0;
		for (std::size_t i = 0; i < str_length; i++) {
			if (str_pred(ptr[i])) {
				count++;
			}
		}
		return count;
	}
	auto filtered_string_view::empty() const -> bool {
		return size() == 0;
	}
	auto filtered_string_view::data() const -> const char* {
		return ptr;
	}
	auto filtered_string_view::predicate() const -> const filter& {
		return str_pred;
	}
	filtered_string_view::operator std::string() const {
		std::string conversion;
		conversion.reserve(str_length);
		for (std::size_t i = 0; i < str_length; i++) {
			if (str_pred(ptr[i])) {
				conversion.push_back(ptr[i]);
			}
		}
		return conversion;
	}
	auto compose(const filtered_string_view& fsv, const std::vector<filter>& filts) -> filtered_string_view {
		auto composed = [filts](const char& input_char) {
			for (const auto& filt : filts) {
				if (not filt(input_char)) {
					return false;
				}
			}
			return true;
		};
		return filtered_string_view(fsv.data(), composed);
	}
	auto operator==(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool {
		if (lhs.size() != rhs.size()) {
			return false;
		}
		for (std::size_t i = 0; i < rhs.size(); i++) {
			if (lhs[i] != rhs[i]) {
				return false;
			}
		}
		return true;
	}
	auto operator<=>(const filtered_string_view& lhs, const filtered_string_view& rhs) -> std::strong_ordering {
		std::size_t lhs_size = lhs.size();
		std::size_t rhs_size = rhs.size();
		for (std::size_t i = 0; i < std::min(lhs_size, rhs_size); i++) {
			if (auto cmp = lhs[i] <=> rhs[i]; cmp != std::strong_ordering::equal) {
				return cmp;
			}
		}
		return lhs_size <=> rhs_size;
	}
	auto operator<<(std::ostream& os, const filtered_string_view& fsv) -> std::ostream& {
		for (std::size_t i = 0; i < fsv.size(); i++) {
			os << fsv[i];
		}
		return os;
	}
	auto split(const filtered_string_view& fsv, const filtered_string_view& tok) -> std::vector<filtered_string_view> {
		std::vector<filtered_string_view> result;
		if (tok.empty() or fsv.empty() or fsv.size() < tok.size()) {
			result.push_back(fsv);
			return result;
		}
		std::size_t offset = 0;
		std::size_t end = fsv.size();
		std::size_t tok_size = tok.size();
		for (std::size_t i = 0; i <= end - tok_size;) {
			bool match = true;
			for (std::size_t j = 0; j < tok_size; j++) {
				if (fsv.at(i + j) != tok.at(j)) {
					match = false;
					break;
				}
			}
			if (match) {
				filtered_string_view temp(
				    fsv.data() + offset + fsv.count_filtered_chars_before(offset),
				    i - offset + fsv.count_filtered_chars_before(i) - fsv.count_filtered_chars_before(offset),
				    fsv.predicate());
				result.push_back(temp);
				offset = i + tok_size;
				i = offset;
			}
			else {
				i++;
			}
		}
		if (offset <= end) {
			filtered_string_view temp(
			    fsv.data() + offset + fsv.count_filtered_chars_before(offset),
			    end - offset + fsv.count_filtered_chars_before(end) - fsv.count_filtered_chars_before(offset),
			    fsv.predicate());
			result.push_back(temp);
		}
		return result;
	}
	auto filtered_string_view::count_filtered_chars_before(std::size_t index) const -> std::size_t {
		std::size_t filtered_count = 0;
		std::size_t raw_index = 0;
		for (std::size_t i = 0; i < str_length && raw_index < index; ++i) {
			if (!str_pred(ptr[i])) {
				filtered_count++;
			}
			else {
				raw_index++;
			}
		}
		return filtered_count;
	}
	auto substr(const filtered_string_view& fsv, std::size_t pos, std::size_t count) -> filtered_string_view {
		if (pos >= fsv.size()) {
			return filtered_string_view("", fsv.predicate());
		}
		std::size_t rcount = (count <= 0) ? fsv.size() - pos : count;
		const char* start_ptr = fsv.data() + pos + fsv.count_filtered_chars_before(pos);
		std::size_t original_length =
		    rcount + fsv.count_filtered_chars_before(pos + rcount) - fsv.count_filtered_chars_before(pos);
		return filtered_string_view(start_ptr, original_length, fsv.predicate());
	}
	filtered_string_view::iter::iter() noexcept
	: fsv(nullptr)
	, pos(0) {}
	filtered_string_view::iter::iter(const filtered_string_view* fsv, std::size_t pos) noexcept
	: fsv(fsv)
	, pos(pos) {}
	auto filtered_string_view::iter::operator*() const -> reference {
		return fsv->at(pos);
	}
	auto filtered_string_view::iter::operator->() const -> pointer {
		return &fsv->at(pos);
	}
	auto filtered_string_view::iter::operator++() -> iter& {
		++pos;
		return *this;
	}
	auto filtered_string_view::iter::operator++(int) -> iter {
		auto tmp = *this;
		++(*this);
		return tmp;
	}
	auto filtered_string_view::iter::operator--() -> iter& {
		--pos;
		return *this;
	}
	auto filtered_string_view::iter::operator--(int) -> iter {
		auto tmp = *this;
		--(*this);
		return tmp;
	}
	filtered_string_view::const_iter::const_iter() noexcept
	: fsv(nullptr)
	, pos(0) {}
	filtered_string_view::const_iter::const_iter(const filtered_string_view* fsv, std::size_t pos) noexcept
	: fsv(fsv)
	, pos(pos) {}
	auto filtered_string_view::const_iter::operator*() const -> reference {
		return fsv->at(pos);
	}
	auto filtered_string_view::const_iter::operator->() const -> pointer {
		return &fsv->at(pos);
	}
	auto filtered_string_view::const_iter::operator++() -> const_iter& {
		++pos;
		return *this;
	}
	auto filtered_string_view::const_iter::operator++(int) -> const_iter {
		auto const tmp = *this;
		++(*this);
		return tmp;
	}
	auto filtered_string_view::const_iter::operator--() -> const_iter& {
		--pos;
		return *this;
	}
	auto filtered_string_view::const_iter::operator--(int) -> const_iter {
		auto const tmp = *this;
		--(*this);
		return tmp;
	}
	auto filtered_string_view::begin() const -> iterator {
		return iterator(this, 0);
	}
	auto filtered_string_view::end() const -> iterator {
		return iterator(this, size());
	}
	auto filtered_string_view::rbegin() const -> reverse_iterator {
		return reverse_iterator(end());
	}
	auto filtered_string_view::rend() const -> reverse_iterator {
		return reverse_iterator(begin());
	}
	auto filtered_string_view::cbegin() const -> const_iterator {
		return const_iterator(this, 0);
	}
	auto filtered_string_view::cend() const -> const_iterator {
		return const_iterator(this, size());
	}
	auto filtered_string_view::crbegin() const -> const_reverse_iterator {
		return const_reverse_iterator(cend());
	}
	auto filtered_string_view::crend() const -> const_reverse_iterator {
		return const_reverse_iterator(cbegin());
	}
} // namespace fsv
