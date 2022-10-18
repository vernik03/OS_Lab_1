#include <iostream>
#include <type_traits>
#include <typeinfo>

#include "trialfuncs.hpp"

#include "Manager.h"

std::mutex m1;
std::condition_variable cv1;

std::mutex m2;
std::condition_variable cv2;

int main()
{

    
    // sanity check
    /*
    static_assert(std::is_same<int,spos::lab1::demo::op_group_traits<spos::lab1::demo::INT>::value_type>(), "wrong typing for INT");
    */

    /*static_assert(std::is_same<bool, os::lab1::compfuncs::op_group_traits<os::lab1::compfuncs::OR>::value_type>(), "wrong typing for OR");

    std::cout << "f(0) and g(0): " << std::endl;
    std::cout << std::boolalpha << "f(0) hard failed is " << std::holds_alternative<os::lab1::compfuncs::hard_fail>(os::lab1::compfuncs::trial_f<os::lab1::compfuncs::INT_SUM>(0)) << std::endl;*/
    /*std::cout << "f(0): " << os::lab1::compfuncs::trial_f<os::lab1::compfuncs::INT_SUM>(0) << std::endl;
    std::cout << "g(1): " << os::lab1::compfuncs::trial_g<os::lab1::compfuncs::INT_SUM>(1) << std::endl;
    std::cout << "f(1): " << os::lab1::compfuncs::trial_f<os::lab1::compfuncs::INT_SUM>(1) << std::endl;*/

    //std::cout << typeid(os::lab1::compfuncs::trial_f<os::lab1::compfuncs::INT_SUM>).name() << std::endl;


	Manager manager;
    manager.runInterface(os::lab1::compfuncs::trial_f<os::lab1::compfuncs::INT_SUM>);
    manager.runInterface(func, 5, func, -3);
}

bool func(int x) {
{
		return x > 0 ? true : false;
}