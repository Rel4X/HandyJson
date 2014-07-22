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

/* Version 1.0 */

#include				"HandyJson.h"

const char*				HandyJson::sp_err = 0x0;

const unsigned char		HandyJson::sp_firstByteMark[7] = {
	0x00,
	0x00,
	0xC0,
	0xE0,
	0xF0,
	0xF8,
	0xFC
};

HandyJson::HandyJson(void) :
	p_name(0), p_next(0), p_prev(0), p_child(0), p_value_as_str(0), p_value_as_int(0), p_value_as_dbl(0)
{
	this->BuildInObject();
}

HandyJson::HandyJson(HandyJson::eTypes t) :
	p_name(0), p_next(0), p_prev(0), p_child(0), p_value_as_str(0), p_value_as_int(0), p_value_as_dbl(0)
{
	this->p_type = t;
}

HandyJson::HandyJson(const HandyJson& hj)
{
	this->p_type = hj.GetType();
	this->p_name = hj.StrDup(hj.GetName());
	this->p_next = hj.GetNext();
	this->p_prev = hj.GetPrev();
	this->p_child = hj.GetChild();
	this->p_value_as_str = hj.StrDup(hj.GetValStr());
	this->p_value_as_int = hj.GetValInt();
	this->p_value_as_dbl = hj.GetValDbl();
}

HandyJson::~HandyJson(void)
{
	if (this->p_child) delete (this->p_child);	// Think about setting null pointers.
	if (this->p_value_as_str) delete (this->p_value_as_str);
	if (this->GetName()) delete (this->p_name);
	if (this->GetNext())
		delete (this->GetNext());
}

/* Basics setters */
bool			HandyJson::SetName(const char* n)
{
	if (!n)
		return (false);
	if (this->GetName())
	{
		delete (this->p_name);
		this->p_name = this->StrDup(n);
		if (this->p_name)
			return (true);
	}
	return (false);
}

bool			HandyJson::SetValStr(const char* s)
{
	if (!s)
		return (false);
	if (this->GetValStr())
	{
		delete (this->GetValStr());
		this->p_value_as_str = this->StrDup(s);
		if (this->p_value_as_str)
			return (true);
	}
	return (false);
}

bool			HandyJson::SetValInt(int i)
{
	this->p_value_as_int = i;
	return (true);
}

bool			HandyJson::SetValDbl(double d)
{
	this->p_value_as_dbl = d;
	return (true);
}

/* Main functions */
int				HandyJson::GetArraySize() const
{
	HandyJson *c = this->GetChild();;
	
	int i = 0;
	while (c)
	{
		i++;
		c = c->GetNext();
	}
	return (i);
}

HandyJson*		HandyJson::GetArrayItem(int item) const
{
	HandyJson *c = this->GetChild();
	
	while (c && item > 0)
	{
		c = c->GetNext();
		--item;
	}
	return (c);
}

HandyJson*		HandyJson::GetObjectItem(const char* string) const
{
	HandyJson* c = this->GetChild();
	
	while (c && this->StrCaseCmp(c->GetName(), string))
		c = c->GetNext();
	return (c);
}

bool			HandyJson::AddItemToArray(HandyJson* item)
{
	HandyJson* c = this->GetChild();

	if (this->GetType() != HandyJson::eTypes::json_array)
		return (false);
	if (!item)
		return (false);
	if (!c)
		this->p_child = item;
	else
	{
		while (c && c->GetNext())
			c = c->GetNext(); 
		c->SuffixItem(item);
	}
	return (true);
}

bool			HandyJson::AddItemToObject(const char* string, HandyJson* item)
{
	if (this->GetType() != HandyJson::eTypes::json_object)
		return (false);
	if (!item)
		return (false); 
	if (item->GetName())
		delete (item->GetName());
	item->p_name = this->StrDup(string);
	
	HandyJson* c = this->GetChild();

	if (!item)
		return (false);
	if (!c)
		this->p_child = item;
	else
	{
		while (c && c->GetNext())
			c = c->GetNext(); 
		c->SuffixItem(item);
	}
	return (true);
}

