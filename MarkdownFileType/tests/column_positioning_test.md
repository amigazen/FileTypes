# Column Positioning Test - IMPLEMENTED

## Inline patterns from column 1 (IMPLEMENTED)

`inline code from column 1`
**bold text from column 1**
*italic text from column 1*
***bold+underline from column 1***

## Lists from column 1 (IMPLEMENTED)

- List item from column 1
+ Another list item from column 1
* Yet another list item from column 1
1. Ordered list from column 1

## Lists from column 2+ with whitespace (IMPLEMENTED)

 - List item from column 2
  - List item from column 3
   - List item from column 4
	- List item with tab
	 - List item with tab + space

 + Another list item from column 2
  + Another list item from column 3
   + Another list item from column 4
	+ Another list item with tab

 * Yet another list item from column 2
  * Yet another list item from column 3
   * Yet another list item from column 4
	* Yet another list item with tab

 1. Ordered list from column 2
  2. Ordered list from column 3
   3. Ordered list from column 4
	1. Ordered list with tab

## Blockquotes from column 2+ with whitespace (IMPLEMENTED)

 > Blockquote from column 2
  > Blockquote from column 3
   > Blockquote from column 4
	> Blockquote with tab

## Mixed inline patterns in lists (IMPLEMENTED)

- List item with `inline code`
- List item with **bold text**
- List item with *italic text*
- List item with ***bold+underline***

 1. Ordered list with `inline code`
  2. Ordered list with **bold text**
   3. Ordered list with *italic text*
	4. Ordered list with ***bold+underline***

## Mixed inline patterns in blockquotes (IMPLEMENTED)

 > Blockquote with `inline code`
  > Blockquote with **bold text**
   > Blockquote with *italic text*
	> Blockquote with ***bold+underline***

## Edge cases (IMPLEMENTED)

- `code` **bold** *italic* ***bold+underline*** in same line
 1. `code` **bold** *italic* ***bold+underline*** in ordered list
  > `code` **bold** *italic* ***bold+underline*** in blockquote