# Edge Cases and Complex Scenarios

## Mixed Content (IMPLEMENTED)

This paragraph has **bold**, *italic*, and `code` all mixed together.

> This blockquote contains **bold** and *italic* text.

1. List with **bold** and *italic*
2. Another with `code`

## Nested Elements (IMPLEMENTED)

### Lists in Blockquotes
> - List item in blockquote
> - Another item
>   - Nested item

### Code in Lists
- Item with `inline code`
- Another item
  - Nested with `more code`

### Links in Lists
- [Link in list](http://example.com)
- ![Image in list](http://example.com/image.jpg)

## Complex Headings (IMPLEMENTED)

# Heading with **bold** and *italic*
## Heading with `code`
### Heading with [link](http://example.com)

## Escaped Characters (NOT IMPLEMENTED - could be added)

\*This should not be italic\*
\`This should not be code\`
\[This should not be a link\]

## Empty Elements (PARTIAL - needs testing)

### Empty Lists
- 
- 
* 
+ 

### Empty Blockquotes
> 
> 
> 

## Mixed Delimiters (COMPLEX - may have issues)

**Bold with _italic_ inside**
*Italic with **bold** inside*
`Code with *italic* inside`

## Complex Links (IMPLEMENTED)

[Link with **bold** text](http://example.com)
[Link with `code` text](http://example.com)
![Image with *italic* alt](http://example.com/image.jpg)

## Task Lists (IMPLEMENTED)

- [ ] Unchecked task
- [x] Checked task
- [ ] Task with **bold** text
- [x] Task with *italic* text
- [ ] Task with `code`

## Reference Links (IMPLEMENTED)

[Link text][ref1]
[Another link][ref2]

[ref1]: http://example.com "Optional title"
[ref2]: http://example.org

## Auto Links (IMPLEMENTED)

<http://example.com>
<mailto:user@example.com>

## HTML Tags (IMPLEMENTED)

<strong>Bold HTML</strong>
<em>Italic HTML</em>
<div>Block HTML</div>
<span>Inline HTML</span>

## Strikethrough (IMPLEMENTED)

~~This text should be strikethrough~~
~~Text with **bold** inside~~
~~Text with *italic* inside~~
~~Text with `code` inside~~

## Complex Nesting (IMPLEMENTED)

> Blockquote with:
> - List item
> - Another item with **bold**
> - Item with `code`
> 
> And more text

1. Ordered list with:
   - Nested unordered
   - Another nested with *italic*
2. Second item with `code` 