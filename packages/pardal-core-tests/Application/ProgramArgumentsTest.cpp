#include <gtest/gtest.h>
#include "Application/ProgramArguments.h"
#include "Containers/UnorderedMap.h"
#include "String/String.h"

using pdl::ProgramArguments;
using pdl::String;
using pdl::StringView;
using pdl::UnorderedMap;


namespace pdl {
namespace Tests {

static UnorderedMap<String, String> Parse(StringView line)
{
    UnorderedMap<String, String> opts = ProgramArguments::CommandLineToProgramArguments(line);
    return opts;
}

TEST(ProgramArgumentsTest, ParsesSimpleDashAndDoubleDashPairs)
{
    auto opts = Parse("app.exe -width 1920 --height 1080");

    ASSERT_EQ(opts.size(), 2u);
    EXPECT_EQ(opts.at("width"), String("1920"));
    EXPECT_EQ(opts.at("height"), String("1080"));
}

TEST(ProgramArgumentsTest, ParsesQuotedValuesWithSpaces)
{
    auto opts = Parse("app.exe --title \"My Awesome App\" -path \"C:/Program Files/App\"");

    ASSERT_EQ(opts.size(), 2u);
    EXPECT_EQ(opts.at("title"), String("My Awesome App"));
    EXPECT_EQ(opts.at("path"), String("C:/Program Files/App"));
}

TEST(ProgramArgumentsTest, HandlesFlagsWithoutValues)
{
    auto opts = Parse("app.exe -debug --vsync");

    ASSERT_EQ(opts.size(), 2u);
    EXPECT_EQ(opts.at("debug"), String());
    EXPECT_EQ(opts.at("vsync"), String());
}

TEST(ProgramArgumentsTest, DoesNotConsumeNextKeyAsValue)
{
    auto opts = Parse("app.exe --a --b 2");

    ASSERT_EQ(opts.size(), 2u);
    EXPECT_EQ(opts.at("a"), String());
    EXPECT_EQ(opts.at("b"), String("2"));
}

TEST(ProgramArgumentsTest, IgnoresBareTokensAndMoreThanTwoDashes)
{
    auto opts = Parse("app.exe bare ---too-many 1 --ok yes");

    ASSERT_EQ(opts.size(), 1u);
    EXPECT_EQ(opts.at("ok"), String("yes"));
}

TEST(ProgramArgumentsTest, IgnoresLoneDashAndDoubleDash)
{
    auto opts = Parse("app.exe - -- -a 1 -- --b 2");

    // Only "a" should be captured once, with value "1"; lone '-' and '--' are ignored,
    // and the trailing "--b" is a key with value "2".
    ASSERT_EQ(opts.size(), 2u);
    EXPECT_EQ(opts.at("a"), String("1"));
    EXPECT_EQ(opts.at("b"), String("2"));
}

} // namespace Tests
} // namespace pdl
