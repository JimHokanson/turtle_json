# Design Choices

# Parsed Value Representation

* '[]' translates into an empty numeric array (TODO)
* '[[1,2],[3,4],[5,6]]' - translates into a numeric array of size [2,3] (in other words, the default behavior is to keep values ordered in memory, rather than needing to shuffle them