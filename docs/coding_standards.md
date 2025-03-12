# Driving Principles

### Clarity over compactness

Phosphor monitors and VGA limitations are no longer a legitimate reason for writing unreadable code

### Assume a developer with non-monospace font and 13-space tabs

Text editors are configurable for a reason, even if you don't like how some people configure them

### Build time

Long build times and painful iteration cycles are the leading cause of murder-suicide among game programmers in studio environments

### Maintainability

Code should be reasonably easy to re-factor, migrate, and generally shuffle around

### Structural isolation with no side-effects

Source files should be self-contained, and not spill tokenization/compilation artifacts into logically unrelated source files


# The Law

Laws are absolute, and never to be defied. Generally reserved for squashing religious battles over non-AST fluff.

### Allman style braces

Namespaces are excepted from this, due to their almost exclusively boiler-plate nature, but should observe this law as a rule when used for complex non-boilerplate purposes.

Single-line brace pairs for zero initialization are exempt from this.

Initializer lists are exempt from this, but use good judgement for readability.

Rationale: Clarity over compactness

### Tabs for indentation, not spaces

Rationale: Assume a developer with non-monospace font and 13-space tabs

### Standardized file extensions

Header files: \*.h

Inline files: \*.inl

C++ compilation unit files: \*.cpp

C compilation unit files: \*.c

Note that this also forbids using extensions for unexpected effects, like #include-ing a cpp file into another file.

The exception to this rule is build machinery which may procedurally collapse multiple compilation units into a single compilation unit.

Rationale: Maintainability, consistency

### No direct STL usage

The RetroFit Template Library (RFTL) is a wrapping layer around the STL, and should be used in place of the STL.

The RFTL layer contains various patches and work-arounds for different STL implementations and versions across different platforms.

RFTL itself must, by necessity, be an exception to this rule.

Rationale: Maintainability


# The Rules

Rules are strong, and exceptions will be scrutinized heavily. Though once you leave the engine layers, it starts to get a little wild-west.

### No code in headers

Rationale: Build time

### No unscoped variables (including anonymous namespaces)

Note that anonymous namespaces are rendered meaningless by build configurations that combine multiple source files into single compilation units, and so cannot appropriately hide variables from spilling out and conflicting with other anonymous namespaces

Rationale: Maintainability

### No aligning / indentation after non-whitespace characters in a line

This includes member variable name aligning

Rationale: Maintainability, and assume a developer with non-monospace font and 13-space tabs

### Standardized naming

Global variable: gUpperCamelCase

Static member variable: sUpperCamelCase

Non-static member variable: mUpperCamelCase

Constant expression variable: kUpperCamelCase

Class/Struct/Union/Enum/Typedef/Typename: UpperCamelCase

Functions: UpperCamelCase

Namespace: lower\_snake\_case (Note that the super-namespace 'RF' is an exception)

Function parameter: lowerCamelCase

Local variables: lowerCamelCase

Defines/macros: UPPER\_SNAKE\_CASE

Template parameters: TBD

Rationale: Clarity over compactness, consistency

### No unscoped 'using' directives

Rationale: Structural isolation with no side-effects

### No #define that isn't #undef'd before file end

Note there will be several careful exceptions to this rule in core libraries

Rationale: Structural isolation with no side-effects

### Observe const-correctness

Rationale: Code safety, interface clarity

### No trailing whitespace on lines

Rationale: You weren't raised in a barn, and even if you were, try not to make it obvious

### No gotos

Rationale: Please...


# The Preferences

Preferences are weak, but should be considered the 'default style' that is expected of all 'clean' code. The closer to core, the stronger these are expected to be observed.

### No lines exceeding 120 character column

Rationale: Clarity over compactness

### No comments exceeding 80 character column

Rationale: Clarity over compactness

### C++ style comments

Rationale: Consistency

### Consistent-style const placement

Const on the right for the first term just like every other term.
Follow the rule, don't use the exception.

Rationale: Consistency

### Standardized include order

First: precompiled header, if applicable, in quotes

Second: direct header file, if the includer is the implementation of it, in quotes

Third: game/engine dependency headers, in descending order of complexity, in quotes (the most dependent headers are first)

Fourth: third-party dependency headers, in quotes

Fifth: standard headers, in brackets

This generally results in 'heavy' files up top, and 'light' files on bottom, w.r.t. the amount of dependencies they add to includer

Rationale: Maintainability

### No one-line flow-control directives

This includes if, else, do, while, and for

Rationale: Clarity over compactness


# Personal Taste

