#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cstddef>

namespace cre {
	using token_id = int;
	namespace token_type {
		enum { SPACE = ' ', NEW_LINE = '\n' };
		enum { PLUS = '+', MINUS = '-', MULTIPLY = '*', DIVIDE = '/' };
		enum { LEFT_PARENTHESIS = '(', RIGHT_PARENTHESIS = ')', COMMA = ',', SEMICOLON = ';' };
		enum { ASSIGN = 200, EQUALITY = 201, NOTEQUALS = 202 };
		enum { FUNCTION = 300, IF = 301, END = 302 };
		enum { NUMBER = 400, VARIABLE = 401 };
		enum { UNKNOWN = 500 };
	}
	template<typename T>
	class token {
	public:
		token(std::size_t line_num, token_id i, const T& cont) : 
			line_number(line_num), id(i), content(cont) {}
		token(std::size_t line_num, token_id i, T&& cont) : 
			line_number(line_num), id(i), content(cont) {}
		
		std::size_t get_line_number() const noexcept {
			return line_number;
		}
		token_id get_token_id() const noexcept {
			return id;
		}
		const T& get_content() const noexcept {
			return content;
		}
		T& get_content_ref() noexcept {
			return content;
		}
	private:
		std::size_t line_number;
		token_id id;
		T content;
	};
}

#endif /* TOKEN_HPP */