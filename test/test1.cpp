#include <iostream>
#include <queue>
#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>

std::mutex mtx_queue;
std::queue<std::future<void>> tasks;

int main()
{
	// 设置随机数引擎和分布器
	std::random_device rd;  // 用于生成随机种子
	std::mt19937 gen(rd()); // Mersenne Twister 引擎
	std::uniform_int_distribution<int> dis(1, 10); // 生成 [1, 100] 范围内的均匀分布整数

	for (int i = 0; i < 10; i++) {
		tasks.emplace(std::async(
			std::launch::async,
			[i, &dis, &gen] {
			int sec = dis(gen);
			std::this_thread::sleep_for(std::chrono::seconds());
			std::cout << "task " << i << " finished.(time consumption: " << sec << "s)" << std::endl;

			{
				std::lock_guard<std::mutex> lock_queue(mtx_queue);

			}
		}));
	}

	return 0;
}