#include <iostream>
#include <thread>
#include <mutex>
#include "core/compress.h"
#include "helper/cpp_assert.h"

void worker(std::vector<uint8_t> & input, std::vector<uint8_t> & output,
    const std::atomic_bool & running, std::atomic_bool & start)
{
    while (running)
    {
        if (start)
        {
            arithmetic::Decode decoder(input, output);
            decoder.decode();
            start = false;
        }

        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
}

int main()
{
    std::vector < std::vector<uint8_t> > input;
    std::vector < std::vector<uint8_t> > output;
    std::vector < std::thread > threads;
    std::vector < std::unique_ptr < std::atomic_bool > > starts;
    std::atomic_bool running(true);
    const int thread_count = static_cast<int>(std::thread::hardware_concurrency());
    std::vector<uint8_t> buffer;
    int current = 0;
    uint16_t block_size = 0;

    input.resize(thread_count);
    output.resize(thread_count);
    starts.resize(thread_count);
    for (int i = 0; i < thread_count; ++i) {
        starts[i] = std::make_unique<std::atomic_bool>(false);
    }

    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(worker,
            std::ref(input[i]), std::ref(output[i]), std::ref(running), std::ref(*starts[i]));
    }

    auto flush_cache = [&starts, &output](const int i)->void
    {
        while (*starts[i]) std::this_thread::sleep_for(std::chrono::nanoseconds(1));
        std::cout.write((char*)output[i].data(), static_cast<std::streamsize>(output[i].size()));
        output[i].clear();
    };

    while (std::cin)
    {
        std::cin.read((char*)&block_size, sizeof(uint16_t));
        buffer.resize(block_size);
        std::cin.read((char*)buffer.data(), block_size);
        const uint64_t read_size = std::cin.gcount();
        if (read_size == 0) break;
        assert_short(read_size == block_size);

        {
            input[current] = buffer;
            *starts[current] = true;
        }

        current++;

        if (current == thread_count)
        {
            current = 0;
            // output data:
            for (int i = 0; i < thread_count; i++) {
                flush_cache(i);
            }
        }
    }

    for (int i = 0; i < current; ++i)
    {
        flush_cache(i);
    }

    running = false;
    for (auto & thread : threads)
    {
        if (thread.joinable()) {
            thread.join();
        }
    }

    std::cout.flush();
    fflush(stdout);
    fflush(stdout);
    fflush(stdout);
    fflush(stdout);
    return 0;
}
