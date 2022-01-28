#include <stack>
#include <type_traits>

#include "parser.hpp"

namespace cre {
	namespace {
		template<typename T>
		T& token_front(std::queue<T>& queue, std::size_t line_num, const std::string& err_message) {
			if (queue.empty())
				throw parsing_error(line_num, err_message);
			else
				return queue.front();
		}
		template<typename T>
		void token_pop(std::queue<T>& queue, std::size_t line_num, const std::string& err_message) {
			if (queue.empty())
				throw parsing_error(line_num, err_message);
			else
				queue.pop();
		}

		bool is_op(token_id id) noexcept {
			switch (id) {
			case token_type::PLUS:
			case token_type::MINUS:
			case token_type::MULTIPLY:
			case token_type::DIVIDE:
			case token_type::EQUALITY:
			case token_type::NOTEQUALS:
			case token_type::ASSIGN:
				return true;
			default:
				return false;
			}
		}

		int get_op_precedence(token_id id) noexcept {
			switch (id) {
			case token_type::ASSIGN:
			case token_type::LEFT_PARENTHESIS:
				return 0;
			case token_type::EQUALITY:
			case token_type::NOTEQUALS:
			case token_type::PLUS:
			case token_type::MINUS:
				return 2;
			case token_type::MULTIPLY:
			case token_type::DIVIDE:
				return 3;
			default:
				return 99;
			}
		}

		template<typename T>
		void pop_stack_until(std::queue<token<T>> &dest, std::stack<token<T>> &src, int precedence) noexcept {
			while (!src.empty()) {
				auto& top = src.top();
				token_id id = top.get_token_id();
				if (precedence > get_op_precedence(id))
					break;
				else
					dest.emplace(std::move(top));
				src.pop();
			}
		}

		template<typename T>
		void pop_stack_until_equal(std::queue<token<T>>& dest, std::stack<token<T>>& src, int precedence) noexcept {
			while (!src.empty()) {
				auto& front = src.top();
				if (front.get_token_id() == precedence)
					break;
				else
					dest.emplace(std::move(front));
				src.pop();
			}
		}
	}


	void parser::parse(std::queue<token<std::string>>& token_queue) {
		std::size_t curr_line_num{ 0 };
		std::string curr_func_name;
		while (!token_queue.empty()) {
			auto &t = token_queue.front();
			curr_line_num = t.get_line_number();
			token_id id = t.get_token_id();

			switch (id) {
			case token_type::FUNCTION: {
				// Pop off function name
				token_pop(token_queue, curr_line_num, "Unable to pop off function token!");

				auto& name_token = token_front(token_queue, curr_line_num,
					"Unable to get function name!");
				std::string func_name{ std::move(name_token.get_content_ref()) };
				curr_line_num = name_token.get_line_number();

				// Pop off function name and '('
				for (int i = 0; i < 2; i++)
					token_pop(token_queue, curr_line_num, "Unable to get tokens after function name \""
						+ func_name + "\"!");

				arg_list args = get_function_args(token_queue, curr_line_num);
				add_function(func_name, std::move(args));
				curr_func_name = std::move(func_name);
				break;
			}
			case token_type::END: {
				add_line(curr_func_name, line({ token<std::any>{t.get_line_number(), 
											t.get_token_id(), std::move(t.get_content_ref()) }}));
				token_queue.pop();
				break;
			}
			default: {
				add_line(curr_func_name, get_line(token_queue, curr_line_num));
				break;
			}
			}
		}
	}

	arg_list parser::get_function_args(std::queue<token<std::string>>& token_queue, std::size_t &curr_line_num) {
		arg_list args;
		while (!token_queue.empty()) {
			auto& front = token_queue.front();
			curr_line_num = front.get_line_number();
			token_id id = front.get_token_id();
			
			if (id == token_type::RIGHT_PARENTHESIS) {
				token_queue.pop();
				break;
			}
			else if (id == token_type::VARIABLE) {
				args.emplace_back(std::move(front.get_content_ref()));
			}
			token_queue.pop();
		}
		return args;
	}

	/*
	* Goes until semicolon.
	*/
	line parser::get_line(std::queue<token<std::string>>& token_queue, std::size_t& curr_line_num) {
		std::stack<token<std::any>> stack;
		line dest;

		while (!token_queue.empty()) {
			auto& front = token_queue.front();
			curr_line_num = front.get_line_number();
			token_id id = front.get_token_id();
			
			if (id == token_type::SEMICOLON) {
				token_queue.pop();
				break;
			}
			else if (id == token_type::LEFT_PARENTHESIS) {
				stack.emplace(front.get_line_number(), front.get_token_id(), 
								std::move(front.get_content_ref()));
			}
			else if (id == token_type::RIGHT_PARENTHESIS) {
				pop_stack_until_equal(dest, stack, token_type::LEFT_PARENTHESIS);
				if (stack.empty())
					throw parsing_error(curr_line_num, "Unbalanced equation!");
				stack.pop();
			}
			else if (is_op(id)) {
				int prec = get_op_precedence(id);
				pop_stack_until(dest, stack, prec);
				stack.emplace(front.get_line_number(), front.get_token_id(), 
								std::move(front.get_content_ref()));
			}
			else if (id == token_type::NUMBER) {
				float res = std::stof(front.get_content());
				dest.emplace(front.get_line_number(), front.get_token_id(), res);
			}
			else {
				dest.emplace(front.get_line_number(), front.get_token_id(), 
							std::move(front.get_content_ref()));
			}
			token_queue.pop();
		}
		// Pop the remaining items off the stack
		pop_stack_until(dest, stack, -1);
		return dest;
	}

	void parser::add_function(const std::string& func, arg_list&& args) {
		func_map.insert({ func, {args, line_list()} });
	}

	void parser::add_line(const std::string& func, const line& l) {
		func_map.at(func).second.push_back(l);
	}

	void parser::add_line(const std::string& func, line&& l) {
		func_map.at(func).second.emplace_back(l);
	}
}
