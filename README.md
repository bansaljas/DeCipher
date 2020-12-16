<h1 align = "center"> DeCipher: Our Very Own Interpreter in C++ </h1>

### Group - 16

**Mentor** - Dr. Sanjeev Sofat, Professor, Computer Science and Engineering

### Team Members:

- Srishti Arora - 18103011
- Jasmeen Bansal - 18103016
- Bhavya Gulati - 18103024
- Saksham Basandrai - 18103048

---

## Documentation:  [Decipher - Documentation](https://github.com/bansaljas/DeCipher/blob/master/DeCipher_Report.pdf)


## Project Synopsis:

<ins>**BACKGROUND:**</ins>

The main purpose of a compiler or an interpreter is to translate a source program written in a high-level source language to machine language. The difference between a compiler and an interpreter is that a compiler generates object code written in the machine language and the interpreter executes the instructions directly.

<p align = "center">
<img src = "https://github.com/bansaljas/DeCipher/blob/master/Images/CompilervsInterpreter.png" alt = "-" width = "600" height = "400">
</p>


<ins>**MOTIVATION:**</ins>

   <p align = "center">
   <b><i>I hear and I forget. I see and I remember. I do and I understand.</i></b>
   </p>

These famous lines said by **Confucius**, holds true in all scenarios. For the best way to fully test the understanding of a concept is by building or creating it.

So, following the same philosophy, we as Computer Science students, use programming languages everyday and thus, we attempt to build our very own Interpreter from scratch, so as to gain a better insight on how it works and functions. To write an interpreter we need to have a lot of technical skills that need to be used together and  writing an interpreter will help us improve those skills and become better software developers.


<ins>**WORKFLOW:**</ins>

Following is a document, entailing all the work we have done till now, the various approaches we tried out and how we implemented our source code for the interpreter: [ WorkFlow Document](https://github.com/bansaljas/DeCipher/blob/master/Workflow.docx)

## Timeline:

<ins>**28 July 2020 - 15 August 2020:**</ins>
- **Team formation and Mentor Selection:**

  - Team Formation.
  - Brainstorming sessions amongst the team members to discuss upon the various viable project ideas.
  - Selecting the mentor best suited to our project ideas.
  - Pitched our idea to the mentor and he approved our idea of making an interpreter.

---

<ins>**16 August 2020 - 31 August 2020:**</ins>
- **Research and Exploration Phase**

   - As suggested by our team mentor, we looked into various research papers based on compilers and interpreters.
   - Exploring various tools like ANTLR, yacc, lex which could be beneficial for our project.
   - Acquainted ourselves with the existing work that has happened, and narrowed down to a few viable approaches.

---

<ins>**1 September 2020 - 15 September 2020:**</ins>
- **Implementation Phase 1**

   - Explored the various terminologies related to the interpreter like Tokenization, Lexical Analyser and a few more.
   - Started with the basic brute force implementation for the addition operation of single digit integers.
   - Looked into the various edge cases and figured out the shortcomings.
   - This implementation lacked handling whitespace given in the input.
   - Ponderred upon the features to be added and discussed their workflow.

---

<ins>**16 September 2020 - 30 September 2020:**</ins>
- **Implementation Phase 2**

   - Created the GitHub repository for collaboration purposes.
   - Now added the subtraction operation functionality as well.
   - Handled the previous shortcoming by adding code for ignoring whitespaces.
   - Further implemented the code for performing operations on multi digit integers.
   - Tried out various edge cases and discussed further workflow

---

<ins>**1 October 2020 - 15 October 2020:**</ins>
- **Implementation Phase 3**

   - As discussed with our mentor, we know majorly focused on exploring Context-Free Grammars.
   - We then prepared a few grammar rules (productions) for our interpreter.
   - Added functionality for multiplication, division, exponentiation along with handling parenthesis.
   - Looked into edge cases and made a few minor fixes.
   
---

<ins>**15 October 2020 - 30 October 2020:**</ins>
- **Implementation Phase 4**

   - Started with the implementation of Abstract Syntax Trees.
   - Added the functionality to evaluate Unary Operators.
   - Explored more into the functionality of an interpreter and completed AST implementation.
   - Created various header files for better code readability.
   
---

<ins>**1 Novemeber 2020 - 15 November 2020:**</ins>
- **Implementation Phase 5**
  
   - Started with the implementation of symbol tables to perform Semantic Analysis.
   - Fixed a few bugs caused due to some edge cases.
   - Added the feature of evaluating procedures having arguments.
   - Improved the Semantic Analyser further and looked into the concept of scopes.

---

<ins>**16 Novemeber 2020 - 30 November 2020:**</ins>
- **Implementation Phase 6**
  
   - Added functionality to handle Global as well as Functional Scopes.
   - Introduced print statement.
   - The process of error handling was improved.
   - Implemented the execution of function calls.
 
---

<ins>**1 December 2020 - Present:**</ins>
- **Implementation Phase 7**

   - Improved the memory system by introducing call stack.
   - Implemented nested procedure call.
   - Introduced read statement for taking input from the user.
   - Aceesing non-local variables for functions.

## How to Run:

- The code will run on *Visual Studio Code 2019*. You need to connect the project with **C++ Boost External Library**.
- Now give the program you'd like to evaluate in *sample.txt* file.
- Now, we can run the *main.cpp* file and get the desired output.
