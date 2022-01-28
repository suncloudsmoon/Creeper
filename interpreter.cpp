#include <stack>
#include <array>
#include "interpreter.hpp"
#include "debug.hpp"

namespace cre {
	namespace {
		float get_val(token_id id, const std::any& cont, var_map& map, std::size_t line_num) {
			if (id == token_type::VARIABLE) {
				std::string var_name = std::any_cast<std::string>(cont);
				if (map.contains(var_name))
					return std::any_cast<float>(map.at(var_name));
				else
					throw interpreter_error(line_num, "Unable to do operation on variable \"" + var_name + "\" because it is not declared!");
			}
			else if (id == token_type::NUMBER) {
				return std::any_cast<float>(cont);
			}
			else {
				throw interpreter_error(line_num, "Cannot do math operations on type other than variable or number!");
			}
		}

		void execute_op(token_id id, std::stack<token<std::any>>& stack, 
								var_map& local_map, std::size_t line_num) {
			if (stack.size() < 2)
				throw interpreter_error(line_num, "Malformed expression!");
			
			token<std::any> right{ std::move(stack.top()) };
			stack.pop();
			token<std::any> left{ std::move(stack.top()) };
			stack.pop();

			if (id == token_type::ASSIGN) {
				if (left.get_token_id() == token_type::VARIABLE) {
					std::string var_name = std::any_cast<std::string>(left.get_content());
					if (local_map.contains(var_name))
						local_map.at(var_name) = std::move(right);
					else
						local_map.emplace(std::move(var_name), std::move(right));
				}
				else {
					throw interpreter_error(line_num, "Left of assignment operator is not a variable!");
				}
			}
			else {
				float left_val = get_val(left.get_token_id(), left.get_content(), local_map, line_num);
				float right_val = get_val(right.get_token_id(), right.get_content(), local_map, line_num);
				float res{ 0 };
				switch (id) {
				case token_type::PLUS:
					res = left_val + right_val;
					break;
				case token_type::MINUS:
					res = left_val - right_val;
					break;
				case token_type::MULTIPLY:
					res = left_val * right_val;
					break;
				case token_type::DIVIDE:
					res = left_val / right_val;
					break;
				case token_type::EQUALITY:
					res = (left_val == right_val);
					break;
				case token_type::NOTEQUALS:
					res = (left_val != right_val);
					break;
				default:
					break;
				}
				stack.emplace(line_num, token_type::NUMBER, res);
			}
			
		}
	}

	void interpreter::default_start() {
		execute("main", std::queue<std::any>(), 100, 0);
	}

	void interpreter::execute(const std::string& func_name, std::queue<std::any> args, 
								int recursion_limit, std::size_t line_num) {
		if (recursion_limit <= 0)
			throw interpreter_error(line_num, "stackoverflow error!");

		var_map local_map;
		const auto& func_contents = functions.at(func_name);
		const auto &func_args = func_contents.first;

		for (auto arg_str : func_args) {
			if (args.empty())
				throw interpreter_error(line_num, "Unable to execute function \"" + func_name + "\" due to mismatching arguments!");
			else
				local_map.emplace(std::move(arg_str), std::move(args.front()));
			args.pop();
		}
		execute_lines(func_contents.second, local_map, line_num);
#ifdef DEBUG
		std::cout << "Variable map:" << std::endl;
		for (const auto& [key, val] : local_map) {
			token<std::any> t = std::any_cast<token<std::any>>(val);
			float val = std::any_cast<float>(t.get_content());
			std::cout << "[" << key << "," << val << "]" << std::endl;
		}
		std::cout << "-------------------" << std::endl;
#endif
	}

	void interpreter::execute_lines(const std::vector<line>& func_lines, var_map& local_map, 
									std::size_t line_num) {
		std::stack<token<std::any>> stack;
		for (auto line : func_lines) {
			while (!line.empty()) {
				auto& front = line.front();
				line_num = front.get_line_number();
				token_id id = front.get_token_id();

				switch (id) {
				case token_type::VARIABLE:
				case token_type::NUMBER:
					stack.emplace(line_num, id, std::move(front.get_content_ref()));
					break;
				case token_type::PLUS:
				case token_type::MINUS:
				case token_type::MULTIPLY:
				case token_type::DIVIDE:
				case token_type::EQUALITY:
				case token_type::NOTEQUALS:
				case token_type::ASSIGN:
					execute_op(id, stack, local_map, line_num);
					break;
				default:
					break;
				}
				line.pop();
			}
		}
	}

}
