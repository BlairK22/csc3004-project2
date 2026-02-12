/* bibleajax.cpp
 * Computer Science, MVNU
 * CSC-3004 Introduction to Software Development
 *
 * Server-side CGI program for Bible verse retrieval via AJAX.
 *
 * STUDENT NAME: Blair Karamaga
 */

#include "Ref.h"
#include "Verse.h"
#include "Bible.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

// Decode URL-encoded characters (e.g., %20 -> space, + -> space)
string urlDecode(const string& str) {
    string result;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '+') {
            result += ' ';
        } else if (str[i] == '%' && i + 2 < str.length()) {
            string hex = str.substr(i + 1, 2);
            char ch = (char)strtol(hex.c_str(), NULL, 16);
            result += ch;
            i += 2;
        } else {
            result += str[i];
        }
    }
    return result;
}

// Parse a value from the query string given a key name
string getFormValue(const string& query, const string& key) {
    string searchKey = key + "=";
    size_t startPos = query.find(searchKey);
    if (startPos == string::npos) return "";
    // Make sure we match the full key, not a substring
    if (startPos > 0 && query[startPos - 1] != '&') {
        // Try finding it after an &
        startPos = query.find("&" + searchKey);
        if (startPos == string::npos) return "";
        startPos += 1; // skip the &
    }
    startPos += searchKey.length();
    size_t endPos = query.find("&", startPos);
    if (endPos == string::npos) endPos = query.length();
    return urlDecode(query.substr(startPos, endPos - startPos));
}

int main() {
    // Output the CGI header - MUST be the first output
    cout << "Content-Type: text/html\r\n\r\n";

    // Get the query string from the environment variable
    const char* queryEnv = getenv("QUERY_STRING");
    string queryString = "";
    if (queryEnv != NULL) queryString = queryEnv;

    // Parse input parameters from query string
    // The HTML form sends book as a number (1-66)
    string bookStr = getFormValue(queryString, "book");
    string chapterStr = getFormValue(queryString, "chapter");
    string verseStr = getFormValue(queryString, "verse");
    // The HTML form uses "num_verse" as the parameter name
    string numVersesStr = getFormValue(queryString, "num_verse");

    // Validate that we received input
    if (bookStr.empty() || chapterStr.empty() || verseStr.empty()) {
        cout << "<p><b>Error:</b> Please enter a book, chapter, and verse.</p>";
        return 0;
    }

    // Convert book to number - the HTML dropdown sends the book number directly
    int bookNum = atoi(bookStr.c_str());
    if (bookNum < 1 || bookNum > 66) {
        cout << "<p><b>Error:</b> Invalid book number: " << bookStr << "</p>";
        return 0;
    }

    // Convert chapter and verse strings to integers
    int chapterNum = atoi(chapterStr.c_str());
    int verseNum = atoi(verseStr.c_str());
    int numVerses = 1;
    if (!numVersesStr.empty()) {
        numVerses = atoi(numVersesStr.c_str());
        if (numVerses < 1) numVerses = 1;
    }

    // Create Bible object with the path to the Bible text file on the server
    Bible bible("/home/class/csc3004/Bibles/web-complete");

    // Create a Ref from the parsed book number, chapter, and verse
    Ref ref(bookNum, chapterNum, verseNum);

    // Look up the verse
    LookupResult result;
    Verse verse = bible.lookup(ref, result);

    // Output the result
    if (result == SUCCESS) {
        Ref verseRef = verse.getRef();
        int currentChapter = verseRef.getChapter();
        int currentBook = verseRef.getBook();

        // Output book name and chapter heading
        cout << "<b>" << verseRef.getBookName() << " " << currentChapter << "</b><br>";

        // Output verse number and text
        cout << verseRef.getVerse() << " " << verse.getVerse() << "<br>";

        // Retrieve additional verses if requested
        for (int i = 1; i < numVerses; i++) {
            verse = bible.nextVerse(result);
            if (result == SUCCESS) {
                verseRef = verse.getRef();
                if (verseRef.getBook() != currentBook) break;
                if (verseRef.getChapter() != currentChapter) {
                    currentChapter = verseRef.getChapter();
                    cout << "<br><b>" << verseRef.getBookName() << " " << currentChapter << "</b><br>";
                }
                cout << verseRef.getVerse() << " " << verse.getVerse() << "<br>";
            } else {
                break;
            }
        }
    } else {
        cout << "<p><b>Error:</b> " << bible.error(result) << "</p>";
    }

    return 0;
}