HandyJson*		HandyJson::DetachItemFromArray(int which)
{
	HandyJson* c = this->GetChild();
	
	while (c && which > 0)
	{
		c = c->GetNext();
		--which;
	}
	if (!c) return (0);
	if (c->GetPrev()) c->p_prev->p_next = c->GetNext();
	if (c->GetNext()) c->p_next->p_prev = c->GetPrev();
	if (c == this->p_child) this->p_child = c->p_next;
	c->p_prev = c->p_next = 0;
	return (c);
}

void			HandyJson::DeleteItemFromArray(int which)
{
	delete (this->DetachItemFromArray(which));
}

HandyJson*		HandyJson::DetachItemFromObject(const char* string)
{
	int i = 0;
	HandyJson* c = this->GetChild();
	while (c && this->StrCaseCmp(c->GetName(), string)) i++, c = c->p_next;
	if (c)
		return (this->DetachItemFromArray(i));
	return 0;
}

void			HandyJson::DeleteItemFromObject(const char* string)
{
	delete (this->DetachItemFromObject(string));
}

void			HandyJson::ReplaceItemInArray(int which, HandyJson* newitem)
{
	HandyJson* c = this->GetChild();

	while (c && which > 0)
	{
		c = c->GetNext();
		--which;
	}
	if (!c)
		return;
	newitem->p_next = c->GetNext();
	newitem->p_prev = c->GetPrev();
	if (newitem->GetNext()) newitem->p_next->p_prev = newitem;
	if (c == this->GetChild())
		this->p_child = newitem;
	else
		newitem->p_prev->p_next = newitem;
	c->p_next = 0;
	c->p_prev = 0;
	delete (c);
}

void			HandyJson::ReplaceItemInObject(const char* string, HandyJson* newitem)
{
	HandyJson* c = this->GetChild();
	int i = 0;

	while (c && this->StrCaseCmp(c->GetName(), string))
	{
		c = c->GetNext();
		++i;
	}
	if (c)
	{
		newitem->p_name = this->StrDup(string);
		this->ReplaceItemInArray(i, newitem);
	}
}

HandyJson*		HandyJson::Duplicate(bool recurse)
{
	HandyJson *newitem, *cptr, *nptr = 0, *newchild;

	newitem = new HandyJson();
	if (!newitem)
		return (0);
	newitem->p_type = this->GetType();
	newitem->p_value_as_int = this->GetValInt();
	newitem->p_value_as_dbl = this->GetValDbl();
	if (this->GetValStr())
	{
		newitem->p_value_as_str = this->StrDup(this->GetValStr());
		if (!newitem->GetValStr())
		{
			delete (newitem);
			return (0);
		}
	}
	if (this->GetName())
	{
		newitem->p_name = this->StrDup(this->GetName());
		if (!newitem->GetName())
		{
			delete (newitem);
			return (0);
		}
	}

	if (recurse == false)
		return (newitem);

	cptr = this->GetChild();
	while (cptr)
	{
		newchild = cptr->Duplicate(true);
		if (!newchild)
		{
			delete (newitem);
			return (0);
		}
		if (nptr)
		{
			nptr->p_next = newchild;
			newchild->p_prev = nptr;
			nptr = newchild;
		}
		else
		{
			newitem->p_child = newchild;
			nptr = newchild;
		}
		cptr = cptr->GetNext();
	}
	return (newitem);
}

bool			HandyJson::ParseWithOpts(const char* value, const char** return_parse_end, bool require_null_terminated)
{
	const char* end = 0;
	
	HandyJson::sp_err = 0;

	end = this->ParseValue(this->Skip(value));
	if (!end)
		return (false);

	if (require_null_terminated == true) 
	{
		end = this->Skip(end);
		if (*end)
		{
			HandyJson::sp_err = end;
			return (false);
		}
	}
	if (return_parse_end)
		*return_parse_end = end;
	return (true);
}

