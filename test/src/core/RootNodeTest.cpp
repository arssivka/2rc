/**
 *  @autor arssivka
 *  @date 9/2/16
 */

#include <gtest/gtest.h>
#include <rrc/core/LinearLauncher.h>
#include <rrc/core/RootNode.h>
#include <Message.pb.h>
#include <rrc/core/RejectMessageFilter.h>
#include "include/DummyNode.h"

using namespace rrc;


class RootNodeFixture : public ::testing::Test {
public:
    RootNodeFixture() {
        mMetaTable.registerTypeId<testmessages::TestMessage>(0);
        mMetaTable.registerTypeId<testmessages::TestMessageContainer>(1);
    }

protected:
    MetaTable mMetaTable;
    LinearLauncher mLinearLauncher;

};


TEST_F(RootNodeFixture, AddAndRemoveListenerTest1) {
    RootNodePtr rootNode = std::make_shared<RootNode>(mLinearLauncher, mMetaTable);
    TypeId typeId = mMetaTable.getTypeId<testmessages::TestMessage>();
    QueueMessageListenerPtr listener = std::make_shared<QueueMessageListener>(typeId);
    rootNode->addListener("test", listener);
    rootNode->entry();
    EXPECT_FALSE(listener.unique());
    rootNode->removeListener("test", listener);
    EXPECT_FALSE(listener.unique());
    rootNode->entry();
    EXPECT_TRUE(listener.unique());
}


TEST_F(RootNodeFixture, AddAndRemoveListenerTest2) {
    RootNodePtr rootNode = std::make_shared<RootNode>(mLinearLauncher, mMetaTable);
    TypeId typeId = mMetaTable.getTypeId<testmessages::TestMessage>();
    QueueMessageListenerPtr listener = std::make_shared<QueueMessageListener>(typeId);
    rootNode->addListener("test", listener);
    EXPECT_FALSE(listener.unique());
    rootNode->removeListener("test", listener);
    EXPECT_FALSE(listener.unique());
    rootNode->entry();
    EXPECT_TRUE(listener.unique());
}


TEST_F(RootNodeFixture, AddAndRemoveListenerTest3) {
    RootNodePtr rootNode = std::make_shared<RootNode>(mLinearLauncher, mMetaTable);
    TypeId typeId = mMetaTable.getTypeId<testmessages::TestMessage>();
    QueueMessageListenerPtr listener = std::make_shared<QueueMessageListener>(typeId);
    EXPECT_NO_THROW(rootNode->removeListener("test", listener));
    rootNode->entry();
    EXPECT_TRUE(listener.unique());
}


TEST_F(RootNodeFixture, AddAndRemoveNodeTest) {
    RootNodePtr rootNode = std::make_shared<RootNode>(mLinearLauncher, mMetaTable);
    std::shared_ptr<DummyNode> dummyNode = std::make_shared<DummyNode>(rootNode, "test");
    rootNode->addNode(dummyNode);
    EXPECT_FALSE(dummyNode.unique());
    rootNode->removeNode(dummyNode);
    EXPECT_FALSE(dummyNode.unique());
    rootNode->entry();
    EXPECT_TRUE(dummyNode.unique());
}


TEST_F(RootNodeFixture, GetTypeIdTest) {
    MetaTable metaTable;
    metaTable.registerTypeId<testmessages::TestMessage>(0u);
    RootNodePtr rootNode = std::make_shared<RootNode>(mLinearLauncher, metaTable);
    EXPECT_EQ(rootNode->getTypeId<testmessages::TestMessage>(), 0u);
    EXPECT_EQ(rootNode->getTypeId<testmessages::TestMessageContainer>(), MetaTable::UNKNOWN_TYPE_ID);
}


TEST_F(RootNodeFixture, SendMessage1) {
    RootNodePtr rootNode = std::make_shared<RootNode>(mLinearLauncher, mMetaTable);
    std::shared_ptr<DummyNode> dummyNode1 = std::make_shared<DummyNode>(rootNode, "test");
    std::shared_ptr<DummyNode> dummyNode2 = std::make_shared<DummyNode>(rootNode, "test");

    testmessages::TestMessage tstMessage;
    tstMessage.set_id(42);
    tstMessage.set_txt("42");

    MessagePtr message1 = std::make_shared<Message>(
            mMetaTable.getTypeId<testmessages::TestMessage>(),
            std::chrono::steady_clock::now(),
            std::make_unique<testmessages::TestMessage>(tstMessage)
    );

    MessagePtr message2 = std::make_shared<Message>(
            mMetaTable.getTypeId<testmessages::TestMessageContainer>(),
            std::chrono::steady_clock::now(),
            std::make_unique<testmessages::TestMessageContainer>()
    );

    rootNode->sendMessage("test", message1);
    rootNode->sendMessage("test", message2);

    rootNode->entry();
    EXPECT_EQ(dummyNode1->tryGetMessage(), message1);
    EXPECT_EQ(dummyNode1->tryGetMessage(), nullptr);
    EXPECT_EQ(dummyNode2->tryGetMessage(), message1);
    EXPECT_EQ(dummyNode2->tryGetMessage(), nullptr);
}


