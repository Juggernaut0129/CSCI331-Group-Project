#include <iostream>
#include <fstream>
using namespace std;

int convert(char x);

class S_Set
{
public:
    S_Set(); //Constructor. Determines whether we are using a new data set or a pre-formatted one, and declares all variables accordingly
    void create(); //Used to open unformatted data and read it in as a brand-new sequence set
    void open(); //Opens an existing header file, then uses it to read in the index and main files as well
    void close(); //Closes all files and updates them
    void insert(int z, char str[], int str_length, char sta[], char cou[], int cou_length, float lat, float lon); //Takes the six inputs need for a new record and makes a new record
    void del(int z); //Delete function requiring only the zip as input
    void update(int z, char str[], int str_length, char sta[], char cou[], int cou_length, float lat, float lon); //Takes all input and updates the associated record
    void sort_index();//Performs a basic sort on the index
    void test_display();//Performs a basic test test display.
    void display(); //Displays the entire set of blocks
    void display(int b, int r); //Displays the specified record in the specified block

private:

    void p_insert(int z, char str[], int str_length, char sta[], char cou[], int cou_length, float lat, float lon, int b, int c); //Takes block and record numbers, as well as data, and puts it into the indicated block record
    void p_delete(int b, int c, int i);
    const static int fields = 6; //Denotes the number of field.
    //The next six variables all show the length of each field, which will be used to read the file in.
    const static int zip_length = 6;
    const static int name_length = 31;
    const static int state_length = 2;
    const static int county_length = 26;
    const static int latitude_length = 9;
    const static int longitude_length = 9;

    const static int max_records = 4; //Tells how many records are possible in each block
    const static int max_blocks = 5000; //Tells how many blocks are possible within our class


    fstream new_sequence; //Only used to load in unformatted data
    fstream header_file; //Used in the default constructor to establish our variables IF we are opening an existing file
    string header_name;
    fstream main_file; //Contains properly formatted data. Address is obtained from the header record.
    string main_name;
    fstream index_file; //Contains the formatted index. Address is obtained from the header record.
    string index_name;

    struct Record
    {
        //Variables associated with each record. The names are self-explanatory. Zip is the zip code and so on.
        int zip = 0;
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

