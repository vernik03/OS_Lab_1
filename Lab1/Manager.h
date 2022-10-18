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

	template<typename T>
	struct FunctionInfo {
		int argument;
		std::variant<hard_fail, soft_fail, T> result;
		std::function<T(int)> function;
	};
	
	
	Manager(){};
	~Manager(){};

	template<typename T>
	void runFunction(std::function<T(int)> f) {

		 f(x);
		lk.unlock();
		cv.notify_one();
	}

	template<typename T>
	FunctionInfo<T> manageFunction(FunctionInfo<T>& f) {
		
		for (int i = 0; i < 3; i++)
		{
			using namespace std::chrono;
			high_resolution_clock::time_point t1 = high_resolution_clock::now();
			
			std::thread t_f(manageFunction(f.function()));
			while (true)
			{
				high_resolution_clock::time_point t2 = high_resolution_clock::now();

				duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
				
				if (time_span.count() > 5)
				{
					break;
				}

				
				//таймер ждёт
				
				//если таймер превысил -> break
				// 
				//проверить, что он не разлочен -> break
			}
			
			
			if (std::get<int>(f.result))
			{
				return f;
			}
		}			
		f.result_variant = hard_fail;
		return f;
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

	template<typename T1, typename T2>
	void runInterface(T1 function, int x1, T2 function2, int x2) {
		bool flag = 1;
		FunctionInfo f{ x1, function(x1), function };
		FunctionInfo g{ x2, function2(x2), function2 };
		while (flag)
		{
			std::cout << "Enter x: ";
			std::cin >> x;
		
			std::thread thr(manageFunction(f));
			std::thread thr2(manageFunction(g));
			thr.join();
			thr2.join();

			std::unique_lock<std::mutex> lk(m);
			cv.wait(lk, [] {return processed; });
			
			/*f = manageFunction(f);
			g = manageFunction(g);*/

			if (std::holds_alternative<hard_fail>(f_info.result))
			{
				std::cout << "Error in f" << std::endl;
			}
			else if (std::holds_alternative<hard_fail>(g))
			{
				std::cout << "Error in g" << std::endl;
			}
			else
			{
				//std::cout << "Good f: " << std::get<int>(f_info.result) << std::endl;
				auto op_group = os::lab1::compfuncs::op_group::AND;
				std::cout << compare(std::get<int>(f), std::get<int>(g), op_group) << std::endl;
			}
			
			
			/*if (std::holds_alternative<int>(f) && std::holds_alternative<int>(g))
			{
				auto op_group = os::lab1::compfuncs::op_group::AND;
				std::cout << compare(std::get<int>(f), std::get<int>(g), op_group) <<std::endl;
			}	
			else
			{
				std::cout << "Error" << std::endl;
			}*/
			std::cout << "Once more? y/n ";
			char c;
			std::cin >> c;
			if (c == 'n')
				flag = 0;			
		}
	}

private:

};
