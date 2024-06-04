Project 01 | DCC205 - Data Structures | March 2022, Federal University of Minas Gerais

# Url Scheduler
## Overview
In this project for Data Structures class students were meant to implement a url scheduler for web crawling. It was to be written in `C/C++` and should feature data structures implemented from scratch, have performance metrics and analysis and be fully documented. Despite the seemingly simple data structure, it was quite the challenge to put it all together.

### A url-what?
A *web crawler* is an essential part of a *search engine* that visits URLs and downloads their content, which is subsequently processed, indexed, and used by the search engine to decide which results to show the users. 
As the Web is becoming increasingly dynamic, in addition to discovering new web pages a crawler needs to keep revisiting those already in the search engine’s index, in order to keep the index fresh by picking up the pages’ changed content. 
The problem of **crawl scheduling** consists in determining when to (re-)download which pages’ content so as to ensure that the search engine has complete and up-to-date view of the Web. (From [Microsoft](https://www.microsoft.com/en-us/research/project/web-crawl-scheduling/), [mirror](https://web.archive.org/web/20240604040921/https://www.microsoft.com/en-us/research/project/web-crawl-scheduling/)).

This project is meant to be a scheduler that will receive, sort and provide URLs for the use of a crawler (which will not be implemented).

#### How this scheduler will sort URLs:
URLs can be decomposed in a few parts:

`<protocol>`://`<subdomain>` `<host>` `<path>`?`<query>`#`<fragment>`

For example:

- `https`://`www`.`google.com` `/search`?`q=search+terms+here`#`<fragment>`
- `https`://`accounts`.`google.com` `/v3/signin/identifier`?`continue=gibberish`

The scheduler will sort new URLs by their `<host>` (in order of appearance), and addresses that belong to the same host should be sorted by the directory/path hierarchy, like so:
```
ufmg.br
-> ufmg.br/dcc -> ufmg.br/icex -> ufmg.br/dcc/courses/ -> ufmg.br/dcc/contact -> ufmg.br/dcc/courses/computer-science
website.com
-> website.com/level1 -> website.com/level1/level2 -> website.com/level1/level2/level3
```

Check out Section 2 and Appendix in the `Documentation.pdf` file for more info.
## Implementation objectives and specifications
- Implement from scratch a data structure to organize received URLs using a *depth-first* policy (prioritize hosts that were found first).
- Write code to recognize and decompose URL addresses based on their hosts and subdomain levels, as well as detect malformed ones. I used `regular expressions` here.
- Implent support for reading input data from files indicated by command-line arguments and flags (with [`getopt`](https://azrael.digipen.edu/~mmead/www/Courses/CS180/getopt.html.), [mirror](https://web.archive.org/web/20240604140838/https://azrael.digipen.edu/~mmead/www/Courses/CS180/getopt.html)).
- Generate multiple performance metrics for analysis, such as execution time, storage use, spacial locality and more. Time and space complexity were also considered.
- Write up a full documentation for the project.


## Important files and execution instructions
- `Documentation.pdf✨` is a complete report the problem descriptions, implementation choices and performance evaluations (in portuguese).
- `src/✨` and `include/✨` are where the scheduler code and classes can be found.
- `bin` is where the compiled program will be stored

The folders `tests/` and `performance/` are simply to store tests and performance results, you may or may not use them. `bin` and `obj` will be used in the compilation process and are there for convenience and organization. Be reminded that the `Makefile` file describes automatic instructions for compiling the project, avoid tinkering.

### How to run
- Requirements: `g++` and `make` from the GNU Compiler Collection (most linux distros already include them)
- From a terminal on this project root folder, compile the project using the `make` command. The resulting `a.out` program should be located in the `bin` folder.
- to run, you may use a command following the structure `program input_file.txt -flags`. Check out some examples:
```shell
bin/a.out input.txt -b -m
bin/a.out input.txt
bin/a.out input.txt -t -m
```
For instructions on what each flag does, what functionalities the scheduler provides, or how the input files should be formatted, check out the Appendix at page 17 in the `Documentation.pdf` file.

### TODO
- change output name from a.out to something more descriptive (and update documentation)
- test execution and metrics generation (and update documentation) 