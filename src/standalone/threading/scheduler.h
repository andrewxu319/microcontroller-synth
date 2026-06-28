#pragma once

#include "atomic_circular_deque.h"
#include "utils/global.h"
#include "synthesis/modules/module.h"

#include <array> 
#include <atomic>
#include <barrier>
#include <deque>   
#include <thread>

namespace synthesis {
    class Scheduler {
    public:
        Scheduler(const std::vector<Module*>& starting_tasks);
        void launch_threads();
        void worker_loop(std::stop_token stop_token, std::barrier<>* init_sync, size_t id);
        void generate_buf();
        void set_num_tasks(size_t value);

        struct WorkerData {
            AtomicCircularDeque<Module*, 64> work_deque;
            size_t id;
            bool busy;
        };

        static constexpr size_t num_threads = 8;

    private:
        std::array<std::jthread, num_threads> threads;
        std::array<WorkerData, num_threads> worker_data;
        alignas(std::hardware_destructive_interference_size)
            std::atomic<uint32_t> buffer_counter;
        const std::vector<Module*>& starting_tasks_;
        size_t num_tasks;
        alignas(std::hardware_destructive_interference_size)
            std::atomic<size_t> tasks_remaining;
        alignas(std::hardware_destructive_interference_size)
            std::atomic<bool> all_tasks_completed;
    };
}