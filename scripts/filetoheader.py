import argparse
import os
import math
import re

def parseArgs():
	parser = argparse.ArgumentParser("filetoheader",
									 description="Converts a binary file to a C header representing the data.")

	parser.add_argument("-i", "--input",
						required=True,
						help="Input file.")

	parser.add_argument("-o", "--output",
						required=True,
						help="Output file name.")

	parser.add_argument("-n", "--name",
						required=True,
						help="Prefix applied to the variable names.")

	return parser.parse_args()

def createCHeaderFile(inData, name):
	upperName = name.upper()

	preLines = \
	[
		"#pragma once\n",
		"\n",
		"#include <stdint.h>\n",
		"#include <stddef.h>\n",
		"\n",
		f"#define {upperName}_LENGTH ((size_t){len(inData)})\n"
		f"extern const uint8_t {name}_Data[{upperName}_LENGTH];\n",
		"\n",
		f"#ifdef {upperName}_IMPL\n",
		f"const uint8_t {name}_Data[{upperName}_LENGTH] =\n",
		"{\n"
	]

	dataLines = []

	for i in range(0, len(inData)):
		line = ""

		if i == 0:
			# First line, so provide indent.
			line += "\t"
		elif (i % 8) == 0:
			# Need comma, new line + indent.
			line += ",\n\t"
		else:
			# Need comma only.
			line += ", "

		value = inData[i]
		line += f"{value:#04x}"

		if i == len(inData) - 1:
			line += "\n"

		dataLines.append(line)

	postLines = \
	[
		"};\n",
		f"#endif // {upperName}_IMPL\n"
	]

	return "".join(preLines + dataLines + postLines)

def main():
	args = parseArgs()
	fileContent = []

	with open(args.input, "rb") as inFile:
		fileContent = inFile.read()

	print("Read file:", args.input, f"({len(fileContent)} bytes)")

	headerData = createCHeaderFile(fileContent, args.name)

	print("Writing header:", args.output)
	with open(args.output, "w") as outFile:
		outFile.write(headerData)

	print("Done.")

if __name__ == "__main__":
	main()