bool			HandyJson::Parse(const char* value)
{
	return (this->ParseWithOpts(value, 0, false));
}

char*			HandyJson::Print()
{
	return (this->PrintValue(0, 1));
}

char*			HandyJson::PrintUnformated()
{
	return (this->PrintValue(0, 0));
}

/* Types functions */
void			HandyJson::BuildInNull()
{
	this->p_type = HandyJson::eTypes::json_null;
}

void			HandyJson::BuildInTrue()
{
	this->p_type = HandyJson::eTypes::json_true;
}

void			HandyJson::BuildInFalse()
{
	this->p_type = HandyJson::eTypes::json_false;
}

void			HandyJson::BuildInNumber(double number)
{
	this->p_type = HandyJson::eTypes::json_number;
	this->p_value_as_dbl = number;
	this->p_value_as_int = (int)number;
}

void			HandyJson::BuildInString(const char* string)
{
	this->p_type = HandyJson::eTypes::json_string;
	this->p_value_as_str = this->StrDup(string);
}

void			HandyJson::BuildInArray()
{
	this->p_type = HandyJson::eTypes::json_array;
}

void			HandyJson::BuildInObject()
{
	this->p_type = HandyJson::eTypes::json_object;
}

/* Arrays building functions */
void			HandyJson::BuildInIntArray(const int* numbers, int count)
{
	int			i;

	HandyJson* n = 0;
	HandyJson* p = 0;
	this->BuildInArray();
	for (i = 0; i < count; ++i)
	{
		n = new HandyJson();
		if (!n)
			return ;
		n->BuildInNumber(numbers[i]);
		if (!i)
			this->p_child = n;
		else
			p->SuffixItem(n);
		p = n;
	}
}

void			HandyJson::BuildInFltArray(const float* numbers, int count)
{
	int			i;

	HandyJson* n = 0;
	HandyJson* p = 0;
	this->BuildInArray();
	for (i = 0; i < count; ++i)
	{
		n = new HandyJson();
		if (!n)
			return ;
		n->BuildInNumber(numbers[i]);
		if (!i)
			this->p_child = n;
		else
			p->SuffixItem(n);
		p = n;
	}
}

void			HandyJson::BuildInDblArray(const double* numbers, int count)
{
	int			i;

	HandyJson* n = 0;
	HandyJson* p = 0;
	this->BuildInArray();
	for (i = 0; i < count; ++i)
	{
		n = new HandyJson();
		if (!n)
			return ;
		n->BuildInNumber(numbers[i]);
		if (!i)
			this->p_child = n;
		else
			p->SuffixItem(n);
		p = n;
	}
}

void			HandyJson::BuildInStrArray(const char** strings, int count)
{
	int			i;

	HandyJson* n = 0;
	HandyJson* p = 0;
	this->BuildInArray();
	for (i = 0; i < count; ++i)
	{
		n = new HandyJson();
		if (!n)
			return ;
		n->BuildInString(strings[i]);
		if (!i)
			this->p_child = n;
		else
			p->SuffixItem(n);
		p = n;
	}
}

/* Internal functions */
const char*		HandyJson::GetErrorPtr(void)
{
	return (HandyJson::sp_err);
}

const char*		HandyJson::ParseNumber(const char* num)
{
	double		n = 0;
	double		sign = 1;
	double		scale = 0;
	int			subscale = 0;
	int			signsubscale = 1;

	if (*num == '-')	{ sign = -1; ++num; }
	if (*num == '0')	{ ++num; }
	if (*num >= '1' && *num <= '9')
	{
		do
		{
			n = (n * 10.0) + (*num++ - '0');
		} while (*num >= '0' && *num <= '9');
	}
	if (*num=='.' && num[1] >= '0' && num[1] <= '9')
	{
		num++;
		do
		{
			n = (n * 10.0) + (*num++ - '0');
			scale--;
		} while (*num >= '0' && *num <= '9');
	}
	if (*num == 'e' || *num == 'E')
	{
		num++;
		if (*num == '+')
			num++;
		else if (*num=='-')
		{
			signsubscale = -1;
			num++;
		}
		while (*num >= '0' && *num <= '9') subscale = (subscale * 10) + (*num++ - '0');
	}
	n = sign * n * pow(10.0, (scale + subscale * signsubscale));
	this->p_value_as_int = (int)n;
	this->p_value_as_dbl = n;
	this->p_type = HandyJson::eTypes::json_number;
	return (num);
}

