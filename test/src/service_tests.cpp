/**
 *  @autors tekatod, arssivka
 *  @date 2/13/17
 */

#include <gtest/gtest.h>
#include <rrc/core.h>

using namespace rrc;

class service_fixture
        : public ::testing::Test {
protected:
    virtual void SetUp() override {
        m_message = make_cow<std::string>("meow!");
        m_test_queue.reset((abstract_queue_adapter<task>*) new stl_queue_adapter<task>());
        m_result_queue.reset((abstract_queue_adapter<task>*) new stl_queue_adapter<task>());
    }

    virtual void TearDown() override {
    }


protected:
    cow_string m_message;
    std::shared_ptr<abstract_queue_adapter<task>> m_test_queue;
    std::shared_ptr<abstract_queue_adapter<task>> m_result_queue;
};

TEST_F(service_fixture, call_test) {
    cow_string result;
    std::shared_ptr<task_packer<cow_string>> callback = std::make_shared<task_packer<cow_string>>(
            m_result_queue, [&result](cow_string buffer) {
                result = std::move(buffer);
            });
    service service(m_test_queue, [](cow_string b) -> cow_string {
        return b;
    });
    EXPECT_TRUE(service.call(callback, m_message));
    EXPECT_TRUE(exec_once(m_test_queue));
    EXPECT_TRUE(exec_once(m_result_queue));
    EXPECT_EQ(*m_message, *result);
}