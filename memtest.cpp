#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <fstream>
#include <chrono>
#include <ctime>    
#include <thread>
#include <immintrin.h>
#include <vector>

const size_t pattern = 0xAAAAAAAAAAAAAAAAull;

void read_memcell(uint64_t *data) {
	std::cout << "Thread started: read_memcell" << std::endl;
	size_t tmp;
	while (true) {
		_mm_clflush(data); // clear cache
		_mm_mfence(); // just to make sure that the access is not reordered
		tmp = *data;
		_mm_mfence();
	}
}

void read_all_data(uint64_t *data, size_t cnt) {
	std::cout << "Thread started: read_all_data" << std::endl;
	while(true) {
		int total_errors = 0;
		for(size_t i = 0; i < cnt; i++) {
			if (data[i] != pattern) {
				total_errors++;
			}
		}
		auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		if (total_errors > 10000) 
			std::cout << "Error cnt " << total_errors << " " << std::ctime(&time);
	}
}

__attribute__((optimize("O0")))
int main(int argc, char *argv[]) {
	if (argc != 2)
		return -1;
	std::ofstream ofile("memory_errors.csv");
	auto size = (size_t) atof(argv[1]);
	const static size_t PAGE_SIZE = 1024 * 4;
	size = (size + PAGE_SIZE - 1) & (~(PAGE_SIZE-1));
	std::cout << "TEST size = " << (double)size / 1024 / 1024 / 1024 << " Gb" << std::endl;
	uint64_t *data = (uint64_t*) mmap(
			nullptr, size,
			PROT_WRITE | PROT_READ,
			MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE,
			-1, 0);
	if (mlock(data, size) != 0) {
		std::cout << "Cannot lock pages" << std::endl;
		return -1;
	}
	size_t cnt = size / sizeof(uint64_t);
	for(size_t i = 0; i < cnt; i++) {
		data[i] = pattern;
	}
	std::vector<std::thread> readers;
	readers.push_back(std::move(std::thread(read_memcell, &data[128])));
	readers.push_back(std::move(std::thread(read_memcell, &data[129])));
	readers.push_back(std::move(std::thread(read_memcell, &data[1000])));
	readers.push_back(std::move(std::thread(read_memcell, &data[1012])));
	readers.push_back(std::move(std::thread(read_memcell, &data[2012])));
	readers.push_back(std::move(std::thread(read_memcell, &data[1024*4])));
	readers.push_back(std::move(std::thread(read_all_data, data, cnt / 2)));
	while(true) {
		int total_errors = 0;
		for(size_t i = 0; i < cnt; i++) {
			if (data[i] != pattern)
				total_errors++;
		}
		auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::cout << "Error cnt " << total_errors << " " << std::ctime(&time);
		ofile << time << ";" << total_errors << std::endl;
		sleep(10 * 60);
	}
}
