#include <iostream>
#include <thread>
#include <atomic>
#include <cstdlib>
#include <windows.h>

std::atomic<bool> in1 = { false };
std::atomic<bool> in2 = { false };
std::atomic<int> last = { 1 };

bool t1_working = true;
bool t2_working = true;

bool t1in = false;
bool t2in = false;

int getRandInt(int x, int y) {
	return std::abs(x) + (std::rand() % std::abs((y - x)));
}

void f1() {
	while (t1_working) {
																// entry protocol
		in1.store(true, std::memory_order_seq_cst);
		last.store(1, std::memory_order_seq_cst);

																// <await(!in2 or last == 2);>
		while (in2.load(std::memory_order_seq_cst) &&			// while (in2 and last == 1) skip;
			   last.load(std::memory_order_seq_cst) == 1)
			/* skip */ Sleep(getRandInt(50, 300));
		
		t1in = true;

		for (int i = 0; i < getRandInt(100, 1000); ++i) /* some work here */;	// critical section
		for (int i = 0; i < 10; ++i) {
			std::cout << "Critical section t1" << std::endl;
			if (t1in && t2in)
				std::cout << "Doesn't work!!!" << std::endl;
		}

		t1in = false;

		in1.store(false);										// exit protocol

		for (int i = 0; i < getRandInt(100, 1000); ++i) /* some work here */;	// non-critical section
		for (int i = 0; i < 5; ++i)
		std::cout << "Non-critical section t1" << std::endl;
	}
}


void f2() {
	while (t2_working) {
																// entry protocol
		in2.store(true, std::memory_order_seq_cst);
		last.store(2, std::memory_order_seq_cst);

																// <await(!in1 or last == 1);>
		while (in1.load(std::memory_order_seq_cst) &&			// while (in1 and last == 2) skip;
			   last.load(std::memory_order_seq_cst) == 2)
			/* skip */ Sleep(getRandInt(50, 300));

		t2in = true;

		for (int i = 0; i < getRandInt(100, 1000); ++i) /* some work here */;	// critical section
		for (int i = 0; i < 10; ++i) {
			std::cout << "Critical section t2" << std::endl;
			if (t1in && t2in)
				std::cout << "Doesn't work!!!" << std::endl;
		}

		t2in = false;

		in2.store(false);										// exit protocol

		for (int i = 0; i < getRandInt(100, 1000); ++i) /* some work here */;	// non-critical section
		for (int i = 0; i < 5; ++i)
		std::cout << "Non-critical section t2" << std::endl;
	}
}

int main() {

	std::srand((unsigned) time(NULL));

	std::thread t1(f1);
	std::thread t2(f2);

	std::getchar();

	t1_working = false;
	t2_working = false;
	t1.join(); t2.join();

	return 0;
}
