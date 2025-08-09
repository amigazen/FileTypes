#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Basic Python 2 syntax highlighting test file
Tests core features that ARE IMPLEMENTED in our plugin
"""

# IMPLEMENTED: import keyword highlighting
import os
import sys

# IMPLEMENTED: Basic variable assignments with strings and numbers
name = "Python"  # IMPLEMENTED: string highlighting
version = 3.9    # NOT IMPLEMENTED: number highlighting (simplified)
is_awesome = True  # IMPLEMENTED: keyword highlighting for True
numbers = [1, 2, 3, 4, 5]  # NOT IMPLEMENTED: bracket/number highlighting

# IMPLEMENTED: Function definition with 'def' keyword
def greet(name):
    """Return a greeting message"""  # IMPLEMENTED: docstring highlighting
    return "Hello, %s!" % name  # IMPLEMENTED: keyword 'return', string highlighting

# IMPLEMENTED: Class definition with 'class' keyword  
class Calculator:
    """A simple calculator class"""  # IMPLEMENTED: docstring highlighting
    
    # IMPLEMENTED: Function definitions and keywords
    def __init__(self):
        self.result = 0
    
    def add(self, x, y):
        """Add two numbers"""  # IMPLEMENTED: docstring highlighting
        return x + y  # IMPLEMENTED: 'return' keyword
    
    def multiply(self, x, y):
        """Multiply two numbers"""  # IMPLEMENTED: docstring highlighting
        return x * y  # IMPLEMENTED: 'return' keyword

# IMPLEMENTED: Control structures with keywords
if __name__ == "__main__":  # IMPLEMENTED: 'if' keyword, string highlighting
    # IMPLEMENTED: Comment highlighting starts here
    calc = Calculator()
    
    # IMPLEMENTED: Function calls and keywords
    result = calc.add(10, 20)
    print "10 + 20 = %d" % result  # IMPLEMENTED: keyword 'print', string
    
    # NOT IMPLEMENTED: Complex expressions like list comprehensions
    squares = [x**2 for x in xrange(10)]  # IMPLEMENTED: keywords 'for', 'in'
    
    # NOT IMPLEMENTED: Complex dictionary comprehensions
    word_counts = dict((word, len(word)) for word in ["hello", "world", "python"])
    
    # IMPLEMENTED: Exception handling keywords
    try:  # IMPLEMENTED: 'try' keyword
        value = int("abc")  # IMPLEMENTED: builtin function 'int'
    except ValueError, e:  # IMPLEMENTED: 'except' keyword
        print "Error: %s" % e  # IMPLEMENTED: 'print' keyword, string
    finally:  # IMPLEMENTED: 'finally' keyword
        print "Cleanup complete"  # IMPLEMENTED: string highlighting

# IMPLEMENTED: Function definitions and keywords
def timer(func):
    """Decorator to time function execution"""  # IMPLEMENTED: docstring
    def wrapper(*args, **kwargs):  # IMPLEMENTED: 'def' keyword
        import time  # IMPLEMENTED: 'import' keyword
        start = time.time()
        result = func(*args, **kwargs)
        end = time.time()
        print "%s took %f seconds" % (func.__name__, end - start)
        return result  # IMPLEMENTED: 'return' keyword
    return wrapper  # IMPLEMENTED: 'return' keyword

# IMPLEMENTED: Decorator highlighting (simplified)
@timer  # IMPLEMENTED: decorator highlighting
def slow_function():
    """A function that takes time to execute"""  # IMPLEMENTED: docstring
    import time  # IMPLEMENTED: 'import' keyword
    time.sleep(1)
    return "Done!"  # IMPLEMENTED: 'return' keyword, string

# IMPLEMENTED: Function definition and control flow
def process_data(data):
    """Process a list of dictionaries"""  # IMPLEMENTED: docstring
    if not data:  # IMPLEMENTED: 'if', 'not' keywords
        return None  # IMPLEMENTED: 'return', 'None' keywords
    
    result = ""  # IMPLEMENTED: string highlighting
    for item in data:  # IMPLEMENTED: 'for', 'in' keywords
        if isinstance(item, dict):  # IMPLEMENTED: 'if' keyword, builtins
            result += str(item.get("value", ""))  # IMPLEMENTED: strings
    
    return result  # IMPLEMENTED: 'return' keyword 