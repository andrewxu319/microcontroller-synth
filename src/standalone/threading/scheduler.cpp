#include "scheduler.h"

#include "utils/rng.h"
#include "utils/timer.h"

#include <functional>

#if defined(_DEBUG) && defined(_MSC_VER)
#include <windows.h>
#endif

using namespace synthesis;

Scheduler::Scheduler(Synthesizer& synthesizer)
    : synthesizer_{ synthesizer },
    threads{},
    worker_data{},
    scheduler_buffer_counter{},
    sound_engine_buffer_counter{},
    tasks_remaining{},
    buffer_ready{ false }
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
        uint32_t local_scheduler_buffer_counter{ scheduler_buffer_counter.load(std::memory_order_acquire) };
        
        // scheduler_buffer_counter needs to be ahead by 1 to proceed
        while (local_scheduler_buffer_counter <= completed_buffer_counter) {
            scheduler_buffer_counter.wait(local_scheduler_buffer_counter, std::memory_order_acquire); // wait while
            if (stop_token.stop_requested()) return;
            local_scheduler_buffer_counter = scheduler_buffer_counter.load(std::memory_order_acquire);
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
                if (tasks_remaining.fetch_sub(1, std::memory_order_release) == 1) { // fetch_sub returns previous value
                    buffer_ready.store(true, std::memory_order_release);
                    buffer_ready.notify_one();
                }
            }
        }

        completed_buffer_counter = local_scheduler_buffer_counter;
    }
}

void Scheduler::scheduler_loop() {
    uint32_t completed_buffer_counter{ 0 };
    
    while (true) { // while (!stop_token.stop_requested())
        uint32_t local_sound_engine_buffer_counter{ sound_engine_buffer_counter.load(std::memory_order_acquire) };
        
        while (local_sound_engine_buffer_counter == completed_buffer_counter) {
            sound_engine_buffer_counter.wait(local_sound_engine_buffer_counter, std::memory_order_acquire); // wait while
            //if (stop_token.stop_requested()) return;
            local_sound_engine_buffer_counter = sound_engine_buffer_counter.load(std::memory_order_acquire);
        }

        buffer_ready.store(false, std::memory_order_release);

        synthesizer_.generate_buf(out_buf.load(std::memory_order_relaxed));

        if constexpr (config::multithread) {
            for (size_t i{}; i < synthesizer_.starting_modules.size(); i++) {
                worker_data[i % num_threads].work_deque.push_back(synthesizer_.starting_modules[i]);
            }
            tasks_remaining.store(synthesizer_.modules.size(), std::memory_order_release);
            scheduler_buffer_counter.fetch_add(1, std::memory_order_release);
            scheduler_buffer_counter.notify_all();

            // workers do work

            buffer_ready.wait(false, std::memory_order_seq_cst); // wait until it's no longer false
        }
        else {
            buffer_ready.store(true, std::memory_order_release);
        }

        completed_buffer_counter = local_sound_engine_buffer_counter;

        utils::timer::end("generate_buf");
        utils::timer::start();
    }
}