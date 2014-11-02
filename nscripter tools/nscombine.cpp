#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
    if(argc!=3)
    {
        cout << "usage: " << argv[0] << "[input list] [output name]" << endl;
        return 1;
    }
    
    ifstream list;
    list.open(argv[1]);
    
    if(list.is_open())
    {
        ofstream script;
        script.open(argv[2]);
            
        string line;
        while(list.good())
        {
            getline(list, line);
            
            if(line!="")
            {
                ifstream input;
                input.open(line.c_str());
                
                if(input.is_open())
                {
                    int scrlen = 1;
                    input.seekg(0, ios::end);
                    scrlen = input.tellg();
                    input.seekg(0, ios::beg);
                    
                    char *buffer = new char[scrlen+1];
                    
                    memset(buffer, '\0', scrlen+1);
                    input.read(buffer, scrlen);
                    
                    if(line!="_head.txt")
                    {
                        char *labelname = new char[line.length()];
                        int i=0;
                        
                        for(i=1; i<line.length()-3; i++)
                            labelname[i] = line[i-1];
                        labelname[i] = '\0';
                        labelname[0] = '*';
                        
                        script << labelname << endl;
                        
                        delete [] labelname;
                    }
                    
                    script << buffer;
                    
                    delete [] buffer;
                    input.close();
                }
                else
                {
                    cout << "Could not open " << line << endl;
                }
            }
        }
        
        list.close();
        script.close();
    }
}