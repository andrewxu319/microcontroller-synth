#include "scheduler.h"

#include "utils/rng.h"

#include <functional>

#if defined(_DEBUG) && defined(_MSC_VER)
#include <windows.h>
#endif

using namespace synthesis;

Scheduler::Scheduler(const std::vector<Module*>& starting_tasks)
    : starting_tasks_{ starting_tasks },
    num_tasks{},
    threads{},
    worker_data{},
    tasks_remaining{},
    all_tasks_completed{ false }
{}

void Scheduler::launch_threads() {
    std::barrier worker_init_sync(num_threads + 1);
    for (size_t i{}; i < num_threads; i++) {
        threads[i] = std::jthread(std::bind_front(&Scheduler::worker_loop, this), &worker_init_sync, i);
    }
    worker_init_sync.arrive_and_wait();
}

// TODO: make modules with internal modules create input-output relationships
void Scheduler::worker_loop(std::stop_token stop_token, std::barrier<>* init_sync, size_t id) {
#if defined(_DEBUG) && defined (_MSC_VER)
    SetThreadDescription(GetCurrentThread(), (L"Synth worker " + std::to_wstring(id)).c_str());
#endif
    WorkerData& data{ worker_data[id] };
    uint32_t completed_buffer_counter{};

    init_sync->arrive_and_wait();

    while (!stop_token.stop_requested()) {
        uint32_t scheduler_buffer_counter{ buffer_counter.load(std::memory_order_acquire) };
        
        while (scheduler_buffer_counter == completed_buffer_counter) {
            buffer_counter.wait(scheduler_buffer_counter, std::memory_order_acquire); // wait while
            if (stop_token.stop_requested()) return;
            scheduler_buffer_counter = buffer_counter.load(std::memory_order_acquire);
        }

        // generate buffer
        while (tasks_remaining.load(std::memory_order_acquire) > 0) {
            Module* current_task{};
            if (data.work_deque.pop_back(&current_task) != 0) {
                // steal
                size_t target{ utils::rng_uniform_int<size_t>(0, num_threads - 1) };
                while (tasks_remaining.load(std::memory_order_acquire) > 0)
                {
                    // rng
                    if (worker_data[target].work_deque.pop_front(&current_task) == 0) {
                        break;
                    } else {
                        target = (target + 1) % num_threads;
                    }
                }
            }

            if (current_task) {
                current_task->generate_buf();
                for (Module* next_task : current_task->outputs) {
                    if (next_task->num_dependencies_visited.fetch_add(1, std::memory_order_acq_rel) + 1 == next_task->num_dependencies) {
                        next_task->num_dependencies_visited.store(0, std::memory_order_relaxed);
                        data.work_deque.push_back(next_task);
                    }
                }
                tasks_remaining.fetch_sub(1, std::memory_order_release);
                if (tasks_remaining.load(std::memory_order_acquire) == 0) {
                    all_tasks_completed.store(true, std::memory_order_release);
                    all_tasks_completed.notify_one();
                }
            }
        }

        completed_buffer_counter = scheduler_buffer_counter;
    }
}

void Scheduler::generate_buf() {
    all_tasks_completed.store(false, std::memory_order_release);
    for (size_t i{}; i < starting_tasks_.size(); i++) {
        worker_data[i % num_threads].work_deque.push_back(starting_tasks_[i]);
    }
    tasks_remaining.store(num_tasks, std::memory_order_release);
    buffer_counter.fetch_add(1, std::memory_order_release);
    buffer_counter.notify_all();

    all_tasks_completed.wait(false, std::memory_order_seq_cst); // wait until it's no longer false
}

void Scheduler::set_num_tasks(size_t value) {
    num_tasks = value;
}