#include "./filtered_string_view.h"

namespace fsv {
	filter filtered_string_view::default_predicate = [](const char&) noexcept { return true; };
	filtered_string_view::filtered_string_view()
	: ptr(nullptr)
	, str_length(0)
	, str_pred(default_predicate) {}
	filtered_string_view::filtered_string_view(const std::string& str)
	: ptr(str.data())
	, str_length(str.size())
	, str_pred(default_predicate) {}
	filtered_string_view::filtered_string_view(const std::string& str, filter predicate)
	: ptr(str.data())
	, str_length(str.size())
	, str_pred(predicate) {}
	filtered_string_view::filtered_string_view(const char* str)
	: ptr(str)
	, str_length(std::strlen(str))
	, str_pred(default_predicate) {}
	filtered_string_view::filtered_string_view(const char* str, filter predicate)
	: ptr(str)
	, str_length(std::strlen(str))
	, str_pred(predicate) {}
	filtered_string_view::filtered_string_view(const filtered_string_view& other)
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
	filtered_string_view::filtered_string_view(const char* str, std::size_t str_len, filter predicate)
	: ptr(str)
	, str_length(str_len)
	, str_pred(predicate) {}
	filtered_string_view& filtered_string_view::operator=(const filtered_string_view& other) {
		if (this != &other) {
			ptr = other.ptr;
			str_length = other.str_length;
			str_pred = other.str_pred;
		}
		return *this;
	}
	filtered_string_view& filtered_string_view::operator=(filtered_string_view&& other) noexcept {
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
	filtered_string_view::~filtered_string_view() noexcept {}
	const char& filtered_string_view::at(std::size_t n) const {
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
	const char& filtered_string_view::operator[](std::size_t n) const {
		return this->at(n);
	}
	std::size_t filtered_string_view::size() const {
		std::size_t count = 0;
		for (std::size_t i = 0; i < str_length; i++) {
			if (str_pred(ptr[i])) {
				count++;
			}
		}
		return count;
	}
	bool filtered_string_view::empty() const {
		return size() == 0;
	}
	const char* filtered_string_view::data() const {
		return ptr;
	}
	const filter& filtered_string_view::predicate() const {
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
	filtered_string_view compose(const filtered_string_view& fsv, const std::vector<filter>& filts) {
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
	bool operator==(const filtered_string_view& lhs, const filtered_string_view& rhs) {
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
	std::vector<filtered_string_view> split(const filtered_string_view& fsv, const filtered_string_view& tok) {
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
				if (i > offset) {
					filtered_string_view temp(fsv.data() + offset, i - offset, fsv.predicate());
					result.push_back(temp);
				}
				else {
					filtered_string_view temp(fsv.data() + offset, 0, fsv.predicate());
					result.push_back(temp);
				}
				offset = i + tok_size;
				i = offset;
			}
			else {
				i++;
			}
		}
		if (offset < end) {
			filtered_string_view temp(fsv.data() + offset, end - offset, fsv.predicate());
			result.push_back(temp);
		}
		return result;
	}
	std::size_t filtered_string_view::count_filtered_chars_before(std::size_t index) const {
		std::size_t filtered_count = 0;
		std::size_t raw_index = 0;

		for (std::size_t i = 0; i < str_length && raw_index < index; ++i) {
			if (!str_pred(ptr[i])) {
				++filtered_count;
			}
			else {
				++raw_index;
			}
		}
		return filtered_count;
	}
} // namespace fsv