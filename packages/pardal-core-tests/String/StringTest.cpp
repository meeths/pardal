#include <gtest/gtest.h>
#include <String/String.h>
#include <String/StringUtils.h>
#include <sstream>
#include <string>

namespace pdl { namespace Tests {

TEST(StringTest, BasicConstructionAndSize)
{
    String s("hello");
    EXPECT_EQ(s.size(), 5u);
    EXPECT_STREQ(s.c_str(), "hello");

    String s2("world");
    EXPECT_EQ(s2.size(), 5u);
    EXPECT_EQ(StringView(s2), StringView("world"));
}

TEST(StringTest, AppendAndFind)
{
    String s("foo");
    s += "bar";
    EXPECT_EQ(StringView(s), StringView("foobar"));
    EXPECT_EQ(s.find('b'), 3u);
    EXPECT_EQ(s.find("bar"), 3u);
}

TEST(StringTest, ReplaceAndTrim)
{
    String s("  a b a  ");
    auto trimmed = StringUtils::Trim(s);
    EXPECT_EQ(StringView(trimmed), StringView("a b a"));

    auto replaced = StringUtils::Replace(trimmed, String(" "), String("_"));
    EXPECT_EQ(StringView(replaced), StringView("a_b_a"));
}

TEST(StringTest, StreamInsertion)
{
    String s("stream");
    std::ostringstream oss;
    oss << s.c_str();
    EXPECT_EQ(oss.str(), std::string("stream"));
}

TEST(StringViewTest, BasicAndEndsWith)
{
    StringView v("abcdef");
    EXPECT_EQ(v.size(), 6u);
    EXPECT_TRUE(v.ends_with(StringView("def")));
    EXPECT_FALSE(v.ends_with(StringView("abc")));

    String s("hello world");
    StringView sv(s);
    EXPECT_TRUE(sv.starts_with(StringView("hello")));
}

TEST(StringInteropTest, StdInterop)
{
    String s("interop");
    std::string stds = static_cast<std::string>(s.c_str());
    EXPECT_EQ(stds, std::string("interop"));

    String fromStd(std::string("abc").c_str());
    EXPECT_EQ(StringView(fromStd), StringView("abc"));
}

}} // namespace
