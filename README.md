# Aesthetic

**WARNING! This programming language is in work in progress!**

This programming language is a research in possibilities of compilers and programming languages in general. It is just another modern language with interesting features. Especially, it will be approached to be reactive.

Aesthetic is planned to be:

- [ ] Interpreted
- [ ] Compiled
- [ ] Native
- [ ] Turing complete (Yes this language is on so early stage of developing so it even isn't turing complete)
- [ ] Event-oriented
- [ ] Reactive
- [ ] Self-hosted

## Examples

Simple loop analogue. This program prints numbers from 0 to 999 including both start and end.

``` Aesthetic
when #start {
    x ::= 0

    when x >= 1000 { ~!x }
    on x { print x }

    x := x + 1
}
```
