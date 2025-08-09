#!/usr/bin/env python3
"""
Python edge cases and unusual syntax patterns
Tests various edge cases for syntax highlighting
MIXED: Some features implemented, others not
"""

# SPECIAL STRING LITERALS - NOT FULLY IMPLEMENTED
raw_string = r"C:\path\to\file"  # IMPLEMENTED: basic string, NOT: raw string prefix
bytes_string = b"binary data"  # IMPLEMENTED: basic string, NOT: bytes prefix  
unicode_string = u"unicode string"  # IMPLEMENTED: basic string, NOT: unicode prefix
# f_string = f"formatted {raw_string}"  # NOT IMPLEMENTED: f-strings removed

# IMPLEMENTED: Triple quoted strings (docstrings)
docstring = """
This is a docstring with "quotes" and 'apostrophes'
It spans multiple lines
"""  # IMPLEMENTED: triple-quoted docstring highlighting

raw_docstring = r"""
Raw docstring with \n literal
"""  # IMPLEMENTED: triple-quoted, NOT: raw string prefix

# NOT IMPLEMENTED: Numbers with different bases (simplified out)
# hex_num = 0x1A2B3C      # Would need complex number parsing
# octal_num = 0o1234567   # Would need complex number parsing  
# binary_num = 0b10101010  # Would need complex number parsing
# float_num = 3.14159e-10  # Would need complex number parsing
# complex_num = 3.14j      # Would need complex number parsing

# IMPLEMENTED: Variable names and string assignments
_private_var = "private"  # IMPLEMENTED: string highlighting
__very_private = "very private"  # IMPLEMENTED: string highlighting
UPPER_CASE = "constant"  # IMPLEMENTED: string highlighting
camelCase = "camel case"  # IMPLEMENTED: string highlighting
snake_case = "snake case"  # IMPLEMENTED: string highlighting

# IMPLEMENTED: Lambda functions (keywords highlighted)
simple_lambda = lambda x: x * 2  # IMPLEMENTED: 'lambda' keyword
complex_lambda = lambda x, y, z: x + y if z else x - y  # IMPLEMENTED: 'lambda', 'if', 'else' keywords

# PARTIALLY IMPLEMENTED: List comprehensions
squares = [x for x in range(10)]  # IMPLEMENTED: 'for', 'in' keywords, 'range' builtin
even_squares = [x for x in range(10) if x % 2 == 0]  # IMPLEMENTED: keywords, NOT: complex expressions
# nested_comp = [[i+j for j in range(3)] for i in range(3)]  # TOO COMPLEX: removed

# PARTIALLY IMPLEMENTED: Dictionary comprehensions  
# word_lengths = {word: len(word) for word in ["hello", "world"]}  # TOO COMPLEX: removed
# conditional_dict = {k: v for k, v in enumerate(range(10)) if v % 2 == 0}  # TOO COMPLEX: removed

# PARTIALLY IMPLEMENTED: Set comprehensions
# unique_squares = {x**2 for x in range(10)}  # TOO COMPLEX: removed  
# conditional_set = {x for x in range(20) if x % 3 == 0}  # TOO COMPLEX: removed

# PARTIALLY IMPLEMENTED: Generator expressions  
# gen_expr = (x**2 for x in range(10))  # TOO COMPLEX: removed
# conditional_gen = (x for x in range(100) if x % 7 == 0)  # TOO COMPLEX: removed

# IMPLEMENTED: Function definitions (basic syntax)
def function_with_defaults(a, b, c):  # IMPLEMENTED: 'def' keyword, simplified params
    """Function with various parameter types"""  # IMPLEMENTED: docstring
    return a + b + c  # IMPLEMENTED: 'return' keyword

# NOT IMPLEMENTED: Type hints (too complex for our plugin)
# def function_with_type_hints(x: int, y: str = "default") -> bool:
#     """Function with type hints"""
#     return len(y) > x