char*			HandyJson::PrintNumber() const
{
	char		*str;
	double		d = this->p_value_as_dbl;

	if (fabs(((double)this->p_value_as_int)-d) <= DBL_EPSILON && d <= INT_MAX && d >= INT_MIN)
	{
		str = new char[21]();
		if (str) sprintf(str, "%d", this->p_value_as_int);
	}
	else
	{
		str = new char[64]();
		if (str)
		{
			if (fabs(floor(d) - d) <= DBL_EPSILON && fabs(d) < 1.0e60) sprintf(str, "%.0f", d);
			else if (fabs(d) < 1.0e-6 || fabs(d) > 1.0e9) sprintf(str, "%e", d);
			else sprintf(str, "%f", d);
		}
	}
	return (str);
}

unsigned		HandyJson::ParseHex4(const char* str)
{
	unsigned h = 0;

	if (*str >= '0' && *str <= '9') h += (*str) - '0';
	else if (*str >= 'A' && *str <= 'F') h += 10 + (*str) - 'A';
	else if (*str >= 'a' && *str <= 'f') h += 10 + (*str) - 'a';
	else return (0);

	h = h << 4;
	++str;

	if (*str >= '0' && *str <= '9') h += (*str) - '0';
	else if (*str >= 'A' && *str <= 'F') h += 10 + (*str) - 'A';
	else if (*str >= 'a' && *str <= 'f') h += 10 + (*str) - 'a';
	else return (0);

	h = h << 4;
	++str;

	if (*str >= '0' && *str <= '9') h += (*str) - '0';
	else if (*str >= 'A' && *str <= 'F') h += 10 + (*str) - 'A';
	else if (*str >= 'a' && *str <= 'f') h += 10 + (*str) - 'a';
	else return (0);

	h = h << 4;
	++str;
	
	if (*str >= '0' && *str <= '9') h += (*str) - '0';
	else if (*str >= 'A' && *str <= 'F') h += 10 + (*str) - 'A';
	else if (*str >= 'a' && *str <= 'f') h += 10 + (*str) - 'a';
	else return (0);

	return (h);
}

const char*		HandyJson::ParseString(const char* str)
{
	const char*	ptr = str + 1;
	char*		ptr2;
	char*		out;
	int			len = 0;
	unsigned	uc,uc2;

	if (*str!='\"')	{ HandyJson::sp_err = str; return (0); }
	
	while (*ptr != '\"' && *ptr && ++len) if (*ptr++ == '\\') ptr++;
	
	out = new char[len + 1]();
	if (!out) return (0);
	
	ptr = str + 1;
	ptr2 = out;
	while (*ptr!='\"' && *ptr)
	{
		if (*ptr != '\\') *ptr2++ = *ptr++;
		else
		{
			ptr++;
			switch (*ptr)
			{
				case 'b': *ptr2++='\b';	break;
				case 'f': *ptr2++='\f';	break;
				case 'n': *ptr2++='\n';	break;
				case 'r': *ptr2++='\r';	break;
				case 't': *ptr2++='\t';	break;
				case 'u':
					uc = this->ParseHex4(ptr + 1); ptr += 4;
					if ((uc >= 0xDC00 && uc <= 0xDFFF) || uc == 0) break;
					if (uc >= 0xD800 && uc <= 0xDBFF)
					{
						if (ptr[1] != '\\' || ptr[2] != 'u') break;
						uc2 = this->ParseHex4(ptr+3); ptr += 6;
						if (uc2 < 0xDC00 || uc2 > 0xDFFF) break;
						uc = 0x10000 + (((uc & 0x3FF) << 10) | (uc2 & 0x3FF));
					}
					len = 4;
					if (uc < 0x80) len = 1;
					else if (uc < 0x800) len = 2;
					else if (uc < 0x10000) len = 3;
					ptr2 += len;
					
					switch (len) {
						case 4: *--ptr2 = ((uc | 0x80) & 0xBF); uc >>= 6;
						case 3: *--ptr2 = ((uc | 0x80) & 0xBF); uc >>= 6;
						case 2: *--ptr2 = ((uc | 0x80) & 0xBF); uc >>= 6;
						case 1: *--ptr2 = (uc | HandyJson::sp_firstByteMark[len]);
					}
					ptr2 += len;
					break;
				default: 
					*ptr2++ = *ptr; 
					break;
			}
			ptr++;
		}
	}
	*ptr2 = 0;
	if (*ptr=='\"') ptr++;
	this->p_value_as_str = out;
	this->p_type = HandyJson::eTypes::json_string;
	return (ptr);
}

