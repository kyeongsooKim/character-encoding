# Unicode Transformation Format
## Introduction
The basic character encoding known as ASCII or US-ASCII is simply just a number to character mapping.
For example, the capital letter **A** in ASCII is defined by the hexadecimal value `0x41`.

> :nerd: In a *nix shell if you type man ascii you will get a full listing of
the US-ASCII chart.

If you open a binary file that used US-ASCII as its encoding in a hex-editor,
or another tool such as octal dump you would see the value `0x41` inside of it.
To see this, we can use the octal dump program.
Assume we had a text file named `ascii.txt` with the string "Hello, World!\n" encoded inside of it.
If we ran the command `od -t cxC ascii.txt` on the terminal, the following output is printed:

```
0000000   H   e   l   l   o   ,       w   o   r   l   d   !  \n
         48  65  6c  6c  6f  2c  20  77  6f  72  6c  64  21  0a
0000016
```
Looking at this output, we can see the string "Hello, World!".
Under each character is the hexadecimal value of the ASCII letter and each character is one byte long.
[On the following page is the usage statement for the program, octal dump.](http://man7.org/linux/man-pages/man1/od.1.html)

In ASCII, each character is only 1 byte long, we can, at most, encode 2^7 characters in total (128 values from [0 − 127]) can be encoded (The MSB is unused as there are no negative ASCII characters).
This is fine for the English language; we have 26 letters, lowercase and uppercase letters, 10 digits for numbers, and a few other special characters like space, comma, etc.

But what about other languages; for example the Chinese written language has ≈ 50000 symbols.
This is only one language.
What about every other written language in the world? As you can see, clearly one byte is not large enough to represent all the written languages of the world.
As a result, Unicode was created to accommodate for other languages in computing.
Further, the task of accommodating other languages in computing became a more and more pressing concern as computing moved away from being reserved for scientists and engineers on campuses and research facilities to the common person.
The need further expanded with the introduction of the internet and the swift interconnection of the world that it brought to life.
Because of this, often web browsers will sport very robust text-encoding code to handle languages and characters of all kinds effectively.
Some examples of languages that cause a lot of trouble to the old ASCII byte system are Chinese, Japanese katakana and hiragana, and the Khmer language (which has a total of 74 letters and is considered the language with the largest alphabet).

Unicode is the computing industry standard for encoding text in a consistent way across multiple platforms.
In Unicode, we use this idea of a code point.
A [code point](http://en.wikipedia.org/wiki/Code_point) is an abstract idea.
The code point of a character may not be the actual value that is encoded in the file.
It is up to the specification of the encoding to determine how to store the code point using the selected encoding scheme.
One nice thing about UTF-8 is that the encoding scheme will encode the first 127 ASCII characters the same way they have always been encoded.
This is good for older applications that only use English text and are unaware of today’s Unicode specifications.
Because of this reason, UTF-8 is the most common encoding used today.
Returning to the concept of a code point, each glyph in the Unicode code page corresponds to a code-point.

# UTF-8

| Glyph  | ASCII | CodePoint |    UTF8    |
|:------:|:-----:|:---------:|:----------:|
|   !    | 0x21  |  U+0021   | 0x21       |
|   é    | NONE  |  U+00E9   | 0xC3A9     |
|   知   | NONE  |  U+77E5   | 0xE79FA5   |
| :nerd: | NONE  |  U+1F913  | 0xF09FA493 |

For a complete list of the glyphs represented in unicode see the [unicode-table](https://unicode-table.com/en/).

- The US-ASCII letters have the same representation across all the listed formats (as described above this is by design).
- Glyphs/characters that are not part of the English alphabet can span multiple bytes in UTF-8; UTF-8 has a variable width encoding.
    - The é has a code point of U+00E9 but is encoded in UTF-8 as 0xC3A9 (2 bytes)
    - The 知 has a code point of U+4E16 but is encoded as 0xE4B896 (3 bytes).
    - The :nerd: has a code point of U+1F913 but is encoded as 0xF09FA493 (4 bytes).

## From encoding to code point

As we can see from the chart above, the code point may not always map directly to the actual encoded value.
So what exactly are the rules for encoding a Unicode code point in UTF-8?

|  Code Point Range  | Bytes |             UTF8 Binary Format            |
|:------------------:|:-----:|:-----------------------------------------:|
| `0x00000-0x00007F` |   1   | `0xxx xxxx`                               |
| `0x00080-0x0007FF` |   2   | `110x xxxx 10xx xxxx`                     |
| `0x00800-0x00FFFF` |   3   | `1110 xxxx 10xx xxxx 10xx xxxx`           |
| `0x10000-0x1FFFFF` |   4   | `1111 0xxx 10xx xxxx 10xx xxxx 10xx xxxx` |

According to what’s shown on this table, if we have a code point whose value is between `0x00-0x7F` we can encode that value using 1 byte.
If we have a code point whose value is between `0x80-0x7FF` we need to use 2 bytes, etc.
The x’s in the encoding of each byte represent the bits we can use to store our Unicode code point in UTF-8 encoding.

You may notice in the encoding format that there are certain digits that are hard coded.
These are markers to determine how many bytes each glyph is composed of.

- The first byte of each glyph will tell you how many bytes are used to represent this glyph. If you read in the first byte and the Most Significant Bit (MSB) is equal to zero (0xxxxxxx), this means that only one byte is needed for this encoding.
- If the MSB is a 1, this means that this glyph is encoded using multiple bytes or the first byte is part of a multibyte sequence.
- The y’s in this pattern represent the possible positions where the encoding marking can exist. For example, if we look at the two byte encoding 110yyxxx, we can then treat the left over y’s as x’s so we really have 110xxxxx, where x are bits we can use for encoding our Unicode code point.

This means we have to process further to determine how many bytes this glyph is made up of.
To check the individual values of a byte, we need to use bitwise operations.
You will need to use the bitwise and, or, and shift operators (`&`, `|`, and `<<`/`>>`) to extract and evaluate these values.
The algorithm to determine the number of bytes is as follows:

```python
if glyph.MSB == 0:
    # The glyph is encoded using 1 byte.
else:
    if glyph.top_3_bits == 110xxxxx:
        # The glyph is encoded using 2 bytes.
    elif glyph.top_4_bits == 1110xxxx:
        # The glyph is encoded using 3 bytes.
    elif glyph.top_5_bits == 11110xxx:
        # The glyph is encoded using 4 bytes.
    else:
        # This is not a valid UTF-8 encoding.
```

### Example:

Consider a file containing the following: !é知🤓 .

```plaintext
0000000   ! 303 251 347 237 245 360 237 244 223
         21  c3  a9  e7  9f  a5  f0  9f  a4  93
0000013
```
We can decipher this octal dump by looking at each byte.

The first byte: 0x21 is `0010 0001` in binary.

The MSB is a 0. That means this character is encoded using 1 byte.
The remaining 7 bits represent the glyph.
Since the MSB is 0, no further processing is required to create the code point.
So, the code point for `!` is U+0021

Consider the next byte: 0xC3 is `11000011` in binary.
In this case, the MSB is a 1. This means the encoding is multiple bytes.
To determine the number of bytes used to represent this glyph, we need to check the most significant bits of the byte for the following patterns:

In this case we determine that the first 3 bits are `110` so that means this glyph is made up of 2 bytes.
We read in the second byte so now we have: 0xC3A9 which is `[110]00011` `[10]101001` in binary.
The bits between the brackets `[ ]` are the marker bits for each byte.
Remember, in the successive bytes the top two most significant bits are always `10`.
This means that it is a continuation byte of the glyph.
We take the x bits of each byte and concatenate them together to build the glyph.
For example from 0xC3 we need to extract the 5 LSBs and from 0xA9 we need to extract 6 LSBs to get U+00E9.

#### Try it on paper:

If the code point of :sunglasses: is U+1F60E what is the encoding (binary format)?

If the encoding of :100: is 0xF09F92AF what is the code point?

[Check your answers here](http://www.ltg.ed.ac.uk/~richard/utf-8.cgi)

# UTF-16

UTF-16 is actually considered one of the [worst Unicode encodings](http://utf8everywhere.org/) but for historical reasons we are stuck with it.
The debate about UTF-16 is an ongoing one, but one of its pitfalls is the fact that characters outside the BMP (basic multilingual plane) often give buggy behavior across operating systems, frameworks and other software programs such as browsers.
Another issue is that UTF-16 is not nearly as portable as UTF-8 (e.g. UTF-16 will function correctly in Java, but then fail in certain edge cases in Python).

Despite these issues, there are also still benefits to UTF-16. Most notably, it is particularly amenable to Asian languages in which there are a great number of characters to encode.
UTF-16 allows for immense character set encodings with ease, whereas UTF-8 is not so friendly.
This makes it especially challenging for Asian languages to abandon UTF-16 in favor for UTF-8. Nonetheless, certain software or software companies with large footprints, like the JVM and Microsoft use UTF-16 internally so it is still relevant today.

One good thing is UTF-16, for the most part, is actually easier to encode than UTF-8. For most Unicode code points, the code point is simply mapped directly into the file.
Consider the string !é知🤓 again.

## UTF16BE
```plaintext
0000000  \0   !  \0 351   w 345 330   > 335 023
         00  21  00  e9  77  e5  d8  3e  dd  13
0000012
```

## UTF16LE
```plaintext
0000000   !  \0 351  \0 345   w   > 330 023 335
         21  00  e9  00  e5  77  3e  d8  13  dd
0000012
```

Each of these code points is encoded into UTF-16 as two bytes with the exception of the emoji which are the last four bytes (this is explained below).
One downside to UTF-16 is that the endianness of the file matters.
So it is possible to encode your file with UTF-16 little endian and UTF-16 big endian.

### Surrogate Pairs

If the code point reaches a hexadecimal value of `0x10000`  or greater we need to create what is known as surrogate pairs.
A surrogate pair is “two two bytes”: a four byte representation of the code point.
This means for these values we need to perform additional operations to encode the code point into UTF-16.

This is the case with the nerd emoji contained in the file.
It's code point is U+1F913 and so it exceeds `0x10000`.

The algorithm for detecting and correctly encoding a surrogate pair is shown below.

```python
def surrogate_pair(code_point=0x1F913, endianness='UTF16BE'):
    code_point = 0x1F913
    encoding = 0
    if code_point > 0x10000:
        tmp = code_point - 0x10000 # 1F913
        ten_msbs = code_point & 0xFFC0
        ten_msbs = ten_msbs >> 10
        ten_lsbs = 0x03FF & tmp
        # Now we create the encoding:
        half1_byte_encoding = 0xD800 + ten_msbs
        half2_byte_encoding = 0xDC00 + ten_lsbs
        if endianness == 'UTF16BE':
            encoding = (half1_byte_encoding << 16) + half2_byte_encoding
        elif endianness == 'UTF16LE':
            encoding = (swap(half1_byte_encoding) << 16) + swap(half2_byte_encoding)
    print(hex(encoding))
```
### Summary of Surrogate Pair Algorithm

1. Determine if the value is greater than 0x10000.
2. If yes, then subtract 0x10000 from the code point and go to step 3. If not, the value is not a surrogate pair, so you should stop.
3. Create the MSB code unit.
    - Shift the value to the right 10 bits.
    - Add 0xD800 to the shifted value.
4. Create the LSB code unit.
    - Bitwise AND that value with 0x3FF .
    - Add 0xDC00 to the new value.
5. Combine the MSB and LSB code unit depending on endianness

### Detecting a surrogate pair

When reading a utf16 encoded file the first (high) surrogate is a 16-bit code value in the range U+D800 to U+DBFF.
The second (low) surrogate is a 16-bit code value in the range U+DC00 to U+DFFF.
Using the surrogate mechanism, UTF-16 can support all 1,114,112 potential Unicode characters.

# The BOM

Some files encoded in UTF-8 or UTF-16 have something known as a BOM or Byte Order Mark.
This BOM is meant to describe the encoding and endianness of the bytes stored in the file.
If the file is encoded using UTF-16BE or UTF-16LE, then the BOM will be the first two bytes in the file.
If the file is encoded in UTF-8, it will be the first 3 bytes of the file.

BOMs are actually not part of the unicode specifications, so its purpose may be ambiguous.
Further, the Unicode Consortium advises against adding BOMs to files.

| Encoding  |    BOM     |
|:---------:|:----------:|
| UTF-8     | `0xEFBBBF` |
| UTF-16LE  | `0xFFFE`   |
| UTF-16BE  | `0xFEFF`   |

Here's our example file yet again in all the formats discussed with a byte order mark

# UTF-8 with BOM
```plaintext
0000000 357 273 277   ! 303 251 347 237 245 360 237 244 223
         ef  bb  bf  21  c3  a9  e7  9f  a5  f0  9f  a4  93
0000016
```

# UTF-16LE with BOM
```
0000000 377 376   !  \0 351  \0 345   w   > 330 023 335
         ff  fe  21  00  e9  00  e5  77  3e  d8  13  dd
0000014
```

# UTF-16BE with BOM
```
0000000 376 377  \0   !  \0 351   w 345 330   > 335 023
         fe  ff  00  21  00  e9  77  e5  d8  3e  dd  13
0000014
```

