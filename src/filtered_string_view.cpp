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
		std::size_t filtered_index = 0;
		for (std::size_t i = 0; i < str_length; i++) {
			if (str_pred(ptr[i])) {
				if (filtered_index == n) {
					return ptr[i];
				}
				filtered_index++;
			}
		}
		throw std::domain_error("filtered_string_view::at(" + std::to_string(n) + "): invalid index");
	}
	const char& filtered_string_view::operator[](std::size_t n) const {
		return this->at(n);
	}
	std::size_t filtered_string_view::size() const {
		std::size_t filtered_count = 0;
		for (std::size_t i = 0; i < str_length; i++) {
			if (str_pred(ptr[i])) {
				filtered_count++;
			}
		}
		return filtered_count;
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
} // namespace fsv