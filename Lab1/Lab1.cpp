#include <iostream>
#include <type_traits>
#include <typeinfo>

#include "trialfuncs.hpp"

#include "Manager.h"


int main()
{

    
    // sanity check
    /*
    static_assert(std::is_same<int,spos::lab1::demo::op_group_traits<spos::lab1::demo::INT>::value_type>(), "wrong typing for INT");
    */

    /*static_assert(std::is_same<bool, os::lab1::compfuncs::op_group_traits<os::lab1::compfuncs::OR>::value_type>(), "wrong typing for OR");

    std::cout << "f(0) and g(0): " << std::endl;
    std::cout << std::boolalpha << "f(0) hard failed is " << std::holds_alternative<os::lab1::compfuncs::hard_fail>(os::lab1::compfuncs::trial_f<os::lab1::compfuncs::INT_SUM>(0)) << std::endl;*/
 /*   auto f1 = os::lab1::compfuncs::trial_f<os::lab1::compfuncs::INT_SUM>(0);
	
    std::cout << "f(0): " << os::lab1::compfuncs::trial_f<os::lab1::compfuncs::INT_SUM>(0) << std::endl;
    std::cout << "g(1): " << os::lab1::compfuncs::trial_g<os::lab1::compfuncs::INT_SUM>(1) << std::endl;
    std::cout << "f(1): " << os::lab1::compfuncs::trial_f<os::lab1::compfuncs::INT_SUM>(1) << std::endl;*/

    //std::cout << typeid(os::lab1::compfuncs::trial_f<os::lab1::compfuncs::INT_SUM>).name() << std::endl;

    

	
	
	Manager manager;
    manager.runInterface(os::lab1::compfuncs::trial_f<os::lab1::compfuncs::INT_SUM>, os::lab1::compfuncs::trial_g<os::lab1::compfuncs::INT_SUM>);
   
}


//high_resolution_clock::time_point t1 = high_resolution_clock::now();
//std::thread t_f(manageFunction(f.function()));
//while (true){
//	high_resolution_clock::time_point t2 = high_resolution_clock::now();
//	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);			
//	if (time_span.count() > 5){
//		if (m1.try_lock()){
//			f.result = result_f;
//			m1.unlock();
//			return f;}
//		else{
//			f.result = hard_fail;
//			m1.unlock();
//			return f;}	}
//	if (m1.try_lock()){
//		f.result = result_f;
//		m1.unlock();
//		return f;}
//	//таймер ждёт
//	//если таймер превысил -> break
//	//проверить, что он не разлочен -> break
//}