#include <doctest/doctest.h>
#include <fmt/core.h>

#include <limits>
#include <sstream>
#include <string_view>
#include <variant>

#include "../lib/ErrorReporter.hpp"
#include "../lib/Lexer.hpp"
#include "../lib/Token.hpp"

// NOLINTBEGIN(cppcoreguidelines*, readability*)

TEST_CASE("lexer") {
  std::ostringstream out_buf;
  std::shared_ptr<std::ostream> stream_ptr(&out_buf, [](std::ostream *) {});
  ErrorReporter er{stream_ptr};

  SUBCASE("emtpy") {
    Lexer l("", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();
    CHECK(tokens.empty());
    CHECK(!er.hasError());
  }

  SUBCASE("multi lines emtpy") {
    Lexer l("\n    \t  \v \n\n\n       \r \n \n", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();
    CHECK(tokens.empty());
    CHECK(!er.hasError());
  }

  SUBCASE("empty string") {
    Lexer l("\"\"", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.at(0)->getType() == TokenType::TOKEN_STRING);
    CHECK(static_cast<ValueToken<TokenType::TOKEN_STRING> *>(tokens.at(0).get())
              ->getValue() == "");
    CHECK(!er.hasError());
  }

  SUBCASE("string ok") {
    Lexer l("\"ok\"", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.at(0)->getType() == TokenType::TOKEN_STRING);
    CHECK(static_cast<ValueToken<TokenType::TOKEN_STRING> *>(tokens.at(0).get())
              ->getValue() == "ok");
    CHECK(!er.hasError());
  }

  SUBCASE("multi line string") {
    Lexer l("\"hello\n\nworld\"", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens.at(0)->getType() == TokenType::TOKEN_STRING);
    CHECK(static_cast<ValueToken<TokenType::TOKEN_STRING> *>(tokens.at(0).get())
              ->getValue() == "hello\n\nworld");
    CHECK(!er.hasError());
  }

  SUBCASE("unterminated string") {
    Lexer l("\"test\n", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();

    CHECK(tokens.empty());
    REQUIRE(er.hasError());
    er.logErrors();
    CHECK(out_buf.str().starts_with("Lexer\nUnterminated string"));
  }

  SUBCASE("numbers literal") {
    Lexer l("123\n0\n1.23\n0.00123", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();

    REQUIRE(tokens.size() == 4);

    REQUIRE(tokens.at(0)->getType() == TokenType::TOKEN_NUMBER);
    CHECK(static_cast<ValueToken<TokenType::TOKEN_NUMBER> *>(tokens.at(0).get())
              ->getValue() == 123.);

    REQUIRE(tokens.at(1)->getType() == TokenType::TOKEN_NUMBER);
    CHECK(static_cast<ValueToken<TokenType::TOKEN_NUMBER> *>(tokens.at(1).get())
              ->getValue() == 0.);

    REQUIRE(tokens.at(2)->getType() == TokenType::TOKEN_NUMBER);
    CHECK(static_cast<ValueToken<TokenType::TOKEN_NUMBER> *>(tokens.at(2).get())
              ->getValue() == 1.23);

    REQUIRE(tokens.at(3)->getType() == TokenType::TOKEN_NUMBER);
    CHECK(static_cast<ValueToken<TokenType::TOKEN_NUMBER> *>(tokens.at(3).get())
              ->getValue() == 0.00123);

    CHECK(!er.hasError());
  }

  SUBCASE("number overlfow") {
    Lexer l(
        "199769313486231570814527423731704356798070567525844996598917476803"
        "157260780028538760589558632766878171540458953514382464234321326889"
        "464182768467546703537516986049910576551282076245490090389328944075"
        "868508455133942304583236903222948165808559332123348274797826204144"
        "723168738177180919299881250404026184124858368.000000",
        er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();

    CHECK(tokens.empty());
    REQUIRE(er.hasError());
    er.logErrors();
    CHECK(out_buf.str().starts_with("Lexer\nOut of range number conversion"));
  }

  SUBCASE("valid identifiers") {
    Lexer l("myVar\n_MyVar\nPrinter1", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();

    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens.at(0)->getType() == TokenType::TOKEN_IDENTIFIER);
    CHECK(static_cast<ValueToken<TokenType::TOKEN_IDENTIFIER> *>(
              tokens.at(0).get())
              ->getValue() == "myVar");

    REQUIRE(tokens.at(1)->getType() == TokenType::TOKEN_IDENTIFIER);
    CHECK(static_cast<ValueToken<TokenType::TOKEN_IDENTIFIER> *>(
              tokens.at(1).get())
              ->getValue() == "_MyVar");

    REQUIRE(tokens.at(2)->getType() == TokenType::TOKEN_IDENTIFIER);
    CHECK(static_cast<ValueToken<TokenType::TOKEN_IDENTIFIER> *>(
              tokens.at(2).get())
              ->getValue() == "Printer1");

    CHECK(!er.hasError());
  }

  SUBCASE("identifier can only start with alpha or underscore") {
    Lexer l("1test .test", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();

    CHECK(tokens.size() == 4);
    REQUIRE(tokens.at(0)->getType() == TokenType::TOKEN_NUMBER);
    REQUIRE(tokens.at(1)->getType() == TokenType::TOKEN_IDENTIFIER);
    REQUIRE(tokens.at(2)->getType() == TokenType::TOKEN_DOT);
    REQUIRE(tokens.at(3)->getType() == TokenType::TOKEN_IDENTIFIER);
    CHECK(!er.hasError());
  }

  SUBCASE("identifier can only contain alphanum and underscore") {
    Lexer l("my-test my_test my1test", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();

    CHECK(tokens.size() == 5);
    REQUIRE(tokens.at(0)->getType() == TokenType::TOKEN_IDENTIFIER);
    REQUIRE(tokens.at(1)->getType() == TokenType::TOKEN_MINUS);
    REQUIRE(tokens.at(2)->getType() == TokenType::TOKEN_IDENTIFIER);
    REQUIRE(tokens.at(3)->getType() == TokenType::TOKEN_IDENTIFIER);
    REQUIRE(tokens.at(4)->getType() == TokenType::TOKEN_IDENTIFIER);
    CHECK(!er.hasError());
  }

  SUBCASE("specific identifiers") {
    Lexer l(
        "and class else false fun for if nil or print return super this "
        "true var while",
        er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();

    REQUIRE(tokens.size() == 16);

    REQUIRE(tokens.at(0)->getType() == TokenType::TOKEN_AND);
    REQUIRE(tokens.at(1)->getType() == TokenType::TOKEN_CLASS);
    REQUIRE(tokens.at(2)->getType() == TokenType::TOKEN_ELSE);
    REQUIRE(tokens.at(3)->getType() == TokenType::TOKEN_FALSE);
    REQUIRE(tokens.at(4)->getType() == TokenType::TOKEN_FUN);
    REQUIRE(tokens.at(5)->getType() == TokenType::TOKEN_FOR);
    REQUIRE(tokens.at(6)->getType() == TokenType::TOKEN_IF);
    REQUIRE(tokens.at(7)->getType() == TokenType::TOKEN_NIL);
    REQUIRE(tokens.at(8)->getType() == TokenType::TOKEN_OR);
    REQUIRE(tokens.at(9)->getType() == TokenType::TOKEN_PRINT);
    REQUIRE(tokens.at(10)->getType() == TokenType::TOKEN_RETURN);
    REQUIRE(tokens.at(11)->getType() == TokenType::TOKEN_SUPER);
    REQUIRE(tokens.at(12)->getType() == TokenType::TOKEN_THIS);
    REQUIRE(tokens.at(13)->getType() == TokenType::TOKEN_TRUE);
    REQUIRE(tokens.at(14)->getType() == TokenType::TOKEN_VAR);
    REQUIRE(tokens.at(15)->getType() == TokenType::TOKEN_WHILE);

    CHECK(!er.hasError());
  }

  SUBCASE("single line comment only") {
    Lexer l("// comment", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();

    CHECK(tokens.empty());
    CHECK(!er.hasError());
  }

  SUBCASE("comment end of line") {
    Lexer l("print \"ok\"; // comment", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();

    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens.at(0)->getType() == TokenType::TOKEN_PRINT);
    REQUIRE(tokens.at(1)->getType() == TokenType::TOKEN_STRING);
    REQUIRE(tokens.at(2)->getType() == TokenType::TOKEN_SEMICOLON);

    CHECK(!er.hasError());
  }

  SUBCASE("comment first line") {
    Lexer l("// comment\n print \"ok\";", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();

    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens.at(0)->getType() == TokenType::TOKEN_PRINT);
    REQUIRE(tokens.at(1)->getType() == TokenType::TOKEN_STRING);
    REQUIRE(tokens.at(2)->getType() == TokenType::TOKEN_SEMICOLON);

    CHECK(!er.hasError());
  }

  SUBCASE("multi line comment only") {
    Lexer l("/* comment \n\n newline */", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();

    CHECK(tokens.empty());
    CHECK(!er.hasError());
  }

  SUBCASE("multi line comment midle") {
    Lexer l("print \"one\";\n /* comment \n\n newline */\nprint \"two\";", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();

    REQUIRE(tokens.size() == 6);

    REQUIRE(tokens.at(0)->getType() == TokenType::TOKEN_PRINT);
    REQUIRE(tokens.at(1)->getType() == TokenType::TOKEN_STRING);
    REQUIRE(tokens.at(2)->getType() == TokenType::TOKEN_SEMICOLON);

    REQUIRE(tokens.at(3)->getType() == TokenType::TOKEN_PRINT);
    REQUIRE(tokens.at(4)->getType() == TokenType::TOKEN_STRING);
    REQUIRE(tokens.at(5)->getType() == TokenType::TOKEN_SEMICOLON);

    CHECK(!er.hasError());
  }

  SUBCASE("unexpected character") {
    Lexer l("@", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();

    CHECK(tokens.empty());
    REQUIRE(er.hasError());
    er.logErrors();
    CHECK(out_buf.str().starts_with("Lexer\nUnexpected character"));
  }

  SUBCASE("unexpected character in string") {
    Lexer l("\"@\"", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();

    REQUIRE(tokens.size() == 1);

    REQUIRE(tokens.at(0)->getType() == TokenType::TOKEN_STRING);
    CHECK(static_cast<ValueToken<TokenType::TOKEN_STRING> *>(tokens.at(0).get())
              ->getValue() == "@");

    CHECK(!er.hasError());
  }

  SUBCASE("unexpected character in comment") {
    Lexer l("// @", er);

    auto tokens_opt = l.scanTokens();
    REQUIRE(tokens_opt.has_value());
    auto &&tokens = tokens_opt.value().get();

    CHECK(tokens.empty());
    CHECK(!er.hasError());
  }
}

// NOLINTEND(cppcoreguidelines*, readability*)