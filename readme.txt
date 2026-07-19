                              ________  _____ 
                             / ____/  |/  / / 
                            / /   / /|_/ / /  
                           / /___/ /  / / /___
                           \____/_/  /_/_____/
                   
                         CB++ Markdown Language

A human-readable markup language with an extremely simple syntax.
Current standart version: 100

We have 7 supported types:
    - Numbers, which can be either integers or floats, both 32-bit
    - Strings with UTF-8 support
    - Raw binary data
    - Lists
    - Tables

Mostly all language syntax looks like [name] [value]. You don`t have to embed 
all your entries inside a root object - the parser will do it for you.

Name can consist of any letters, numbers and '_'
It can not start with a number, however.

Here are some examples:

1) Defining a simple entry

    > MyInteger 52
    > MyFloat 3.1495
    > MyFloatExp 5e-2
    > MyString "Hello world!"

    If a duplicated entry name occurs, and if the duplicate one has the same type,
    then the old one is overriden. Arrays are concatenated in this case.

    > MySickNCoolValue 0
    > ...
    > MySickNCoolValue 52   # <- there will be one integer of value 52

2) Number type suffixes

    You can type 'f' or 'i' after any number to force its type 
    to float or integer accordingly.

3) Boolean values

    > MyFlag true

    There are 'true' and 'false' keywords, which will be parsed as
    an integer with value either 1 or 0.

4) Defining a table

    > MyTable {
    >     [any sub-entries here]
    > }

    The nesting depth is limited by the compile-time constant.

5) Defining a list

    > MyList [
    >     1
    >     2
    >     "string!"
    >     {
    >         Value "i`m inside nameless object inside a list!"
    >     }
    > ]

    When describing list contents you only write values w/o their names.
    These values can later be accessed by their indices.

6) Comments

    Any sequence of characters starting with '#' and ending with either '#' or '\n'
    is considered a comment. Yes, you can embed '#...#'-like comments everywhere you want.

7) String functionalities

    7.1) Escape characters
        Strings support the following list of escape characters:

            \n  - newline
            \t  - tabulation
            \"  - double quote
            \\  - backslash

    7.2) Text file reference
        > FileReference @"path/to/file"

        Any string prefixed by '@' is considered as a 'file reference'.
        The parser will try opening said file, reading it`s contents as text and then pasting
        it as this variable`s value. The path is relative to the current assets root path.

    7.3) Binary file reference
        > BinaryReference &"path/to/file"

        Works as a text reference, but interpretes file`s content as raw binary data.

    All file references have a size limit, which is 64 MB by default

    Also note that UTF-8 is supported only inside strings, and using unicode anywhere 
    else may lead to undefined behaviour.

8) Keywords

    8.1) version [int]

        The 'version' keyword allows you to specify this file`s CML standart version. The parser will discard
        sources it`ll find incompatible (too old or newer than the current version)

    8.2) include "path/to/file.cml"

        Allows you to include the specified file in the place of the keyword. The included file is
        copy-pasted in the place of the keyword.

    8.3) concat "path.to.parent_table"

        Copies parent`s entries in the current table (only tables are supported as child and parent objects).
        Parent base values can later be overriden by redefining them in the child object after the keyword invocation.
