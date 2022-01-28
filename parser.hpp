#ifndef PARSER_HPP
#define PARSER_HPP

#include <queue>
#include <vector>
#include <unordered_map>
#include <string>
#include <any>
#include <utility>
#include <stdexcept>
#include <cstddef>
#include "token.hpp"

namespace cre {
	using arg_list = std::vector<std::string>;
	using line = std::queue<token<std::any>>;
	using line_list = std::vector<line>;
	class parser {
	public:
		void parse(std::queue<token<std::string>>& token_queue);
		std::unordered_map<std::string, std::pair<arg_list, line_list>>& get_func_map() noexcept {
			return func_map;
		}
	private:
		arg_list get_function_args(std::queue<token<std::string>>& token_queue, std::size_t& curr_line_num);
		line get_line(std::queue<token<std::string>>& token_queue, std::size_t& curr_line_num);

		void add_function(const std::string& func, arg_list&& args);

		void add_line(const std::string& func, const line& l);
		void add_line(const std::string& func, line&& l);

		std::unordered_map<std::string, std::pair<arg_list, line_list>> func_map;
	};
	class parsing_error : public std::runtime_error {
	public:
		parsing_error(std::size_t line_num, const std::string &err_message) : 
			line_number(line_num), std::runtime_error(err_message) {}
		std::size_t get_line_number() const noexcept {
			return line_number;
		}
	private:
		std::size_t line_number;
	};
}

#endif /* PARSER_HPP */