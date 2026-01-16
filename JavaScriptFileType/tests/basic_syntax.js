// Basic JavaScript syntax test file

// Variable declarations
var x = 10;
let y = 20;
const z = 30;

// Function declaration
function greet(name) {
    return "Hello, " + name + "!";
}

// Arrow function
const add = (a, b) => a + b;

// Object literal
const person = {
    name: "John",
    age: 30,
    city: "New York"
};

// Array
const numbers = [1, 2, 3, 4, 5];

// String literals
const singleQuote = 'This is a single-quoted string';
const doubleQuote = "This is a double-quoted string";
const templateLiteral = `This is a template literal with ${x} value`;

// Control structures
if (x > 5) {
    console.log("x is greater than 5");
} else {
    console.log("x is not greater than 5");
}

for (let i = 0; i < numbers.length; i++) {
    console.log(numbers[i]);
}

while (x > 0) {
    x--;
}

// Regular expression
const pattern = /test/gi;

// Comments
/* This is a multi-line comment
   that spans multiple lines */

// Keywords
try {
    throw new Error("Something went wrong");
} catch (error) {
    console.error(error);
} finally {
    console.log("Cleanup");
}


