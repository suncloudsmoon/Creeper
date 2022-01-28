#include <iostream>
#include <string>
#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"

void print_all_tokens(cre::lexer &l);
void print_all_functions(cre::parser& p);

int main() {
	std::string cont{ "fn main() a is 2*2*(10+10)/10/10/10/10/10/10; b is 5*(1+2); end" };
	cre::lexer l;
	l.lex(cont);
	print_all_tokens(l);

	cre::parser p;
	p.parse(l.get_token_queue());
	print_all_functions(p);

	cre::interpreter i{ std::move(p.get_func_map()) };
	i.default_start();
}

void print_all_tokens(cre::lexer& l) {
	auto copy = l.get_token_queue();
	std::cout << "Tokens: [";
	while (!copy.empty()) {
		const auto& token = copy.front();
		std::cout << "{" << token.get_line_number() << " " << token.get_token_id() 
					<< " " << token.get_content() << "},";
		copy.pop();
	}
	std::cout << "]" << std::endl;
}

void print_all_functions(cre::parser& p) {
	for (const auto&[key, val] : p.get_func_map()) {
		std::cout << "-----------------------" << std::endl;
		
		std::cout << "Function: " << key << std::endl;

		const auto &arg_list = val.first;
		std::cout << "Args(" << arg_list.size() << "):" << std::endl;
		for (const auto& arg : arg_list) {
			std::cout << arg_list.front() << ",";
		}
		std::cout << std::endl;
		
		std::cout << "Line(s): " << std::endl;
		const auto &line_list = val.second;
		for (auto line : line_list) {
			while (!line.empty()) {
				const auto& front = line.front();
				std::cout << "{" << front.get_line_number() << "," << front.get_token_id() << "} ";
				line.pop();
			}
			std::cout << std::endl;
		}

		std::cout << "-----------------------" << std::endl;
	}
}