Where not specified, style is left to the discretion of the owner of a body of code. When working in code that you do not own, it is expected that you will generally follow the style of the owner, within reason.

Note that 'ownership' is a vague concept, so you will need to exercise a balance of responsibility and respectfulness.


# General code safety

You may wish to peruse the following:

Stroustrup & Sutter: [https://github.com/isocpp/CppCoreGuidelines]

### Avoid anonymous namespaces

Anonymous namespaces suffer from symbol collisions that lead to difficult to track down memory corruption. They also don't provide any value when using build patterns that combine multiple compilation units. Similar problems can happen with named namespaces, but anonymous namespaces provide a false sense of security with no other meaningful value.

Note that MSVC v19 seems to be atleast clever enough to mangle hashes into the symbols for anonymous namespaces, but they are still shared by the entire compilation unit. Clang v9 and GCC v9.2 don't even offer this minimal protection.

### Avoid default parameters

Default parameters relinquish control to the caller, for what is often conceptually intended to be a callee responsibility. This also creates all sorts of havoc and unexpected (but well-defined) behavior in virtual methods.

(The underlying machinery behind this and all the problems they create make for a great interview question, as most programmers will have to figure it out on the spot. Those that already know will still have to be pretty technically competent to be able to explain the machinery)

### Use robust static asserts

When making assumptions about code that is not directly adjacent, assume that code might be changed, and use static asserts to guard against changes. This helps prevent others from accidentally breaking code they didn't know about, or forgot to update.

Where practical, consider decltype(var) over a primitive type, to clarify the intent and handle cases where the variable type may be changed later.

In general, view static asserts as a way to protect code from maintenance work that would introduce accidental bugs, and also double as codifying assumptions and intents.


# Autoformat

While no one should ever be running bulk auto-formats on code they are not the owner of for style elements not covered in the coding standards, minor code changes and additions may cause stylistic divergences with a modern IDE. These should be expected to creep in from time-to-time when collaborating with others.

If you are not particularly passionate about minor whitespace nuances, but want to be consistent, you may wish to use an auto-format configuration. Currently, there is a clang-format file included with the solution that should cover most of the basics. Be advised that it does NOT supersede this document and is NOT without its faults.

The following is a vs2017.vssettings import that generally matches the general engine style, in the event clang-format is not working:

```
<UserSettings>
	<ApplicationIdentity version="15.0"/>
	<ToolsOptions>
		<ToolsOptionsCategory name="TextEditor" RegisteredName="TextEditor">
			<ToolsOptionsSubCategory name="C/C++" RegisteredName="C/C++" PackageName="Text Management Package">
				<PropertyValue name="InsertTabs">true</PropertyValue>
			</ToolsOptionsSubCategory>
			<ToolsOptionsSubCategory name="C/C++ Specific" RegisteredName="C/C++ Specific" PackageName="Visual C++ Language Manager Package">
				<PropertyValue name="SpaceAroundBinaryOperator">2</PropertyValue>
				<PropertyValue name="SpaceAfterSemicolon">true</PropertyValue>
				<PropertyValue name="PreserveInitListSpace">true</PropertyValue>
				<PropertyValue name="SpaceBeforeFunctionParenthesis2">1</PropertyValue>
				<PropertyValue name="NewlineFunctionBrace">2</PropertyValue>
				<PropertyValue name="IndentBlockContents">true</PropertyValue>
				<PropertyValue name="SpaceBeforeInitListBrace">false</PropertyValue>
				<PropertyValue name="SpaceWithinCastParentheses">false</PropertyValue>
				<PropertyValue name="NewlineInitListBrace">0</PropertyValue>
				<PropertyValue name="IndentCaseContents">true</PropertyValue>
				<PropertyValue name="SpaceAroundConditionalOperator">2</PropertyValue>
				<PropertyValue name="SpaceWithinInitListBraces">true</PropertyValue>
				<PropertyValue name="SpaceBeforeLambdaParenthesis">false</PropertyValue>
				<PropertyValue name="NewlineScopeBrace">true</PropertyValue>
				<PropertyValue name="NewlineEmptyFunctionCloseBrace">true</PropertyValue>
				<PropertyValue name="PreserveBlock">0</PropertyValue>
				<PropertyValue name="SpaceBeforeConstructorColon">true</PropertyValue>
				<PropertyValue name="SpaceBeforeBlockBrace">false</PropertyValue>
				<PropertyValue name="NewlineKeywordWhile">false</PropertyValue>
				<PropertyValue name="NewlineLambdaBrace">2</PropertyValue>
				<PropertyValue name="EnableExpandPrecedence">false</PropertyValue>
				<PropertyValue name="AddSemicolonForClassTypes">true</PropertyValue>
				<PropertyValue name="SpaceBeforeFunctionParenthesis">false</PropertyValue>
				<PropertyValue name="NewlineNamespaceBrace">0</PropertyValue>
				<PropertyValue name="AutoFormatOnSemicolon">true</PropertyValue>
				<PropertyValue name="RenameRenameStrings">false</PropertyValue>
				<PropertyValue name="UseForwardSlashForIncludeAutoComplete">true</PropertyValue>
				<PropertyValue name="NewlineControlBlockBrace">2</PropertyValue>
				<PropertyValue name="IndentNamespaceContents">false</PropertyValue>
				<PropertyValue name="IndentLambdaBraces">false</PropertyValue>
				<PropertyValue name="CompleteSlashStar">true</PropertyValue>
				<PropertyValue name="SpaceBeforeComma">false</PropertyValue>
				<PropertyValue name="IndentAccessSpecifiers">false</PropertyValue>
				<PropertyValue name="SpaceAroundAssignmentOperator">2</PropertyValue>
				<PropertyValue name="RemoveSpaceBeforeSemicolon">true</PropertyValue>
				<PropertyValue name="SpaceBetweenEmptyLambdaBrackets">false</PropertyValue>
				<PropertyValue name="SpaceBeforeControlBlockParenthesis">false</PropertyValue>
				<PropertyValue name="SpaceBetweenEmptyFunctionParentheses">false</PropertyValue>
				<PropertyValue name="PreserveParameterIndentation">false</PropertyValue>
				<PropertyValue name="AutomaticOutliningOfStatementBlocks">true</PropertyValue>
				<PropertyValue name="SpaceBeforeEmptyBracket">false</PropertyValue>
				<PropertyValue name="SpaceWithinExpressionParentheses">true</PropertyValue>
				<PropertyValue name="AutoFormatOnPaste2">1</PropertyValue>
				<PropertyValue name="AutoFormatOnPaste">true</PropertyValue>
				<PropertyValue name="TrimSpaceUnaryOperator">true</PropertyValue>
				<PropertyValue name="TrimSpaceAroundScope">true</PropertyValue>
				<PropertyValue name="GroupBrackets">true</PropertyValue>
				<PropertyValue name="SpaceBetweenEmptyBrackets">false</PropertyValue>
				<PropertyValue name="SpaceWithinControlBlockParentheses">true</PropertyValue>
				<PropertyValue name="NewlineTypeBrace">2</PropertyValue>
				<PropertyValue name="SpaceAfterCastParenthesis">false</PropertyValue>
				<PropertyValue name="IncludeHintsSuppress">true</PropertyValue>
				<PropertyValue name="IndentBraces">false</PropertyValue>
				<PropertyValue name="PreserveCommentIndentation">true</PropertyValue>
				<PropertyValue name="IndentPreprocessor">0</PropertyValue>
				<PropertyValue name="IndentCaseBraces">false</PropertyValue>
				<PropertyValue name="AlignParameters">false</PropertyValue>
				<PropertyValue name="AutoFormatOnBraceCompletion">true</PropertyValue>
				<PropertyValue name="AutoFormatOnClosingBrace">true</PropertyValue>
				<PropertyValue name="CompleteParensInRawString">true</PropertyValue>
				<PropertyValue name="SpaceBeforeInheritanceColon">true</PropertyValue>
				<PropertyValue name="NewlineKeywordCatch">true</PropertyValue>
				<PropertyValue name="NewlineKeywordElse">true</PropertyValue>
				<PropertyValue name="AutoIndentOnTab">false</PropertyValue>
				<PropertyValue name="IndentCaseLabels">true</PropertyValue>
				<PropertyValue name="SpaceWithinLambdaBrackets">false</PropertyValue>
				<PropertyValue name="SpaceWithinBrackets">false</PropertyValue>
				<PropertyValue name="SpaceAfterComma">true</PropertyValue>
				<PropertyValue name="SpaceBetweenEmptyBraces">false</PropertyValue>
				<PropertyValue name="IndentGotoLabels">0</PropertyValue>
				<PropertyValue name="SpaceBeforeBracket">false</PropertyValue>
				<PropertyValue name="SpaceWithinFunctionParentheses">true</PropertyValue>
				<PropertyValue name="NewlineEmptyTypeCloseBrace">true</PropertyValue>
				<PropertyValue name="IndentationReference">2</PropertyValue>
			</ToolsOptionsSubCategory>
		</ToolsOptionsCategory>
	</ToolsOptions>
</UserSettings>
```