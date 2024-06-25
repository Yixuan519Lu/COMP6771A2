#include "./filtered_string_view.h"

namespace fsv {
	filter filtered_string_view::default_predicate = [](const char&) { return true; };
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

} // namespace fsv