//#include <iostream>
//#include <future>
//#include <thread>
//#include <chrono>
////#include <functional>
//
//// �첽���������������ص�������Ϊ����
//int asyncfunc(std::function<void(int, std::function<void(int)>)> callback) {
//    // ģ���첽����������һ��ֵ
//    std::cout << "Async function running..." << std::endl;
//    int result = 42;
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//    std::cout << "Async function done..." << std::endl;
//
//    // ���õ�һ���ص����������뷵��ֵ�͵ڶ����ص�����
//    callback(result, [](int result) {
//        std::cout << "Callback function 2 running with result from callback: " << result << std::endl;
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//        std::cout << "Callback function 2 done..." << std::endl;
//    });
//    return result;
//}
//
//// ��һ���ص������������첽�����ķ���ֵ�͵ڶ����ص�������Ϊ����
//void callback(int result, std::function<void(int)> callback2) {
//    std::cout << "Callback function 1 running with result: " << result << std::endl;
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//    std::cout << "Callback function 1 done..." << std::endl;
//
//    // ���õڶ����ص������������첽�����ķ���ֵ
//    callback2(result);
//}
//
//// ִ�����첽�����޹صĲ���
//void innerfunc1() {
//    std::cout << "Inner function 1 running..." << std::endl;
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//    std::cout << "Inner function 1 done..." << std::endl;
//}
//
//// ִ�б���ȴ��첽����ִ����Ϻ���ִ�еĲ�������ʹ���첽�����ķ���ֵ
//void innerfunc2(int result) {
//    std::cout << "Inner function 2 running with result: " << result << std::endl;
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//    std::cout << "Inner function 2 done..." << std::endl;
//}
//
//// ������
//void func() {
//    // �첽ִ�� asyncfunc���������һ���ص�����
//    auto future = std::async(std::launch::async, asyncfunc, [](int result, std::function<void(int)> callback2) {
//        // �ڵ�һ���ص������е��õڶ����ص�����
//        callback(result, callback2);
//    });
//
//    // ִ�����첽�����޹صĲ���
//    innerfunc1();
//
//    // ��ȡ�첽�����ķ���ֵ
//    int result = future.get();
//
//    // ִ�б���ȴ��첽����ִ����Ϻ���ִ�еĲ�������ʹ���첽�����ķ���ֵ
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