char*			HandyJson::PrintStringPtr(const char* str) const
{
	const char*		ptr;
	char*			ptr2, *out;
	int				len = 0;
	unsigned char	token;
	
	if (!str) return (0);
	ptr = str;
	while ((token = *ptr) && ++len) 
	{
		if (strchr("\"\\\b\f\n\r\t", token))
			len++;
		else if (token < 32) 
			len += 5;
		ptr++;
	}
	out = new char[len + 3]();
	if (!out) return (0);

	ptr2 = out;
	ptr = str;
	*ptr2++ = '\"';
	while (*ptr)
	{
		if ((unsigned char)*ptr > 31 && *ptr != '\"' && *ptr != '\\')
			*ptr2++ = *ptr++;
		else
		{
			*ptr2++ = '\\';
			switch (token = *ptr++)
			{
				case '\\':	*ptr2++='\\';	break;
				case '\"':	*ptr2++='\"';	break;
				case '\b':	*ptr2++='b';	break;
				case '\f':	*ptr2++='f';	break;
				case '\n':	*ptr2++='n';	break;
				case '\r':	*ptr2++='r';	break;
				case '\t':	*ptr2++='t';	break;
				default:
					sprintf(ptr2, "u%04x", token);
					ptr2 += 5;
					break;
			}
		}
	}
	*ptr2++  ='\"';
	*ptr2++ = 0;
	return (out);
}

char*			HandyJson::PrintString() const
{
	return (this->PrintStringPtr(this->p_value_as_str));
}

const char*		HandyJson::ParseValue(const char* value)
{
	if (!value)							return (0);
	if (!strncmp(value, "null", 4))		{ this->p_type = HandyJson::eTypes::json_null; return (value + 4); }
	if (!strncmp(value, "false", 5))	{ this->p_type = HandyJson::eTypes::json_false; return (value + 5); }
	if (!strncmp(value, "true", 4))		{ this->p_type = HandyJson::eTypes::json_true; this->p_value_as_int = 1; return (value + 4); }
	if (*value == '\"')					{ return (this->ParseString(value)); }
	if (*value == '-' || (*value >= '0' && *value <= '9')) { return (this->ParseNumber(value)); }
	if (*value == '[')					{ return (this->ParseArray(value)); }
	if (*value == '{')					{ return (this->ParseObject(value)); }

	HandyJson::sp_err = value;
	return (0);
}

char*			HandyJson::PrintValue(int depth, int fmt) const
{
	char*		out = 0;
	switch (this->GetType())
	{
	case HandyJson::eTypes::json_null	:		out = this->StrDup("null");	break;
	case HandyJson::eTypes::json_false	:		out = this->StrDup("false"); break;
	case HandyJson::eTypes::json_true	:		out = this->StrDup("true"); break;
	case HandyJson::eTypes::json_number	:		out = this->PrintNumber(); break;
	case HandyJson::eTypes::json_string	:		out = this->PrintString(); break;
	case HandyJson::eTypes::json_array	:		out = this->PrintArray(depth, fmt); break;
	case HandyJson::eTypes::json_object	:		out = this->PrintObject(depth, fmt); break;
	}
	return (out);
}

