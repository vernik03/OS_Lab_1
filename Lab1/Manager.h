#pragma once
#include<thread>
#include <functional>
#include "windows.h"

#include "trialfuncs.hpp"


class Manager
{
public:
	int x;

	struct hard_fail {};
	struct soft_fail {};

	template<typename T, typename N>
	struct FunctionInfo {
		N argument;
		//T result;
		std::variant<hard_fail, soft_fail, T> result_variant;
		std::function<T(N, ...)> function;
	};
	
	FunctionInfo<os::lab1::compfuncs::comp_result<int>, int> f_info;
	//FunctionInfo<os::lab1::compfuncs::comp_result<int>, int> g_info;

	/*template<typename T>
	using result_variant = std::variant<hard_fail, soft_fail, T>;*/
	
	Manager(){};
	~Manager(){};

	template<typename F, typename N>
	void runFunction(std::function<F(N num, ...)> f) {		
		f_info.result_variant = f(x);
	}

	template<typename T, typename F, typename N>
	FunctionInfo<T, N> manageFunction(FunctionInfo<T, N>& info) {
		
		for (int i = 0; i < 3; i++)
		{
			std::thread thr(runFunction(info.function));
			thr.join();
			Sleep(2000);
			if (std::get<int>(info.result_variant))
			{
				break;
			}
		}			
		info.result_variant = hard_fail;
	}

	bool compare(int f, int g, os::lab1::compfuncs::op_group op_group) {
		switch (op_group) {
		case 0:
			std::cout << "f AND g = ";
			return f && g;
		case 1:
			std::cout << "OR";			
			return f || g;
		}	
	}

	template<typename T>
	void runInterface(T function) {
		bool flag = 1;
		while (flag)
		{
			std::cout << "Enter x: ";
			std::cin >> x;
			f_info.argument = x;
			g_info.argument = x;
			f_info.function = function;
			g_info.function = function;

			f_info = manageFunction(f_info);
			g_info = manageFunction(g_info);
			
			if (std::holds_alternative<int>(f) && std::holds_alternative<int>(g))
			{
				auto op_group = os::lab1::compfuncs::op_group::AND;
				std::cout << compare(std::get<int>(f), std::get<int>(g), op_group) <<std::endl;
			}	
			else
			{
				std::cout << "Error" << std::endl;
			}
			std::cout << "Once more? y/n ";
			char c;
			std::cin >> c;
			if (c == 'n')
				flag = 0;			
		}
	}

private:

};
