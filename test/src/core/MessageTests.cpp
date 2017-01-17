/**
 *  @autor tekatod
 *  @date 1/11/17
 */
#include "gtest/gtest.h"
#include <rrc/core/MetaFunctions.h>
#include <rrc/core/MetaGenerators.h>
#include <rrc/core/MetaArray.h>
#include <rrc/core/Message.h>

using namespace rrc::meta;
using namespace rrc;

// We can't test fucking metaprogramming without fucking types!
typedef int fuck;


TEST(MetaTest, Message) {
    Message fuckMessage(0, 0, 0, 0, 0);
    auto res = fuckMessage.getMetaData();
    auto pack = meta::Pack<int8_t, INT32_ID, INT32_ID>::value;
    EXPECT_EQ(res[0], pack);
    EXPECT_EQ(fuckMessage.getMetaDataSize(), (int_fast8_t) 3);
    Message notOnlyFuckMessage(std::vector<fuck>{0, 0, 0}, 0);
    auto res1 = notOnlyFuckMessage.getMetaData();
    auto pack1 = meta::Pack<int8_t, INT32_ARRAY_ID, INT32_ID>::value;
    EXPECT_EQ((int_fast8_t) 1, notOnlyFuckMessage.getMetaDataSize());
    EXPECT_EQ(res1[0], pack1);
    Message notOnlyFuckMessage1(std::map<fuck, fuck>{});
    auto res2 = notOnlyFuckMessage1.getMetaData();
    auto pack2 = meta::Pack<int8_t, STRUCTURE_ARRAY_ID, INT32_ID>::value;
    EXPECT_EQ((int_fast8_t) 2, notOnlyFuckMessage1.getMetaDataSize());
    EXPECT_EQ(res2[0], pack2);
}