const char*		HandyJson::ParseArray(const char* value)
{
	HandyJson*	child;

	if (*value != '[')
	{
		HandyJson::sp_err = value;
		return (0);
	}
	this->p_type = HandyJson::eTypes::json_array;
	value = this->Skip(value + 1);
	if (*value == ']')
		return (value + 1);

	this->p_child = child = new HandyJson();
	if (!this->p_child)
		return (0);
	value = this->Skip(child->ParseValue(this->Skip(value)));
	if (!value)
		return (0);
	while (*value == ',')
	{
		HandyJson *new_item;

		if (!(new_item = new HandyJson()))
			return (0);
		child->p_next = new_item;
		new_item->p_prev = child;
		child = new_item;
		value = this->Skip(child->ParseValue(this->Skip(value + 1)));
		if (!value)
			return (0);
	}
	if (*value == ']')
		return (value + 1);
	HandyJson::sp_err = value;
	return (0);
}

char*			HandyJson::PrintArray(int depth, int fmt) const
{
	char**		entries;
	char*		out = 0, *ptr, *ret;
	int			len = 5;
	HandyJson*	child = this->GetChild();
	int			numentries = 0, i = 0, fail = 0;
	
	while (child) ++numentries, child = child->p_next;
	if (!numentries)
	{
		out = new char[3]();
		if (out) strcpy(out, "[]");
		out[2] = 0;
		return (out);
	}
	entries = new char*[numentries * sizeof(char*)]();
	if (!entries) return (0);
	memset(entries, 0, numentries * sizeof(char*));
	child = this->GetChild();
	while (child && !fail)
	{
		ret = child->PrintValue(depth + 1, fmt);
		entries[i++] = ret;
		if (ret) len += strlen(ret) + 2 + (fmt ? 1 : 0);
		else fail = 1;
		child = child->GetNext();
	}
	if (!fail) out = new char[len]();
	if (!out) fail = 1;

	if (fail)
	{
		for (i = 0; i < numentries; ++i) if (entries[i]) delete (entries[i]);
		delete (entries);
		return (0);
	}
	
	*out = '[';
	ptr = out + 1;
	*ptr = 0;
	for (i = 0; i < numentries; ++i)
	{
		strcpy(ptr, entries[i]);
		ptr += strlen(entries[i]);
		if (i != numentries - 1)
		{
			*ptr++ =',';
			if (fmt)
				*ptr++ = ' ';
			*ptr = 0;
		}
		delete (entries[i]);
	}
	delete (entries);
	*ptr++ = ']';
	*ptr++ = 0;
	return (out);
}

const char*		HandyJson::ParseObject(const char* value)
{
	HandyJson*	child;

	if (*value != '{')	{ HandyJson::sp_err = value; return (0); }
	
	this->p_type = HandyJson::eTypes::json_object;
	value = this->Skip(value + 1);
	if (*value == '}')
		return (value + 1);
	
	this->p_child = child = new HandyJson();
	if (!this->GetChild()) return (0);
	value = this->Skip(child->ParseString(this->Skip(value)));
	if (!value)
		return (0);
	child->p_name = child->p_value_as_str;
	child->p_value_as_str = 0;
	if (*value!=':') { HandyJson::sp_err = value; return (0); }
	value = this->Skip(child->ParseValue(this->Skip(value + 1)));
	if (!value) 
		return (0);
	
	while (*value==',')
	{
		HandyJson *new_item;

		if (!(new_item = new HandyJson())) return (0);
		child->p_next = new_item;
		new_item->p_prev = child;
		child = new_item;
		value = this->Skip(child->ParseValue(this->Skip(value + 1)));
		if (!value)
			return (0);
		child->p_name = child->p_value_as_str;
		child->p_value_as_str = 0;
		if (*value!=':') { HandyJson::sp_err = value; return (0); }
		value = this->Skip(child->ParseValue(this->Skip(value + 1)));
		if (!value)
			return (0);
	}
	
	if (*value == '}')
		return (value + 1);
	HandyJson::sp_err = value;
	return (0);
}

