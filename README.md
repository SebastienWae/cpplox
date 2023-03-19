# cpplox
`cpplox` is an interpreter for the Lox language written in C++.

## Roadmap
- [x] Lexer
- [x] Expressions
- [x] Interpreter
- [x] Statements
- [x] Environment
- [x] Scopes
- [ ] Control flow
- [ ] Functions
- [ ] Class

## Installing
### Building from source 
#### Dependencies:
- GCC 12+
- CMake 3.22+
#### Steps:
- clone repository
  ```
  git clone --recurse-submodules https://github.com/SebastienWae/cpplox
  ```
- build project
  ```
  cd ccplox
  ./externals/vcpkg/bootstrap-vcpkg.sh
  cmake --preset config-release
  cmake --build --preset build-release
  ```

## Usage
### REPL
```
$ cpplox
cpplox: Lox interpreter - v0.0.1
To exit, press Ctrl+d or type "exit"
>> var a = 1;
>> print a + 2;
3
```
### File
```
cpplox </path/to/file.lox>
```

## Lox language
Lox is a language with a C-like syntax that was created by [Robert Nystrom](https://journal.stuffwithstuff.com/) for his book [Crafting Interpreters](https://craftinginterpreters.com/).

A detailed introduction can be found in [chapther 3](https://craftinginterpreters.com/the-lox-language.html) and the grammar in the [Appendix I](https://craftinginterpreters.com/appendix-i.html).

`cpplox` also implement [block comments](https://en.wikipedia.org/wiki/Comment_(computer_programming)#Block_comment), as well as the [comma](https://en.wikipedia.org/wiki/Comma_operator) and [ternary conditional](https://en.wikipedia.org/wiki/Ternary_conditional_operator) operators.

### Grammar
#### Lexical
```
NUMBER         → DIGIT+ ( "." DIGIT+ )? ;
STRING         → "\"" <any char except "\"">* "\"" ;
IDENTIFIER     → ALPHA ( ALPHA | DIGIT )* ;
ALPHA          → "a" ... "z" | "A" ... "Z" | "_" ;
DIGIT          → "0" ... "9" ;
```

#### Syntax
```
program        → declaration* EOF ;

declaration    → varDecl
               | statement ;

varDecl        → "var" IDENTIFIER ( "=" assign )? ( "," IDENTIFIER ( "=" assign )? )* ";"


statement      → exprStmt
               | printStmt
               | block ;

block          → "{" declaration* "}" ;
exprStmt       → expression ";" ;
printStmt      → "print" expression ";" ;
```
```
NUMBER         → DIGIT+ ( "." DIGIT+ )? ;
STRING         → "\"" <any char except "\"">* "\"" ;
IDENTIFIER     → ALPHA ( ALPHA | DIGIT )* ;
ALPHA          → "a" ... "z" | "A" ... "Z" | "_" ;
DIGIT          → "0" ... "9" ;

expression     → assignment ( "," expression )* ;
assignment     → IDENTIFIER "=" assignment
               | equality "?" assignment ":" assignment
               | equality ;
equality       → comparison ( ( "!=" | "==" ) comparison )* ;
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term           → factor ( ( "-" | "+" ) factor )* ;
factor         → unary ( ( "/" | "*" ) unary )* ;
unary          → ( "!" | "-" ) unary
               | primary ;
primary        → NUMBER | STRING | "true" | "false" | "nil"
               | "(" expression ")"
               | IDENTIFIER ;
```

### Examples
More examples are available [here](https://github.com/munificent/craftinginterpreters/tree/master/test).

```
var imAVariable = "here is my value";
```

```
print "Hello, world!";
```

```
var a = 1;
while (a < 10) {
  print a;
  a = a + 1;
}
```

```
fun returnFunction() {
  var outside = "outside";

  fun inner() {
    print outside;
  }

  return inner;
}
```

```
class Breakfast {
  cook() {
    print "Eggs a-fryin'!";
  }

  serve(who) {
    print "Enjoy your breakfast, " + who + ".";
  }
}
```

```
var a = 1, b = 2, c = 3;
var x = (a, b, c);
print x; 
```

```
fun fib(n) {
  return n < 2 ? n : fib(n - 1) + fib(n - 2);
}
```

## Running the tests
```sh
cmake --preset config-debug
cmake --build --preset test-debug
ctest --output-on-failure --preset test-debug
```

## License
[GNU General Public License v3.0](/LICENSE)