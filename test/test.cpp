//#include <iostream>
//#include <future>
//#include <thread>
//#include <chrono>
////#include <functional>
//
//// 异步函数，接受两个回调函数作为参数
//int asyncfunc(std::function<void(int, std::function<void(int)>)> callback) {
//    // 模拟异步操作，返回一个值
//    std::cout << "Async function running..." << std::endl;
//    int result = 42;
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//    std::cout << "Async function done..." << std::endl;
//
//    // 调用第一个回调函数并传入返回值和第二个回调函数
//    callback(result, [](int result) {
//        std::cout << "Callback function 2 running with result from callback: " << result << std::endl;
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//        std::cout << "Callback function 2 done..." << std::endl;
//    });
//    return result;
//}
//
//// 第一个回调函数，接受异步函数的返回值和第二个回调函数作为参数
//void callback(int result, std::function<void(int)> callback2) {
//    std::cout << "Callback function 1 running with result: " << result << std::endl;
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//    std::cout << "Callback function 1 done..." << std::endl;
//
//    // 调用第二个回调函数并传入异步函数的返回值
//    callback2(result);
//}
//
//// 执行与异步函数无关的操作
//void innerfunc1() {
//    std::cout << "Inner function 1 running..." << std::endl;
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//    std::cout << "Inner function 1 done..." << std::endl;
//}
//
//// 执行必须等待异步函数执行完毕后再执行的操作，并使用异步函数的返回值
//void innerfunc2(int result) {
//    std::cout << "Inner function 2 running with result: " << result << std::endl;
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//    std::cout << "Inner function 2 done..." << std::endl;
//}
//
//// 主函数
//void func() {
//    // 异步执行 asyncfunc，并传入第一个回调函数
//    auto future = std::async(std::launch::async, asyncfunc, [](int result, std::function<void(int)> callback2) {
//        // 在第一个回调函数中调用第二个回调函数
//        callback(result, callback2);
//    });
//
//    // 执行与异步函数无关的操作
//    innerfunc1();
//
//    // 获取异步函数的返回值
//    int result = future.get();
//
//    // 执行必须等待异步函数执行完毕后再执行的操作，并使用异步函数的返回值
//    innerfunc2(result);
//}
//
//int main() {
//    func();
//    return 0;
//}
//
////#include <iostream>
////#include <future>
////#include <thread>
////#include <chrono>
////
////void func1()
////{
////	std::cout << "func1 running..." << std::endl;
////	std::this_thread::sleep_for(std::chrono::seconds(10));
////	std::cout << "func1 done..." << std::endl;
////}
////
////void func2()
////{
////	auto future = std::async(std::launch::async, func1);
////
////	std::cout << "func2 running..." << std::endl;
////	std::this_thread::sleep_for(std::chrono::seconds(1));
////	std::cout << "func2 done..." << std::endl;
////}
////
////int main()
////{
////	auto future = std::async(std::launch::async, func2);
////	std::cout << "main running..." << std::endl;
////	//future.wait();
////	std::cout << "main done..." << std::endl;
////
////	return 0;
////}