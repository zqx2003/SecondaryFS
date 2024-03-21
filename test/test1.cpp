//#include <iostream>
//#include <thread>
//#include <mutex>
//#include <future>
//
//void DiskHandler() {
//    std::cout << "���̴�����" << std::endl;
//}
//
//class MyClass {
//public:
//    void asyncFunction(std::streampos disk_addr, std::istream* disk) {
//        mtx_r.lock();
//        disk->seekg(disk_addr, std::ios::beg);
//
//        // �첽��ʽ������
//        auto future = std::async(std::launch::async,
//            [this, disk, disk_addr]() { /* ������ */
//            std::vector<char> buffer(BUFSIZE);
//            disk->read(buffer.data(), BUFSIZE);
//            mtx_r.unlock(); // ���첽������ɺ����
//        });
//        // ʹ�ûص�����ģ������ж�
//        future.wait(); // �ȴ��첽�������
//        DiskHandler();
//    }
//
//private:
//    std::mutex mtx_r;
//    static const int BUFSIZE = 10;
//};
//
//int main() {
//    MyClass obj;
//    obj.asyncFunction(0, &std::cin);
//    return 0;
//}