char*			HandyJson::PrintObject(int depth, int fmt) const
{
	char**		entries = 0, **names = 0;
	char*		out = 0, *ptr, *ret, *str;
	int			len = 7, i = 0, j;
	HandyJson*	child = this->GetChild();
	int			numentries = 0, fail = 0;

	while (child) numentries++, child = child->GetNext();
	if (!numentries)
	{
		out = new char[fmt ? depth + 4 : 3]();
		if (!out)
			return (0);
		ptr = out;
		*ptr++ = '{';
		if (fmt)
		{ 
			*ptr++ = '\n';
			for (i = 0; i < depth - 1; ++i)
				*ptr++ = '\t';
		}
		*ptr++ = '}';
		*ptr++ = 0;
		return (out);
	}
	entries = new char*[numentries * sizeof(char*)]();
	if (!entries) return (0);
	names = new char*[numentries * sizeof(char*)]();
	if (!names)
	{ 
		delete (entries); 
		return (0); 
	}
	memset(entries, 0, sizeof(char*) * numentries);
	memset(names, 0, sizeof(char*) * numentries);
	
	child = this->GetChild();
	++depth;
	if (fmt)
		len += depth;
	while (child)
	{
		names[i] = str = child->PrintStringPtr(child->GetName());
		entries[i++] = ret = child->PrintValue(depth, fmt);
		if (str && ret)
			len += strlen(ret) + strlen(str) + 2 + (fmt ? 2 + depth : 0);
		else
			fail = 1;
		child = child->GetNext();
	}
	
	if (!fail) out = new char[len]();
	if (!out) fail = 1;

	if (fail)
	{
		for (i = 0; i < numentries; ++i)
		{
			if (names[i])
				delete (names[i]);
			if (entries[i])
				delete (entries[i]);
		}
		delete (names);
		delete (entries);
		return (0);
	}
	
	*out = '{';
	ptr = out + 1;
	if (fmt)
		*ptr++ = '\n';
	*ptr = 0;
	for (i = 0; i< numentries; ++i)
	{
		if (fmt)
			for (j = 0; j < depth; ++j)
				*ptr++ = '\t';
		strcpy(ptr, names[i]);
		ptr += strlen(names[i]);
		*ptr++ = ':';
		if (fmt)
			*ptr++ = '\t';
		strcpy(ptr, entries[i]);
		ptr += strlen(entries[i]);
		if (i != numentries - 1)
			*ptr++ = ',';
		if (fmt)
			*ptr++ = '\n';
		*ptr = 0;
		delete (names[i]);
		delete (entries[i]);
	}
	
	delete (names);
	delete (entries);
	if (fmt)
		for (i = 0; i < depth - 1; ++i)
			*ptr++ = '\t';
	*ptr++ = '}';
	*ptr++ = 0;
	return (out);
}

void			HandyJson::SuffixItem(HandyJson* item)
{
	this->p_next = item;
	item->p_prev = this;
}

/* Some usefull functions */
const char*		HandyJson::Skip(const char* in)
{
	while (in && *in && (unsigned char)*in <= 32)
		in++;
	return in;
}

int				HandyJson::StrCaseCmp(const char* s1, const char* s2)
{
	if (!s1)
		return ((s1 == s2) ? 0 : 1);
	if (!s2)
		return (1);
	for	(; tolower(*s1) == tolower(*s2); ++s1, ++s2)
	{
		if (*s1 == 0)
			return (0);
	}
	return (tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)s2));
}

char*			HandyJson::StrDup(const char* s)
{
	size_t		len;
    char*		copy;

	if (!s)
		return (0);
    len = strlen(s) + 1;
    if (!(copy = new char[len]()))
		return (0);
	memset(copy, 0, len);
    memcpy(copy, s, len);
    return (copy);
}
