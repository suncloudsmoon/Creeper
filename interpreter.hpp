#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <string>
#include <unordered_map>
#include <utility>
#include <any>
#include <queue>
#include <stdexcept>
#include <cstddef>
#include "parser.hpp"
#include "token.hpp"

namespace cre {
	using variable_name = std::string;
	using var_map = std::unordered_map<variable_name, std::any>;
	using func_map = std::unordered_map<std::string, std::pair<arg_list, line_list>>;
	class interpreter {
	public:
		interpreter(func_map&& m) : functions(m) {}
		void default_start();
		void execute(const std::string& func_name, std::queue<std::any> args, 
			int recursion_limit, std::size_t line_num);
	private:
		void execute_lines(const std::vector<line>& func_lines, var_map& local_map, std::size_t line_num);
		var_map global_var_map;
		func_map functions;
	};
	class interpreter_error : public std::runtime_error {
	public:
		interpreter_error(std::size_t line_num, const std::string &err_message) :
			line_number(line_num), std::runtime_error(err_message) {}
		std::size_t get_line_number() const noexcept {
			return line_number;
		}
	private:
		std::size_t line_number;
	};
}

#endif /* INTERPRETER_HPP */