TEST_F(RootNodeFixture, SetTopicMessageFilterTest1) {
    RootNodePtr rootNode = std::make_shared<RootNode>(mLinearLauncher, mMetaTable);
    std::shared_ptr<DummyNode> dummyNode1 = std::make_shared<DummyNode>(rootNode, "test");
    std::shared_ptr<DummyNode> dummyNode2 = std::make_shared<DummyNode>(rootNode, "test");

    testmessages::TestMessage tstMessage;
    tstMessage.set_id(42);
    tstMessage.set_txt("42");

    MessagePtr message1 = std::make_shared<Message>(
            mMetaTable.getTypeId<testmessages::TestMessage>(),
            std::chrono::steady_clock::now(),
            std::make_unique<testmessages::TestMessage>(tstMessage)
    );

    MessagePtr message2 = std::make_shared<Message>(
            mMetaTable.getTypeId<testmessages::TestMessageContainer>(),
            std::chrono::steady_clock::now(),
            std::make_unique<testmessages::TestMessageContainer>()
    );

    rootNode->setTopicMessageFilter("test", std::make_shared<RejectMessageFilter>());
    rootNode->entry();
    rootNode->sendMessage("test", message1);
    rootNode->sendMessage("test", message2);
    rootNode->entry();
    EXPECT_EQ(dummyNode1->tryGetMessage(), nullptr);
    EXPECT_EQ(dummyNode2->tryGetMessage(), nullptr);
}


TEST_F(RootNodeFixture, SetTopicMessageFilterTest) {
    RootNodePtr rootNode = std::make_shared<RootNode>(mLinearLauncher, mMetaTable);
    auto filter = std::make_shared<RejectMessageFilter>();
    TypeId typeId = mMetaTable.getTypeId<testmessages::TestMessage>();
    QueueMessageListenerPtr listener = std::make_shared<QueueMessageListener>(typeId);
    rootNode->setTopicMessageFilter("test", filter);
    rootNode->addListener("test", listener);
    rootNode->entry();
    rootNode->removeListener("test", listener);
    rootNode->entry();
    EXPECT_FALSE(filter.unique());
}


TEST_F(RootNodeFixture, RemoveTopicTest1) {
    RootNodePtr rootNode = std::make_shared<RootNode>(mLinearLauncher, mMetaTable);
    TypeId typeId = mMetaTable.getTypeId<testmessages::TestMessage>();
    QueueMessageListenerPtr listener = std::make_shared<QueueMessageListener>(typeId);
    QueueMessageListenerPtr listener2 = std::make_shared<QueueMessageListener>(typeId);
    rootNode->addListener("test", listener);
    rootNode->addListener("test", listener2);
    rootNode->entry();
    EXPECT_NO_THROW(rootNode->removeTopic("test"));
    EXPECT_FALSE(listener.unique());
    EXPECT_FALSE(listener2.unique());
    rootNode->entry();
    EXPECT_TRUE(listener.unique());
    EXPECT_TRUE(listener2.unique());
    std::vector<RootNode::Key> topicNames = rootNode->getTopicNames();
    EXPECT_TRUE(std::find(topicNames.cbegin(), topicNames.cend(), "test") == topicNames.end());
}


TEST_F(RootNodeFixture, RemoveTopicTest2) {
    RootNodePtr rootNode = std::make_shared<RootNode>(mLinearLauncher, mMetaTable);
    EXPECT_NO_THROW(rootNode->removeTopic("test"));
    EXPECT_TRUE(rootNode->getTopicNames().empty());
}


TEST_F(RootNodeFixture, GetTopicNamesTest) {
    RootNodePtr rootNode = std::make_shared<RootNode>(mLinearLauncher, mMetaTable);
    TypeId typeId = mMetaTable.getTypeId<testmessages::TestMessage>();
    QueueMessageListenerPtr listener = std::make_shared<QueueMessageListener>(typeId);
    EXPECT_TRUE(rootNode->getTopicNames().empty());
    rootNode->addListener("test", listener);
    EXPECT_TRUE(rootNode->getTopicNames().empty());
    rootNode->entry();
    EXPECT_FALSE(rootNode->getTopicNames().empty());
}


TEST_F(RootNodeFixture, SetTopicAutoRemoveFlagTest) {
    RootNodePtr rootNode = std::make_shared<RootNode>(mLinearLauncher, mMetaTable);
    TypeId typeId = mMetaTable.getTypeId<testmessages::TestMessage>();
    QueueMessageListenerPtr listener = std::make_shared<QueueMessageListener>(typeId);
    EXPECT_TRUE(rootNode->getTopicNames().empty());
    rootNode->setTopicAutoRemoveFlag("test", true);
    EXPECT_TRUE(rootNode->getTopicNames().empty());
    rootNode->entry();
    EXPECT_FALSE(rootNode->getTopicNames().empty());
}