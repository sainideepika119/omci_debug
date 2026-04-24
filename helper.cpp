//============================================================================
// Name        : helper.cpp
// Author      : Deepika
// Version     :
// Copyright   : Your copyright notice
// Description : Printing app for omci.msg files, Ansi-style
//============================================================================

#include "helper.h"

string input_file_name = "";
string output_file_name = "";


const char* hex_char_to_bin(char c)
{
    switch(toupper(c))
    {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': return "1010";
        case 'B': return "1011";
        case 'C': return "1100";
        case 'D': return "1101";
        case 'E': return "1110";
        case 'F': return "1111";
    }
    return "";
}

string hex_str_to_bin_str(const string& hex)
{

	string bin;
    for(unsigned i = 0; i != hex.length(); ++i)
       bin += hex_char_to_bin(hex[i]);
    return bin;
}

void print(vector<string> v)
{
	cout<<"Printing v =>"<<endl;
	cout<<endl;

	for(auto x: v)
		cout<<x<<endl;
	cout<<endl;

	return;
}

void print_msg(vector<string> v)
{
	time_t now = time(0);
	char* dt = ctime(&now);

	string mt = "";
	string cls = "";
	string inst = "";
	string attrMask = "";

	ofstream myfile (output_file_name);

	if (myfile.is_open())
	{
		myfile<<"File creation time => "<<dt<<endl;
		myfile<<endl<<endl;

		for(unsigned int i=0;i<v.size();i++)
		{
			mt = "";
			cls = "";
			inst = "";
			attrMask = "";

//			myfile<<v[i]<<endl;

			if(v[i].size() == 0) continue;

			myfile<<"Transaction ID <"<< v[i][0]<<v[i][1]<<v[i][2]<<v[i][3]<<">"<<endl;

			mt = mt + v[i][4] + v[i][5];

			myfile<<"Messgae Type <"<<v[i][4]<<v[i][5]<<"> <"<<m[mt]<<">"<<endl;

//			myfile<<"Device ID <"<<v[i][6]<<v[i][7]<<">"<<endl;

			cls = cls + v[i][8] + v[i][9] + v[i][10] + v[i][11];
			inst = inst + v[i][12] + v[i][13] + v[i][14] + v[i][15];

			myfile<<"ME Class <"<<cls<<"><"<<stoi(cls, 0, 16)<<"> ME Instance <"<<inst<<"><"<<stoi(inst, 0, 16)<<">"<<endl;

			for(int j=0;j<16;j=j+2)
			{
				if((j)%8 == 0)myfile<<" ";
				myfile<<v[i][j]<<v[i][j+1]<<" ";
			}
			myfile<<endl;
			for(int j=16;j<48;j=j+2)
			{
				if((j)%8 == 0)myfile<<" ";
				myfile<<v[i][j]<<v[i][j+1]<<" ";
			}
			myfile<<endl;
			for(int j=48;j<80;j=j+2)
			{
				if((j)%8 == 0)myfile<<" ";
				myfile<<v[i][j]<<v[i][j+1]<<" ";
			}
			myfile<<endl;
			for(int j=80;j<96;j=j+2)
			{
				if((j)%8 == 0)myfile<<" ";
				myfile<<v[i][j]<<v[i][j+1]<<" ";
			}
			myfile<<endl;


			if(mt == SET || mt == GET)
			{
				attrMask = attrMask + v[i][16] + v[i][17] + v[i][18] + v[i][19];
				string attr = hex_str_to_bin_str(attrMask);
				myfile<<"attrMask <"<<attr<<">"<<endl;

				if(mt == SET)
				{
					auto it = class_map.find(stoi(cls, 0, 16));
					if(it != class_map.end())
					{
						myfile << it->second[0].name<<endl;
						int index = 20;
						for(int j=0;j<16;j++)
						{
							if(attr[j] == '1')
							{
								myfile << left << setw(30) << it->second[j+1].name;
								for(int k=0;k<(it->second[j+1].size*2);k++)
									myfile << v[i][index++];
								myfile << endl;
							}
						}
					}
					else
					{
						myfile<<"Class id not present in mib list"<<endl;
					}
				}
			}
			else if(mt == SET_RES || mt == GET_RES)
			{
//				attrMask = "";
				attrMask = attrMask +  v[i][18] + v[i][19] + v[i][20] + v[i][21];
				string attr = hex_str_to_bin_str(attrMask);
				myfile<<"attrMask <"<<attr<<">"<<endl;

				if(mt == GET_RES)
				{
					auto it = class_map.find(stoi(cls, 0, 16));
					if(it != class_map.end())
					{
						myfile << it->second[0].name<<endl;
						int index = 22;
						for(int j=0;j<16;j++)
						{
							if(attr[j] == '1')
							{
								myfile << left << setw(30) << it->second[j+1].name;
								for(int k=0;k<(it->second[j+1].size*2);k++)
									myfile << v[i][index++];
								myfile << endl;
							}
						}
					}
					else
					{
						myfile<<"Class id not present in mib list"<<endl;
					}
				}
			}
			myfile<<endl<<endl;
		}
	}

	myfile.close();
	return;
}

int main(int argc, char** argv)
{
	bool ONT_BROADCOM = false;
	bool ONT_REALTEK = false;
	bool OLT_BROADCOM = false;
	string arg = argv[1];

	if(arg == "ont_broadcom")
		ONT_BROADCOM = true;
	else if (arg == "ont_realtek")
		ONT_REALTEK = true;
	else if(arg == "olt_broadcom")
		OLT_BROADCOM = true;

	if( argc == 2 )
	{
		cout << "No input file was passed. Usage: omci.msg"<<endl;
		cout << "No output file is passed. Usage: output.txt"<<endl;
		input_file_name = "omci.msg";
		output_file_name = "output.txt";
	}
	else if(argc == 3)
	{
		cout << "Input Usage: "<<argv[2]<<endl;
		cout << "No output file is passed. Usage: output.txt"<<endl;
		input_file_name = argv[2];
		output_file_name = "output.txt";
	}
	else{
		cout << "Input Usage: "<<argv[2]<<endl;
		cout << "Output Usage: "<<argv[3]<<endl;
		input_file_name = argv[2];
		output_file_name = argv[3];
	}

	ifstream fin;

	fin.open(input_file_name);
	string line;
	vector<string> v;
	string output =   "";


	while (fin)
	{
		getline(fin, line);
		stringstream ss ;
		ss.str(line);
		string segment;

//REALTEK_ONT
if(ONT_REALTEK == true)
{
		output = "";
		while(std::getline(ss, segment, '\n'));

		cout<<segment<<endl;
		for (char ch : segment) {
            if (ch != ' ' && ch != '\t') {  // remove spaces and tabs
                output += ch;
            }
        }
        // cout<<output.size()<<endl;
        if(output.size()==97)
        	v.push_back(output);
}
//REALTEK_ONT


//BROADCOM_ONT		
if(ONT_BROADCOM == true)
{

		// while(std::getline(ss, segment, ':'));

		// if(segment.size()==96)
		// 	v.push_back(segment);
}
//BROADCOM_ONT

// BROADCOM_OLT
if(OLT_BROADCOM == true)
{

        // while(std::getline(ss, segment, '='));
        
		// if(segment.size()==98)
		// 	v.push_back(segment.substr(0,96));
}
// BROADCOM_OLT

	}	

	fin.close();

	print_msg(v);

	return EXIT_SUCCESS;
}
