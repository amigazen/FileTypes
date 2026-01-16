//
//  edge_cases.m
//  Objective-C Edge Cases Test
//

#import <Foundation/Foundation.h>

// Preprocessor directives
#define MAX_VALUE 100
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#ifdef DEBUG
    #define LOG(x) NSLog(@"%@", x)
#else
    #define LOG(x)
#endif

#ifndef RELEASE
    #define TEST_MODE 1
#endif

// String literals
NSString *singleLine = @"Single line string";
NSString *multiLine = @"Multi line \
string continuation";
NSString *withEscapes = @"String with \"quotes\" and \n newline";

// Character literals
char ch = 'A';
char newline = '\n';
char tab = '\t';

// Numbers
int decimal = 42;
int hex = 0xFF;
int octal = 077;
float floating = 3.14f;
double precise = 2.71828;
long long big = 1234567890LL;

// Comments in various positions
@interface CommentedClass : NSObject // Inline comment
{
    int value; /* Block comment */
    // Single line comment
    NSString *name; // Another inline
}
@end

@implementation CommentedClass
- (void)method
{
    // Comment before code
    int x = 10;
    /* Block comment */
    int y = 20; // Inline comment
    /* Multi-line
       block comment */
}
@end

// Preprocessor with Objective-C
#if TARGET_OS_MAC
    #import <Cocoa/Cocoa.h>
#elif TARGET_OS_IPHONE
    #import <UIKit/UIKit.h>
#endif

// Nested comments (should be handled)
/*
 Outer comment
 /* Inner comment - this is actually invalid but test it */
 Still in outer comment
 */

// Operators
int a = 10 + 20;
int b = 30 - 15;
int c = 5 * 6;
int d = 100 / 4;
BOOL flag = YES && NO || YES;
id obj = nil ? nil : [[NSObject alloc] init];

// Method calls with various syntax
[obj method];
[obj method:arg];
[obj method:arg1 withArg:arg2];
[[obj method] anotherMethod];
[obj methodWithBlock:^{
    NSLog(@"Block content");
}];

// Selectors
SEL selector = @selector(methodName);
SEL multiArg = @selector(method:withArg:);

// Protocols
@protocol EmptyProtocol
@end

@protocol ProtocolWithMethods
- (void)method;
+ (Class)classMethod;
@end

int main(int argc, const char * argv[])
{
    @autoreleasepool {
        NSLog(@"Edge cases test");
    }
    return 0;
}
