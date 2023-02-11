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
cpplox 0.0.1
Type "help" for more informations
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