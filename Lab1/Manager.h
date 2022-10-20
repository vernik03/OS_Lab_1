#pragma once
#include<thread>
#include <functional>
#include "windows.h"

#include "trialfuncs.hpp"

static char question_res = NULL;

static std::mutex m_f;
static std::condition_variable cv_f;
static std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, int> result_f;
static bool ready_f = false;

static std::mutex m_g;
static std::condition_variable cv_g;
static std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, int>  result_g;
static bool ready_g = false;

class Manager
{
public:
	int x;

	struct hard_fail {};
	struct soft_fail {};

	enum class func
	{
		f, g
	};

	template<typename T>
	struct FunctionInfo {
		func type;
		int argument;
		std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, T> result;
		std::function<std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, T>(int)> function;
		int try_count = 0;
		bool is_canceled = false;
	};

	//template<typename T>
	void runFunctionF(std::function<std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, int>(int)> f, int x) {
		result_f = f(x);
		ready_f = true;
		cv_f.notify_one();
	}
	
	//template<typename T>
	void runFunctionG(std::function<std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, int>(int)> g, int x) {
		result_g = g(x);
		ready_g = true;
		cv_g.notify_one();
	}

	template<typename T>
	FunctionInfo<T> manageFunction(FunctionInfo<T>& f) {
		for (int i = 0; i < 3; i++)
		{
			f.try_count++;
			if (f.type == func::f) {
				std::thread rf_thread(runFunctionF(f.function, f.argument));
			}
			else {
				std::thread rg_thread(runFunctionG(f.function, f.argument));
			}
			if (f.type == func::f) {
				std::unique_lock lk(m_f);
				cv_f.wait(lk, [] {return ready_f; });//? (lk)
			}
			else {
				std::unique_lock lk(m_g);
				cv_g.wait(lk, [] {return ready_g; });//? (lk)
			}
			switch (result_f.index()) {
			case 0:
				f.result = os::lab1::compfuncs::hard_fail();
				return f;
			case 1:
				f.result = os::lab1::compfuncs::soft_fail();
				break;
			case 2:
				f.result = std::get<2>(result_f);
				return f;
			}
			if (std::holds_alternative<soft_fail>(f.result))
			{
				using namespace std::chrono;
				high_resolution_clock::time_point t1 = high_resolution_clock::now();
				std::thread tr_question(question("Please confirm that computation should be stopped y(es, stop) / n(ot yet)[n]"));
				while (true)
				{
					high_resolution_clock::time_point t2 = high_resolution_clock::now();
					duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
					if (time_span.count() > 5)
					{
						break;
					}
					if (question_res == 'y')
					{
						f.is_canceled = true;
						question_res = NULL;
						return f;
					}
				}
			}
		}	
		return f;
	}
	
	
	void question(std::string s) {
		std::cout << s << std::endl;
		std::cin >> question_res;
	}

	//template<typename T1, typename T2>
	bool INT_SUM(int f, int g) {
		//return reinterpret_cast<int>(f + g);
		return f + g;
	}

	template<typename T1, typename T2>
	bool output(FunctionInfo<T1> f1, FunctionInfo<T2> f2) {

		std::string f1_name, f2_name;
		
		if (f1.type == func::f) {
			f1_name = 'f'; f2_name = 'g';
		}
		else {
			f1_name = 'g', f2_name = 'f';
		};
		
		if (f1.is_canceled)
		{
			std::cout << "Computation canceled. ";
		}
		if (std::holds_alternative<soft_fail>(f1.result))
		{
			std::cout << "Computation failed because of soft_fail in " + f1_name + "()" << std::endl;
			std::cout << "Tryed " << f1.try_count << " times" << std::endl;
			std::cout << f2_name + "(): " << f2.result << std::endl;
			return true;
		}
		else if (std::holds_alternative<hard_fail>(f1.result)) {
			
			std::cout << "Computation failed because of hard_fail in " + f1_name + "()" << std::endl;
			std::cout << f2_name + "(): " << f2.result << std::endl;
			return true;
		}
		return false;
	}
	
	//template<typename T1, typename T2>
	void runInterface(std::function<std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, int>(int)> function, int x1,
		std::function<std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, int>(int)> function2, int x2) {
		bool flag = 1;
		FunctionInfo f { func::f, x1, function(x1), function };
		FunctionInfo g { func::g, x2, function2(x2), function2 };
		while (flag)
		{
			std::cout << "Enter x: ";
			std::cin >> x;

			std::thread thr(manageFunction(f));
			std::thread thr2(manageFunction(g));
			thr.join();
			thr2.join();


			if (!output(f, g) && !output(g, f))
			{
				std::cout << "f() " << INT_SUM(std::get<2>(f.result), std::get<2>(g.result)) << std::endl;
			};

			std::cout << "Once more? y/n ";
			char c;
			std::cin >> c;
			if (c == 'n') {
				flag = 0;
			}
		}
	}
};
