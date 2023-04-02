# prog_3
3. bit operations
#################################################################################
Purpose of work
This work consists of two parts.

The first part asks you to understand the algorithm for encoding integers, called varint (variable integer). This way of coding allows you to use a variable number of bytes to represent integers and thus makes the data compact. You need to develop an application for writing and reading numbers in raw form and in varint format, compare these two methods in terms of efficiency, and draw conclusions about the applicability of the proposed coding method.

In the second part you should independently realize the UTF-8 encoding algorithm. This algorithm solves a similar problem - allows you to encode integers with a variable number of bytes, but is used to encode character codes and supports backward compatibility with ASCII encoding.

Base 128 Varint
When designing binary data formats there may be a need for compact storage of integers. On the one hand one wants to use as few bytes as possible for small numbers. On the other hand, a fixed dimensional solution is not flexible enough. As an example, one might think of a network protocol that transmits first the data volume and then the data itself. In most cases the data volumes are expected to be small, and the minimum number of bytes needed to encode the message length can be used, but at the same time it should be possible to send large packets.

In such situations the varint encoding format is used. Integer numbers are encoded as follows:

The highest bit of each byte is a service bit. If there is a 1 in the high bit, it is not the last byte, there is a continuation. If the high bit is 0 it is the last byte of the number.
The lower 7 bits are used to store part of the number being encoded. We will consider serialization with the Little Endian byte order.

Example
#################################################################################
Suppose we need to encode the number 1. In binary code, the one looks like this

0000 0001
It is the only byte necessary for encoding, so leave 0 in the high bit.

Let's encode the number 300. Binary code:

0000 0001 0010 1100
Since we plan to store one service bit and 7 bits of the encoded number in each byte, divide this representation into groups of 7 bits:

0000010 0101100
Change the order to Little Endian:

0101100 0000010
Add a service bit to each group:

10101100 00000010
#################################################################################
In open source projects you can find various ways of implementing this coding method:
Git: varint.c
SQLite: lsm1/lsm_varint.c
Google Protobuf: CodedOutputStream::WriteVarint32ToArray

Task 1
#################################################################################
Develop an application that generates 1000000 random numbers and writes them into two binary files. Write the numbers in uncompressed format to the uncompressed.dat file and in varint format to the compressed.dat file. Compare the sizes of the files.

Implement the reading of numbers from the two files. Add a check: the sequences of numbers from the two files must coincide.

Using the format varint is most effective when the vast majority of the numbers have small values. To do the work, use the function of generating random numbers

#################################################################################

UTF-8
#################################################################################
A similar task of compact encoding arises when working with texts. You are already familiar with single-byte encodings: ASCII, cp1251, koi8-r and others. The problem is that they use a single byte to encode a character, and a document using one of these encodings cannot contain characters in more than two languages. The problem is solved by using UTF-8 encoding.

You can read more about the history of encodings in Joel Spolsky - The Absolute Minimum Every Software Developer Absolutely, Positively Must Know About Unicode and Character Sets (No Excuses!).

Encoding Algorithm:

The numbers [0;28-1]
 will be represented as 0xxxxxxxxx, "as is":

00000000 - 0
00000001 - 1
...
00000101 - 5
...
01111111 - 127
For larger values, we will store as many ones in the high byte as the number of bytes required to represent the encoded number. 110xxxxx is two, 1110xxxx is three, etc. All subsequent bytes are 10xxxxxxx.

Bits marked with x are filled with bits of the number to be encoded.

The table can be used to select the length of the code:

Number of significant bits of the encoded number Number of bytes in the code Template
Template 7 1 0xxxxxxx
11 2 110xxxxx 10xxxxx
16 3 1110xxxxx 10xxxxx 10xxxxx
21 4 11110xxxxx 10xxxxx 10xxxxx 10xxxxx
Example of number encoding
Let the number 0xa8cc be encoded. The number occupies 2 bytes, 16 bits. Therefore, 3 bytes are needed to encode the number.

We use the pattern: 1110xxxxx 10xxxxx 10xxxxx

Binary representation of number 0xa8cc: 1010100011001100

The significant bits of the number should now be substituted into the pattern:

Template: 1110xxxxx 10xxxxx 10xxxxx
Number: 1010 100011 001100
Result: 11101010 10100011 10001100
Hexadecimal representation of the encoded number: 0xa8cc

Example decoding a number
Let's perform inverse operation. Let input byte stream: 11101010 10100011 10001100.

Read first byte: 11101010.

The byte starts with three ones. Therefore, there are three bytes in total in the encoded number. One is already read, we need to read two more: 1010001011 10001100.

We know the pattern for the three-byte code, all that is left is to select significant bits from the encoded number:

Template: 1110xxxxx 10xxxxx 10xxxxx
Read code: 11101010 10100011 10001100
Significant bits: 1010 100011 001100
Decoded number: 0b1010100011001100 = 0xa8cc.

Data losses
There may be data loss in the input stream. For example, given the following sequence: 10001100 11101010 10100011 10001100. The first byte has the form 10xxxxxxxx. This cannot be the first byte, since according to the coding algorithm, it was found that bytes following the first byte take this form. Therefore, part of the data at the beginning was lost. In this case, bytes can be skipped until the byte denoting the beginning of the code is read.

Reverse situation: 11101010 10100011. In this sequence, the first byte indicates that the encoded representation is 3 bytes, but in fact only two are present. This is followed by either the con
#################################################################################

Task 2
#################################################################################
Develop an application to encode and decode numbers using the algorithm described above.

Example of encoding:

$ ./coder encode points.txt units.bin
Here:

encode - encoding command

points.txt - input text file containing numbers written in hexadecimal notation.

An example of the file contents:

7
1e7
79e7
1e79e7
units.bin - output binary file.

Example result (use hexdump utility to view):

$ hexdump -C units.bin

00000000 07 c7 a7 e7 a7 f7 a7 a7
Example decoding:

$ ./coder decode units.bin points.txt
Run with incorrect number of arguments or incorrect command:

$ ./coder foobar
Usage:
coder encode <in-file-name> <out-file-name>
coder decode <in-file-name> <out-file-name>
Read encoded numbers into a variable of type uint32_t. To represent encoded number in memory, you can use structure:

enum {
    MaxCodeLength = 4
};

typedef struct {
    uint8_t code[MaxCodeLength];
    size_t length;
} CodeUnits;
Here:

code - encoded bytes, in order from highest to lowest.
length - number of bytes in the encoded representation.
The following project structure is suggested:

.
|-- Makefile
`-- src
    |-- coder.c
    |-- coder.h
    |-- command.c
    |-- command.h
    |-- main.c
The files command.h and command.c:

int encode_file(const char *in_file_name, const char *out_file_name)
Encodes a text file into a binary file, implementing the encode command.

Parameters:	
char *in_file_name (const) -
path to input text file

char *out_file_name (const) - path to output binary file.
path to output binary file

Result:	
0 in case of success, -1 in case of error

int decode_file(const char *in_file_name, const char *out_file_name)
Decode a binary file, implementing the decode command.

Parameters:	
char *in_file_name (const) -
path to input binary file

char *out_file_name (const) - path to output text file.
path to output text file

Result:	
0 in case of success, -1 in case of error

Files coder.h and coder.c:

int encode(uint32_t code_point, CodeUnits *code_units)
Parameters:	
code_point - number to encode
code_unit - output parameter, encoding result
Result:	
0 if encoding is successful, -1 otherwise

uint32_t decode(const CodeUnit *code_unit)
Assumption: code_unit is the correct code, obtained via the read_next_code_unit function.

Parameters: code_unit - the encoded representation of the number
Return code_point:
 	decoding result
int read_next_code_unit(FILE *in, CodeUnits *code_units)
Reads the code_units sequence from the in stream. Implementation note: if the code_unit being read is broken, the function skips bytes until it finds a valid leading byte.

Result: 0 on success, -1 on error or EOF
int write_code_unit(FILE *out, const CodeUnit *code_unit)
Note: normally a different decomposition is used in similar tasks. The proposed variant is intentionally simplified for the training purposes.

Command line arguments
Processing of command line arguments
#################################################################################