# IMPLEMENTED: Class definition with basic methods
class UnusualClass:  # IMPLEMENTED: 'class' keyword
    """Class with unusual method names and patterns"""  # IMPLEMENTED: docstring
    
    def __init__(self):  # IMPLEMENTED: 'def' keyword
        self._value = 0
    
    def __str__(self):  # IMPLEMENTED: 'def' keyword
        return "UnusualClass"  # IMPLEMENTED: 'return', string (removed f-string)
    
    def __repr__(self):  # IMPLEMENTED: 'def' keyword
        return self.__str__()  # IMPLEMENTED: 'return' keyword
    
    def __len__(self):  # IMPLEMENTED: 'def' keyword
        return self._value  # IMPLEMENTED: 'return' keyword
    
    def __getitem__(self, key):  # IMPLEMENTED: 'def' keyword
        return key  # IMPLEMENTED: 'return' keyword (simplified)
    
    def __setitem__(self, key, value):  # IMPLEMENTED: 'def' keyword
        self._value = key + value
    
    def get_value(self):  # IMPLEMENTED: 'def' keyword (simplified method)
        return self._value  # IMPLEMENTED: 'return' keyword
    
    # NOT IMPLEMENTED: Property decorators (too complex)
    # @property
    # def value(self):
    #     return self._value

# IMPLEMENTED: Basic decorator definition
def simple_decorator(func):  # IMPLEMENTED: 'def' keyword
    def wrapper():  # IMPLEMENTED: 'def' keyword
        print "Before function"  # IMPLEMENTED: 'print', string
        result = func()
        print "After function"  # IMPLEMENTED: 'print', string
        return result  # IMPLEMENTED: 'return' keyword
    return wrapper  # IMPLEMENTED: 'return' keyword

# IMPLEMENTED: Decorator usage (simplified)
@simple_decorator  # IMPLEMENTED: decorator highlighting
def greet():  # IMPLEMENTED: 'def' keyword
    print "Hello, World!"  # IMPLEMENTED: 'print', string

# IMPLEMENTED: Exception handling keywords
try:  # IMPLEMENTED: 'try' keyword
    result = 1 / 0
except ZeroDivisionError:  # IMPLEMENTED: 'except' keyword
    print "Division by zero"  # IMPLEMENTED: 'print', string
except ValueError:  # IMPLEMENTED: 'except' keyword
    print "Value error"  # IMPLEMENTED: 'print', string
except Exception:  # IMPLEMENTED: 'except' keyword
    print "General exception"  # IMPLEMENTED: 'print', string
else:  # IMPLEMENTED: 'else' keyword
    print "No exception occurred"  # IMPLEMENTED: 'print', string
finally:  # IMPLEMENTED: 'finally' keyword
    print "Cleanup always happens"  # IMPLEMENTED: 'print', string

# IMPLEMENTED: Basic control statements
temp_var = "temporary"  # IMPLEMENTED: string
del temp_var  # IMPLEMENTED: 'del' keyword

# IMPLEMENTED: Global variables
global_var = "global"  # IMPLEMENTED: string

def function_with_global():  # IMPLEMENTED: 'def' keyword
    global global_var  # IMPLEMENTED: 'global' keyword
    global_var = "modified"  # IMPLEMENTED: string

# IMPLEMENTED: Boolean operations and keywords
bool_and = True and False  # IMPLEMENTED: 'True', 'and', 'False' keywords
bool_or = True or False  # IMPLEMENTED: 'True', 'or', 'False' keywords
bool_not = not True  # IMPLEMENTED: 'not', 'True' keywords

# IMPLEMENTED: Membership testing keywords
test_list = [1, 2, 3, 4, 5]
in_list = 5 in test_list  # IMPLEMENTED: 'in' keyword
not_in_list = 6 not in test_list  # IMPLEMENTED: 'not', 'in' keywords

# IMPLEMENTED: Identity testing keywords
is_none = None is None  # IMPLEMENTED: 'None', 'is' keywords
is_not_none = [] is not None  # IMPLEMENTED: 'is', 'not', 'None' keywords

# IMPLEMENTED: Conditional expressions with keywords
result = "positive" if True else "negative"  # IMPLEMENTED: 'if', 'else' keywords, strings

# NOT IMPLEMENTED: Complex features removed for simplicity
# - Context managers (too complex)
# - Async/await (too complex)
# - Walrus operator (too complex)
# - Dictionary/list unpacking (too complex)
# - Generator functions with yield (could be implemented)
# - Complex string formatting (f-strings, .format())
# - Raw strings with prefixes (could be implemented)
# - Bytes and complex numbers (too complex)
# - Bitwise operations (could be implemented)
# - Comparison chaining (too complex) 