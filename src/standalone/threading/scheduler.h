#pragma once

#include "atomic_circular_deque.h"
#include "utils/global.h"
#include "synthesis/synthesizer.h"
#include "synthesis/modules/module.h"

#include <array> 
#include <atomic>
#include <barrier>
#include <deque>   
#include <thread>

namespace synthesis {
    class Scheduler {
    public:
        Scheduler(Synthesizer& synthesizer);
        void launch_threads();
        void worker_loop(std::stop_token stop_token, std::barrier<>* init_sync, size_t id);
        void scheduler_loop();

        struct WorkerData {
            AtomicCircularDeque<Module*, 64> work_deque;
            size_t id;
            bool busy;
        };

        static constexpr size_t num_threads = 8;
        alignas(std::hardware_destructive_interference_size)
            std::atomic<float_s*> out_buf;
        alignas(std::hardware_destructive_interference_size)
            std::atomic<uint32_t> sound_engine_buffer_counter;
        alignas(std::hardware_destructive_interference_size)
            std::atomic<bool> buffer_ready;

    private:
        Synthesizer& synthesizer_;
        std::array<std::jthread, num_threads> threads;
        std::array<WorkerData, num_threads> worker_data;
        alignas(std::hardware_destructive_interference_size)
            std::atomic<uint32_t> scheduler_buffer_counter; // feels like pipeline latches
        alignas(std::hardware_destructive_interference_size)
            std::atomic<size_t> tasks_remaining;
    };
}