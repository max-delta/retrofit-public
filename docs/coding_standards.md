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

# The rules

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

Namespace: lowerCamelCase (Note that the super-namespace 'RF' is an exception)

Function parameter: lowerCamelCase

Local variables: lowerCamelCase

Defines/macros: UPPER\_SNAKE\_CASE

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

# Personal taste

Where not specified, style is left to the discretion of the owner of a body of code. When working in code that you do not own, it is expected that you will generally follow the style of the owner, within reason.

Note that 'ownership' is a vague concept, so you will need to exercise a balance of responsibility and respectfulness.

# General code safety

You may wish to peruse the following:

Stroustrup & Sutter: [https://github.com/isocpp/CppCoreGuidelines]

# Autoformat

While no one should ever be running bulk auto-formats on code they are not the owner of for style elements not covered in the coding standards, minor code changes and additions may cause stylistic divergences with a modern IDE. These should be expected to creep in from time-to-time when collaborating with others.

If you are not particularly passionate about minor whitespace nuances, but want to be consistent, you may wish to use the following auto-format vs2017.vssettings import to better match the general engine style:

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