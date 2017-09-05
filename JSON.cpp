#include "PreCompile.h"

#if 0
Implement state machine parsing from http://json.org/.
When reading string literals, replace U+2028 and U+2029 with \u2028 and \u2029. (http://timelessrepo.com/json-isnt-a-javascript-subset)
Control characters for string literals are the 65 control codes from: https://en.wikipedia.org/wiki/Control_character
Whitespace between tokens is defined as the 25 characters: https://en.wikipedia.org/wiki/Whitespace_character 
Parsing function accepts a parameter on whether to allow comments in the JSON.
Very optionally - Allow output to be minified: http://crockford.com/javascript/jsmin  (Likely do this only for canonical)
Optional Flag to verify (or maybe require) whether input was Canonical: http://wiki.laptop.org/go/Canonical_JSON
Output is either automatically indented or canonical.
If canonical, do not worry about Normalization form - that will be enforced, if necessary, by the client.
If canonical, return whether a double was truncated to an int

#endif