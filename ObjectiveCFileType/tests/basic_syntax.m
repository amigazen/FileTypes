//
//  basic_syntax.m
//  Objective-C Basic Syntax Test
//

#import <Foundation/Foundation.h>

/* This is a block comment
   that spans multiple lines */

@interface MyClass : NSObject
{
    int value;
    NSString *name;
}

@property (nonatomic, strong) NSString *name;
@property (nonatomic, assign) int value;

- (void)setValue:(int)newValue;
- (int)getValue;
+ (MyClass *)sharedInstance;

@end

@implementation MyClass

@synthesize name;
@synthesize value;

- (id)init
{
    self = [super init];
    if (self) {
        value = 0;
        name = @"Default";
    }
    return self;
}

- (void)setValue:(int)newValue
{
    value = newValue;
}

- (int)getValue
{
    return value;
}

+ (MyClass *)sharedInstance
{
    static MyClass *instance = nil;
    @synchronized(self) {
        if (instance == nil) {
            instance = [[MyClass alloc] init];
        }
    }
    return instance;
}

@end

int main(int argc, const char * argv[])
{
    @autoreleasepool {
        MyClass *obj = [[MyClass alloc] init];
        [obj setValue:42];
        NSLog(@"Value: %d", [obj getValue]);
        
        NSString *str = @"Hello, Objective-C!";
        NSLog(@"%@", str);
    }
    return 0;
}
