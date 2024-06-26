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
} // namespace fsv