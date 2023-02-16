# cpplox
`cpplox` is an interpreter for the Lox language written in C++.

## Roadmap
TODO...

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
  TODO...
  ```
- install binary 
  ```
  TODO...
  ```

## Usage
### REPL
```
$ cpplox
cpplox: Lox interpreter - v0.0.1
To exit, press Ctrl+d or type "exit"
> var a = 1
> print a + 2
3
```
### File
```
cpplox </path/to/file.lox>
```

## Lox language
Lox is a language with a C-like syntax that was created by [Robert Nystrom](https://journal.stuffwithstuff.com/) for his book [Crafting Interpreters](https://craftinginterpreters.com/).

A more detailed introduction can be found in [this chapter](https://craftinginterpreters.com/the-lox-language.html).

### Lexical grammar
```ABNF
NUMBER         → DIGIT+ ( "." DIGIT+ )?
STRING         → "\"" <any char except "\"">* "\""
IDENTIFIER     → ALPHA ( ALPHA | DIGIT )*
ALPHA          → "a" ... "z" | "A" ... "Z" | "_"
DIGIT          → "0" ... "9"
```

### Syntax grammar
```ABNF
program        → declaration* EOF


; declarations
declaration    → classDecl
               | funDecl
               | varDecl
               | statement

classDecl      → "class" IDENTIFIER ( "<" IDENTIFIER )?
                 "{" function* "}"
funDecl        → "fun" function
varDecl        → "var" IDENTIFIER ( "=" expression )? ";"


; statements
statement      → exprStmt
               | forStmt
               | ifStmt
               | printStmt
               | returnStmt
               | whileStmt
               | block

exprStmt       → expression ";"
forStmt        → "for" "(" ( varDecl | exprStmt | ";" )
                           expression? ";"
                           expression? ")" statement
ifStmt         → "if" "(" expression ")" statement
                 ( "else" statement )?
printStmt      → "print" expression ";"
returnStmt     → "return" expression? ";"
whileStmt      → "while" "(" expression ")" statement
block          → "{" declaration* "}"


; expressions
expression     → assignment

assignment     → ( call "." )? IDENTIFIER "=" assignment
               | logic_or

logic_or       → logic_and ( "or" logic_and )*
logic_and      → equality ( "and" equality )*
equality       → comparison ( ( "!=" | "==" ) comparison )*
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )*
term           → factor ( ( "-" | "+" ) factor )*
factor         → unary ( ( "/" | "*" ) unary )*

unary          → ( "!" | "-" ) unary | call
call           → primary ( "(" arguments? ")" | "." IDENTIFIER )*
primary        → "true" | "false" | "nil" | "this"
               | NUMBER | STRING | IDENTIFIER | "(" expression ")"
               | "super" "." IDENTIFIER


; utility
function       → IDENTIFIER "(" parameters? ")" block
parameters     → IDENTIFIER ( "," IDENTIFIER )*
arguments      → expression ( "," expression )*
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

## Running the tests
TODO...

## License
[GNU General Public License v3.0](/LICENSE)