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
 *  @date 3/1/17
 */
#pragma once


#include <functional>
#include <memory>
#include <forward_list>
#include "shared_function.h"

namespace rrc {
    typedef shared_function<void()> finalize_callback;

    class finalizer {
    public:
        void add_callback(finalize_callback callback_ptr);

        void remove_callback(finalize_callback callback_ptr);

        void exec_and_clear();
    private:
        std::forward_list<finalize_callback> m_callback_list;

    };
}
