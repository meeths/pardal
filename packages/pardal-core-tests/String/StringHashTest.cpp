#include <String/StringHash.h>
#include <gtest/gtest.h>

namespace pdl { namespace Tests
{

struct StringHashTest : testing::Test
{
};
	
#define STRING_TO_HASH "abcABC!1234()=&&$"
	
TEST(StringHashTest, ConstexprProducesSameHash )
{
	const String nonConstexprString(STRING_TO_HASH);
	const StringHash stringHash(nonConstexprString);
	constexpr ConstStringHash constantStringHash(STRING_TO_HASH);
	ASSERT_TRUE(constantStringHash == stringHash);
}
}}
