//
//  advanced_features.m
//  Objective-C Advanced Features Test
//

#import <Foundation/Foundation.h>

@protocol MyProtocol <NSObject>
@required
- (void)requiredMethod;
@optional
- (void)optionalMethod;
@end

@class ForwardClass;

@interface AdvancedClass : NSObject <MyProtocol>
{
    @private
    int privateVar;
    @protected
    int protectedVar;
    @public
    int publicVar;
}

@property (nonatomic, copy) NSString *copiedString;
@property (nonatomic, weak) id delegate;
@property (nonatomic, readonly) int readOnlyValue;

- (void)methodWithBlock:(void (^)(int result))completion;
- (void)methodWithSelector:(SEL)selector;

@end

@implementation AdvancedClass

- (void)requiredMethod
{
    NSLog(@"Required method called");
}

- (void)optionalMethod
{
    NSLog(@"Optional method called");
}

- (void)methodWithBlock:(void (^)(int result))completion
{
    int result = 100;
    if (completion) {
        completion(result);
    }
}

- (void)methodWithSelector:(SEL)selector
{
    if ([self respondsToSelector:selector]) {
        [self performSelector:selector];
    }
}

@end

@interface CategoryClass : NSObject
- (void)originalMethod;
@end

@implementation CategoryClass
- (void)originalMethod
{
    NSLog(@"Original method");
}
@end

// Category extension
@interface CategoryClass (Extended)
- (void)extendedMethod;
@end

@implementation CategoryClass (Extended)
- (void)extendedMethod
{
    NSLog(@"Extended method");
}
@end

int main(int argc, const char * argv[])
{
    @autoreleasepool {
        AdvancedClass *obj = [[AdvancedClass alloc] init];
        
        // Block usage
        [obj methodWithBlock:^(int result) {
            NSLog(@"Block result: %d", result);
        }];
        
        // Exception handling
        @try {
            NSArray *array = @[@"one", @"two", @"three"];
            id item = array[10]; // This will throw
        }
        @catch (NSException *exception) {
            NSLog(@"Caught exception: %@", exception);
        }
        @finally {
            NSLog(@"Finally block executed");
        }
        
        // Fast enumeration
        NSArray *items = @[@"a", @"b", @"c"];
        for (NSString *item in items) {
            NSLog(@"Item: %@", item);
        }
    }
    return 0;
}
