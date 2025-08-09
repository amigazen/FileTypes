# Comprehensive Markdown Test

## Headings (IMPLEMENTED)

# Heading 1
## Heading 2
### Heading 3
#### Heading 4
##### Heading 5
###### Heading 6

## Emphasis (IMPLEMENTED)

*italic text*
**bold text**
***bold and italic***
_italic with underscores_
__bold with underscores__

## Inline Code (IMPLEMENTED)

`printf("Hello, World!");`

## Links (IMPLEMENTED)

[Link text](http://example.com)
[Another link](http://example.org)

## Images (IMPLEMENTED)

![Alt text](http://example.com/image.jpg)

## Lists (IMPLEMENTED)

- Unordered list item 1
- Unordered list item 2
  - Nested item
    - Another nested item
* Another unordered list
* With asterisks
+ Yet another list
+ With plus signs
1. Ordered list item 1
2. Ordered list item 2
3. Ordered list item 3

## Blockquotes (IMPLEMENTED)

> This is a blockquote
> With multiple lines
> Each line should be highlighted

## Code Blocks (IMPLEMENTED)

```
function hello() {
  console.log("Hello, World!");
}
```

## Strikethrough (IMPLEMENTED - but uses italic underline as there is no strikethrough on Amiga)

~~This text should be strikethrough~~

## HTML Tags (IMPLEMENTED)

<strong>Bold HTML</strong>
<em>Italic HTML</em>
<div>Block HTML</div>

## ❌ Horizontal Rules (NOT IMPLEMENTED - could be added)

---

***

___

## Task Lists (IMPLEMENTED)

- [ ] Unchecked task
- [x] Checked task
- [ ] Another unchecked task

## Reference Links (IMPLEMENTED)

[Link text][ref1]
[Another link][ref2]

[ref1]: http://example.com "Optional title"
[ref2]: http://example.org

## Auto Links (IMPLEMENTED)

<http://example.com>
<mailto:user@example.com>

## Mixed Content (IMPLEMENTED)

This paragraph contains **bold text**, *italic text*, and `inline code` all mixed together.

> This blockquote contains **bold** and *italic* text as well.

1. List item with **bold** and *italic*
2. Another item with `code` 