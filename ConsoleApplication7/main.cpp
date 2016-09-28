#include <fstream>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <ctime>

using std::string;
using std::cout;
using std::cin;
using std::endl;

int countWords(string &str);
bool isDigit(string &str);

struct MisspelledWord {
	int startIndex, length;
	string error, word;

	MisspelledWord(int startIndex, int length, string error, string word) {
		this->startIndex = startIndex;
		this->length = length;
		this->error = error;
		this->word = word;
	}
};

int main() {
	std::multimap<string, string> dictionary;
	std::vector<MisspelledWord> wordBuffer;
	string mainText = "";
	int maxErrors = 0, maxWords = 0, numErrors = 0, startIndex = 0, offset = 0;

	srand(time(NULL));

	// Read in misspellings dictionary from text file
	std::ifstream myfile("misspellings.txt");
	if (myfile.is_open())
	{
		string line;
		while (getline(myfile, line))
		{
			string::iterator iter;
			int index = 0, startIndex = 0;
			string misspelling;
			string originalWord;
			for (iter = line.begin(); iter < line.end(); ++iter, ++index)
			{
				// Parse raw text pulled from https://en.wikipedia.org/wiki/Wikipedia:Lists_of_common_misspellings
				// FIXME - Fix lines that have multiple word inside parenthesis
				if (line[index] == '(') {
					misspelling = line.substr(0, index - 1);
					startIndex = index + 1;
					if (line.substr(startIndex, 10) == "variant of") {
						startIndex += 11;
						index += 11;
						continue;
					}
				}
				if (line[index] == ')' || line[index] == '[' || line[index] == ',') {
					originalWord = line.substr(startIndex, index - startIndex);
					dictionary.insert(std::pair<string, string>(originalWord, misspelling));
					break;
				}
			}
		}
		myfile.close();
	}
	else cout << "Unable to open file";

	cout << "Please input text:\n";
	std::getline(std::cin, mainText);
	maxWords = countWords(mainText);

	// Prompt for number of spelling errors to introduce
	cout << "\nNumber of words to change? ";
	cin >> maxErrors;

	// Ensure value is within the number of words in the text
	while (maxErrors > maxWords || maxErrors < 0) {
		cout << "Please enter a value between 0 and " << maxWords << ": ";
		cin >> maxErrors;
	}

	// Look for words that are found in the dictionary and use those misspellings
	// before adding random errors
	for (int i = 0; i < mainText.size(); i++) {
		if (i < mainText.length() - 1) {
			// Check if end of word has been reached
			if (!isalpha(mainText[i])) {
				// Check first to see if max number of errors have already been included
				if (numErrors < maxErrors)
				{
					// Create substring of current word
					string word = mainText.substr(startIndex, i - startIndex);
					string lowerCase = word;
					std::transform(lowerCase.begin(), lowerCase.end(), lowerCase.begin(), tolower);
					// Check if word is in misspellings dictionary
					if (dictionary.find(lowerCase) != dictionary.end()) {
						// Add word to word buffer to be used later to randomly add back in throughout text
						string misspelled = dictionary.find(lowerCase)->second;
						if (isupper(word[0])) { misspelled[0] = toupper(misspelled[0]); }
						offset += (misspelled.length() - word.length());
						if (startIndex + offset < 0) {
							MisspelledWord tmpWord(startIndex, i - startIndex, misspelled, word);
							wordBuffer.push_back(tmpWord);
						}
						else {
							MisspelledWord tmpWord(startIndex + offset, i - startIndex + offset, misspelled, word);
							wordBuffer.push_back(tmpWord);
						}
						//mainText.replace(startIndex, i - startIndex, misspelled);
						//numErrors++;
					}
				}
				// Move past whitespace and punctuation to next word
				while (!isalpha(mainText[i])) { 
					i++; 
				}
				startIndex = i;
			}
		}
	}

	std::random_shuffle(wordBuffer.begin(), wordBuffer.end());
	//cout << "DEBUG: Found " << wordBuffer.size() << " misspellings." << endl;

	// Randomly add words from misspellings list first
	for (int i = 0; i < wordBuffer.size(); i++) {
		if (numErrors < maxErrors) {
			//mainText.replace(wordBuffer[i].startIndex, wordBuffer[i].length, wordBuffer[i].word);
			int ind = mainText.find(wordBuffer[i].word, 0);
			if (ind >= 0) {
				mainText.replace(ind, wordBuffer[i].word.size(), wordBuffer[i].error);
				numErrors++;
			}
			else {
				//cout << "DEBUG: Could not find " << wordBuffer[i].word << endl;
			}
		}
	}

	// Add random spelling errors after to reach desired number of errors
	while (numErrors < maxErrors) {
		int randIndex = rand() % mainText.length();
		if (rand() % 2) 
		{
			if (isalpha(mainText[randIndex])) {
				// Randomly change one letter to a random letter 
				mainText[randIndex] = 'a' + rand() % 26;
				numErrors++;
			}
		}
		else 
		{
			if (isalpha(mainText[randIndex])) {
				// Duplicate a letter
				string letter(1, mainText[randIndex]);
				mainText.insert(randIndex, letter);
				numErrors++;
			}
		}
	}

	cout << endl << "Revised:\n" << mainText << endl << endl;
}

int countWords(string &str)
{
	int numWords = 0;
	bool isSpace = true;

	for (int i = 0; i < str.length(); i++) {
		if (isspace(str[i])) {
			isSpace = true;
		}
		else if (isSpace) {
			numWords++;
			isSpace = false;
		}
	}

	return numWords;
}

bool isDigit(string &str) {
	for (int i = 0; i < str.length(); i++) {
		if (!isdigit(str[i])) {
			return false;
		}
	}
	return true;
}