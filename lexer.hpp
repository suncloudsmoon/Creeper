#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <queue>
#include "token.hpp"

namespace cre {
	class lexer {
	public:
		void lex(const std::string& cont);
		std::queue<token<std::string>>& get_token_queue() noexcept {
			return token_queue;
		}
	private:
		void push(const token<std::string>& t);
		void push(token<std::string>&& t);
		std::queue<token<std::string>> token_queue;
	};
}

#endif /* LEXER_HPP */