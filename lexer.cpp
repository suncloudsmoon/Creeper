#include <string_view>
#include <cctype>
#include <cstddef>
#include "lexer.hpp"

namespace cre {
	namespace {
		token_id get_string_id(std::string_view str) {
			if (str.empty())
				return token_type::UNKNOWN;
			else if (str == "fn") {
				return token_type::FUNCTION;
			}
			else if (str == "if") {
				return token_type::IF;
			}
			else if (str == "end") {
				return token_type::END;
			}
			else if (str == "is") {
				return token_type::ASSIGN;
			}
			else if (str == "equals") {
				return token_type::EQUALITY;
			}
			else if (str == "notequals") {
				return token_type::NOTEQUALS;
			}
			else if (std::isdigit(str[0])) {
				return token_type::NUMBER;
			}
			else if (std::isalpha(str[0])) {
				return token_type::VARIABLE;
			}
			else {
				return token_type::UNKNOWN;
			}
		}
		// [begin,end] -> [inclusive, exclusive]
		std::string_view substring(std::string_view cont, std::size_t begin, std::size_t end) {
			return cont.substr(begin, end - begin);
		}
	}
	void lexer::lex(const std::string& cont) {
		std::size_t previous_index{ 0 };
		std::size_t line_num{ 1 };
		char previous_char{ NULL };
		for (std::size_t index = 0; index < cont.length(); index++) {
			char c = cont[index];
			switch (c) {
			case token_type::NEW_LINE:
				line_num++;
				previous_index = index + 1;
				break;
			case token_type::SPACE: {
				if (c == previous_char) {
					previous_index = index + 1;
					break;
				}
				if (std::string_view str = substring(cont, previous_index, index); str.length()) {
					token_id str_type = get_string_id(str);
					push(token<std::string>{ line_num, str_type, std::string(str) });
				}
				previous_index = index + 1;
				break;
			}
			case token_type::PLUS:
			case token_type::MINUS:
			case token_type::MULTIPLY:
			case token_type::DIVIDE:
			case token_type::LEFT_PARENTHESIS:
			case token_type::RIGHT_PARENTHESIS:
			case token_type::COMMA:
			case token_type::SEMICOLON: {
				if (std::string_view str = substring(cont, previous_index, index); str.length()) {
					token_id str_type = get_string_id(str);
					push(token<std::string>{ line_num, str_type, std::string(str) });
				}
				push(token<std::string>{ line_num, c, std::string(1, c) });
				previous_index = index + 1;
				break;
			}
			default:
				break;
			}
			previous_char = c;
		}
		if (previous_index < cont.length()) {
			if (std::string_view str = substring(cont, previous_index, cont.length()); str.length()) {
				token_id id = get_string_id(str);
				push(token<std::string>{ line_num, id, std::string(str) });
			}
		}
	}

	void lexer::push(const token<std::string>& t) {
		token_queue.push(t);
	}
	void lexer::push(token<std::string>&& t) {
		token_queue.emplace(t);
	}
}