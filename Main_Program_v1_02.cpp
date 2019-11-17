#include <iostream>
#include <fstream>
#include <bits/stdc++.h> 
#include <string>
using namespace std;

class S_Set
{
public:
    S_Set(); //Default constructor. Determines whether we are using a new data set or a pre-formatted one, and declares all variables accordingly
    ~S_Set(); //Destructor. This might not be strictly necessary, but we'll keep it around for now
    void create(); //Used to open unformatted data and read it in as a brand-new sequence set
    void open(); //Opens an existing header file, then uses it to read in the index and main files as well
    void close(); //Closes all files. Make sure to update all files as we do so!
    void insert(); //Prompts the user for all relevant information on the record they want to insert, then puts it in a Record struct and sends it to the private version
    void del(); //Delete function(syntax error if we spell it out). Asks the user for a zip code, and sends it to the private version. Uses the returned boolean to report the success/failure
    void update(); //Asks for a zip code, sends it to the private version
    void populate(); //Sorts the program out, making sure each active block is filled to 75% capacity
    void display_record(); //Prompts the user for a zip code, then sends it to the private version
    bool is_empty(int RN, int type); //Checks the emptiness of the variable indicated by "type" and finds it using the RN (relative number)
    void display_SS(); //Asks the user whether they want to display the whole record or just some of it, and sends the info to the private version
    void validate(); //Checks the stale flags on the header and index files. If they are out of date, it loads the main file in and uses it to update both

private:
    const static int fields; //Denotes the number of field. Determined by default constructor
    //The next six variables all show the length of each field, which will be used to read the file in.
    const static int zip_length = 6;
    const static int name_length = 31;
    const static int state_length = 2;
    const static int county_length = 26;
    const static int latitude_length = 9;
    const static int longitude_length = 9;

    const static int max_records = 4; //Tells how many records are possible in each block
    const static int max_blocks = 1600; //Tells how many blocks are possible within our class


    fstream new_sequence; //Only used to load in unformatted data
    fstream header_file; //Used in the default constructor to establish our variables IF we are opening an existing file
    fstream main_file; //Contains properly formatted data. Address is obtained from the header record.
    fstream index_file; //Contains the formatted index. Address is obtained from the header record.

    struct Record
    {
        //Variables associated with each record. The names are self-explanatory. Zip is the zip code and so on.
        int zip;
        float latitude;
        float longitude;
		char street[name_length];
        char state[state_length];
        char county[county_length];
        //These two variables are used to count how much of their respective arrays are in use.
        //For example, if street[] contained "James", then street_length would be 5.
        int street_length = 0; // might caused [Warning] non-static data member initializers only available with -std=c++11 or -std=gnu++11
        int county_length = 0; // might caused [Warning] non-static data member initializers only available with -std=c++11 or -std=gnu++11
    };

    struct Block
    {	
        int record_count = 0; //might caused [Warining] //Denotes how many records are in this block. "0" indicates an inactive block
        int next; //RBN of the next block of like type
        int prev; //RBN of the previous block of like type (only used in active blocks)
        Record data[max_records]; //Array containing all records in the block
    };

    struct Index_Node
    {
        int key; //Zip code, used to sort the file
        int block_number; //Block number containing the indicated record
        int record_number; //Record number of the indicated record
    };

    Index_Node index[max_records * max_blocks];//Array containing the maximum possible number of records, though most will likely go unused
    int index_length; //Tells how many of the spaces are actually in use

    void private_insert(Record r); //Private version of insert, searches the block for an active block with room. If none are found, populate and try again
    bool private_delete(int key); //Searches the index, and if possible deletes the record indicated by this key. Returns the success or failure to the public version
    void private_update(int key); //Finds the record in the index, then has the user input all variable changes
    void private_display_record(int key); //Finds the record in the index, then displays it to the terminal
    void private_display_SS(bool fields[]); //The array will be size "fields" (see variable) and uses booleans to show the desired fields (or all of them)


    Block addresses[max_blocks]; //Array containing all our blocks in an unsorted format
    int available_blocks; //Integer telling how many blocks are still inactive
    int first_available; //Contains the RBN of the first inactive block
};

S_Set::S_Set() {
	//need body//
}

S_Set::~S_Set() {
    //need body//
}

// public create() function by Aaron
// takes in the TXT file provided and generates a new file with the records
// NEEDS header generation and index file generation.
void S_Set::create(){
	string line, filename;
	cout << "Unformatted File Name: ";				// prompt the user for an input file name
	cin >> filename;
	new_sequence.open(filename);
	
	for(int i=0; i<10; i++){						// skips the meta data in the unformatted file
		getline(new_sequence, line);				// 
	}
	
	header_file.open("header.txt");
	index_file.open("index.txt");
	
	main_file.open("main.txt",ios::out);
	if(main_file.is_open())
		while(getline(new_sequence, line)){			// reads in the data from the unformatted file
			main_file << line << endl;				// adds it to the main records file
		}
	
	// close opened files
	new_sequence.close();
	header_file.close();
	main_file.close();
	index_file.close();
	// cout << "done" << endl;
}

//public insert() function by Khuong
void S_Set::insert() {
	Record record;
	string street, state, county;
	//Prompts the user for all relevant information...
	cout << ".....Creating a new record..... " << endl;
	cout << "Enter zip code: ";
	cin >> record.zip;
	cout << "Enter street name: ";
	cin >> street;
	record.street_length = street.length();		//set street length
	strcpy(record.street, street.c_str());		//copy string street to array char
	cout << "Enter state name: ";
	cin >> state;
	strcpy(record.state, state.c_str());		//copy string state to array char
	cout << "Enter county: ";
	cin >> county;
	record.county_length = county.length();		//set county length
	strcpy(record.county, county.c_str());		//copy string county to array char
	cout << "Enter latitude: ";
	cin >> record.latitude;
	cout << "Enter longitude: ";
	cin >> record.longitude;
	
/*	//Output record
	cout << "-----New Record-----" << endl;
	cout << "Zip code: " << record.zip << endl;
	cout << "Street name: ";
	for(int i = 0; i < record.street_length; i++)
		cout << record.street[i];
	cout << endl;
	cout << "State: ";
	for(int i = 0; i < state_length; i++) 
		cout << record.state[i];
	cout << endl;
	cout << "County: ";
	for(int i = 0; i < record.county_length; i++)
		cout << record.county[i];
	cout << endl;
	cout << "Latitude: " << record.latitude << endl;
	cout << "Longitude: " << record.longitude << endl;
*/		
	//Private version of insert
//	private_insert(record);	
}


int main()
{   S_Set sSet;
    char add;
 	sSet.create();
    do {
		sSet.insert();
		cout << endl << "Add more record(Y/N): ";
		cin >> add;
	} while(add == 'Y' || add == 'Y');
 
    cout << "That's all, folks!" << endl;
    return 0;
}
