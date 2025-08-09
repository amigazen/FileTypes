#!/usr/bin/env python
"""
Simple Python example for testing syntax highlighting
FOCUSES ON FEATURES WE HAVE IMPLEMENTED
"""

# IMPLEMENTED: Basic imports with keyword highlighting
import os  # IMPLEMENTED: 'import' keyword
import sys  # IMPLEMENTED: 'import' keyword

# IMPLEMENTED: Simple variable assignments
name = "World"  # IMPLEMENTED: string highlighting
age = 25  # NOT IMPLEMENTED: number highlighting (simplified out)
is_student = True  # IMPLEMENTED: 'True' keyword highlighting

# IMPLEMENTED: Simple function definition
def greet(person_name):  # IMPLEMENTED: 'def' keyword
    """Simple greeting function"""  # IMPLEMENTED: docstring highlighting
    return "Hello, " + person_name + "!"  # IMPLEMENTED: 'return' keyword, strings

# IMPLEMENTED: Simple class definition
class Person:  # IMPLEMENTED: 'class' keyword
    """Simple person class"""  # IMPLEMENTED: docstring highlighting
    
    def __init__(self, name, age):  # IMPLEMENTED: 'def' keyword
        self.name = name
        self.age = age
    
    def introduce(self):  # IMPLEMENTED: 'def' keyword
        return "My name is " + self.name  # IMPLEMENTED: 'return', strings

# IMPLEMENTED: Main execution block
if __name__ == "__main__":  # IMPLEMENTED: 'if' keyword, string highlighting
    # IMPLEMENTED: Comment highlighting
    person = Person(name, age)
    
    # IMPLEMENTED: Function calls and print statements
    print greet(person.name)  # IMPLEMENTED: 'print' builtin, function calls
    print person.introduce()  # IMPLEMENTED: 'print' builtin
    
    # IMPLEMENTED: Simple list (though brackets not highlighted)
    numbers = [1, 2, 3, 4, 5]  # NOT IMPLEMENTED: bracket/number highlighting
    print "Numbers: " + str(numbers)  # IMPLEMENTED: 'print', strings, 'str' builtin
    
    # IMPLEMENTED: Simple loop with keywords
    for num in numbers:  # IMPLEMENTED: 'for', 'in' keywords
        print "Number: " + str(num)  # IMPLEMENTED: 'print', strings
    
    # IMPLEMENTED: Simple condition with keywords
    if age >= 18:  # IMPLEMENTED: 'if' keyword
        print "You are an adult"  # IMPLEMENTED: 'print', string
    else:  # IMPLEMENTED: 'else' keyword
        print "You are a minor"  # IMPLEMENTED: 'print', string

# NOTE: F-strings (f"text") are NOT IMPLEMENTED in our simplified plugin
# We changed them to string concatenation which IS IMPLEMENTED 