/*
  Copyright (c) 2014 Pascal "Relax" Assens
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
 
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
 
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

/*
	Purpose :
	HandyJson is a very easy to use single class, based on cJSON, to parse and build JSON data.
*/
/*
	Thanks to Dave Gamble.
*/

#pragma once

#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	<stdlib.h>
#include	<math.h>
#include	<float.h>
#include	<limits.h>

class		HandyJson
{
	/* Json types */
public:
	enum	eTypes
	{
		json_false		=	0,
		json_true		=	1,
		json_null		=	2,
		json_number		=	3,
		json_string		=	4,
		json_array		=	5,
		json_object		=	6
	};

private:
	static const char*			sp_err;
	static const unsigned char	sp_firstByteMark[7];

private:
	HandyJson::eTypes	p_type;				// The type of the node (Look above).
	char*				p_name;				// The name of the node. Needed if the node is to be inserted in an object.
	HandyJson*			p_next;				// The following node.
	HandyJson*			p_prev;				// The previous node.
	HandyJson*			p_child;			// If type is json_array or json_object, there will be a child.
	char*				p_value_as_str;		// Value, if type is json_string.
	int					p_value_as_int;		// Value, if type is json_number.
	double				p_value_as_dbl;		// Value, if type is json_number.

public:
	HandyJson(void);
	HandyJson(HandyJson::eTypes);
	HandyJson(const HandyJson&);
	~HandyJson(void);

public:
	/* Basics getters */														//
	HandyJson::eTypes	GetType() const		{ return (this->p_type); }			//
	const char*			GetName() const		{ return (this->p_name); }			// Get stuff.
	HandyJson*			GetNext() const		{ return (this->p_next); }			//
	HandyJson*			GetPrev() const		{ return (this->p_prev); }			//
	HandyJson*			GetChild() const	{ return (this->p_child); }			//
	char*				GetValStr() const	{ return (this->p_value_as_str); }	//
	int					GetValInt() const	{ return (this->p_value_as_int); }	//
	double				GetValDbl() const	{ return (this->p_value_as_dbl); }	//

public:
	/* Basics setters */
	void				SetName(const char*);		//
	void				SetValStr(const char*);		// Set stuff.
	void				SetValInt(int);				//
	void				SetValDbl(double);			//

public:
	/* Main functions */
	bool				Parse(const char*);								// Build a HandyJson tree from a const char*.
	bool				ParseWithOpts(const char*, const char**, bool);	
	char*				Print();										// Build a char* from a HandyJson tree.
	char*				PrintUnformated();								// Same than Print() but does not format the output.

	/* Handling functions */
	HandyJson*			GetObjectItem(const char*) const;				// Get an item in an object, using its name.
	int					GetArraySize() const;							// Get the size of an array.
	HandyJson*			GetArrayItem(int) const;						// Get an item in an array, using index.
	bool				AddItemToArray(HandyJson*);						// Push back an item in an array.
	bool				AddItemToObject(const char*, HandyJson*);		// Push back an item in an object.
	HandyJson*			DetachItemFromArray(int);						// Detach an item from an array, using index.
	HandyJson*			DetachItemFromObject(const char*);				// Detach an item from an object, using its name.
	void				DeleteItemFromArray(int);						// Delete an item from an array, using index.
	void				DeleteItemFromObject(const char*);				// Delete an item from an object, using its name.
	void				ReplaceItemInArray(int, HandyJson*);			// Replace an item in an array, using index.
	void				ReplaceItemInObject(const char*, HandyJson*);	// Replace an item in an object, using its name.
	HandyJson*			Duplicate(bool);								// Duplicate the HandyJson value.

	/* Types functions */
	void				BuildInNull();					//
	void				BuildInTrue();					// Those fuctions are used to specify a node-type
	void				BuildInFalse();					// to a fresh and new HandyJson item.
	void				BuildInNumber(double);			//
	void				BuildInString(const char*);		// By default a HandyJson item is BuildInObject().
	void				BuildInArray();					//
	void				BuildInObject();				//

	/* Array building functions */
	void				BuildInIntArray(const int*, int);		//
	void				BuildInFltArray(const float*, int);		// Those functions are just made to make array
	void				BuildInDblArray(const double*, int);	// building easier and faster.
	void				BuildInStrArray(const char**, int);		//

	/* Error function */
	const char*			GetErrorPtr();	// This function is used to get the sp_err value which may be set after a fail.

private:
	/*
		+--------------------+
		| Internal functions |
		+--------------------+
								*/
	/* Parsing functions */
	const char*			ParseValue(const char*);	//
	const char*			ParseNumber(const char*);	// Those functions are used to parse a JSON data
	const char*			ParseString(const char*);	// and build a HandyJson structure. They all are
	const char*			ParseArray(const char*);	// called by the public function Parse().
	const char*			ParseObject(const char*);	//
	unsigned			ParseHex4(const char*);		//

	/* Printing functions */
	char*				PrintValue(int, int) const;			//
	char*				PrintNumber() const;				// Those functions are used to build a JSON data
	char*				PrintString() const;				// using de HandyJson structure. They all are
	char*				PrintStringPtr(const char*) const;	// called by the public function Print().
	char*				PrintArray(int, int) const;			//
	char*				PrintObject(int, int) const;		//

	/* Linking function */
	void				SuffixItem(HandyJson*);	// Used to make some links between items.

private:
	/* Some usefull functions */
	static const char*			Skip(const char*);
	static int					StrCaseCmp(const char*, const char*);
	static char*				StrDup(const char*);
};

