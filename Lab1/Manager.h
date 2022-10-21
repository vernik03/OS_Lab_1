#pragma once
#include<thread>
#include <functional>
#include "windows.h"

#include "trialfuncs.hpp"
#include <shared_mutex>

static char question_res = NULL;

static std::mutex m_f;
static std::condition_variable cv_f;
static std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, int> result_f;
static bool ready_f = false;
static bool hunged_f = false;


static std::mutex m_stopper;
static bool stopper = true;
static std::condition_variable cv_stopper;

static std::mutex m_g;
static std::condition_variable cv_g;
static std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, int>  result_g;
static bool ready_g = false;
static bool hunged_g = false;




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
		std::cout << "f(x)!\n";
		result_f = f(x);
		ready_f = true;
		std::cout << "f(x)ready\n";
		cv_f.notify_one();
		std::cout << "f(x)notify\n";
	}
	
	//template<typename T>
	void runFunctionG(std::function<std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, int>(int)> g, int x) {

		std::cout << "g(x)!\n";
		result_g = g(x);
		ready_g = true;

		std::cout << "g(x)ready\n";
		cv_g.notify_one();
		std::cout << "g(x)notify\n";
	}
	
	void waiter_f() {
		Sleep(10000);
		hunged_f = true;
		cv_f.notify_one();
	}
	
	void waiter_g() {
		Sleep(10000);
		hunged_g = true;
		cv_g.notify_one();
	}

	template<typename T>
	void manageFunction(FunctionInfo<T>& f) {
		for (int i = 0; i < 3; i++)
		{
			f.try_count++;
			if (f.type == func::f) {
				std::cout << "f() works" << std::endl;
				std::thread rf_thread([this, f] {runFunctionF(f.function, f.argument); });
				rf_thread.detach();
				std::thread waiter_f_thread([this] {waiter_f(); });
				waiter_f_thread.detach();
				/*result_f = f.function(x);
				ready_f = true;
				cv_f.notify_one();*/
			}
			else {
				std::cout << "g() works" << std::endl;
				std::thread rg_thread([this, f] {runFunctionG(f.function, f.argument); });
				rg_thread.detach();
				std::thread waiter_g_thread([this] {waiter_g(); });
				waiter_g_thread.detach();
				/*result_g = f.function(x);
				ready_g = true;
				cv_g.notify_one();*/
			}
			std::cout << "if" << std::endl;

			if (f.type == func::f) {
				std::cout << "unique_lock\n";
				std::cout << "unique_lock_end\n";
				std::unique_lock lk_f(m_f);
				cv_f.wait(lk_f, [] {return ready_f || hunged_f; });//? (lk)
				std::cout << "f() done" << std::endl;
				if (hunged_f)
				{
					result_f = os::lab1::compfuncs::soft_fail();
				}
			}
			else {
				std::cout << "unique_lock\n";
				std::cout << "unique_lock_end\n";
				std::unique_lock lk_g(m_g);
				cv_g.wait(lk_g, [] {return ready_g || hunged_g; });//? (lk)
				if (hunged_g)
				{
					result_f = os::lab1::compfuncs::soft_fail();
				}
				std::cout << "g() done" << std::endl;
			}
			switch (result_f.index()) {
			case 0:
				f.result = os::lab1::compfuncs::hard_fail();
				return;
			case 1:
				f.result = os::lab1::compfuncs::soft_fail();
				break;
			case 2:
				f.result = std::get<2>(result_f);
				return;
			}
			if (std::holds_alternative<os::lab1::compfuncs::soft_fail>(f.result))
			{
				std::unique_lock stopper_lk(m_stopper);
				cv_stopper.wait(stopper_lk, [] {return stopper; });
				using namespace std::chrono;
				high_resolution_clock::time_point t1 = high_resolution_clock::now();
				std::thread tr_question([this] {question("Please confirm that computation should be stopped y(es, stop) / n(ot yet)[n]"); });
				tr_question.detach();
				stopper = false;
				while (true)
				{
					high_resolution_clock::time_point t2 = high_resolution_clock::now();
					duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
					if (time_span.count() > 5)
					{
						stopper = true;
						cv_stopper.notify_all();
						break;
					}
					if (question_res == 'y' || question_res == 'Y')
					{
						stopper = true;
						f.is_canceled = true;
						question_res = NULL;
						cv_stopper.notify_all();
						return;
					}
					else if (question_res == 'n' || question_res == 'N')
					{
						stopper = true;
						cv_stopper.notify_all();
						break;
					}
					else if (question_res != NULL)
					{
					
						stopper = true;
						cv_stopper.notify_all();
						break;
					}
				}
			}
		}	
		return;
	}

	template<typename T1, typename T2>
	void startReset(FunctionInfo<T1> f, FunctionInfo<T2> g) {
		result_f = NULL;
		result_g = NULL;
		f.result = result_f;
		g.result = result_g;
		ready_f = false;
		ready_g = false;
		hunged_f = false;
		hunged_g = false;
		question_res = NULL;
	}
	
	
	void question(std::string s) {
		std::cout << s << std::endl;
		std::cin >> question_res;
	}

	//template<typename T1, typename T2>
	bool INT_SUM(int f, int g) {
		//return reinterpret_cast<int>(f + g);
		std::cout << "f() + g()";
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
		if (std::holds_alternative<os::lab1::compfuncs::soft_fail>(f1.result))
		{
			std::cout << "Computation failed because of soft_fail in " + f1_name + "()" << std::endl;
			std::cout << "Tryed " << f1.try_count << " times" << std::endl;
			std::cout << f2_name + "(): " << f2.result << std::endl;
			return true;
		}
		else if (std::holds_alternative<os::lab1::compfuncs::hard_fail>(f1.result)) {
			
			std::cout << "Computation failed because of hard_fail in " + f1_name + "()" << std::endl;
			std::cout << f2_name + "(): " << f2.result << std::endl;
			return true;
		}
		return false;
	}
	
	//template<typename T1, typename T2>
	void runInterface(std::function<std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, int>(int)> function,
		std::function<std::variant<os::lab1::compfuncs::hard_fail, os::lab1::compfuncs::soft_fail, int>(int)> function2) {		
		std::cout << "Manager started" << std::endl;
		FunctionInfo f { func::f, 0, result_f, function };
		FunctionInfo g { func::g, 0, result_g, function2 };
		while (true)
		{
			startReset(f, g);
			std::cout << "Enter x: ";
			std::cin >> x;
			f.argument = x;
			g.argument = x;

			std::thread thr1([this, &f] {manageFunction(f); });
			std::thread thr2([this, &g] {manageFunction(g); });
			thr1.join();
			thr2.join();


			if (!output(f, g) && !output(g, f))
			{
				std::cout <<  INT_SUM(std::get<2>(f.result), std::get<2>(g.result)) << std::endl;
			};

			while (true)
			{
				std::cout << "Once more? y/n ";
				char c;
				std::cin >> c;

				if (c == 'y' || c == 'Y')
				{
					break;
				}
				else if (c == 'n' || c == 'N')
				{
					return;
				}
				else {
					std::cout << "Wrong input" << std::endl;
				}
			}
			
		}
	}
};
