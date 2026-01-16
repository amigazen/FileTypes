// Edge cases and tricky JavaScript syntax

// Nested template literals
const nested = `Outer ${`Inner ${x}`} template`;

// Regex in different contexts
const regex1 = /test/;
const regex2 = new RegExp("test", "gi");
const regex3 = /\/test\//;

// Comments in strings (should not be highlighted as comments)
const stringWithSlash = "// This is not a comment";
const stringWithAsterisk = "/* This is not a comment */";

// Division vs regex (context-dependent)
const division = x / 2;
const regex = /test/gi;

// String escaping
const escaped = "This has \"quotes\" and 'apostrophes'";
const escaped2 = 'This has \'apostrophes\' and "quotes"';
const escaped3 = `This has \`backticks\` and ${x} interpolation`;

// Numbers in different formats
const decimal = 123.456;
const hex = 0xFF;
const octal = 0o755;
const binary = 0b1010;
const scientific = 1e10;
const negative = -42;

// Operators
const add = a + b;
const subtract = a - b;
const multiply = a * b;
const divide = a / b;
const modulo = a % b;
const power = a ** b;
const increment = ++a;
const decrement = --a;
const equal = a == b;
const strictEqual = a === b;
const notEqual = a != b;
const strictNotEqual = a !== b;
const lessThan = a < b;
const greaterThan = a > b;
const lessOrEqual = a <= b;
const greaterOrEqual = a >= b;
const logicalAnd = a && b;
const logicalOr = a || b;
const logicalNot = !a;
const bitwiseAnd = a & b;
const bitwiseOr = a | b;
const bitwiseXor = a ^ b;
const bitwiseNot = ~a;
const leftShift = a << b;
const rightShift = a >> b;
const unsignedRightShift = a >>> b;
const ternary = condition ? value1 : value2;
const assignment = a = b;
const addAssign = a += b;
const subtractAssign = a -= b;
const multiplyAssign = a *= b;
const divideAssign = a /= b;
const moduloAssign = a %= b;
const powerAssign = a **= b;
const leftShiftAssign = a <<= b;
const rightShiftAssign = a >>= b;
const unsignedRightShiftAssign = a >>>= b;
const bitwiseAndAssign = a &= b;
const bitwiseOrAssign = a |= b;
const bitwiseXorAssign = a ^= b;
const nullishCoalescing = a ?? b;
const optionalChaining = obj?.property;
const optionalCall = func?.();

// Keywords in different contexts
const obj = {
    class: "test",  // class as property name
    function: "test",  // function as property name
    return: "test",  // return as property name
    if: "test",  // if as property name
    else: "test"  // else as property name
};

// Built-in objects and methods
const arr = new Array(1, 2, 3);
const str = new String("test");
const num = new Number(42);
const bool = new Boolean(true);
const date = new Date();
const math = Math.PI;
const json = JSON.parse('{"key": "value"}');
const consoleLog = console.log("test");

// Null, undefined, NaN, Infinity
const nullValue = null;
const undefinedValue = undefined;
const notANumber = NaN;
const infinity = Infinity;
const negativeInfinity = -Infinity;

// Type checking
const isNull = value === null;
const isUndefined = value === undefined;
const isNaN = isNaN(value);
const isFinite = isFinite(value);
const typeOf = typeof value;
const instanceOf = value instanceof Array;


