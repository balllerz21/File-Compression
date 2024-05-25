//
// STARTER CODE: util.h
//

/* Name: Maria Guallpa
 * Class: CS 251 Fall 2022
 * Project 6: File Compression
 * Machine Used: Clion on Mac
 * Date: 11/28/2022
 * */

#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <fstream>

typedef hashmap hashmapF;
typedef unordered_map <int, string> hashmapE;

struct HuffmanNode{
    int character;
    int count;
    HuffmanNode* zero;
    HuffmanNode* one;
};

struct compare
{
    bool operator()(const HuffmanNode *lhs,
                    const HuffmanNode *rhs)
    {
        return lhs->count > rhs->count;
    }
};

/*This function is used to recursively delete a tree's nodes */
void deletion(HuffmanNode *root)
{
    if (root == nullptr)
    {
        return;
    }
    deletion(root->zero);
    deletion(root->one);
    delete root;
}
//
// *This method frees the memory allocated for the Huffman tree.
//
void freeTree(HuffmanNode* node)
{
    deletion(node);
}

//
// *This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//
void buildFrequencyMap(string filename, bool isFile, hashmapF &map)
{
    char ch;
    int freq = 0;
    /*If the file is an ifstream, then the if statement below
     * is called */
    if (isFile == true)
    {
       fstream ifstream(filename, fstream::in);
       while (ifstream >> noskipws >> ch)
       {
           if (map.containsKey(ch))
           {
               freq = map.get(ch);
               freq++;
               map.put(ch, freq);
           }
           else
           {
               map.put(ch, 1);
           }
       }
    }

    /*If the file is a string, then the if statement below
     * is called */
    else if(isFile == false)
    {
        stringstream fin(filename);
        while (fin >> noskipws >> ch)
        {
            if (map.containsKey(ch))
            {
                freq = map.get(ch);
                freq++;
                map.put(ch, freq);
            }
            else
            {
                map.put(ch, 1);
            }
        }
    }
    map.put(PSEUDO_EOF,1);
}

//
// *This function builds an encoding tree from the frequency map.
//
HuffmanNode* buildEncodingTree(hashmapF &map)
{
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, compare> pq;
    //creates nodes from the map
    for (int ch : map.keys())
    {
        HuffmanNode *newNode = new HuffmanNode();
        newNode->character = ch;
        newNode->count = map.get(ch);
        newNode->one = nullptr;
        newNode->zero = nullptr;
        pq.push(newNode);
    }

    //puts the nides into a priority queue tree
    while (pq.size() > 1)
    {
        HuffmanNode* left = pq.top();
        pq.pop();
        HuffmanNode* right = pq.top();
        pq.pop();
        HuffmanNode* node = new HuffmanNode();
        node->character = NOT_A_CHAR;
        node->count = left->count + right->count;
        node->zero = left;
        node->one = right;
        pq.push(node);
    }

    return pq.top();
}

//
// *Recursive helper function for building the encoding map. It builds the map by adding the nodes into a mao
//
void _buildEncodingMap(HuffmanNode *node, hashmapE &encodingMap, string &str) {
    if (node->character != NOT_A_CHAR)
    {
        encodingMap[node->character] = str;
        return;
    }
    string zero = str + "0";
    string one = str + "1";
    _buildEncodingMap(node->zero, encodingMap, zero);
    _buildEncodingMap(node->one, encodingMap, one);
}

//
// *This function builds the encoding map from an encoding tree.
//
hashmapE buildEncodingMap(HuffmanNode* tree) {
    hashmapE encodingMap;
    string str = "";
    _buildEncodingMap(tree, encodingMap, str);
    return encodingMap;  // TO DO: update this return
}

//
// *This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream& input, hashmapE &encodingMap, ofbitstream& output, int &size, bool makeFile)
{
    char ch;
    string encodes;

    /*Goes through the ifstream and reads all characters*/
    while (input >> noskipws >> ch)
    {
        //gets the values AKA nums
        string str = encodingMap[ch];
        encodes += str;
        if (makeFile == true)
        {
            //turns the str value into bits
            for (auto chars: str)
            {
                if (chars == '0')
                {
                    output.writeBit(0);
                }
                else
                {
                    output.writeBit(1);
                }
                size++;
            }
        }
    }
    /*Since this only goes up until eof, eof must be done seperately*/
    string EOFs = encodingMap[PSEUDO_EOF];
    if (makeFile==true)
    {
        for (char chars: EOFs)
        {
            if (chars == '0')
            {
                output.writeBit(0);
            }
            else
            {
                output.writeBit(1);
            }
            size++;
        }
    }
    encodes += encodingMap[PSEUDO_EOF];
    return encodes;
}


//
// *This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output)
{
    string str = "";
    int bit = 0;
    HuffmanNode *curr = encodingTree;

    //reads ifstream
    while(!input.eof())
    {
        //reads bit and traverses tree to get characters to make words
        bit = input.readBit();
        if (bit == 0)
        {
            curr = curr->zero;
        }
        if (bit == 1)
        {
            curr = curr->one;
        }
        if (curr->character != PSEUDO_EOF && curr->character != NOT_A_CHAR)
        {
            str += char(curr->character);
            output.put(char(curr->character));
            curr = encodingTree;
        }
        if (curr->character == PSEUDO_EOF)
        {
            break;
        }
    }
    return str;  // TO DO: update this return
}

//
// *This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename)
{
    hashmap freqMap;
    int size;
    ifstream aFile(filename);

    buildFrequencyMap(filename, true, freqMap);
    HuffmanNode *root = buildEncodingTree(freqMap);
    hashmapE encodingMap = buildEncodingMap(root);

    filename = filename + ".huf";
    ofbitstream file(filename);
    file << freqMap;
    string result = encode(aFile, encodingMap, file, size, true);
    freeTree(root);
    return result;

}

//
// *This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note: this function should reverse what the compress
// function did.
//
string decompress(string filename)
{
    string secret = "";
    string newName = "";
    newName = filename.substr(0, filename.find("."));
    ifbitstream input(filename);

    newName = newName + "_unc.txt";
    ofstream output(newName);
    hashmap freqMap;
    input >> freqMap;
    HuffmanNode *tree = buildEncodingTree(freqMap);
    secret= decode(input, tree, output);
    freeTree(tree);
    return secret;
}

