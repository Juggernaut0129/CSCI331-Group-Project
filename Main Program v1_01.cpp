#include <iostream>
#include <fstream>
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
        char state[state_length];
        char street[name_length];
        char county[county_length];
        //These two variables are used to count how much of their respective arrays are in use.
        //For example, if street[] contained "James", then street_length would be 5.
        int street_length = 0;
        int county_length = 0;
    };

    struct Block
    {
        int record_count = 0; //Denotes how many records are in this block. "0" indicates an inactive block
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




int main()
{
    cout << "That's all, folks!" << endl;
    return 0;
}
