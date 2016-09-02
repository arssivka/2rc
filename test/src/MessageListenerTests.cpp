/**
 *  @autor arssivka
 *  @date 9/2/16
 */

#include <gtest/gtest.h>
#include <rrc/core/MetaTable.h>
#include <Message.pb.h>
#include <rrc/core/MessageListener.h>


using namespace rrc;

class ListenerFixture : public ::testing::Test {
public:
    ListenerFixture() {
        mMetaTable.registerTypeId<message::TestMessage>(1);
        mMetaTable.registerTypeId<message::TestMessageContainer>(2);
    }

    ~ListenerFixture() {
        //Destroy all the good members
    }

protected:
    MetaTable mMetaTable;

};


TEST_F(ListenerFixture, GetTypeId) {
    TypeId typeId = mMetaTable.getTypeId<message::TestMessage>();
    MessageListener listener(typeId);
    EXPECT_EQ(listener.getTypeId(), typeId);
}


TEST_F(ListenerFixture, GetMessageFromEmptyQueue) {
    TypeId typeId = mMetaTable.getTypeId<message::TestMessage>();
    MessageListener listener(typeId);
    EXPECT_EQ(listener.tryDequeueMessage(), nullptr);
}


TEST_F(ListenerFixture, GetMessages1) {
    TypeId typeId = mMetaTable.getTypeId<message::TestMessage>();
    MessageListener listener(typeId);
    MessagePtr message = std::make_shared<Message>(
            typeId,
            std::chrono::steady_clock::now(),
            std::make_unique<message::TestMessage>()
    );
    listener.enqueueMessage(message);
    ASSERT_EQ(listener.tryDequeueMessage(), message);
    ASSERT_EQ(listener.tryDequeueMessage(), nullptr);
}


TEST_F(ListenerFixture, GetMessages2) {
    TypeId typeId = mMetaTable.getTypeId<message::TestMessage>();
    MessageListener listener(typeId);
    MessagePtr message = std::make_shared<Message>(
            typeId,
            std::chrono::steady_clock::now(),
            std::make_unique<message::TestMessage>()
    );
    listener.enqueueMessage(message);
    listener.enqueueMessage(message);
    ASSERT_EQ(listener.tryDequeueMessage(), message);
    ASSERT_EQ(listener.tryDequeueMessage(), message);
    ASSERT_EQ(listener.tryDequeueMessage(), nullptr);
}


TEST_F(ListenerFixture, GetMessages3) {
    TypeId typeId1 = mMetaTable.getTypeId<message::TestMessage>();
    TypeId typeId2 = mMetaTable.getTypeId<message::TestMessageContainer>();
    MessageListener listener(typeId1);

    MessagePtr message1 = std::make_shared<Message>(
            typeId1,
            std::chrono::steady_clock::now(),
            std::make_unique<message::TestMessage>()
    );

    MessagePtr message2 = std::make_shared<Message>(
            typeId2,
            std::chrono::steady_clock::now(),
            std::make_unique<message::TestMessageContainer>()
    );

    listener.enqueueMessage(message1);
    listener.enqueueMessage(message2);
    ASSERT_EQ(listener.tryDequeueMessage(), message1);
    ASSERT_EQ(listener.tryDequeueMessage(), nullptr);
}