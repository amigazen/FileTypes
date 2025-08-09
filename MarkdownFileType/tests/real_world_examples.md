# Real-World Markdown Examples - MOSTLY IMPLEMENTED

## README.md Example (IMPLEMENTED)

# My Awesome Project

A **fantastic** project that does *amazing* things with `code`.

## Features

- **Fast**: Lightning quick performance
- **Reliable**: Rock solid stability  
- **Easy**: Simple to use
- **Flexible**: Adapts to your needs

## Installation

1. Download the latest release
2. Extract the archive
3. Run the installer
4. Follow the setup wizard

## Usage

```bash
./myproject --config config.yaml
```

## Configuration

Create a `config.yaml` file:

```yaml
database:
  host: localhost
  port: 5432
  name: myapp
```

## API Reference

### `getUser(id)`

Retrieves a user by ID.

**Parameters:**
- `id` (string): The user ID

**Returns:**
- `User` object or `null` if not found

**Example:**
```javascript
const user = getUser("12345");
if (user) {
  console.log(`Hello, ${user.name}!`);
}
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests
5. Submit a pull request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Changelog

### v1.2.0
- Added new feature X
- Fixed bug in Y
- Improved performance

### v1.1.0
- Initial release
- Basic functionality

## Horizontal Rule (NOT IMPLEMENTED)
---

## Documentation Example (IMPLEMENTED)

# API Documentation

## Overview

This API provides access to our powerful services.

## Authentication

All API requests require authentication using an API key.

```http
Authorization: Bearer YOUR_API_KEY
```

## Endpoints

### GET /users

Retrieves a list of users.

**Query Parameters:**
- `page` (integer): Page number (default: 1)
- `limit` (integer): Items per page (default: 10)
- `sort` (string): Sort field (default: "name")

**Response:**
```json
{
  "users": [
    {
      "id": "123",
      "name": "John Doe",
      "email": "john@example.com"
    }
  ],
  "total": 100,
  "page": 1
}
```

### POST /users

Creates a new user.

**Request Body:**
```json
{
  "name": "Jane Smith",
  "email": "jane@example.com"
}
```

**Response:**
```json
{
  "id": "124",
  "name": "Jane Smith",
  "email": "jane@example.com",
  "created_at": "2023-01-01T00:00:00Z"
}
```

## Error Handling

All errors return a standard error response:

```json
{
  "error": {
    "code": "VALIDATION_ERROR",
    "message": "Invalid email format",
    "details": {
      "field": "email"
    }
  }
}
```

## Rate Limiting

- **Free tier**: 100 requests/hour
- **Pro tier**: 1000 requests/hour
- **Enterprise**: Custom limits

## SDK Examples

### JavaScript

```javascript
import { ApiClient } from '@mycompany/api';

const client = new ApiClient('YOUR_API_KEY');

// Get users
const users = await client.users.list({
  page: 1,
  limit: 10
});

// Create user
const user = await client.users.create({
  name: 'John Doe',
  email: 'john@example.com'
});
```

### Python

```python
from mycompany_api import ApiClient

client = ApiClient('YOUR_API_KEY')

# Get users
users = client.users.list(page=1, limit=10)

# Create user
user = client.users.create(
    name='John Doe',
    email='john@example.com'
)
```

## Horizontal Rule (NOT IMPLEMENTED)
---

## Blog Post Example (IMPLEMENTED)

# Getting Started with Markdown

*Published on January 1, 2023*

Markdown is a **lightweight markup language** that you can use to add formatting elements to plaintext text documents. Created by John Gruber in 2004, Markdown is now one of the world's most popular markup languages.

## Why Use Markdown?

- **Easy to learn**: The syntax is straightforward and memorable
- **Portable**: Files containing Markdown-formatted text can be opened using virtually any application
- **Platform independent**: You can create Markdown-formatted text on any device running any operating system
- **Future proof**: Even if the application you're using stops working at some point in the future, you'll still be able to read your text using a text editing application

## Basic Syntax

### Headings

To create a heading, add number signs (`#`) in front of a word or phrase. The number of number signs you use should correspond to the heading level.

```markdown
# Heading level 1
## Heading level 2
### Heading level 3
#### Heading level 4
##### Heading level 5
###### Heading level 6
```

### Emphasis

You can add emphasis by making text bold or italic.

```markdown
**bold text**
*italicized text*
***bold and italicized text***
```

### Lists

You can organize items into ordered and unordered lists.

#### Unordered Lists

To create an unordered list, add dashes (`-`), asterisks (`*`), or plus signs (`+`) in front of line items.

```markdown
- First item
- Second item
- Third item
- Fourth item
```

#### Ordered Lists

To create an ordered list, add line items with numbers followed by periods.

```markdown
1. First item
2. Second item
3. Third item
4. Fourth item
```

### Links

To create a link, enclose the link text in brackets and then follow it immediately with the URL in parentheses.

```markdown
[My website](https://www.example.com)
```

### Images

To add an image, add an exclamation mark (`!`), followed by alt text in brackets, and the path or URL to the image asset in parentheses.

```markdown
![Alt text](https://www.example.com/image.jpg)
```

## Advanced Features

### Code Blocks

To create code blocks, indent every line of the block by at least 4 spaces.

```markdown
    <html>
      <head>
        <title>Test</title>
      </head>
    </html>
```

### Blockquotes

To create a blockquote, add a `>` in front of a paragraph.

```markdown
> Dorothy followed her through many of the beautiful rooms in her castle.
```

### Horizontal Rules

To create a horizontal rule, use three or more asterisks (`***`), dashes (`---`), or underscores (`___`) on a line by themselves.

```markdown
---
```

## Conclusion

Markdown is a powerful tool for creating formatted text documents. Its simplicity and portability make it an excellent choice for documentation, note-taking, and content creation.

*Happy writing!* 