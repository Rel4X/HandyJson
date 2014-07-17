#include		<iostream>
#include		<fstream>
#include		"HandyJson.h"

char*			LoadFile(const char* fname)
{
	std::fstream	file;
	int				fsize;
	char*			data;

	file.open(fname, std::ios::in | std::ios::out);
	if (file.is_open())
	{
		file.seekg(0, std::ios_base::end);
		fsize = file.tellg();
		file.seekg(0, std::ios_base::beg);
		data = new char[fsize + 1]();
		memset(data, 0, fsize + 1);
		file.read(data, fsize);
		file.close();
		return (data);
	}
	return (0);
}

bool			HandlingHandyJsonItems()
{
	char*			json_data;
	HandyJson		root;

	json_data = LoadFile("HJ_Test.txt");												// File loading,
	if (!json_data) { std::cout << "Failed to load file." << std::endl; return (false); }	// and check.

	/*
		+----------------------------------+
		| Building the root using the file |
		+----------------------------------+
												*/
	if (!root.Parse(json_data)) { std::cout << "Could not parse the file." << std::endl; return (false); }

	/*
		+------------------------------+
		| Printing the HandyJson tree |
		+------------------------------+
											*/
	std::cout << root.Print() << std::endl;
	std::cout << root.PrintUnformated() << std::endl;
	std::cout << std::endl;

	/*
		+--------------------------------+
		| Get an item in the main object |
		+--------------------------------+
											*/
	std::cout << "Get an item in the main object:" << std::endl;
	HandyJson*		item;
	item = root.GetObjectItem("String");
	if (item)
	{
		std::cout << "Item named \"String\" found." << std::endl;
		std::cout << "Value is: " << item->GetValStr() << std::endl;
	}
	else { std::cout << "\"String\": Item not found." << std::endl; return (false); }
	std::cout << std::endl;

	/*
		+--------------------------------------+
		| Get an array item in the main object |
		+--------------------------------------+
													*/
	std::cout << "Get an array item in the main object:" << std::endl;
	HandyJson*		thearray;
	thearray = root.GetObjectItem("String Array");
	if (thearray) { std::cout << "Item named \"String Array\" found." << std::endl; }
	else { std::cout << "\"String Array\": Item not found." << std::endl; return (false); }
	std::cout << std::endl;

	/*
		+--------------------------+
		| Get the size of an array |
		+--------------------------+
										*/
	std::cout << "Get the size of an array:" << std::endl;
	std::cout << thearray->GetArraySize() << std::endl;
	std::cout << std::endl;

	/*
		+-----------------------------------------+
		| Get a value in an array, using an index |
		+-----------------------------------------+
													*/
	std::cout << "Get a value in an array, using an index:" << std::endl;
	HandyJson*		item_array;
	item_array = thearray->GetArrayItem(3);
	if (item_array) { std::cout << "Item of index 3 value: " << item_array->GetValStr() << std::endl; }
	else { std::cout << "Item of idex 3 not found." << std::endl; return (false); }
	std::cout << std::endl;

	/*
		+------------------------+
		| Going through an array |
		+------------------------+
									*/
	std::cout << "Going through an array:" << std::endl;
	HandyJson*		iterator;
	iterator = thearray->GetChild();
	while (iterator)
	{
		std::cout << iterator->GetValStr() << std::endl;
		iterator = iterator->GetNext();
	}
	std::cout << std::endl;

	return (true);
}

bool			BuildingHandyJsonTree()
{
	/*
		+-------------------+
		| Building the root |
		+-------------------+
								*/
	HandyJson*		root;
	root = new HandyJson();

	/*
		+----------------------------+
		| Creating a new string item |
		+----------------------------+
										*/
	HandyJson*		new_string;
	new_string = new HandyJson();
	new_string->BuildInString("Hello Corsica !");
	std::cout << new_string->Print() << std::endl;	// Printing the node.
	std::cout << std::endl;

	/*
		+------------------------------+
		| Adding it to the root object |
		+------------------------------+
											*/
	if (root->AddItemToObject("First Item", new_string) == true)
		std::cout << "Item added to object" << std::endl;
	{ std::cout << "Item not added to object." << std::endl; return (false); }
	std::cout << std::endl;

	/*
		+-------------------------------------+
		| Adding a new array item in the root |
		+-------------------------------------+
												*/
	HandyJson*		new_array = new HandyJson(HandyJson::eTypes::json_array);
	if (root->AddItemToObject("Array", new_array))
		std::cout << "Array added to object." << std::endl;
	else { std::cout << "Array not added to object." << std::endl; return (false); }
	std::cout << std::endl;
	
	/*
		+----------------------------------------------------+
		| Building array items using a pre-made array of int |
		+----------------------------------------------------+
																*/
	int			int_tab[5] = { 0, 1, 2, 3, 4 };
	new_array->BuildInIntArray(int_tab, 5);
	std::cout << "Array items built." << std::endl;
	std::cout << std::endl;

	/*
		+--------------------+
		| Duplicating a node |
		+--------------------+
								*/
	HandyJson*		duplication;
	duplication = new_array->Duplicate(true);			// The boolean parameter specify if the duplication is
														// recursive or not.
	root->AddItemToObject("Duplication", duplication);
	std::cout << "Node duplicated, and added to root." << std::endl;
	std::cout << std::endl;

	/*
		+------------------------------+
		| Adding new item into a array |
		+------------------------------+
											*/
	HandyJson*		new_item;
	new_item = new HandyJson();
	new_item->BuildInNumber(42);
	if (new_array->AddItemToArray(new_item) == true)
	{ std::cout << "Item added in array." << std::endl; }
	else { std::cout << "Item not added to array" << std::endl; return (false); }
	std::cout << std::endl;
	
	/*
		+------------------------------------+
		| Deleting an item array using index |
		+------------------------------------+
												*/
	new_array->DeleteItemFromArray(0);
	std::cout << std::endl;




	std::cout << root->Print() << std::endl;
	return (true);
}


// Fonction qui check la validite d un item au print.
// Verifier a chaque changement de type d une node que les valeurs sont maj correctement (genre dans les buildin).
// Virer le milliard de warning
int				main(int ac, char** av)
{
	
	// HandlingHandyJisonItems();
	BuildingHandyJsonTree();
	system("PAUSE");
	return (0);
}