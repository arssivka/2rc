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
 *  @date 3/17/17
 */
#pragma once


#include <forward_list>
#include <dotconf.h>
#include "callback_defines.h"

namespace rrc {
    template <class T>
    class notifier {
    public:
        typedef T callback_type;

        bool add_listener(callback_type callback, const rrc::result_callback& result) {
            if (callback) {
                m_listeners.push_front(callback);
                if (result) result(RESULT_CODE_SUCCESS);
                return true;
            } else if (result) {
                result(RESULT_CODE_FAIL);
            }
            return false;
        }

        bool remove_listener(const callback_type& callback, const rrc::result_callback& result) {
            result_code code = RESULT_CODE_FAIL;
            m_listeners.remove_if([&code, &callback](const key_callback& curr) -> bool {
                if (callback == curr) {
                    code = RESULT_CODE_SUCCESS;
                    return true;
                }
                return false;
            });
            if (result) result(code);
            return code == RESULT_CODE_SUCCESS;
        }

        template <class... Args>
        void notify(Args&&... args) {
            for (auto&& callback : m_listeners) {
                callback(std::forward<Args>(args)...);
            }
        }

    private:
        std::forward_list<callback_type> m_listeners;
    };
}

