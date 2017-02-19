/**
 * Copyright 2016 Arseniy Ivin <arssivka@yandex.ru>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *  @autor arssivka
 *  @date 2/7/17
 */

#pragma once


#include <vector>
#include "service.h"
#include "service_holder.h"
#include "queue_adapter_factory.h"

namespace rrc {
    class service_mechanism {
    public:
        using key_type = std::string;
        using message_type = service_holder<key_type>::message_type;
        using listener_type = service_holder<key_type>::listener_type;
        using callback_type = service_holder<key_type>::callback_type;

        service_mechanism(std::shared_ptr<abstract_task_queue_adapter> sync_queue,
                          queue_adapter_factory<task>& task_queue_factory);

        void add_service(key_type key, std::shared_ptr<service> service_ptr) {
            m_services_queue->enqueue(
                    [this, key_cap = std::move(key),
                            service_ptr_cap = std::move(service_ptr)]() mutable {
                        m_service_holder.remove_service(key_cap, std::move(service_ptr_cap));
                    });
            this->enqueue_update();
        }

        void remove_service(key_type key, const std::shared_ptr<service> service_ptr) {
            m_services_queue->enqueue(
                    [this, key_cap = std::move(key),
                            service_ptr_cap = std::move(service_ptr)]() mutable {
                        m_service_holder.remove_service(key_cap, std::move(service_ptr_cap));
                    });
            this->enqueue_update();
        }

        void call(key_type key, std::shared_ptr<listener_type> listener_ptr, message_type input) {
            m_calls_queue->enqueue(
                    [this, key_cap = std::move(key),
                            listener_ptr_cap = std::move(listener_ptr),
                            input_cap = std::move(input)]() mutable {
                        m_service_holder.call(key_cap, std::move(listener_ptr_cap), std::move(input_cap));
                    });
            this->enqueue_update();
        }

        std::vector<key_type> keys() const;

    private:
        void enqueue_update();

        void apply_queues();

    private:
        service_holder<key_type> m_service_holder;
        std::shared_ptr<abstract_task_queue_adapter> m_sync_queue;
        std::unique_ptr<abstract_task_queue_adapter> m_services_queue;
        std::unique_ptr<abstract_task_queue_adapter> m_calls_queue;
        std::atomic_flag m_changes_enqueued_flag;
    };
}