    Block addresses[max_blocks]; //Array containing all our blocks in an unsorted format
    int available_blocks = max_blocks; //Integer telling how many blocks are still inactive
    int first_available = 0; //Contains the RBN of the first inactive block
    int first_used = 0;

};

S_Set::S_Set()
{
    available_blocks = max_blocks;
    int c = 0;
    first_available = 0;
}

void S_Set::create()
{
    string answer;
    index_length = 0;
    first_used = 0;
    //Prompt the user for a file full of unformatted data and read it in
    /*cout << endl << "Please name the file you would like to open: ";
    cin >> answer;*/
    answer = "postal.txt";
    //Open the file and keep trying until we succeed
    new_sequence.open(answer);
    while(!new_sequence.is_open())
    {
        cout << endl << "That file doesn't exist. Please try again: ";
        cin >> answer;
        new_sequence.open(answer);
    }
    //For the time being, just skip it and move on using a flag character
    char a = 'a';
    while(a != '*')
    {
        a = new_sequence.get();
    }
    //Begin loading info. Start with the first available block and load three records in it
    //For this test, we will start with one record
    //A is currently "*", so we must begin loading once more
    int c = 0;
    int t = 0;
    int c_record = 0;
    int c_block = 0;
    //Load in all records
    while(c_block < 2000)
    {
        c_record = 0;
    //Load in some records
    while(c_record < 3)
    {
        c = 0;
        t = 0;
    //Load zip code
    while(c < 6)
    {
        a = new_sequence.get();
        c++;
        if(a != ' ')
        {
            t = t * 10;
            t += convert(a);
        }
    }
    addresses[c_block].data[c_record].zip = t;
    //Load the street name
    while(c < 37)
    {
        a = new_sequence.get();
        c++;
        if(a != ' ')
        {
            addresses[c_block].data[c_record].street[addresses[c_block].data[c_record].street_length++] = a;
        }
        else if(a == ' ' && c < 37) //Check to see if it is just a space
        {
            a = new_sequence.get();
            c++;
            if(a != ' ') //If it is, place the necessary space and move on
            {
                addresses[c_block].data[c_record].street[addresses[c_block].data[c_record].street_length++] = ' ';
                addresses[c_block].data[c_record].street[addresses[c_block].data[c_record].street_length++] = a;
            }
        }
    }
    int z = 0;
    //Load the state
    a = new_sequence.get();
    addresses[c_block].data[c_record].state[0] = a;
    a = new_sequence.get();
    addresses[c_block].data[c_record].state[1] = a;
    c += 2;
    //Load the county name
    while(c < 75)
    {
        a = new_sequence.get();
        c++;
        if(a != ' ')
        {
            addresses[c_block].data[c_record].county[addresses[c_block].data[c_record].county_length++] = a;
        }
        else if(a == ' ' && c < 75) //Check to see if it is just a space
        {
            a = new_sequence.get();
            c++;
            if(a != ' ') //If it is, place the necessary space and move on
            {
                addresses[c_block].data[c_record].county[addresses[c_block].data[c_record].county_length++] = ' ';
                addresses[c_block].data[c_record].county[addresses[c_block].data[c_record].county_length++] = a;
            }
        }
    }
    //Load Latitude
    bool negative = false;
    float f = 0.0;
    float m = 10.0;
    a = new_sequence.get();
    c++;
    while(a == ' ') //Eliminate whitespace
    {
        a = new_sequence.get();
        c++;
    }
    if(a == '-') //If the coordinate is negative, set the flag and move on
    {
        a = new_sequence.get();
        c++;
        negative = true;
    }
    while(c < 85)
    {
        if(a == '.')
        {
            //Do nothing
        }
        else if(negative) //'Add' digit to the negative float and move on
        {
            f -= convert(a) * m;
            m = m / 10;
        }
        else //Add a digit to the float and move on
        {
            f += convert(a) * m;
            m = m / 10;
        }
        a = new_sequence.get();
        c++;
    }
    addresses[c_block].data[c_record].latitude = f; //Assign the final value to our record
    //Load longitude
    negative = false;
    m = 10.0;
    f = 0.0;
    while(c < 95)
    {
        if(a == '.' || a == ' ') //For whitespace/decimal points
        {
            //Do nothing
        }
        else if(a == '-') //Check if the coordinate is negative, set the flag accordingly and move on
        {
            negative = true;
        }
        else if(negative) //'Add' digit to the negative float and move on
        {
            f -= convert(a) * m;
            m = m / 10;
        }
        else //Add a digit to the float and move on
        {
            f += convert(a) * m;
            m = m / 10;
        }
        a = new_sequence.get();
        c++;
    }
    addresses[c_block].data[c_record].longitude = f; //Assign the final value to our record
    //Load this record into the index
    index[index_length].key = addresses[c_block].data[c_record].zip;
    index[index_length].block_number = c_block;
    index[index_length++].record_number = c_record;
    //Continue loop
    addresses[c_block].record_count += 1;
    c_record++;
    }
    //Perform block upkeep
    available_blocks--;
    first_available++;
    addresses[c_block].prev = c_block - 1;
    addresses[c_block].next = c_block + 1;
    c_block++;
    }
    //Set the final block's "next" pointer to be NULL
    addresses[c_block - 1].next = -1;
    //Set the pointers to and from our first inactive block
    first_available = c_block;
    addresses[c_block].prev = -1;
    addresses[c_block].next = c_block + 1;
    c_block++;
    //Load in all inactive records
    while(c_block < max_blocks)
    {
        addresses[c_block].prev = c_block - 1;
        addresses[c_block].next = c_block + 1;
        addresses[c_block].record_count = 0;
        c_block++;
    }
    //Once the entire set is loaded into memory, prompt the user for the output file names.
    /*cout << endl << "Please name the main data file: ";
    cin >> answer;*/
    answer = "m.txt";
    main_name = answer;
    main_file.open(answer);
    /*cout << endl << "Please name the header file: ";
    cin >> answer;*/
    answer = "h.txt";
    header_name = answer;
    header_file.open(answer);
    /*cout << endl << "Please name the index file: ";
    cin >> answer;*/
    answer = "i.txt";
    index_name = answer;
    index_file.open(answer);
    if(index_file.is_open() && main_file.is_open() && header_file.is_open())
        cout << "Creation was successful!";
    else
        cout << "Creation was unsuccessful!";
    close();
}

void S_Set::open()
{
    string answer;
    char a;
    bool good = true; //Flag to determine if we can use the header file without any modifications
    //Reset position in the file
    header_file.close();
    header_file.open(header_name);
    //Prompt the user for the HEADER file, which will be used to open the other two
    while(!header_file.is_open())//Keep trying until we have a real file
    {
        /*cout << endl << "Please enter the name of the header file: ";
        cin >> answer;*/
        answer = "h.txt";
        header_file.open(answer);
    }
    header_name = answer;
    //Begin reading in header file, check each stage to make sure this is a compatible file
    //Check extension
    a = header_file.get();
    answer = "";
    while(a != '|' && good)
    {
        answer += a;
        a = header_file.get();
    }
    if(answer != "txt")
        good = false;
    //Check maximum records
    a = header_file.get();
    answer = "";
    while(a != '|' && good)
    {
        answer += a;
        a = header_file.get();
    }
    if(answer != "4")
        good = false;
    //Check record size
    a = header_file.get();
    answer = "";
    while(a != '|' && good)
    {
        answer += a;
        a = header_file.get();
    }
    if(answer != "95")
        good = false;
    //Check field count
    a = header_file.get();
    answer = "";
    while(a != '|' && good)
    {
        answer += a;
        a = header_file.get();
    }
    if(answer != "6")
        good = false;
    //Check schema
    a = header_file.get();
    answer = "";
    while(a != '|' && good)
    {
        answer += a;
        a = header_file.get();
    }
    if(answer != "AOS")
        good = false;
    //Check key type
    a = header_file.get();
    answer = "";
    while(a != '|' && good)
    {
        answer += a;
        a = header_file.get();
    }
    if(answer != "Zip")
        good = false;
    //Check that the main file exists
    a = header_file.get();
    answer = "";
    while(a != '|' && good)
    {
        answer += a;
        a = header_file.get();
    }
    main_file.close();
    main_file.open(answer);
    if(!main_file.is_open())
        good = false;
    main_name = answer;
     //Check that the index file exists
    a = header_file.get();
    answer = "";
    while(a != '|' && good)
    {
        answer += a;
        a = header_file.get();
    }
    index_file.close();
    index_file.open(answer);
    if(!index_file.is_open())
        good = false;
    index_name = answer;
    //Load stale flag
    a = header_file.get();
    answer = a;
    if (answer != "0")
        good = false;
    //Reset position in the main file
    main_file.close();
    main_file.open(main_name);
    //Load in first set of metadata and compare
    int c = 0;
    int t = 0;
    if(good)
    {
        while(c < 4)
        {
            a = main_file.get();
            t = t * 10;
            t += convert(a);
            c++;
        }
        if(t != max_blocks)
            good = false;
    }

    if(good) //If everything is compatible, read in the information
    {
        //Load in the number of open blocks
        c = 0;
        t = 0;
        while(c < 4)
        {
            a = main_file.get();
            t = t * 10;
            t += convert(a);
            c++;
        }
        available_blocks = t;
        //Load in the RBN of the first open block
        c = 0;
        t = 0;
        while(c < 4)
        {
            a = main_file.get();
            t = t * 10;
            t += convert(a);
            c++;
        }
        first_available = t;
        //Load in the RBN of the first active block
        c = 0;
        t = 0;
        while(c < 4)
        {
            a = main_file.get();
            t = t * 10;
            t += convert(a);
            c++;
        }
        first_used = t;
        //Clear extra zero
        a = main_file.get();
        a = main_file.get();

        //Begin loop to load in each block
        int c_block = 0;
        int c_record = 0;
        while(c_block < max_blocks)
        {
            addresses[c_block].record_count = 0;
            c_record = 0;
            while(c_record < max_records)
            {
                c = 0;
                t = 0;
                while(c < 6) //Load in zip
                {
                    a = main_file.get();
                    if(a != ' ')
                    {
                        t = t * 10;
                        t += convert(a);
                    }
                    c++;
                }
                addresses[c_block].data[c_record].zip = t;
                //A zip of 0 indicates an empty record. If it is not empty, load in the rest
                if(addresses[c_block].data[c_record].zip != 0)
                {
                    addresses[c_block].record_count += 1;
                    //Load the street
                    addresses[c_block].data[c_record].street_length = 0;
                    while(c < 37)
                    {
                        a = main_file.get();
                        c++;
                        if(a != ' ')
                        {
                            addresses[c_block].data[c_record].street[addresses[c_block].data[c_record].street_length++] = a;
                        }
                        else if(a == ' ' && c < 37) //Check to see if it is just a space
                        {
                            a = main_file.get();
                            c++;
                            if(a != ' ') //If it is, place the necessary space and move on
                            {
                                addresses[c_block].data[c_record].street[addresses[c_block].data[c_record].street_length++] = ' ';
                                addresses[c_block].data[c_record].street[addresses[c_block].data[c_record].street_length++] = a;
                            }
                        }
                    }
                    //Load the state
                    a = main_file.get();
                    addresses[c_block].data[c_record].state[0] = a;
                    a = main_file.get();
                    addresses[c_block].data[c_record].state[1] = a;
                    c += 2;
                    //Load the county
                    int w = 0;
                    while(c < 75)
                    {
                        a = main_file.get();
                        c++;
                        if(a != ' ')
                        {
                            addresses[c_block].data[c_record].county[w++] = a;
                        }
                        else if(a == ' ' && c < 75) //Check to see if it is just a space
                        {
                            a = main_file.get();
                            c++;
                            if(a != ' ') //If it is, place the necessary space and move on
                            {
                                addresses[c_block].data[c_record].county[w++] = ' ';
                                addresses[c_block].data[c_record].county[w++] = a;
                            }
                        }
                    }
                    addresses[c_block].data[c_record].county_length = w;
                    //Load the latitude
                    bool negative = false;
                    float f = 0.0;
                    float m = 10.0;
                    a = main_file.get();
                    c++;
                    while(a == ' ') //Eliminate whitespace
                    {
                        a = main_file.get();
                        c++;
                    }
                    if(a == '-') //If the coordinate is negative, set the flag and move on
                    {
                        a = main_file.get();
                        c++;
                        negative = true;
                    }
                    while(c < 85)
                    {
                        if(a == '.')
                        {
                            //Do nothing
                        }
                        else if(negative) //'Add' digit to the negative float and move on
                        {
                            f -= convert(a) * m;
                            m = m / 10;
                        }
                        else //Add a digit to the float and move on
                        {
                            f += convert(a) * m;
                            m = m / 10;
                        }
                        a = main_file.get();
                        c++;
                    }
                    addresses[c_block].data[c_record].latitude = f; //Assign the final value to our record
                    negative = false;
                    m = 10.0;
                    f = 0.0;
                     //Load in the longitude
                    while(a != '*')
                    {
                        if(a == '.' || a == ' ') //For whitespace/decimal points
                        {
                            //Do nothing
                        }
                        else if(a == '-') //Check if the coordinate is negative, set the flag accordingly and move on
                        {
                            negative = true;
                        }
                        else if(negative) //'Add' digit to the negative float and move on
                        {
                            f -= convert(a) * m;
                            m = m / 10;
                        }
                        else //Add a digit to the float and move on
                        {
                            f += convert(a) * m;
                            m = m / 10;
                        }
                        a = main_file.get();
                    }
                    addresses[c_block].data[c_record].longitude = f; //Assign the final value to our record
                }
                while(a != '*')
                {
                    a = main_file.get();
                }

                a = main_file.get();
                c_record++;
            }
            //Load in "prev" and "next" pointers
            //Prev is done first
            int x = 0;
            a = main_file.get();
            while(a != '*')
            {
                if(a == '-')//If this pointer is null
                {
                    //Remove the excess 1 and end this loop
                    a = main_file.get();
                    x = -1;
                }
                else//If it is anything else, we have to load the whole number in
                {
                    x = x * 10;
                    x += convert(a);
                }
                a = main_file.get();
            }
            addresses[c_block].prev = x;
            //Next is done in the same way
            x = 0;
            a = main_file.get();
            while(a != '*')
            {
                if(a == '-')//If this pointer is null
                {
                    //Remove the excess 1 and end this loop
                    a = main_file.get();
                    x = -1;
                }
                else//If it is anything else, we have to load the whole number in
                {
                    x = x * 10;
                    x += convert(a);
                }
                a = main_file.get();
            }
            //Eliminate final asterisk
            a = main_file.get();
            addresses[c_block].next = x;
            c_block++;
        }

        //Load index file
        index_file.close();
        index_file.open(index_name);
        int s = 0;
        c = 0;
        //Load in the metadata, which designates our index size
        while(c < 4)
        {
            a = index_file.get();
            s = s * 10;
            s += convert(a);
            c++;
        }
        a = index_file.get(); //Discard spare zero
        index_length = s;
        c = 0;
        int d;
        //Load in all index values
        while(c < index_length)
        {
            d = 0;
            s = 0;
            //Load the first five values as the key
            while(d < 6)
            {
                a = index_file.get();
                s = s * 10;
                s += convert(a);
                d++;
            }
            index[c].key = s;
            //Load the next four values as the block number
            s = 0;
            while(d < 10)
            {
                a = index_file.get();
                s = s * 10;
                s += convert(a);
                d++;
            }
            index[c].block_number = s;
            //Load the last value as the index number
            a = index_file.get();
            index[c].record_number = convert(a);
            c++;
        }
    }
    else
        cout << endl << "Sorry, that file isn't compatible.";
    cout << "Open successful!";
}

void S_Set::close()
{
    //Reset position in all files
    index_file.close();
    index_file.open(index_name);
    main_file.close();
    main_file.open(main_name);
    index_file.close();
    index_file.open(index_name);
    //Load the main data file
    int c_block = 0;
    int c_record = 0;
    int c;
    int d;
    int temp;
    //Load in metadata
    d = 1000;
    //Load in number of blocks
    while(max_blocks < d)
    {
        main_file << 0;
        d = d / 10;
    }
    main_file << max_blocks;
    //Load in the number of active blocks
    d = 1000;
    while(available_blocks < d)
    {
        d = d / 10;
        main_file << 0;
    }
    main_file << available_blocks;
    //Load in the RBN of the first inactive block
    d = 1000;
    while(first_available < d)
    {
        d = d / 10;
        main_file << 0;
    }
    main_file << first_available;
    //Load in the RBN of the first active block
    d = 1000;
    while(first_used < d)
    {
        d = d / 10;
        main_file << 0;
    }
    main_file << first_used << "\n";
    //Load the blocks
    while(c_block < max_blocks) //Loop until all blocks have been used
    {
        c_record = 0;
        //Print all active records
        while(c_record < addresses[c_block].record_count)
        {
            //Add each relevant record, with appropriate spacing, to the file
        //Add the zip
            c = 100000;
            //Fill white space
            while(addresses[c_block].data[c_record].zip < c)
            {
                main_file << ' ';
                c = c / 10;
            }
            //Once white space is filled, add the zip
            main_file << addresses[c_block].data[c_record].zip;
        //Add the street
            c = 0;
            //Put in street
            while(c < addresses[c_block].data[c_record].street_length)
            {
                main_file << addresses[c_block].data[c_record].street[c++];
            }
            //Fill the remainder with whitespace
            while(c < 31)
            {
                main_file << ' ';
                c++;
            }
        //Add the state
            main_file << addresses[c_block].data[c_record].state[0];
            main_file << addresses[c_block].data[c_record].state[1];
        //Add the county
            c = 0;
            //Put in county
            while(c < addresses[c_block].data[c_record].county_length)
            {
                main_file << addresses[c_block].data[c_record].county[c++];
            }
            //Fill the remainder with whitespace
            while(c < 36)
            {
                main_file << ' ';
                c++;
            }
        //Add the latitude
            temp = addresses[c_block].data[c_record].latitude * 10000;
            d = 100;
            c = -100;
            if(temp > 0) //Add an extra space for positive coordinates
            {
                main_file << ' ';
            }
            while(addresses[c_block].data[c_record].latitude < d && addresses[c_block].data[c_record].latitude > c) //Fill spare whitespace
            {
                main_file << ' ';
                c = c / 10;
                d = d / 10;
            }
            //Add latitude and fill remaining white space
            main_file << addresses[c_block].data[c_record].latitude;
            while(temp % 10 == 0) //Eliminate floating zeros and fill them with white space
            {
                main_file << ' ';
                temp = temp / 10;
            }
        //Add the longitude
            temp = addresses[c_block].data[c_record].longitude * 10000;
            d = 100;
            c = -100;
            if(temp > 0) //Add an extra space for positive coordinates
            {
                main_file << ' ';
            }
            while(addresses[c_block].data[c_record].longitude < d && addresses[c_block].data[c_record].longitude > c) //Fill empty spaces with whitespace
            {
                main_file << ' ';
                c = c / 10;
                d = d / 10;
            }
            //Add longitude and fill remaining white space
            main_file << addresses[c_block].data[c_record].longitude;
            while(temp % 10 == 0) //Eliminate floating zeros and fill them with white space
            {
                main_file << ' ';
                temp = temp / 10;
            }
            //Fill remaining whitespace
            main_file << "*" << "\n";
            c_record++;
        }
        //Print any remaining inactive records
        while(c_record < max_records)
        {
            c = 0;
            while(c < 93)
            {
                main_file << ' ';
                c++;
            }
            main_file << "*" << "\n";
            c_record++;
        }
        //Load in the previous and next pointers
        main_file << addresses[c_block].prev << "*" << addresses[c_block].next << "*" << "\n";
        c_block++;
    }
    //Load index file
    d = 1000;
    temp = max_blocks - available_blocks;
    while(temp < d)
    {
        d = d / 10;
        index_file << 0;
    }
    index_file << temp;
    //Load stale flag
    index_file << 0 << "\n";
    c = 0;
    //Load in index values
    while(c < index_length)
    {
        //Load in key
        d = 10000;
        while(index[c].key < d)
        {
            index_file << 0;
            d = d / 10;
        }
        index_file << index[c].key;
        //Load in block number
        d = 1000;
        while(index[c].block_number < d)
        {
            index_file << 0;
            d = d / 10;
        }
        if(index[c].block_number != 0) //Avoid reprinting
            index_file << index[c].block_number;
        //Load in record number
        index_file << index[c].record_number << "\n";
        c++;
    }
    //Load the header file
    header_file << "txt|";
    header_file << "4|";
    header_file << "95|";
    header_file << "6|";
    header_file << "AOS|";
    header_file << "Zip|";
    header_file << main_name << "|";
    header_file << index_name << "|";
    header_file << "0|";
}

void S_Set::sort_index()
{
    //While an inefficient use of our time, the bubble sort is ideal
    //It requires barely any additional computational power, just time
    //We have plenty of that!
    Index_Node temp;
    int c = 0;
    int end = max_blocks * max_records;
    bool swap = true;
    while(swap) //Continue looping until we go through the entire list without swapping anything
    {
        swap = false;
        c = 0;
        while(c < end - 1)//Iterate this loop until we've sorted the entire list
        {
            if(index[c].key > index[c + 1].key && index[c + 1].key != 0)//If the first value is larger and neither are empty, swap them
            {
                temp.key = index[c].key;
                index[c].key = index[c+1].key;
                index[c+1].key = temp.key;
                swap = true; //Set flag
            }
            else if(index[c].key == 0 && index[c+1].key != 0)//If our first value is 0 and the second is not, swap them
            {
                temp.key = index[c].key;
                index[c].key = index[c+1].key;
                index[c+1].key = temp.key;
                swap = true; //Set flag
            }
            //Otherwise, just move on
            c++;
        }
    }
}

void S_Set::display(int b, int c)
{
    if(c < addresses[b].record_count)//Check that this is a real record
    {
        int a;
        //Print zip code
        cout << endl << addresses[b].data[c].zip << " ";
        //Print street
        a = 0;
        while(a < addresses[b].data[c].street_length)
        {
            cout << addresses[b].data[c].street[a];
            a++;
        }
        //Print state
        cout << " " << addresses[b].data[c].state[0] << addresses[b].data[c].state[1] << " ";
        //Print county
        a = 0;
        while(a < addresses[b].data[c].county_length)
        {
            cout << addresses[b].data[c].county[a];
            a++;
        }
        //Print latitude and longitude
        cout << " " << addresses[b].data[c].latitude << " " << addresses[b].data[c].longitude;
    }
    else //If this not a real record, print nothing
        cout << endl;
}

void S_Set::display()
{
    int b = 0;
    int c = 0;
    while(b < max_blocks) //Print all blocks
    {
        cout << endl  << endl << "BLOCK " << b << ": ";
        c = 0;
        while(c < max_records)
        {
            display(b, c);
            c++;
        }
        b++;
    }
}

void S_Set::insert(int z, char str[], int str_length, char sta[], char cou[], int cou_length, float lat, float lon)
{
    int b = 0;
    int c = 0;
    //Firstly, iterate through the active blocks until we find one that has space
    b = first_used;
    while(addresses[b].record_count == max_records && b != -1)
    {
        b = addresses[b].next;
    }
    //Check if all active blocks were full. If they were, make a new one
    if(b == -1)
    {
        //Take the first unavailable block
        b = first_available;
        //Insert our record
        p_insert(z, str, str_length, sta, cou, cou_length, lat, lon, b, c);
        //Strip records from one of the full ones and add it so we are above minimum capacity
        c = max_records - 1;
        while(addresses[b].record_count < max_records / 2)
        {
            //Extremely long line that just means we are taking all values from one record and moving it to another
            p_insert(addresses[b].data[c].zip, addresses[b].data[c].street, addresses[b].data[c].street_length, addresses[b].data[c].state, addresses[b].data[c].county, addresses[b].data[c].county_length, addresses[b].data[c].latitude, addresses[b].data[c].longitude, b, c);
            del(addresses[b].data[c].zip);
            c--;
        }
    }
    //When we have found an active block with space, simply insert
    else
    {
        p_insert(z, str, str_length, sta, cou, cou_length, lat, lon, b, addresses[b].record_count);
    }
}

void S_Set::p_insert(int z, char str[], int str_length, char sta[], char cou[], int cou_length, float lat, float lon, int b, int c)
{
    //Load zip code
    addresses[b].data[c].zip = z;
    //Load street
    int d = 0;
    while(d < str_length)
    {
        addresses[b].data[c].street[d] = str[d];
        d++;
    }
    //Assign street length
    addresses[b].data[c].street_length = str_length;
    //Load the state
    addresses[b].data[c].state[0] = sta[0];
    addresses[b].data[c].state[1] = sta[1];
    //Load the county
    d = 0;
    while(d < cou_length)
    {
        addresses[b].data[c].county[d] = cou[d];
        d++;
    }
    //Assign county length
    addresses[b].data[c].county_length = cou_length;
    //Load latitude
    addresses[b].data[c].latitude = lat;
    //Load longitude
    addresses[b].data[c].longitude = lon;
    //Increase the record count in the addresses
    addresses[b].record_count += 1;
}

void S_Set::del(int z)
{
    //First, make sure the index is sorted
    sort_index();
    //Once that is done, perform a binary search
    int c = index_length / 2;
    int min = 0;
    int max = index_length;
    while(index[c].key != z && max - min != 0)
    {
        if(index[c].key < z) //If the current position is less than our target
        {
            //Raise the floor
            min = c;
        }
        else if(index[c].key > z) //If the current position is greater than our target
        {
            //Lower the ceiling
            max = c;
        }
        //Move our midpoint
        c = (max + min) / 2;
    }
    //The loop will terminate when we've found the right zip code OR searched everything
    //If we found it, delete
    if(index[c].key == z)
    {
        int b = index[c].block_number;
        int r = index[c].record_number;
        //Call the private version of the delete function
        p_delete(b, r, c);
    }
}

void S_Set::p_delete(int b, int c, int i)
{
    //Check to see if this is the last record in the file
    if(c == addresses[b].record_count - 1)
    {
        //If this is the last record, simply blank the zip code, update the necessary variable, and update the index
        addresses[b].data[c].zip = 0;
        addresses[b].record_count -= 1;
        index[i].key = 0;
    }
    //If this is not the last one
    else
    {
        //Swap its position with the last one
        Index_Node temp;
        //Find that node's location
        int z = addresses[b].data[addresses[b].record_count - 1].zip;
        int t = index_length / 2;
        int min = 0;
        int max = index_length;
        while(index[t].key != z && max - min != 0)
        {
            if(index[t].key < z) //If the current position is less than our target
            {
                //Raise the floor
                min = t;
            }
            else if(index[t].key > z) //If the current position is greater than our target
            {
                //Lower the ceiling
                max = t;
            }
            //Move our midpoint
            t = (max + min) / 2;
        }
        //Load our secondary record's index entry into the temporary index node
        temp.key = index[t].key;

        //Now move our current entry
        //Load zip
        addresses[b].data[c].zip = addresses[b].data[addresses[b].record_count - 1].zip;
        //Load street
        int d = 0;
        while(d < addresses[b].data[addresses[b].record_count - 1].street_length)
        {
            addresses[b].data[c].street[d] = addresses[b].data[addresses[b].record_count - 1].street[d];
            d++;
        }
        addresses[b].data[c].street_length = addresses[b].data[addresses[b].record_count - 1].street_length;
        //Load state
        addresses[b].data[c].state[0] = addresses[b].data[addresses[b].record_count - 1].state[0];
        addresses[b].data[c].state[1] = addresses[b].data[addresses[b].record_count - 1].state[1];
        //Load county
        d = 0;
        while(d < addresses[b].data[addresses[b].record_count - 1].county_length)
        {
            addresses[b].data[c].county[d] = addresses[b].data[addresses[b].record_count - 1].county[d];
            d++;
        }
        addresses[b].data[c].county_length = addresses[b].data[addresses[b].record_count - 1].county_length;
        //Load latitude and longitude
        addresses[b].data[c].latitude = addresses[b].data[addresses[b].record_count - 1].latitude;
        addresses[b].data[c].longitude = addresses[b].data[addresses[b].record_count - 1].longitude;

        //Delete the now useless end record
        p_delete(b, addresses[b].record_count - 1 , t);
        //Move in our new index
        index[i].key = temp.key;
    }
}

void S_Set::update(int z, char str[], int str_length, char sta[], char cou[], int cou_length, float lat, float lon)
{
    //The easiest way to make sure everything updates correctly is to delete the record and reinsert it
    del(z);
    insert(z, str, str_length, sta, cou, cou_length, lat, lon);
}

int main()
{
    //Establishing a couple values for use later on in insert() and update()
    char str[30];
    str[0] = 'I';
    str[1] = 'c';
    str[2] = 'e';
    char sta[2];
    sta[0] = 'M';
    sta[1] = 'N';
    char cou[30];
    cou[0] = 'R';
    cou[1] = 'i';
    cou[2] = 'c';
    cou[3] = 'e';
    //Begin tests
    cout << "Testing create: ";
    S_Set test;
    test.create();
    test.close();
    cout << endl << endl << "Testing open: ";
    test.open();
    //Print a few blocks to confirm
    test.display(0,0);
    test.display(0,1);
    test.display(0,2);
    test.display(0,3);
    test.display(1,0);
    test.display(1,1);
    test.display(1,2);
    test.display(1,3);
    test.display(2,0);
    test.display(2,1);
    test.display(2,2);
    test.display(2,3);
    cout << endl << endl << "Testing delete: ";
    test.del(1001);
    //Print a few blocks to confirm
    test.display(0,0);
    test.display(0,1);
    test.display(0,2);
    test.display(0,3);
    test.display(1,0);
    test.display(1,1);
    test.display(1,2);
    test.display(1,3);
    test.display(2,0);
    test.display(2,1);
    test.display(2,2);
    test.display(2,3);
    //Print a few blocks to confirm
    cout << endl << endl << "Testing update.";
    test.update(544, str, 3, sta, cou, 4, 42.5678, -18.0976);
    test.display(0,0);
    test.display(0,1);
    test.display(0,2);
    test.display(0,3);
    test.display(1,0);
    test.display(1,1);
    test.display(1,2);
    test.display(1,3);
    test.display(2,0);
    test.display(2,1);
    test.display(2,2);
    test.display(2,3);
    cout << endl << endl << "Testing insert: ";
    test.insert(9786, str, 3, sta, cou, 4, 42.5678, -18.0976);
    //Print a few blocks to confirm
    test.display(0,0);
    test.display(0,1);
    test.display(0,2);
    test.display(0,3);
    test.display(1,0);
    test.display(1,1);
    test.display(1,2);
    test.display(1,3);
    test.display(2,0);
    test.display(2,1);
    test.display(2,2);
    test.display(2,3);
    cout << endl << "That's all, folks!" << endl;
    return 0;
}

int convert(char x)
{
    if(x == '0')
    {
        return 0;
    }
    else if(x == '1')
    {
        return 1;
    }
    else if(x == '2')
    {
        return 2;
    }
    else if(x == '3')
    {
        return 3;
    }
    else if(x == '4')
    {
        return 4;
    }
    else if(x == '5')
    {
        return 5;
    }
    else if(x == '6')
    {
        return 6;
    }
    else if(x == '7')
    {
        return 7;
    }
    else if(x == '8')
    {
        return 8;
    }
    else if(x == '9')
    {
        return 9;
    }
    else
        return 0;
}
