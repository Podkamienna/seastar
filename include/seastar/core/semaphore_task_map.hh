/*
 * This file is open source software, licensed to you under the terms
 * of the Apache License, Version 2.0 (the "License").  See the NOTICE file
 * distributed with this work for additional information regarding copyright
 * ownership.  You may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (C) 2016 ScyllaDB
 */


#ifndef SEASTAR_SEMAPHORE_TASK_MAP_HH
#define SEASTAR_SEMAPHORE_TASK_MAP_HH

#ifdef SEASTAR_DEADLOCK_DETECTION
#include <seastar/core/semaphore.hh>
#include <seastar/core/task.hh>

namespace seastar::internal {
semaphore_task_map &get_task_semaphore_map();

class semaphore_task_maps {
public:
    semaphore_task_map() = default;

    void add (seastar::semaphore *sem, seastar::task *tsk) {
    auto [_, inserted_tsk_sem] = task_semaphore_map.insert({tsk, sem})

    if (!inserted_tsk_sem) {
    std::cerr << "Something unexepected happened, the task " << tsk << " was already waiting on the semaphore " << sem << endl;
    }

    auto sem_tsks_iterator = semaphore_tasks_map.find(sem);
    if (sem_tsks_iterator == semaphore_tasks_map.end()) {
        semaphore_tasks_map[semaphore].insert(tsk);
    } else {
        auto [_, inserted_sem_tsks] = *sem_tsks_iterator.second.insert(tsk);
        if (!inserted_sem_tsks) {
        std::cerr << "Something unexepected happened, the task " << tsk << " was already waiting on the semaphore " << sem << endl;
        }
    }

    }

    void delete (seastar::semaphore *sem, seastar::task *tsk) {
    auto tsk_sem_iterator = task_semaphore_map.find(tsk);
    if (tsk_sem_iterator == task_semaphore_map.end()) {
    std::cerr << "Something unexepected happened, the task " << tsk << " was not waiting on the semaphore " << sem << endl;
    } else {
        if (*tsk_sem_iterator.second != sem) {
        std::cerr << "Something unexepected happened, the task " << tsk << " was not waiting on the semaphore " << sem << endl;
        } else {
        task_semaphore_map.erase(tsk_sem_iterator);
        }
    }

    auto sem_tsks_iterator = semaphore_tasks_map.find(tsk);
    if (sem_tsks_iterator == semaphore_tasks_map.end()) {
    std::cerr << "Something unexepected happened, the task " << tsk << " was not waiting on the semaphore " << sem << endl;
    } else {
        auto tsk_set_iterator = *sem_tsks_iterator.second.find(tsk);
        if (tsk_set_iterator == *sem_tsks_iterator.second.end()) {
        std::cerr << "Something unexepected happened, the task " << tsk << " was not waiting on the semaphore " << sem << endl;
        } else {
        *sem_tsks_iterator.second.erase(tsk_set_iterator);
        }
    }
    }

private:
    /**
    * Map from task to a semaphore that it is currently waiting on. If the task expires,
    * here it is not longer considered to be waiting on the semaphore.
    */
    std::unordered_map<seastar::task*, seastar::semaphore*> task_semaphore_map;

    /**
    * Map from a semaphore to set of tasks waiting on it. If a task expires,
    * here it is not longer considered to be waiting on the semaphore.
    */
    std::unordered_map<seastar::semaphore*, std::unordered_set<seastar::task*>> semaphore_tasks_map;
};


}
#endif

#endif //SEASTAR_SEMAPHORE_TASK_MAP_HH
