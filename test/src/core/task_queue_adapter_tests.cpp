/**
 *  @autor tekatod
 *  @date 2/7/17
 */

#include <gtest/gtest.h>
#include <rrc/core/abstract_task_queue_adapter.h>
#include <rrc/core/stl_queue_adapter.h>

using namespace rrc;

TEST(task_queue_adapter_tests, exec_once_test) {
    std::unique_ptr<abstract_task_queue_adapter> adapter(
            (abstract_task_queue_adapter*) new stl_queue_adapter<task>());
    int r;
    adapter->enqueue_task([](int first, int second, int& output) {
        output = first + second;
    }, 40, 2, std::ref(r));
    EXPECT_TRUE(adapter->exec_once());
    EXPECT_EQ(r, (int) 42);
}

TEST(task_queue_adapter_tests, exec_all_test) {
    std::unique_ptr<abstract_task_queue_adapter> adapter(
            (abstract_task_queue_adapter*) new stl_queue_adapter<task>());
    int r;
    int r1;
    int r2;
    adapter->enqueue_task([](int x1, int x2, int& x3) {
        x3 = x1 + x2;
    }, 40, 2, std::ref(r));
    adapter->enqueue_task([](int x1, int& x3) {
        x3 = x1;
    }, 42, std::ref(r1));
    adapter->enqueue_task([](int x1, int x2, int& x3) {
        x3 = x1 - x2;
    }, 44, 2, std::ref(r2));
    adapter->exec_all();
    EXPECT_EQ(r, (int) 42);
    EXPECT_EQ(r1, (int) 42);
    EXPECT_EQ(r2, (int) 42);
}