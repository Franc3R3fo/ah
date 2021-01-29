# DOCUMENTATION FOR ***ah*** LANGUAGE
--------------------------------------
## Credits
----------
| Authors | Programming/Scripting/Markup Languages Used | Starting Date | Ending Date | Role |
| ------- | -------------------------------------------- | ------------- | ----------- | ---- |
| Franc3R3fo | C++, Markdown | 27/01/2021 | -- | Founder |

## About.the_name
-----------------
I named it after a common exclamation of surprise. ah. Why? Sometimes i think back at some features (also those hidden) and start to laugh.

## About.the_language
--------
This is a assembly-like interpreted programming language written in C++ during spare time.
Syntax of interpreter is `name file.ref file2.ref ...`
* `name` is the name of the interpreter. You can compile it and name it after your dog or you favorite city, doesn't matter.
* `file.ref` is the source file you pass to interpreter, you can pass multiple files just listing them.
* You can also pass flags and option by cmdline just adding the prefix `--`. See FLAGS for more details.

## Technical specifications
-------------------------
### LEGENDA
| Symbol | Meaning |
| ------ | ------- |
| ❌ | No |
| ✅ | Yes |

### LABEL NAMING
|    Pattern   | What it is? | Implemented? |
| ------------ | ----------- | ------------ |
| `.something` | it represent a ext_set (a special section, see SECTIONS) | ✅ |
|` _something` | it represent a label for a procedure or a function | ✅ |

### DELIMITERS
| Delimiter | What it is? | Implemented? |
| --------- | ----------- | ------------ |
| `"` | open/close a string | ✅ |
| `;` | open a comment, stronger than `"`, it overrides the string | ✅ |

### FLAGS
Flag is passed to the interpreter as `--option_name`.
Other parameters (except for those required by some option ) passed count as source files names.
| Flag | What it is? | Implemented? |
| --------- | ----------- | ------------ |
| `spm` | print a log of the sections content | ✅ |
| `rpm` | print a log of the registers content | ✅ |
| `rpm` | print a log of the proc queries | ✅ |
| `nsr` | program won't automatically add 9 system registers | ✅ |
| `nrr` | program won't automatically add return register | ✅ |
| `sasm` | print a log when the program starts, and when the program stops | ✅ |
| `nosr` | tells to the program how many system registers are needed (require number parameter after it) | ✅ |

### SECTIONS
| Section | What it Does? | Implemented? |
| ------- | ------------- | ------------ |
| `.data` | it contains declarations of registers | ✅ |
| `.link` | it contains link to other source files | ✅ |
| `.path` | the "procedure" that is executed on runtime | ✅ |

### DECLARING REGISTERS UNDER ***.data*** SECTION
In this section registers can be declared in two ways, and it will makes it ***permanent*** (cannot be deleted).
| Pattern| Meaning of the pattern | Implemented? |
| ------ | ---------------------- | ------------ |
| `[identifier] db [some_value]` | `db` means that you are declaring a register and you are going to initialize this register | ✅ |
| `[identifier] rtb` | `rtb` means that you are only declaring a register with empty value | ✅ |

### COMMANDS
| Command | Parameters | What it Does? | Implemented? | Constraints |
| ------- | ---------- | ------------- | ------------ | ----------- |
| mov | (a,b) | assign b to a | ✅ | |
| add | (a,b,c) | assign (b+c) to a | ✅ | |
| sub | (a,b,c) | assign (b-c) to a | ✅ | |
| mul | (a,b,c) | assign (b*c) to a | ✅ | |
| div | (a,b,c) | assign (b\c) to a | ✅ | |
| mod | (a,b,c) | assign (b%c) to a | ✅ | |
| xor | (a,b,c) | assign (b^c) to a | ✅ | |
|  or | (a,b,c) | assign (b\|c) to a | ✅ | |
| and | (a,b,c) | assign (b&c) to a | ✅ | |
| dec | (a,b) | declare a and assign b to a | ✅ | registers declared in this way will be deleted at the end of the procedure were they had been declared |
| rsv | (a) | declare a | ✅ | registers declared in this way will be deleted at the end of the procedure were they had been declared |
| run | (a...) | run procedure a with arguments (...) put in system registers (ra0...) | ✅ | arguments must be registers |
| ret | (a) | return a | ✅ | |
| pik | (a,b...) | run function b with arguments (...) put in system registers (ra0...) and push result to a | ✅ | arguments must be registers |
| del | (a) | delete register | ✅ | |
| rst | (a) | reset register, make it empty | ✅ | |
| drr | (a) | declare a register as invulnerable (cannot be deleted) | ✅ | |
| srr | (a) | set a register as invulnerable (cannot be deleted) | ✅ | |
| proc | () | call system processor to elaborate id and args in sys registers | ✅ | |
| link | (a) | link source file a | ✅ | only works in section .link |
| exit | () | cause program exit from the current procedure | ✅ | only works in procedures |
| quit | () | cause program to quit | ✅ | only works in procedures |

### SYSCALLs
| ID | What it Does | ARG0 | ARG1 | ARG2 | ARG3 | ARG4 | ARG5 | ARG6 | ARG7 | Implemented? | Notes |
| -- | ------------ | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ------------ | ----- |
| 0 | Standard Output, print arguments to screen | number of arguments | argument | ... | ... | ... | ... | ... | ✅ | |
| 1 | Standard Input | number of arguments | type (0 : numeric, 1 : string) | ... | ... | ... | ... | ... | ✅ | each sys register will be updated with the input desired |

### DEFAULT SYSTEM REGISTERS
| Name | Content | Category | Implemented? |
| ---- | ------- | -------- | ------------ |
| rid | A Number o a String | System Register | ✅ |
| ra0 | A Number o a String | System Register | ✅ |
| ra1 | A Number o a String | System Register | ✅ |
| ra2 | A Number o a String | System Register | ✅ |
| ra3 | A Number o a String | System Register | ✅ |
| ra4 | A Number o a String | System Register | ✅ |
| ra5 | A Number o a String | System Register | ✅ |
| ra6 | A Number o a String | System Register | ✅ |
| ra7 | A Number o a String | System Register | ✅ |
| ... | A Number o a String | System Register | ✅ |
| ra(n-1) | A Number o a String (n is the number of system registers loaded) | System Register | ✅ |
| rrr | A Number o a String | Return Register | ✅ |

## Some notes
-----------
* Sections with the same label will be merged by concatening the second section to the first one
* Some registers will be deleted exiting from scope if not set ***invulnerable***
* There are some hidden features, i challenge you to find them all.

## Example Code
-------------
```
; Section with initial data
section .data
    a db 60       ; declaration and initialization of a
    c db " hello "; declaration and initialization of c
    b rtb         ; declaration of b (b is empty)

section .path
    run _label0 ; tells to execute _label0

section _label0
    mov b,a;    ; b = a;
    add a,a,b;  ; a = a+b;
    mov rid,0   ; rid = 0 : standard output
    mov ra0,3   ; ra0 = 0 : number of registers to output (1-7)
    mov ra1,b   ; ra1 = b
    mov ra2,c   ; ra2 = c
    mov ra3,a   ; ra3 = a
    proc        ; call system processor
```

## Current Version
------------------
| Release | Type |
| ------- | ---- |
| 1.0.0 | ***Experimental*** |

## Version Log
--------------
| Release | Date | Phase | Name |
| ------- | ---- | ----- | ---- |
| 1.0.0 | 29/01/2021 | ***Beta Testing*** | |

## Updates
----------
(List currently is empty)
