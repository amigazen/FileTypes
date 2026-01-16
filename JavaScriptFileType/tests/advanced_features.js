// Advanced JavaScript features test file

// ES6 Classes
class Animal {
    constructor(name) {
        this.name = name;
    }
    
    speak() {
        return `${this.name} makes a sound`;
    }
}

class Dog extends Animal {
    constructor(name, breed) {
        super(name);
        this.breed = breed;
    }
    
    speak() {
        return `${this.name} barks`;
    }
}

// Promises and async/await
async function fetchData(url) {
    try {
        const response = await fetch(url);
        const data = await response.json();
        return data;
    } catch (error) {
        console.error("Error fetching data:", error);
        throw error;
    }
}

// Promise chain
fetchData("https://api.example.com/data")
    .then(data => {
        console.log("Data received:", data);
        return processData(data);
    })
    .catch(error => {
        console.error("Error:", error);
    });

// Array methods
const numbers = [1, 2, 3, 4, 5];
const doubled = numbers.map(n => n * 2);
const evens = numbers.filter(n => n % 2 === 0);
const sum = numbers.reduce((acc, n) => acc + n, 0);

// Destructuring
const { name, age } = person;
const [first, second, ...rest] = numbers;

// Spread operator
const newNumbers = [...numbers, 6, 7, 8];
const newPerson = { ...person, city: "Boston" };

// Template literals with expressions
const message = `Hello, ${name}! You are ${age} years old.`;

// Regular expressions
const emailPattern = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
const match = "test@example.com".match(emailPattern);

// Modules (ES6)
export function calculateSum(a, b) {
    return a + b;
}

export default class Calculator {
    add(a, b) {
        return a + b;
    }
}

// Import (would be in another file)
// import Calculator, { calculateSum } from './calculator.js';

// Generators
function* numberGenerator() {
    let i = 0;
    while (true) {
        yield i++;
    }
}

// Symbols
const sym1 = Symbol("description");
const sym2 = Symbol("description");

// Map and Set
const map = new Map();
map.set("key1", "value1");
map.set("key2", "value2");

const set = new Set([1, 2, 3, 4, 5]);


